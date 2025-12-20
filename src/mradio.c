#include "fdLib.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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

/* ------------- hdl     gui-horizontal dial ---------------------- */

t_widgetbehavior mradio_widgetbehavior;
static t_class *mradio_class;

/*This should be in g_all_guis.h*/
typedef struct _mradio {
  t_iemgui x_gui;
  t_float *x_matrix;
  t_symbol *x_bindsym;
  int x_n, x_m;
  int x_pos, x_val, x_foc, x_old;
  int x_mode; // mode 0, 1, or 3 are modulo (default), limit, reflect
  unsigned char x_keep, x_focflag;
} t_mradio;

/* widget helper functions */

static void mradio_doresize(t_mradio *x, int nsize, int msize) {
  int oldn = x->x_n;
  int oldm = x->x_m;
  int newn = (nsize < 1 ? 1 : nsize);
  int newm = (msize < 1 ? 1 : msize);

  if (newn == oldn && newm == oldm)
    return;

  if (!oldn) {
    // initialization only
    x->x_matrix = (t_float *)getbytes(newn * newm * sizeof(t_float));
  } else {
    freebytes(x->x_matrix, (size_t)sizeof(t_float) * oldn * oldm);
    x->x_matrix = (t_float *)getbytes(newn * newm * sizeof(t_float));
  }

  for (int i = 0; i < newn; ++i)
    for (int j = 0; j < newm; ++j)
      x->x_matrix[i * newm + j] = 0;

  x->x_n = newn;
  x->x_m = newm;
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
    int height = x->x_gui.x_h;
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

  d = dx = x->x_gui.x_w;
  dy = x->x_gui.x_h;

  for (int i = 0; i < x->x_n; ++i) {
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

    for (int j = 0; j < x->x_m; ++j) {
      int idx = i * x->x_m + j;
      fill_color = (x->x_matrix[idx] == 1) ? THISGUI->i_foregroundcolor
                                           : THISGUI->i_backgroundcolor;
      focus_outline_color = (x->x_foc == idx) ? THISGUI->i_foregroundcolor
                                              : THISGUI->i_backgroundcolor;
      sprintf(tag, "%pBASE%d", x, idx);
      pdgui_vmess(0, "crs iiii", canvas, "coords", tag, xx11, yy11, xx12, yy12);
      pdgui_vmess(0, "crs ri rk rk", canvas, "itemconfigure", tag, "-width",
                  zoom, "-fill", THISGUI->i_backgroundcolor, "-outline",
                  THISGUI->i_foregroundcolor);

      sprintf(tag, "%pBUT%d", x, idx);
      pdgui_vmess(0, "crs iiii", canvas, "coords", tag, xx21, yy21, xx22, yy22);
      pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag, "-fill",
                  fill_color, "-outline", fill_color);

      sprintf(tag, "%pFOC%d", x, idx);
      pdgui_vmess(0, "crs iiii", canvas, "coords", tag, xx31, yy31, xx32, yy32);
      pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline",
                  focus_outline_color);

      xx11 += dx;
      xx12 += dx;
      xx21 += dx;
      xx22 += dx;
      xx31 += dx;
      xx32 += dx;
    }
    yy11 += dy;
    yy12 += dy;
    yy21 += dy;
    yy22 += dy;
    yy31 += dy;
    yy32 += dy;
    xx11 = xx11b;
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

  for (int i = 0; i < x->x_n; ++i) {
    for (int j = 0; j < x->x_m; ++j) {
      int idx = i * x->x_m + j;
      sprintf(tag, "%pBASE", x);
      sprintf(tag_n, "%pBASE%d", x, idx);
      pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle", 0, 0, 0, 0,
                  "-tags", 3, tags);

      sprintf(tag, "%pBUT", x);
      sprintf(tag_n, "%pBUT%d", x, idx);
      pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle", 0, 0, 0, 0,
                  "-tags", 3, tags);

      sprintf(tag, "%pFOC", x);
      sprintf(tag_n, "%pFOC%d", x, idx);
      pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle", 0, 0, 0, 0,
                  "-tags", 3, tags);
    }
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
  int n = x->x_n, i = 0;
  int val = x->x_foc;
  char tag[128];

  if (glist_isvisible(glist)) {
    t_canvas *canvas = glist_getcanvas(glist);

    for (int i = 0; i < x->x_n; ++i) {
      for (int j = 0; j < x->x_m; ++j) {
        int idx = i * x->x_m + j;
        if (x->x_focflag) {
          sprintf(tag, "%pFOC%d", x, idx);
          pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline",
                      (idx == val) ? THISGUI->i_foregroundcolor
                                   : THISGUI->i_backgroundcolor);
        }
        // post("onlist %d : %d",i,(int)atom_getfloat(x->x_matrix+i));
        if (x->x_matrix[idx] != 0) {
          sprintf(tag, "%pBUT%d", x, idx);
          pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag, "-fill",
                      THISGUI->i_foregroundcolor, "-outline",
                      THISGUI->i_foregroundcolor);
        } else {
          sprintf(tag, "%pBUT%d", x, idx);
          pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag, "-fill",
                      THISGUI->i_backgroundcolor, "-outline",
                      THISGUI->i_backgroundcolor);
        }
      }
    }
  }
}

/* ------------------------ hdl widgetbehaviour-----------------------------
 */

static void mradio_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1,
                           int *xp2, int *yp2) {
  t_mradio *x = (t_mradio *)z;

  *xp1 = text_xpix(&x->x_gui.x_obj, glist);
  *yp1 = text_ypix(&x->x_gui.x_obj, glist);
  *xp2 = *xp1 + x->x_gui.x_w * x->x_n;
  *yp2 = *yp1 + x->x_gui.x_h * x->x_m;
}

static void mradio_save(t_gobj *z, t_binbuf *b) {
  t_mradio *x = (t_mradio *)z;
  t_symbol *bflcol[3];
  t_symbol *srl[3];
  int n;
  const char *objname = "mradio";

  iemgui_save(&x->x_gui, srl, bflcol);
  binbuf_addv(b, "ssiisiiisssiiiisss", gensym("#X"), gensym("obj"),
              (int)x->x_gui.x_obj.te_xpix, (int)x->x_gui.x_obj.te_ypix,
              gensym(objname), x->x_gui.x_w / IEMGUI_ZOOM(x), (int)x->x_keep,
              x->x_n, srl[0], srl[1], srl[2], x->x_gui.x_ldx, x->x_gui.x_ldy,
              iem_fstyletoint(&x->x_gui.x_fsf), x->x_gui.x_fontsize, bflcol[0],
              bflcol[1], bflcol[2]);
  binbuf_addv(b, ";");
  obj_saveformat(&x->x_gui.x_obj, b);
}

static void mradio_properties(t_gobj *z, t_glist *owner) {
  (void)*owner; // silence -Wunused-parameter
  t_mradio *x = (t_mradio *)z;
  const char *objname = "mradio";
  float zoom = (float)IEMGUI_ZOOM(x); // cast to float to avoid warning
  iemgui_new_dialog(x,                // void*x the class
                    &x->x_gui,        // t_iemgui
                    objname,          // object name
                    x->x_gui.x_w / zoom, IEM_GUI_MINSIZE, // width, width_min
                    0, 0,                                 // height, height_min
                    0, 0,  // range_min, range_max
                    0,     // schedule
                    0,     // mode
                    "mod", // label mode0
                    "lim", // label mode1
                    1,     // canloadbang
                    -1,    // steady
                    x->x_n // number
  );
}

static void mradio_dialog(t_mradio *x, t_symbol *s, int argc, t_atom *argv) {
  (void)s; // silence -Wunused-parameter
  // TODO: add mode argument to the creation args
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

  if (x->x_n != num) {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    mradio_doresize(x, num, num);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  } else {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO + sr_flags);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_MOVE);
    canvas_fixlinesfor(x->x_gui.x_glist, (t_text *)x);
  }
}

static void mradio_mode(t_mradio *x, t_floatarg f) {
  x->x_mode = (unsigned char)(f > 0 ? 1 : 0);
}

static void mradio_keep(t_mradio *x, t_floatarg f) {
  x->x_keep = (unsigned char)(f > 0 ? 1 : 0);
}

static void mradio_list(t_mradio *x, t_symbol *s, int argc, t_atom *argv) {
#if 1
  post("not implemented.");
  return;
#else
  (void)s; // silence -Wunused-parameter
  int n, v, i = 0;
  if (argc == 0)
    return;
  if (argc > x->x_n)
    n = x->x_n;
  if (argc <= x->x_n)
    n = argc;
  while (n--) {
    v = (IS_A_FLOAT(argv, i) && (atom_getintarg(i, argc, argv) != 0)) ? 1 : 0;
    x->x_matrix[i] = (t_float)v;
    i++;
  }
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
#endif
}

static void mradio_bang(t_mradio *x) {
#if 1
  post("not implemented.");
  return;
#else
  // FIXME:
  outlet_list(x->x_gui.x_obj.ob_outlet, 0, x->x_n, x->x_matrix);

  if (x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
    pd_list(x->x_gui.x_snd->s_thing, 0, x->x_n, x->x_matrix);
#endif
}

static void mradio_toggle_cell(t_mradio *x, t_floatarg row, t_floatarg col) {
  int i = (int)row;

  if (i < 0)
    i = 0;
  if (i >= x->x_n)
    i = x->x_n - 1;

  int j = (int)col;

  if (j < 0)
    j = 0;
  if (j >= x->x_m)
    j = x->x_m - 1;

  int idx = (i * x->x_m) + j;
  x->x_matrix[idx] = (x->x_matrix[idx] == 1) ? 0 : 1;
  post("row,col = (%d,%d)\nmtx[%d] = %d\n", i, j, idx, x->x_matrix[idx]);
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void mradio_float(t_mradio *x, t_floatarg f) {
#if 1
  post("not implemented.");
  return;
#else
  int next_pos, i = (int)f;
  if (x->x_mode) {
    if (i < 0)
      i = 0;
    if (i >= x->x_n)
      i = x->x_n - 1;
    next_pos = i;

  } else {
    next_pos = ((unsigned int)(i < 0 ? -i : i)) % x->x_n;
  }

  // send it out the outlet
  outlet_float(x->x_gui.x_obj.ob_outlet, atom_getfloat(x->x_matrix + next_pos));
  // send it to the receiver
  if (x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing) {
    pd_float(x->x_gui.x_snd->s_thing, atom_getfloat(x->x_matrix + next_pos));
  }
  // update the focus index
  x->x_foc = next_pos;
  if (x->x_focflag && x->x_foc != x->x_old)
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);

  x->x_old = x->x_foc;
#endif
}

static void mradio_print(t_mradio *x) {
  post("------------------------[mradio info]------------------------");
  post("num: %d\ndrawn: %d\npos: %d\n val: %d\nfoc: %d\nold: %d\nfflag: "
       "%d\nkeep:%d",
       x->x_n, x->x_pos, x->x_val, x->x_foc, x->x_old, x->x_focflag, x->x_keep);
  // int i;
  // for(i=0; i<=x->x_n; i++) {
  //   post("onlist[%d]:%d", i,x->x_matrix[i]);
  // }

  // t_binbuf *b = (t_binbuf *)x->b_binbuf;
  // t_atom *vec;
  // int start, end, n, i = 0;
  // vec = binbuf_getvec(b);
  // n = binbuf_getnatom(b);

  // if (!text_nthline(n, vec, i, &start, &end)) {
  //   postatom(x->x_n, x->x_matrix);
  //   endpost();
  // } else {
  //   while (text_nthline(n, vec, i, &start, &end)) {
  //     int outc = end - start, k;
  //     t_atom *outv = FD_ALLOCA((outc) * sizeof(t_atom));
  //     for (k = 0; k < outc; k++) {
  //       outv[k] = vec[start + k];
  //     }
  //     postatom(outc, outv);
  //     endpost();
  //     i++;
  //   }
  // }
}

static void mradio_click(t_mradio *x, t_floatarg xpos, t_floatarg ypos,
                         t_floatarg shift, t_floatarg ctrl, t_floatarg alt) {
  (void)shift; // silence -Wunused-parameter
  (void)ctrl;  // silence -Wunused-parameter
  (void)alt;   // silence -Wunused-parameter

  int row;
  int yy = (int)ypos - (int)text_ypix(&x->x_gui.x_obj, x->x_gui.x_glist);
  row = yy / (float)x->x_gui.x_h;
  if (row >= x->x_m)
    row = x->x_m - 1;
  if (row < 0)
    row = 0;

  int col;
  int xx = (int)xpos - (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
  col = xx / (float)x->x_gui.x_w;
  if (col >= x->x_n)
    col = x->x_n - 1;
  if (col < 0)
    col = 0;

  post("row,col = (%d,%d)\nxx,yy = (%d,%d)\n", row, col, xx, yy);
  mradio_toggle_cell(x, (t_float)row, (t_float)col);
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
    for (int i = 0; i < x->x_n; ++i) {
      for (int j = 0; j < x->x_m; ++j) {
        int idx = i * x->x_m + j;
        sprintf(tag, "%pFOC%d", x, idx);
        pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline",
                    THISGUI->i_backgroundcolor);
      }
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

static void mradio_resize(t_mradio *x, t_floatarg nsize, t_floatarg msize) {
  int n = (int)nsize;
  int m = (int)msize;

  if (n < 1)
    n = 1;

  if (m < 1)
    m = 1;

  if (n > IEM_RADIO_MAX)
    n = IEM_RADIO_MAX;

  if (m > IEM_RADIO_MAX)
    m = IEM_RADIO_MAX;

  if (n != x->x_n || m != x->x_m) {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    mradio_doresize(x, n, m);
    x->x_n = n;
    x->x_m = m;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  }
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

static void mradio_clear(t_mradio *x) {

  for (int i = 0; i < x->x_n; ++i)
    for (int j = 0; j < x->x_m; ++j)
      x->x_matrix[i * x->x_m + j] = 0;

  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void mradio_free(t_mradio *x) {
  freebytes(x->x_matrix, (size_t)sizeof(t_float) * x->x_n * x->x_m);
  iemgui_free((t_iemgui *)x);
}

static void *mradio_donew(t_symbol *s, int argc, t_atom *argv, int old) {
  (void)old;
  t_mradio *x = (t_mradio *)iemgui_new(mradio_class);

  int ldx = 0, ldy = -8 * IEM_GUI_DEFAULTSIZE_SCALE;
  int keep = 1, num = 8, a = IEM_GUI_DEFAULTSIZE;
  int fs = x->x_gui.x_fontsize;

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

  // TODO: add this to args
  x->x_mode = 0;
  // Initialize internal memory
  x->x_n = 0; // bash this to zero to ensure initialization
  mradio_doresize(x, num, num);

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
      class_new(gensym("mradio"), (t_newmethod)mradio_new,
                (t_method)mradio_free, sizeof(t_mradio), 0, A_GIMME, 0);

  class_addbang(mradio_class, mradio_bang);
  class_addfloat(mradio_class, mradio_float);
  class_addlist(mradio_class, mradio_list);
  class_addmethod(mradio_class, (t_method)mradio_click, gensym("click"),
                  A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_dialog, gensym("dialog"),
                  A_GIMME, 0);
  class_addmethod(mradio_class, (t_method)mradio_clear, gensym("clear"),
                  A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_focus, gensym("focus"),
                  A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_loadbang, gensym("loadbang"),
                  A_DEFFLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_keep, gensym("keep"), A_FLOAT,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_mode, gensym("mode"), A_FLOAT,
                  0);
  class_addmethod(mradio_class, (t_method)mradio_step, gensym("step"), A_NULL);
  class_addmethod(mradio_class, (t_method)mradio_next, gensym("next"), A_NULL);
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
  class_addmethod(mradio_class, (t_method)mradio_resize, gensym("number"),
                  A_FLOAT, A_FLOAT, 0);
  class_addmethod(mradio_class, (t_method)mradio_resize, gensym("resize"),
                  A_FLOAT, A_FLOAT, 0);
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
