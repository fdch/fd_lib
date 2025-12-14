/* Copyright (c) 1997-1999 Miller Puckette.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution. */

/* [hv]dial.c written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001 */
/* thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja */

/* name change to [vv]mradio by MSP (it's a mradio button really) and changed to
   put out a "float" as in sliders, toggles, etc. */

#include "m_pd.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Copyright (c) 1997-1999 Miller Puckette.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution. */

/* g_7_guis.c written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001 */
/* thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja */

/* hradio hacked to behave like a small array
 * and store/recall t_atoms by Fede Camara Halac 2017 */

/* Updated July 2018 for printing and corrected click action:
 * Used define for some default settings. */

/* typesetting and cleanup June 2019 // fdch */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ctype.h>

#include "g_all_guis.h"

#ifdef _WIN32
#include <io.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
/* Works on Linux/macOS/MinGW/Cygwin with GCC/Clang, no header needed */
#define FD_ALLOCA(nbytes) __builtin_alloca(nbytes)

#elif defined(_MSC_VER)
#include <malloc.h>
#define FD_ALLOCA(nbytes) _alloca(nbytes)

#else
#include <alloca.h>
#define FD_ALLOCA(nbytes) alloca(nbytes)
#endif

/* random helper function */
static int text_nthline(int n, t_atom *vec, int line, int *startp, int *endp) {
  int i, cnt = 0;
  for (i = 0; i < n; i++) {
    if (cnt == line) {
      int j = i;
      while (j < n && vec[j].a_type != A_SEMI && vec[j].a_type != A_COMMA) {
        j++;
      }
      *startp = i;
      *endp = j;
      return (1);
    } else if (vec[i].a_type == A_SEMI || vec[i].a_type == A_COMMA) {
      cnt++;
    }
  }
  return (0);
}

/* ------------- hdl     gui-horizontal dial ---------------------- */

t_widgetbehavior mradio_widgetbehavior;
static t_class *mradio_class;

/*This should be in g_all_guis.h*/
typedef struct _mradio {
  t_iemgui x_gui;
  t_atom *x_onlist;
  t_atom *x_drawnlist;
  t_atom *x_listout;
  t_symbol *x_bindsym;
  t_iem_orientation x_orientation;
  int x_number, x_pos, x_val, x_foc, x_old;
  unsigned char x_keep, x_focflag;
  /* the textbuf hack */
  t_binbuf *b_binbuf;
  t_canvas *b_canvas;
  t_guiconnect *b_guiconnect;
  t_symbol *b_sym;
  int b_status;
} t_mradio;

/*--------Shamelessly Taken from x_text--------*/

static void textbuf_hack_senditup(t_mradio *x) {
  if (!x->b_guiconnect)
    return;
  /* send the binbuf directly and let the GUI figure out when to do
   * linebreaks and how to escape special character $1*/
  pdgui_vmess("pdtk_textwindow_clear", "^", x);
  pdgui_vmess("pdtk_textwindow_appendatoms", "^A", x,
              binbuf_getnatom(x->b_binbuf), binbuf_getvec(x->b_binbuf));

  pdgui_vmess("pdtk_textwindow_setdirty", "^i", x, 0);
}

static void textbuf_hack_read(t_mradio *x, t_symbol *s, int argc,
                              t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  int cr = 0;
  t_symbol *filename;
  while (argc && argv->a_type == A_SYMBOL &&
         *argv->a_w.w_symbol->s_name == '-') {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-c")) {
      cr = 1;
    } else {
      pd_error(x, "text read: unknown flag ...");
      postatom(argc, argv);
      endpost();
    }
    argc--;
    argv++;
  }
  if (argc && argv->a_type == A_SYMBOL) {
    filename = argv->a_w.w_symbol;
    argc--;
    argv++;
  } else {
    pd_error(x, "text read: no file name given");
    return;
  }
  if (argc) {
    post("warning: text define ignoring extra argument: ");
    postatom(argc, argv);
    endpost();
  }
  if (binbuf_read_via_canvas(x->b_binbuf, filename->s_name, x->b_canvas, cr))
    pd_error(x, "%s: read failed", filename->s_name);
  textbuf_hack_senditup(x);
}

static void textbuf_hack_write(t_mradio *x, t_symbol *s, int argc,
                               t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  int cr = 0;
  t_symbol *filename;
  char buf[MAXPDSTRING];
  while (argc && argv->a_type == A_SYMBOL &&
         *argv->a_w.w_symbol->s_name == '-') {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-c")) {
      cr = 1;
    } else {
      pd_error(x, "text write: unknown flag ...");
      postatom(argc, argv);
      endpost();
    }
    argc--;
    argv++;
  }
  if (argc && argv->a_type == A_SYMBOL) {
    filename = argv->a_w.w_symbol;
    argc--;
    argv++;
  } else {
    pd_error(x, "text write: no file name given");
    return;
  }
  if (argc) {
    post("warning: text define ignoring extra argument: ");
    postatom(argc, argv);
    endpost();
  }

  canvas_makefilename(x->b_canvas, filename->s_name, buf, MAXPDSTRING);

  if (binbuf_write(x->b_binbuf, buf, "", cr))
    pd_error(x, "%s: write failed", filename->s_name);
}

static void textbuf_hack_free(t_mradio *x) {
  t_pd *x2;
  binbuf_free(x->b_binbuf);
  if (x->b_guiconnect) {
    pdgui_vmess("destroy", "^", x);
    guiconnect_notarget(x->b_guiconnect, 1000);
  }

  /* just in case we're still bound to #A from loading... */
  while ((x2 = pd_findbyclass(gensym("#A"), mradio_class))) {
    pd_unbind(x2, gensym("#A"));
  }
}

static void textbuf_hack_open(t_mradio *x) {
  x->b_status = 1;
  char tag[128];
  if (x->b_guiconnect) {
    sprintf(tag, ".x%lx.text", (unsigned long)x);
    pdgui_vmess("wm", "r^", "deiconify", x);
    pdgui_vmess("raise", "^", x);
    pdgui_vmess("focus", "s", tag);
  } else {
    sprintf(tag, "%dx%d", 200, 340);
    pdgui_vmess("pdtk_textwindow_open", "^r si", x, tag, x->b_sym->s_name,
                x->x_gui.x_fontsize * IEMGUI_ZOOM(x));
    sprintf(tag, ".x%lx", (unsigned long)x);
    x->b_guiconnect = guiconnect_new(&x->x_gui.x_obj.ob_pd, gensym(tag));
    textbuf_hack_senditup(x);
  }
}

static void textbuf_hack_close(t_mradio *x) {
  if (x->b_status) {
    pdgui_vmess("pdtk_textwindow_doclose", "^", x);
    if (x->b_guiconnect) {
      guiconnect_notarget(x->b_guiconnect, 1000);
      x->b_guiconnect = 0;
      x->b_status = 0;
    }
  }
}

/* widget helper functions */

static void mradio_resizer(t_mradio *x, int nsize) {
  int oldn = x->x_number;
  int newn = (nsize < 1 ? 1 : nsize);

  if (newn == oldn)
    return;

  if (oldn == 0) {
    // initialization only
    x->x_onlist = (t_atom *)getbytes(newn * sizeof(t_atom));
    x->x_drawnlist = (t_atom *)getbytes(newn * sizeof(t_atom));
    x->x_listout = (t_atom *)getbytes(newn * sizeof(t_atom));
  } else if (newn < oldn) {
    // old number is larger than the new
    size_t oldsize = (size_t)sizeof(t_atom) * oldn;
    size_t newsize = (size_t)sizeof(t_atom) * newn;

    resizebytes(x->x_onlist, oldsize, newsize);
    resizebytes(x->x_drawnlist, oldsize, newsize);
    resizebytes(x->x_listout, oldsize, newsize);
  } else {
    // new number is larger than old, discard everything
    freebytes(x->x_onlist, (size_t)sizeof(t_atom) * oldn);
    freebytes(x->x_drawnlist, (size_t)sizeof(t_atom) * oldn);
    freebytes(x->x_listout, (size_t)sizeof(t_atom) * oldn);
    x->x_onlist = (t_atom *)getbytes(newn * sizeof(t_atom));
    x->x_drawnlist = (t_atom *)getbytes(newn * sizeof(t_atom));
    x->x_listout = (t_atom *)getbytes(newn * sizeof(t_atom));
  }

  for (int i = 0, j = 0; i <= newn - 1; i++, j++) {
    SETFLOAT(&x->x_onlist[i], 0);
    SETFLOAT(&x->x_drawnlist[i], 0);
    SETFLOAT(&x->x_listout[i], 0);
  }

  x->x_number = newn;
}

/* cannot use iemgui's default draw_iolets, because
 * - vmradio would use show the outlet at the 0th button rather than the last...
 */
static void mradio_draw_io(t_mradio *x, t_glist *glist, int old_snd_rcv_flags) {
  int zoom = IEMGUI_ZOOM(x);
  int xpos = text_xpix(&x->x_gui.x_obj, glist);
  int ypos = text_ypix(&x->x_gui.x_obj, glist);
  int iow = IOWIDTH * zoom, ioh = IEM_GUI_IOHEIGHT * zoom;
  t_canvas *canvas = glist_getcanvas(glist);
  char tag_object[128], tag_but[128], tag[128];
  char *tags[] = {tag_object, tag};

  (void)old_snd_rcv_flags;

  sprintf(tag_object, "%pOBJ", x);
  sprintf(tag_but, "%pBUT", x);

  sprintf(tag, "%pOUT%d", x, 0);
  pdgui_vmess(0, "crs", canvas, "delete", tag);
  if (!x->x_gui.x_fsf.x_snd_able) {
    int height =
        x->x_gui.x_h * ((x->x_orientation == horizontal) ? 1 : x->x_number);
    pdgui_vmess(0, "crr iiii rk rk rS", canvas, "create", "rectangle", xpos,
                ypos + height + zoom - ioh, xpos + iow, ypos + height, "-fill",
                THISGUI->i_foregroundcolor, "-outline",
                THISGUI->i_foregroundcolor, "-tags", 2, tags);

    /* keep buttons above outlet */
    pdgui_vmess(0, "crss", canvas, "lower", tag, tag_but);
  }

  sprintf(tag, "%pIN%d", x, 0);
  pdgui_vmess(0, "crs", canvas, "delete", tag);
  if (!x->x_gui.x_fsf.x_rcv_able) {
    pdgui_vmess(0, "crr iiii rk rk rS", canvas, "create", "rectangle", xpos,
                ypos, xpos + iow, ypos - zoom + ioh, "-fill",
                THISGUI->i_foregroundcolor, "-outline",
                THISGUI->i_foregroundcolor, "-tags", 2, tags);

    /* keep buttons above inlet */
    pdgui_vmess(0, "crss", canvas, "lower", tag, tag_but);
  }
}

static void mradio_draw_config(t_mradio *x, t_glist *glist) {
  int i;
  int zoom = IEMGUI_ZOOM(x);
  int fill_color, focus_outline_color;
  t_iemgui *iemgui = &x->x_gui;
  t_canvas *canvas = glist_getcanvas(glist);
  int xx11b = text_xpix(&x->x_gui.x_obj, glist);
  int yy11b = text_ypix(&x->x_gui.x_obj, glist);
  int d, dx = 0, dy = 0, d4, s6;

  int xx11 = xx11b, xx12 = 0, xx21 = 0, xx22 = 0, xx31 = 0, xx32 = 0;
  int yy11 = yy11b, yy12 = 0, yy21 = 0, yy22 = 0, yy31 = 0, yy32 = 0;

  char tag[128];
  t_atom fontatoms[3];
  SETSYMBOL(fontatoms + 0, gensym(iemgui->x_font));
  SETFLOAT(fontatoms + 1, -iemgui->x_fontsize * zoom);
  SETSYMBOL(fontatoms + 2, gensym(sys_fontweight));

  if (x->x_orientation == horizontal)
    d = dx = x->x_gui.x_w;
  else
    d = dy = x->x_gui.x_h;

  xx12 = xx11 + d;
  yy12 = yy11 + d;

  d4 = d / 4;
  s6 = d / 8;
  // the circle
  xx21 = xx11 + d4;
  xx22 = xx12 - d4;
  yy21 = yy11 + d4;
  yy22 = yy12 - d4;
  // the focus rectangle
  xx31 = xx11b + s6;
  yy31 = yy11 + s6;
  xx32 = xx11b + dx - s6;
  yy32 = yy12 - s6;

  for (i = 0; i < x->x_number; i++) {
    fill_color = (atom_getfloat(x->x_onlist + i) == 1)
                     ? THISGUI->i_foregroundcolor
                     : THISGUI->i_backgroundcolor;
    focus_outline_color = (x->x_foc == i) ? THISGUI->i_foregroundcolor
                                          : THISGUI->i_backgroundcolor;
    // define the BASE tag
    sprintf(tag, "%pBASE%d", x, i);
    // pass the coordinates to the BASE tag object
    pdgui_vmess(0, "crs iiii", canvas, "coords", tag, xx11, yy11, xx12, yy12);
    // pass the configuration for width (zoom), and fill and outline colors
    pdgui_vmess(0, "crs ri rk rk", canvas, "itemconfigure", tag, "-width", zoom,
                "-fill", THISGUI->i_backgroundcolor, "-outline",
                THISGUI->i_foregroundcolor);

    // define the BUT tag
    sprintf(tag, "%pBUT%d", x, i);
    // pass the coordinates to the BUT tag object
    pdgui_vmess(0, "crs iiii", canvas, "coords", tag, xx21, yy21, xx22, yy22);
    // pass the configuration for fill and outline colors
    pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag, "-fill",
                fill_color, "-outline", fill_color);

    // define the FOC tag
    sprintf(tag, "%pFOC%d", x, i);
    // pass the coordinates to the FOC tag object
    pdgui_vmess(0, "crs iiii", canvas, "coords", tag, xx31, yy31, xx32, yy32);
    // pass the configuration for the FOC tag object
    pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline",
                focus_outline_color);

    xx11 += dx;
    xx12 += dx;
    xx21 += dx;
    xx22 += dx;
    xx31 += dx;
    xx32 += dx;
    yy11 += dy;
    yy12 += dy;
    yy21 += dy;
    yy22 += dy;
    yy31 += dy;
    yy32 += dy;
  }

  sprintf(tag, "%pLABEL", x);
  pdgui_vmess(0, "crs ii", canvas, "coords", tag, xx11b + x->x_gui.x_ldx * zoom,
              yy11b + x->x_gui.x_ldy * zoom);
  pdgui_vmess(0, "crs rA rk", canvas, "itemconfigure", tag, "-font", 3,
              fontatoms, "-fill", x->x_gui.x_lcol);
  iemgui_dolabel(x, &x->x_gui, x->x_gui.x_lab, 1);
}

static void mradio_draw_new(t_mradio *x, t_glist *glist) {
  t_canvas *canvas = glist_getcanvas(glist);
  int i;
  char tag_n[128], tag[128], tag_object[128];
  char *tags[] = {tag_object, tag, tag_n, "text"};
  sprintf(tag_object, "%pOBJ", x);

  for (i = 0; i < x->x_number; i++) {
    sprintf(tag, "%pBASE", x);
    sprintf(tag_n, "%pBASE%d", x, i);
    pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle", 0, 0, 0, 0,
                "-tags", 3, tags);

    sprintf(tag, "%pBUT", x);
    sprintf(tag_n, "%pBUT%d", x, i);
    pdgui_vmess(0, "crr iiii rS", canvas, "create", "oval", 0, 0, 0, 0, "-tags",
                3, tags);

    sprintf(tag, "%pFOC", x);
    sprintf(tag_n, "%pFOC%d", x, i);
    pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle", 0, 0, 0, 0,
                "-tags", 3, tags);
  }
  /* make sure the buttons are above their base */
  sprintf(tag, "%pBUT", x);
  sprintf(tag_n, "%pBASE", x);
  pdgui_vmess(0, "crss", canvas, "raise", tag, tag_n);

  sprintf(tag, "%pLABEL", x);
  sprintf(tag_n, "label");
  pdgui_vmess(0, "crr ii rs rS", canvas, "create", "text", 0, 0, "-anchor", "w",
              "-tags", 4, tags);

  mradio_draw_config(x, glist);
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO);
}

static void mradio_draw_select(t_mradio *x, t_glist *glist) {
  t_canvas *canvas = glist_getcanvas(glist);
  char tag[128];
  unsigned int col = THISGUI->i_foregroundcolor, lcol = x->x_gui.x_lcol;

  if (x->x_gui.x_fsf.x_selected)
    lcol = col = THISGUI->i_selectcolor;

  sprintf(tag, "%pBASE", x);
  pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline", col);
  sprintf(tag, "%pLABEL", x);
  pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-fill", lcol);
}

static void mradio_draw_update(t_gobj *client, t_glist *glist) {
  t_mradio *x = (t_mradio *)client;
  int n = x->x_number, i = 0;
  int val = x->x_foc;
  char tag[128];

  if (glist_isvisible(glist)) {
    t_canvas *canvas = glist_getcanvas(glist);

    while (n--) {
      if (x->x_focflag) {
        sprintf(tag, "%pFOC%d", x, i);
        pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline",
                    (i == val) ? THISGUI->i_foregroundcolor
                               : THISGUI->i_backgroundcolor);
      }
      // post("onlist %d : %d",i,(int)atom_getfloat(x->x_onlist+i));
      if (atom_getfloat(x->x_onlist + i)) {
        sprintf(tag, "%pBUT%d", x, i);
        pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag, "-fill",
                    THISGUI->i_foregroundcolor, "-outline",
                    THISGUI->i_foregroundcolor);
      } else {
        sprintf(tag, "%pBUT%d", x, i);
        pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag, "-fill",
                    THISGUI->i_backgroundcolor, "-outline",
                    THISGUI->i_backgroundcolor);
      }
      SETFLOAT(&x->x_drawnlist[i], atom_getfloat(x->x_onlist + i));
      i++;
    }
  }
}

/* ------------------------ hdl widgetbehaviour----------------------------- */

static void mradio_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1,
                           int *xp2, int *yp2) {
  t_mradio *x = (t_mradio *)z;

  *xp1 = text_xpix(&x->x_gui.x_obj, glist);
  *yp1 = text_ypix(&x->x_gui.x_obj, glist);
  if (x->x_orientation == horizontal) {
    *xp2 = *xp1 + x->x_gui.x_w * x->x_number;
    *yp2 = *yp1 + x->x_gui.x_h;
  } else {
    *xp2 = *xp1 + x->x_gui.x_w;
    *yp2 = *yp1 + x->x_gui.x_h * x->x_number;
  }
}

static void mradio_save(t_gobj *z, t_binbuf *b) {
  t_mradio *x = (t_mradio *)z;
  t_symbol *bflcol[3];
  t_symbol *srl[3];
  t_binbuf *bb = (t_binbuf *)x->b_binbuf;
  int n;
  const char *objname = x->x_orientation == horizontal ? "hmradio" : "vmradio";

  iemgui_save(&x->x_gui, srl, bflcol);
  binbuf_addv(b, "ssiisiiisssiiiisss", gensym("#X"), gensym("obj"),
              (int)x->x_gui.x_obj.te_xpix, (int)x->x_gui.x_obj.te_ypix,
              gensym(objname), x->x_gui.x_w / IEMGUI_ZOOM(x), (int)x->x_keep,
              x->x_number, srl[0], srl[1], srl[2], x->x_gui.x_ldx,
              x->x_gui.x_ldy, iem_fstyletoint(&x->x_gui.x_fsf),
              x->x_gui.x_fontsize, bflcol[0], bflcol[1], bflcol[2]);
  binbuf_addv(b, ";");
  if (!bb)
    return;
  n = binbuf_getnatom(bb);
  if (x->x_keep && n) {
    binbuf_addv(b, "ss", gensym("#A"), gensym("preset"));
    binbuf_addbinbuf(b, x->b_binbuf); // TODO: try bb here
    binbuf_addsemi(b);
  }
  obj_saveformat(&x->x_gui.x_obj, b);
}

static void mradio_properties(t_gobj *z, t_glist *owner) {
  (void)*owner; // silence -Wunused-parameter
  t_mradio *x = (t_mradio *)z;
  const char *objname =
      (x->x_orientation == horizontal) ? "hmradio" : "vmradio";
  float zoom = (float)IEMGUI_ZOOM(x); // cast to float to avoid warning
  iemgui_new_dialog(x,                // void*x the class
                    &x->x_gui,        // t_iemgui
                    objname,          // object name
                    x->x_gui.x_w / zoom, IEM_GUI_MINSIZE, // width, width_min
                    0, 0,                                 // height, height_min
                    0, 0,       // range_min, range_max
                    0,          // schedule
                    0,          // mode
                    "mode0",    // label mode0
                    "mode1",    // label mode1
                    1,          // canloadbang
                    -1,         // steady
                    x->x_number // number
  );
}

static void mradio_dialog(t_mradio *x, t_symbol *s, int argc, t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  t_symbol *srl[3];
  int sr_flags;
  int a = (int)atom_getintarg(0, argc, argv);
  int keep = (int)atom_getintarg(4, argc, argv);
  int num = (int)atom_getintarg(6, argc, argv);

  if (keep != 0)
    keep = 1;

  x->x_keep = (unsigned char)keep;
  sr_flags = iemgui_dialog(&x->x_gui, srl, argc, argv);
  x->x_gui.x_w = iemgui_clip_size(a);
  x->x_gui.x_h = x->x_gui.x_w;

  if (x->x_number != num) {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    mradio_resizer(x, num);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  } else {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO + sr_flags);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_MOVE);
    canvas_fixlinesfor(x->x_gui.x_glist, (t_text *)x);
  }
}

static void mradio_keep(t_mradio *x, t_floatarg f) {
  x->x_keep = (unsigned char)(f > 0 ? 1 : 0);
}

// TODO: unify preset and set into a single function
static void mradio_preset(t_mradio *x, t_symbol *s, int argc, t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  int n = 0;
  if (argc) {
    if (argc > x->x_number && argc < IEM_RADIO_MAX)
      n = x->x_number;

    if (argc <= x->x_number)
      n = argc;

    int i = 0;
    while (n--) {
      if (!IS_A_FLOAT(argv, i)) {
        goto error;
      }
    }
    binbuf_restore(x->b_binbuf, argc, argv);
    textbuf_hack_senditup(x);
  } else {
  error:
    pd_error(x, "Can't parse:");
    postatom(argc, argv);
    endpost();
  }
}

static void mradio_set(t_mradio *x, t_symbol *s, int argc, t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  int n = 0, i = 0;
  if (argc) {
    if (argc > x->x_number && argc < IEM_RADIO_MAX)
      n = x->x_number;

    if (argc <= x->x_number)
      n = argc;

    while (n--) {
      if (IS_A_FLOAT(argv, i)) {
        SETFLOAT(&x->x_onlist[i],
                 (t_float)(atom_getintarg(i, argc, argv) == 0) ? 0 : 1);
        SETFLOAT(&x->x_listout[i],
                 (t_float)(atom_getintarg(i, argc, argv) == 0) ? 0 : 1);
        i++;
      } else {
        pd_error(x, "mradio_set: index %d is not a number", i);
      }
    }
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
  } else {
    pd_error(x, "Can't parse:");
    postatom(argc, argv);
    endpost();
  }
}

// TODO: is this needed?
static void textbuf_hack_addline(t_mradio *x, t_symbol *s, int argc,
                                 t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  t_binbuf *b = binbuf_new();
  binbuf_restore(b, argc, argv);
  binbuf_add(x->b_binbuf, binbuf_getnatom(b), binbuf_getvec(b));
  binbuf_free(b);
  // TODO: is this needed here?
  mradio_preset(x, gensym("preset"), argc, argv);
}

static void mradio_bang(t_mradio *x) {
  outlet_list(x->x_gui.x_obj.ob_outlet, 0, x->x_number, x->x_listout);

  if (x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
    pd_list(x->x_gui.x_snd->s_thing, 0, x->x_number, x->x_listout);
}

static void mradio_fout(t_mradio *x, t_floatarg f) {
  int i = (int)f;

  if (i < 0)
    i = 0;
  if (i >= x->x_number)
    i = x->x_number - 1;

  int curr_number = atom_getfloat(x->x_drawnlist + i);

  SETFLOAT(&x->x_onlist[i], curr_number == 1 ? 0 : 1);
  SETFLOAT(&x->x_listout[i], atom_getfloat(x->x_onlist + i));
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void mradio_float(t_mradio *x, t_floatarg f) {
  // TODO: fix this with edges and output mode
  //  modulo the float
  int abs_i = (unsigned int)(f < 0 ? -f : f);
  int modulo_i = abs_i % x->x_number;

  int next_pos = modulo_i;

  // send it out the outlet
  outlet_float(x->x_gui.x_obj.ob_outlet, atom_getfloat(x->x_onlist + next_pos));
  // send it to the receiver
  if (x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing) {
    pd_float(x->x_gui.x_snd->s_thing, atom_getfloat(x->x_onlist + next_pos));
  }
  // update the focus index
  x->x_foc = next_pos;
  if (x->x_focflag && x->x_foc != x->x_old)
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);

  x->x_old = x->x_foc;
}

static void mradio_print(t_mradio *x) {
  post("------------------------[mradio info]------------------------");
  post("num: %d\ndrawn: %d\npos: %d\n\
val: %d\nfoc: %d\nold: %d\nfflag: %d\nkeep:%d",
       x->x_number, x->x_pos, x->x_val, x->x_foc, x->x_old, x->x_focflag,
       x->x_keep);
  // int i;
  // for(i=0; i<=x->x_number; i++) {
  //   post("onlist[%d]:%d", i,x->x_onlist[i]);
  // }
  // for(i=0; i<=x->x_number; i++) {
  //   post("drawnlist[%d]:%d", i,x->x_drawnlist[i]);
  // }

  t_binbuf *b = (t_binbuf *)x->b_binbuf;
  t_atom *vec;
  int start, end, n, i = 0;
  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);

  if (!text_nthline(n, vec, i, &start, &end)) {
    postatom(x->x_number, x->x_listout);
    endpost();
  } else {
    while (text_nthline(n, vec, i, &start, &end)) {
      int outc = end - start, k;
      t_atom *outv = FD_ALLOCA((outc) * sizeof(t_atom));
      for (k = 0; k < outc; k++) {
        outv[k] = vec[start + k];
      }
      postatom(outc, outv);
      endpost();
      i++;
    }
  }
}

static int get_selection(t_mradio *x, t_floatarg xpos, t_floatarg ypos) {
  int selected;
  if (x->x_orientation == horizontal) {
    int xx = (int)xpos - (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
    selected = xx / (float)x->x_gui.x_w;
  } else {
    int yy = (int)ypos - (int)text_ypix(&x->x_gui.x_obj, x->x_gui.x_glist);
    selected = yy / (float)x->x_gui.x_h;
  }
  if (selected >= x->x_number)
    selected = x->x_number - 1;
  if (selected < 0)
    selected = 0;

  return (selected);
}

static void mradio_click(t_mradio *x, t_floatarg xpos, t_floatarg ypos,
                         t_floatarg shift, t_floatarg ctrl, t_floatarg alt) {
  (void)ctrl; // silence -Wunused-parameter
  (void)alt;  // silence -Wunused-parameter
  if (!shift) {
    mradio_fout(x, (t_float)get_selection(x, xpos, ypos));
  } else {
    textbuf_hack_open(x);
  }
}

static int mradio_newclick(t_gobj *z, struct _glist *glist, int xpix, int ypix,
                           int shift, int alt, int dbl, int doit) {
  (void)dbl;   // silence -Wunused-parameter
  (void)glist; // silence -Wunused-parameter
  if (doit)
    mradio_click((t_mradio *)z, (t_floatarg)xpix, (t_floatarg)ypix,
                 (t_floatarg)shift, 0, (t_floatarg)alt);
  return (1);
}

static void mradio_focus(t_mradio *x, t_floatarg foc) {
  t_canvas *canvas = glist_getcanvas(x->x_gui.x_glist);
  unsigned char focus = (int)foc > 0 ? 1 : 0;

  if (x->x_focflag == focus)
    return;

  char tag[128];
  if (focus)
    x->x_focflag = 1;
  else {
    x->x_focflag = 0;
    for (int i = 0; i < x->x_number; i++) {
      sprintf(tag, "%pFOC%d", x, i);
      pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline",
                  THISGUI->i_backgroundcolor);
    }
  }
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void mradio_loadbang(t_mradio *x, t_floatarg action) {
  if (action == LB_LOAD && x->x_gui.x_isa.x_loadinit)
    mradio_bang(x);
}

static void mradio_step(t_mradio *x, t_floatarg f) {
  mradio_float(x, f + x->x_old);
}

static void mradio_next(t_mradio *x) { mradio_step(x, (t_float)1); }

static void mradio_prev(t_mradio *x) { mradio_step(x, (t_float)-1); }

static void mradio_number(t_mradio *x, t_floatarg num) {
  int n = (int)num;

  if (num < 1)
    n = 1;

  if (num > IEM_RADIO_MAX)
    n = IEM_RADIO_MAX;

  if (n != x->x_number) {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    mradio_resizer(x, n);
    x->x_number = n;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  }
}

// FIXME: orientation needs to be looked at
static void mradio_orientation(t_mradio *x, t_floatarg forient) {
  x->x_orientation = !!(int)forient;
  iemgui_size(x, &x->x_gui);
}

static void mradio_size(t_mradio *x, t_symbol *s, int ac, t_atom *av) {
  (void)s; // silence -Wunused-parameter
  x->x_gui.x_w =
      iemgui_clip_size((int)atom_getfloatarg(0, ac, av)) * IEMGUI_ZOOM(x);
  x->x_gui.x_h = x->x_gui.x_w;
  iemgui_size((void *)x, &x->x_gui);
}

static void mradio_delta(t_mradio *x, t_symbol *s, int ac, t_atom *av) {
  iemgui_delta((void *)x, &x->x_gui, s, ac, av);
}

static void mradio_pos(t_mradio *x, t_symbol *s, int ac, t_atom *av) {
  iemgui_pos((void *)x, &x->x_gui, s, ac, av);
}

static void mradio_color(t_mradio *x, t_symbol *s, int ac, t_atom *av) {
  iemgui_color((void *)x, &x->x_gui, s, ac, av);
}

static void mradio_send(t_mradio *x, t_symbol *s) {
  iemgui_send(x, &x->x_gui, s);
}

static void mradio_receive(t_mradio *x, t_symbol *s) {
  iemgui_receive(x, &x->x_gui, s);
}

static void mradio_label(t_mradio *x, t_symbol *s) {
  iemgui_label((void *)x, &x->x_gui, s);
}

static void mradio_label_pos(t_mradio *x, t_symbol *s, int ac, t_atom *av) {
  iemgui_label_pos((void *)x, &x->x_gui, s, ac, av);
}

static void mradio_label_font(t_mradio *x, t_symbol *s, int ac, t_atom *av) {
  iemgui_label_font((void *)x, &x->x_gui, s, ac, av);
}

static void mradio_init(t_mradio *x, t_floatarg f) {
  x->x_gui.x_isa.x_loadinit = (f == 0.0) ? 0 : 1;
}

static void mradio_store(t_mradio *x, t_floatarg ff) {
  t_binbuf *b = (t_binbuf *)x->b_binbuf;
  int start, end, n, i, lineno;
  int f = (int)ff;
  t_atom *vec;
  if (!b)
    return;

  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);
  if (f && ((lineno = f < 0 ? 0 : f) >= 0) &&
      text_nthline(n, vec, lineno, &start, &end)) {
    post("n:%d, lineno:%d,start:%d,end:%d", n, lineno, start, end);
    int oldn = n;
    n = n + (x->x_number - (end - start));
    if (n > oldn)
      (void)binbuf_resize(b, n);

    vec = binbuf_getvec(b);
    memmove(&vec[start + x->x_number], &vec[end], sizeof(*vec) * (oldn - end));
    // post("after memmove");
    if (n < oldn) {
      (void)binbuf_resize(b, n);
      vec = binbuf_getvec(b);
    }
    // post("before loop");
    for (i = 0; i < x->x_number; i++) {
      vec[start + i] = x->x_listout[i];
      // post("loop:%d",n);
    }
  } else {
    binbuf_restore(x->b_binbuf, x->x_number, x->x_listout);
    binbuf_addsemi(x->b_binbuf);
    textbuf_hack_senditup(x);
  }
}

// TODO: unify recall and flush?
static void mradio_recall(t_mradio *x, t_floatarg f) {
  t_binbuf *b = (t_binbuf *)x->b_binbuf;
  int start, end, n;
  t_atom *vec;
  if (!b)
    return;

  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);

  if (text_nthline(n, vec, f, &start, &end)) {
    int outc = end - start, k;
    t_atom *outv = FD_ALLOCA((outc) * sizeof(t_atom));
    for (k = 0; k < outc; k++) {
      outv[k] = vec[start + k];
    }
    mradio_set(x, gensym("set"), outc, outv);
  }
}

static void mradio_flush(t_mradio *x, t_symbol *s) {
  if (s->s_thing) {
    t_binbuf *b = (t_binbuf *)x->b_binbuf;
    int start, end, n, i = 0;
    t_atom *vec;
    if (!b)
      return;

    vec = binbuf_getvec(b);
    n = binbuf_getnatom(b);
    while (text_nthline(n, vec, i, &start, &end)) {
      int outc = end - start, k;
      t_atom *outv = FD_ALLOCA((outc) * sizeof(t_atom));
      for (k = 0; k < outc; k++) {
        outv[k] = vec[start + k];
      }
      pd_list(s->s_thing, gensym("list"), outc, outv);
      i++;
    }
  }
}

static void mradio_clear(t_mradio *x) {
  int n = x->x_number;
  int i = 0;
  while (n--) {
    SETFLOAT(&x->x_onlist[i], 0);
    SETFLOAT(&x->x_drawnlist[i], 0);
    SETFLOAT(&x->x_listout[i], 0);
    i++;
  }
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
  binbuf_clear(x->b_binbuf);
  textbuf_hack_senditup(x);
}

static void mradio_free(t_mradio *x) {
  textbuf_hack_free(x);
  freebytes(x->x_onlist, (size_t)sizeof(t_atom) * x->x_number);
  freebytes(x->x_drawnlist, (size_t)sizeof(t_atom) * x->x_number);
  freebytes(x->x_listout, (size_t)sizeof(t_atom) * x->x_number);
  iemgui_free((t_iemgui *)x);
}

static void *mradio_donew(t_symbol *s, int argc, t_atom *argv, int old) {
  (void)old;
  t_mradio *x = (t_mradio *)iemgui_new(mradio_class);

  int ldx = 0, ldy = -8 * IEM_GUI_DEFAULTSIZE_SCALE;
  int keep = 1, num = 8, a = IEM_GUI_DEFAULTSIZE;
  int fs = x->x_gui.x_fontsize;

  if ('v' == *s->s_name)
    x->x_orientation = vertical;

  IEMGUI_SETDRAWFUNCTIONS(x, mradio);

  if ((argc == 13) && IS_A_FLOAT(argv, 0) && IS_A_FLOAT(argv, 1) &&
      IS_A_FLOAT(argv, 2) && (IS_A_SYMBOL(argv, 3) || IS_A_FLOAT(argv, 3)) &&
      (IS_A_SYMBOL(argv, 4) || IS_A_FLOAT(argv, 4)) &&
      (IS_A_SYMBOL(argv, 5) || IS_A_FLOAT(argv, 5)) && IS_A_FLOAT(argv, 6) &&
      IS_A_FLOAT(argv, 7) && IS_A_FLOAT(argv, 8) && IS_A_FLOAT(argv, 9)) {
    a = (int)atom_getintarg(0, argc, argv);
    keep = (int)atom_getintarg(1, argc, argv);
    num = (int)atom_getintarg(2, argc, argv);
    iemgui_new_getnames(&x->x_gui, 3, argv);
    ldx = (int)atom_getintarg(6, argc, argv);
    ldy = (int)atom_getintarg(7, argc, argv);
    iem_inttofstyle(&x->x_gui.x_fsf, atom_getintarg(8, argc, argv));
    fs = (int)atom_getintarg(9, argc, argv);
    iemgui_all_loadcolors(&x->x_gui, argv + 10, argv + 11, argv + 12);
  } else
    iemgui_new_getnames(&x->x_gui, 4, 0);

  x->x_keep = (unsigned char)keep;
  x->x_gui.x_fsf.x_snd_able = (0 != x->x_gui.x_snd);
  x->x_gui.x_fsf.x_rcv_able = (0 != x->x_gui.x_rcv);
  if (x->x_gui.x_fsf.x_font_style == 1)
    strcpy(x->x_gui.x_font, "helvetica");
  else if (x->x_gui.x_fsf.x_font_style == 2)
    strcpy(x->x_gui.x_font, "times");
  else {
    x->x_gui.x_fsf.x_font_style = 0;
    strcpy(x->x_gui.x_font, sys_font);
  }

  if (num < 1)
    num = 1;

  if (num > IEM_RADIO_MAX)
    num = IEM_RADIO_MAX;

  // Initialize internal memory
  x->x_number = 0; // bash this to zero to ensure initialization
  mradio_resizer(x, num);

  // Text initialization
  x->b_binbuf = binbuf_new();
  x->b_canvas = canvas_getcurrent();
  x->b_sym = gensym("text");
  t_symbol *asym = gensym("#A");
  /* bashily unbind #A -- this would create garbage if #A were
  multiply bound but we believe in this context it's at most
  bound to whichever text_define or array was created most recently */
  asym->s_thing = 0;
  /* and now bind #A to us to receive following messages in the
  saved file or copy buffer */
  pd_bind(&x->x_gui.x_obj.ob_pd, asym);

  // bind the receiver on the iemgui object if it exists
  if (x->x_gui.x_fsf.x_rcv_able)
    pd_bind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);

  x->x_gui.x_ldx = ldx;
  x->x_gui.x_ldy = ldy;
  x->x_gui.x_fontsize = (fs < 4) ? 4 : fs;
  x->x_gui.x_w = iemgui_clip_size(a);
  x->x_gui.x_h = x->x_gui.x_w;
  iemgui_verify_snd_ne_rcv(&x->x_gui);
  iemgui_newzoom(&x->x_gui);
  outlet_new(&x->x_gui.x_obj, &s_list);
  return (x);
}

static void *mradio_new(t_symbol *s, int argc, t_atom *argv) {
  return (mradio_donew(s, argc, argv, 0));
}

void g_mradio_setup(void) {
  mradio_class =
      class_new(gensym("hmradio"), (t_newmethod)mradio_new,
                (t_method)mradio_free, sizeof(t_mradio), 0, A_GIMME, 0);
  class_addcreator((t_newmethod)mradio_new, gensym("vmradio"), A_GIMME, 0);

  class_addcreator((t_newmethod)mradio_new, gensym("mdb"), A_GIMME, 0);
  class_addcreator((t_newmethod)mradio_new, gensym("mradiobut"), A_GIMME, 0);
  class_addcreator((t_newmethod)mradio_new, gensym("mradiobutton"), A_GIMME, 0);

  class_addbang(mradio_class, mradio_bang);
  class_addfloat(mradio_class, mradio_float);
  class_addmethod(mradio_class, (t_method)mradio_click, gensym("click"),
                  A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_dialog, gensym("dialog"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_clear, gensym("clear"),
                  A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_focus, gensym("focus"),
                  A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_preset, gensym("preset"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_loadbang, gensym("loadbang"),
                  A_DEFFLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_set, gensym("set"), A_FLOAT,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_keep, gensym("keep"), A_FLOAT,
                  0);
  class_addmethod(mradio_class, (t_method)textbuf_hack_open, gensym("open"), 0);
  class_addmethod(mradio_class, (t_method)textbuf_hack_close, gensym("close"),
                  0);
  class_addmethod(mradio_class, (t_method)textbuf_hack_addline,
                  gensym("addline"), A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_step, gensym("step"), A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_next, gensym("next"), A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_store, gensym("store"),
                  A_DEFFLOAT, A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_recall, gensym("recall"),
                  A_DEFFLOAT, A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_flush, gensym("flush"),
                  A_SYMBOL, A_NULL);
  class_addmethod(mradio_class, (t_method)textbuf_hack_write, gensym("write"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)textbuf_hack_read, gensym("read"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_prev, gensym("prev"), A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_size, gensym("size"), A_GIMME,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_delta, gensym("delta"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_pos, gensym("pos"), A_GIMME,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_color, gensym("color"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_send, gensym("send"), A_DEFSYM,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_receive, gensym("receive"),
                  A_DEFSYM, 0);
  class_addmethod(mradio_class, (t_method)mradio_label, gensym("label"),
                  A_DEFSYM, 0);
  class_addmethod(mradio_class, (t_method)mradio_label_pos, gensym("label_pos"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_label_font,
                  gensym("label_font"), A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_init, gensym("init"), A_FLOAT,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_number, gensym("number"),
                  A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_orientation,
                  gensym("orientation"), A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)iemgui_zoom, gensym("zoom"), A_CANT,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_print, gensym("print"),
                  A_NULL);
  mradio_widgetbehavior.w_getrectfn = mradio_getrect;
  mradio_widgetbehavior.w_displacefn = iemgui_displace;
  mradio_widgetbehavior.w_selectfn = iemgui_select;
  mradio_widgetbehavior.w_activatefn = NULL;
  mradio_widgetbehavior.w_deletefn = iemgui_delete;
  mradio_widgetbehavior.w_visfn = iemgui_vis;
  mradio_widgetbehavior.w_clickfn = mradio_newclick;
  class_setwidget(mradio_class, &mradio_widgetbehavior);

  class_sethelpsymbol(mradio_class, gensym("mradio"));
  class_setsavefn(mradio_class, mradio_save);
  class_setpropertiesfn(mradio_class, mradio_properties);

  post("[mradio] version 0.3.4beta ~ fdch");
}
