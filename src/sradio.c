/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "fdLib.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ctype.h>

#include "m_imp.h"
#include "s_stuff.h"    /* just for sys_hostfontsize, phooey */
#include "g_all_guis.h"


#ifdef _WIN32
#include <io.h>
#endif

static t_class *sradio_class;

typedef struct _textbuf
{
    t_object b_ob;
    t_binbuf *b_binbuf;
    t_canvas *b_canvas;
    t_guiconnect *b_guiconnect;
    int b_status;
} t_textbuf;

/*This should be in g_all_guis.h*/
typedef struct _sradio
{
  t_iemgui  x_gui;
  t_textbuf x_textbuf;
  int       *x_onlist;
  int       *x_drawnlist;
  t_atom    *x_listout;
  t_symbol  *x_bindsym;
  int        x_number, x_drawn, x_pos, x_val, x_foc, x_old, x_focflag;
  unsigned char x_keep;
} t_sradio;

/*--------Shamelessly Taken from x_text--------*/

/* --- common code for text define, textfile, and qlist for storing text -- */

static void textbuf_senditup(t_textbuf *x)
{
  if (!x->b_guiconnect)
    return;
  pdgui_vmess("pdtk_textwindow_clear", "^", x);
  pdgui_vmess("pdtk_textwindow_appendatoms", "^A",
      x, binbuf_getnatom(x->b_binbuf), binbuf_getvec(x->b_binbuf));
  pdgui_vmess("pdtk_textwindow_setdirty", "^i", x, 0);
}

static void textbuf_read(t_sradio *z, t_symbol *s, int argc, t_atom *argv)
{
  (void)s;
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  int cr = 0;
  t_symbol *filename;
  while (argc && argv->a_type == A_SYMBOL &&
      *argv->a_w.w_symbol->s_name == '-')
  {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-c"))
      cr = 1;
    else
    {
        pd_error(x, "text read: unknown flag ...");
        postatom(argc, argv); endpost();
    }
    argc--; argv++;
  }
  if (argc && argv->a_type == A_SYMBOL)
  {
    filename = argv->a_w.w_symbol;
    argc--; argv++;
  }
  else
  {
    pd_error(x, "text read: no file name given");
    return;
  }
  if (argc)
  {
    post("warning: text define ignoring extra argument: ");
    postatom(argc, argv);
    endpost();
  }
  if (binbuf_read_via_canvas(x->b_binbuf, filename->s_name, x->b_canvas, cr))
    pd_error(x, "%s: read failed", filename->s_name);
  textbuf_senditup(x);
}

static void textbuf_write(t_sradio *z, t_symbol *s, int argc, t_atom *argv)
{
  (void)s;
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  int cr = 0;
  t_symbol *filename;
  char buf[MAXPDSTRING];
  while (argc && argv->a_type == A_SYMBOL &&
      *argv->a_w.w_symbol->s_name == '-')
  {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-c"))
      cr = 1;
    else
    {
      pd_error(x, "text write: unknown flag ...");
      postatom(argc, argv); endpost();
    }
    argc--; argv++;
  }
  if (argc && argv->a_type == A_SYMBOL)
  {
    filename = argv->a_w.w_symbol;
    argc--; argv++;
  }
  else
  {
    pd_error(x, "text write: no file name given");
    return;
  }
  if (argc)
  {
    post("warning: text define ignoring extra argument: ");
    postatom(argc, argv); endpost();
  }

  canvas_makefilename(x->b_canvas, filename->s_name,
                      buf, MAXPDSTRING);
  if (binbuf_write(x->b_binbuf, buf, "", cr))
    pd_error(x, "%s: write failed", filename->s_name);
}

static void textbuf_free(t_textbuf *x)
{
  t_pd *x2;
  binbuf_free(x->b_binbuf);
  if (x->b_guiconnect)
  {
    pdgui_vmess("destroy", "^", x);
    guiconnect_notarget(x->b_guiconnect, 1000);
  }
  /* just in case we're still bound to #A from loading... */
  while ((x2 = pd_findbyclass(gensym("#A"), sradio_class)))
    pd_unbind(x2, gensym("#A"));
}

/* random helper function */
static int text_nthline(int n, t_atom *vec, int line, int *startp, int *endp)
{
  int i, cnt = 0;
  for (i = 0; i < n; i++)
  {
    if (cnt == line)
    {
      int j=i;
      while (j < n && vec[j].a_type != A_SEMI 
        && vec[j].a_type != A_COMMA)
          j++;
      *startp = i;
      *endp = j;
      return (1);
    }
    else if (vec[i].a_type == A_SEMI || vec[i].a_type == A_COMMA)
      cnt++;
  }
  return (0);
}

static void textbuf_open(t_sradio *z)
{
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  x->b_status = 1;
  if (x->b_guiconnect)
  {
    char textid[128];
    sprintf(textid, ".x%lx.text", (unsigned long)z);
    pdgui_vmess("wm", "r^", "deiconify", z);
    pdgui_vmess("raise", "^", z);
    pdgui_vmess("focus", "s", textid);
  }
  else
  {
    char buf[40];
    sprintf(buf, "%dx%d", 600, 340);
    pdgui_vmess("pdtk_textwindow_open", "^r si", z, buf,
        "text", sys_hostfontsize(glist_getfont(x->b_canvas),
            glist_getzoom(x->b_canvas)));
    sprintf(buf, ".x%lx", (unsigned long)z);
    x->b_guiconnect = guiconnect_new(&x->b_ob.ob_pd, gensym(buf));
    textbuf_senditup(x);
  }
}

static void textbuf_close(t_sradio *z)
{
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  if(x->b_status)
  {
    pdgui_vmess("pdtk_textwindow_doclose", "^", z);
    if (x->b_guiconnect)
    {
      guiconnect_notarget(x->b_guiconnect, 1000);
      x->b_guiconnect = 0;
      x->b_status = 0;
    }
  }
}

/*--------End from x_text--------*/

static void sradio_resizer(t_sradio *x, int nsize)
{
  int i,j;
  int size = (int)nsize;//fd:NEVER USE VALUES DIRECTLY FROM FUNCTION ARGS!
  freebytes(x->x_onlist, sizeof(int));
  freebytes(x->x_drawnlist, sizeof(int));
  freebytes(x->x_listout, sizeof(t_atom));
  x->x_onlist = (int *)getbytes(size * sizeof(int));
  x->x_drawnlist = (int *)getbytes(size * sizeof(int));
  x->x_listout = (t_atom *)getbytes(size * sizeof(t_atom));
  for (i=0,j=0;i<=size-1;i++,j++)
  {
    x->x_onlist[i] = 0;
    x->x_drawnlist[j] = 0;
    SETFLOAT(&x->x_listout[i], 0.0);
  }
}

/*Shamelessly taken from hradio*/


static t_widgetbehavior sradio_widgetbehavior;

/* widget helper functions */

static void sradio_draw_update(t_gobj *client, t_glist *glist)
{
  t_sradio *x = (t_sradio *)client;
  int n=x->x_number,i,j;
  int val = x->x_foc;
  // post("before update");
  if(glist_isvisible(glist))
  {
    t_canvas *canvas=glist_getcanvas(glist);
    char tag[128];
    for (i=0,j=0;i<n;i++,j++)
    {
      sprintf(tag, "%pFOC%d", x, i);
      pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag,
        "-outline", (x->x_focflag&&i==val)?x->x_gui.x_fcol:x->x_gui.x_bcol);
      // post("onlist %d : %d",i,x->x_onlist[i]);
      sprintf(tag, "%pBUT%d", x, i);
      const int col = (x->x_onlist[i] ? x->x_gui.x_fcol : x->x_gui.x_bcol );
      pdgui_vmess(0, "crs rk rk", canvas, "itemconfigure", tag,
          "-fill", col,
          "-outline", col);
      x->x_drawnlist[i] = x->x_onlist[j];
    }
  }
}

static void sradio_draw_new(t_sradio *x, t_glist *glist)
{
    t_canvas *canvas = glist_getcanvas(glist);
    int i;
    char tag_n[128], tag[128], tag_object[128];
    char *tags[] = {tag_object, tag, tag_n, "text"};
    sprintf(tag_object, "%pOBJ", x);

    for(i=0; i<x->x_number; i++) {
        sprintf(tag, "%pBASE", x);
        sprintf(tag_n, "%pBASE%d", x, i);
        pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle",
            0, 0, 0, 0, "-tags", 3, tags);

        sprintf(tag, "%pBUT", x);
        sprintf(tag_n, "%pBUT%d", x, i);
        pdgui_vmess(0, "crr iiii rS", canvas, "create", "oval",
            0, 0, 0, 0, "-tags", 3, tags);

        sprintf(tag, "%pFOC", x);
        sprintf(tag_n, "%pFOC%d", x, i);
        pdgui_vmess(0, "crr iiii rS", canvas, "create", "rectangle",
            0, 0, 0, 0, "-tags", 3, tags);
        x->x_drawnlist[i] = x->x_onlist[i];
    }
    /* make sure the buttons are above their base */
    sprintf(tag, "%pBUT", x);
    sprintf(tag_n, "%pBASE", x);
    pdgui_vmess(0, "crss", canvas, "raise", tag, tag_n);

    sprintf(tag, "%pLABEL", x);
    sprintf(tag_n, "label");
    pdgui_vmess(0, "crr ii rs rS", canvas, "create", "text",
        0, 0, "-anchor", "w", "-tags", 4, tags);

    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO);

}

static void sradio_draw_config(t_sradio *x, t_glist *glist)
{
  t_canvas *canvas = glist_getcanvas(glist);
  t_iemgui *iemgui = &x->x_gui;
  int i,n   =  x->x_number;
  int dx    =  x->x_gui.x_w;
  int s4    =  dx/4;
  int s6    =  dx/6;
  int yy11  =  text_ypix(&x->x_gui.x_obj, glist);
  int xx11b =  text_xpix(&x->x_gui.x_obj, glist);
  int xx11  =  xx11b;
  int yy12  =  yy11  + dx;
  int yy21  =  yy11  + s4;
  int yy22  =  yy12  - s4;
  int xx21  =  xx11b + s4;
  int xx22  =  xx11b + dx - s4;
  int xx31  =  xx11b + s6;
  int yy31  =  yy11  + s6;
  int xx32  =  xx11b + dx - s6;
  int yy32  =  yy12  - s6;
  char tag[128];
  int zoom = IEMGUI_ZOOM(x);
  t_atom fontatoms[3];
  SETSYMBOL(fontatoms+0, gensym(iemgui->x_font));
  SETFLOAT (fontatoms+1, -iemgui->x_fontsize*zoom);
  SETSYMBOL(fontatoms+2, gensym(sys_fontweight));
  for(i=0; i<n; i++)
  {
    unsigned int col = x->x_onlist[i] ? x->x_gui.x_fcol : x->x_gui.x_bcol;
    //  base
    sprintf(tag, "%pBASE%d", x, i);
    pdgui_vmess(0, "crs iiii", canvas, "coords", tag,
        xx11, yy11, xx11+dx, yy12);
    pdgui_vmess(0, "crs ri rk rk", canvas, "itemconfigure", tag,
        "-width", zoom, "-fill", x->x_gui.x_bcol,
        "-outline", THISGUI->i_foregroundcolor);

    // oval (button)
    sprintf(tag, "%pBUT%d", x, i);
    pdgui_vmess(0, "crs iiii", canvas, "coords", tag,
        xx21, yy21, xx22, yy22);
    pdgui_vmess(0, "crs ri rk rk", canvas, "itemconfigure", tag,
        "-width", zoom, "-fill", col, "-outline", col);

    // foc (square)
    sprintf(tag, "%pFOC%d", x, i);
    pdgui_vmess(0, "crs iiii", canvas, "coords", tag,
        xx31, yy31, xx32, yy32);
    pdgui_vmess(0, "crs ri rk rk", canvas, "itemconfigure", tag,
        "-width", zoom, "-fill", col,
        "-outline", (x->x_foc==i)?x->x_gui.x_fcol:x->x_gui.x_bcol);
    xx11 += dx;
    xx21 += dx;
    xx22 += dx;
    xx31 += dx;
    xx32 += dx;
    x->x_drawnlist[i] = x->x_onlist[i];
  }

    sprintf(tag, "%pLABEL", x);
    pdgui_vmess(0, "crs ii", canvas, "coords", tag,
        xx11b + x->x_gui.x_ldx * zoom, yy11+dx + x->x_gui.x_ldy * zoom);
    pdgui_vmess(0, "crs rA rk", canvas, "itemconfigure", tag,
        "-font", 3, fontatoms,
        "-fill", x->x_gui.x_lcol);
    iemgui_dolabel(x, &x->x_gui, x->x_gui.x_lab, 1);
}

#define sradio_draw_io 0
static void sradio_draw_select(t_radio* x, t_glist* glist)
{
    t_canvas *canvas = glist_getcanvas(glist);
    char tag[128];
    unsigned int col = THISGUI->i_foregroundcolor, lcol = x->x_gui.x_lcol;

    if(x->x_gui.x_fsf.x_selected)
        lcol = col = THISGUI->i_selectcolor;

    sprintf(tag, "%pBASE", x);
    pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-outline", col);
    sprintf(tag, "%pLABEL", x);
    pdgui_vmess(0, "crs rk", canvas, "itemconfigure", tag, "-fill", lcol);
}

/* ------------------------ widgetbehaviour----------------------------- */

static void sradio_getrect(t_gobj *z, t_glist *glist, 
                            int *xp1, int *yp1, int *xp2, int *yp2)
{
  t_sradio *x = (t_sradio *)z;
  *xp1 = text_xpix(&x->x_gui.x_obj, glist);
  *yp1 = text_ypix(&x->x_gui.x_obj, glist);
  *xp2 = *xp1 + x->x_gui.x_w*x->x_number;
  *yp2 = *yp1 + x->x_gui.x_h;
}

static void sradio_save(t_gobj *z, t_binbuf *b)
{
  t_sradio *x  = (t_sradio *)z;
  t_binbuf *bb = (t_binbuf *) x->x_textbuf.b_binbuf;
  int n;
  t_symbol *bflcol[3];
  t_symbol *srl[3];
  iemgui_save(&x->x_gui, srl, bflcol);
  binbuf_addv(b, "ssiisiiisssiiiisss", 
    gensym("#X"), gensym("obj"),
    (int)x->x_gui.x_obj.te_xpix, (int)x->x_gui.x_obj.te_ypix,
    gensym("sradio"),
    x->x_gui.x_w, (int)x->x_keep, x->x_number,
    srl[0], srl[1], srl[2],
    x->x_gui.x_ldx, x->x_gui.x_ldy,
    iem_fstyletoint(&x->x_gui.x_fsf), x->x_gui.x_fontsize,
    bflcol[0], bflcol[1], bflcol[2]);
  binbuf_addv(b, ";");
  if (!bb)
    return;
  n = binbuf_getnatom(bb);
  if (x->x_keep && n)
  {
    binbuf_addv(b, "ss", gensym("#A"), gensym("preset"));
    binbuf_addbinbuf(b, x->x_textbuf.b_binbuf);
    binbuf_addsemi(b);
  }
  obj_saveformat(&x->x_gui.x_obj, b);
}

static void sradio_properties(t_gobj *z, t_glist *owner)
{
  (void)owner;
  t_sradio *x = (t_sradio *)z;
  iemgui_new_dialog(x, &x->x_gui, "sradio",
      (t_float)x->x_gui.x_w/IEMGUI_ZOOM(x), IEM_GUI_MINSIZE,
      0, 0,
      0, 0,
      0,
      -1, "new-only", "new&old",
      1, -1, x->x_number);
}

static void sradio_dialog(t_sradio *x, t_symbol *s, int argc, t_atom *argv) 
{
  (void)s;
  t_symbol *srl[3];
  int a = (int)atom_getfloatarg(0, argc, argv);
  int num = (int)atom_getfloatarg(6, argc, argv);
  int redraw = 0;
  t_atom undo[18];
  iemgui_setdialogatoms(&x->x_gui, 18, undo);
  SETFLOAT(undo+1, 0);
  SETFLOAT(undo+2, 0);
  SETFLOAT(undo+3, 0);
  SETFLOAT(undo+6, x->x_number);

  pd_undo_set_objectstate(x->x_gui.x_glist, (t_pd*)x, gensym("dialog"),
                          18, undo,
                          argc, argv);

  iemgui_dialog(&x->x_gui, srl, argc, argv);
  x->x_gui.x_w = iemgui_clip_size(a) * IEMGUI_ZOOM(x);
  x->x_gui.x_h = x->x_gui.x_w;
  if (num != x->x_number && glist_isvisible(x->x_gui.x_glist))
  {
      /* we need to recreate the buttons */
      (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
      redraw = 1;
  }
  x->x_number = num;

  if (redraw && gobj_shouldvis((t_gobj *)x, x->x_gui.x_glist))
  {
      sradio_resizer(x, num);
      (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
      canvas_fixlinesfor(x->x_gui.x_glist, (t_text*)x);
  } else {
      /* just reconfigure */
      iemgui_size((void *)x, &x->x_gui);
  }
}

static void sradio_keep(t_sradio *x, t_floatarg f)
{ x->x_keep = (unsigned char) (f>0?1:0); }

static void sradio_preset(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
  (void)s;
  if(!argc)
    return;
  binbuf_restore(x->x_textbuf.b_binbuf, argc, argv);
  textbuf_senditup(&x->x_textbuf);
}

//'addline' is needed because pdtk_textwindow uses that method
static void textbuf_addline(t_sradio *x, t_symbol *s, int argc, t_atom *argv) 
{
  (void)s;
  sradio_preset(x,gensym("preset"),argc,argv);
}

static void sradio_set(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
  (void)s;
  if(!argc)
    return;

  int n;
  if (argc > x->x_number && argc < IEM_RADIO_MAX)
    n = x->x_number;
  if (argc <= x->x_number)
    n = argc;
  int i = 0;
  while (n--)
  {
    if (!IS_A_FLOAT(argv, i))
      pd_error(x,"sradio_set: index %d is not a number", i);
    else
    {
      x->x_onlist[i] = !!(int)atom_getintarg(i,argc,argv);
      SETFLOAT(&x->x_listout[i], (t_float)x->x_onlist[i]);
    }
    i++;
  }
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void sradio_store(t_sradio *x, t_floatarg ff)
{
  t_binbuf *b = (t_binbuf *) x->x_textbuf.b_binbuf;
  int start, end, n, i, lineno;
  int f = (int)ff;
  t_atom *vec;
  if (!b)
    return;
  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);
  if (f && ((lineno = f<0?0:f)>=0) && text_nthline(n, vec, lineno, &start, &end))
  {
    // post("n:%d, lineno:%d,start:%d,end:%d",n,lineno,start,end);
    int oldn = n;
    n = n + (x->x_number - (end-start));
    if (n > oldn)
      (void)binbuf_resize(b, n);
    vec = binbuf_getvec(b);
    memmove(&vec[start + x->x_number], &vec[end], sizeof(*vec) * (oldn - end));
    if (n < oldn)
    {
      (void)binbuf_resize(b, n);
      vec = binbuf_getvec(b);
    }
    for (i=0; i<x->x_number; i++)
      vec[start+i] = x->x_listout[i];
  }
  else
  {
    binbuf_restore(x->x_textbuf.b_binbuf, x->x_number, x->x_listout);
    binbuf_addsemi(x->x_textbuf.b_binbuf);
    textbuf_senditup(&x->x_textbuf);
  }
}

static void sradio_recall(t_sradio *x, t_floatarg f)
{
  t_binbuf *b = (t_binbuf *) x->x_textbuf.b_binbuf;
  int start, end, n;
  t_atom *vec;
  if (!b)
    return;
  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);

  if (text_nthline(n, vec, f, &start, &end))
  {
    int outc = end - start, k;
    t_atom *outv = (t_atom*)getbytes(outc * sizeof(t_atom));
    for (k = 0; k < outc; k++)
      outv[k] = vec[start+k];
    sradio_set(x, gensym("set"),outc, outv);
    freebytes(outv, outc * sizeof(t_atom));
  }
}

static void sradio_flush(t_sradio *x, t_symbol *s)
{
  if(s->s_thing)
  {
    t_binbuf *b = (t_binbuf *) x->x_textbuf.b_binbuf;
    int start, end, n, i=0;
    t_atom *vec;
    if (!b)
      return;
    vec = binbuf_getvec(b);
    n   = binbuf_getnatom(b);
    while (text_nthline(n, vec, i, &start, &end))
    {
      int outc = end - start, k;
      t_atom *outv = (t_atom*)getbytes(outc * sizeof(t_atom));
      for (k = 0; k < outc; k++)
        outv[k] = vec[start+k];
      pd_list(s->s_thing, gensym("list"),outc, outv);
      freebytes(outv, outc * sizeof(t_atom));
      i++;
    }
  }
}

static void sradio_clear(t_sradio *x)
{
  int n = x->x_number;
  int i=0;
  while (n--)
  {
    x->x_onlist[i] = x->x_drawnlist[i]= 0;
    SETFLOAT(&x->x_listout[i], 0.0);
    i++;
  }
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
  binbuf_clear(x->x_textbuf.b_binbuf);
  textbuf_senditup(&x->x_textbuf);
}

static void sradio_bang(t_sradio *x)
{
  outlet_list(x->x_gui.x_obj.ob_outlet,0,x->x_number,x->x_listout);
  if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
    pd_list(x->x_gui.x_snd->s_thing, 0,x->x_number, x->x_listout);
}

static void sradio_fout(t_sradio *x, t_floatarg f)
{
  int i=(int)f;
  x->x_onlist[i] = !x->x_drawnlist[i];
  SETFLOAT(&x->x_listout[i], (t_float)x->x_onlist[i]);
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void sradio_float(t_sradio *x, t_floatarg f)
{
  int i=((int)f)%x->x_number;
  if(i < 0)
    i = 0;
  outlet_float(x->x_gui.x_obj.ob_outlet, x->x_onlist[i]);
  if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
    pd_float(x->x_gui.x_snd->s_thing, x->x_onlist[i]);
  if (x->x_focflag)
  {
    x->x_foc = i;
    if (x->x_foc != x->x_old)
      (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    x->x_old = x->x_foc;
  }
}

static void sradio_print(t_sradio *x)
{
  post("------------------------[sradio info]------------------------");
  post("num: %d\ndrawn: %d\npos: %d\n\
val: %d\nfoc: %d\nold: %d\nfflag: %d\nkeep:%d",
    x->x_number, x->x_drawn, x->x_pos, x->x_val, 
    x->x_foc, x->x_old, x->x_focflag, x->x_keep);
  // int i;
  // for(i=0; i<=x->x_number; i++) {
  //   post("onlist[%d]:%d", i,x->x_onlist[i]);
  // }
  // for(i=0; i<=x->x_number; i++) {
  //   post("drawnlist[%d]:%d", i,x->x_drawnlist[i]);
  // }

  t_binbuf *b = (t_binbuf *) x->x_textbuf.b_binbuf;
  t_atom *vec;
  int start, end, n, i=0;
  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);
  if (!text_nthline(n, vec, i, &start, &end))
  {
    postatom(x->x_number, x->x_listout);
    endpost();
  }
  else
  {
    while (text_nthline(n, vec, i, &start, &end))
    {
      int outc = end - start, k;
      t_atom *outv = (t_atom*)getbytes(outc * sizeof(t_atom));
      for (k = 0; k < outc; k++)
        outv[k] = vec[start+k];
      postatom(outc, outv);
      endpost();
      freebytes(outv, outc * sizeof(t_atom));
      i++;
    }
  }
}

static void sradio_click(t_sradio *x,
  t_floatarg xpos, t_floatarg ypos,
  t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
  (void)ypos;
  (void)ctrl;
  (void)alt;
  int selected = 0.;
  int xx = (int)xpos - (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
  selected = xx / x->x_gui.x_w;
  if(selected >= x->x_number)
      selected = x->x_number-1;
  if(selected < 0)
      selected = 0;
  if (!shift)
    sradio_fout(x, (t_floatarg)selected);
  else
    textbuf_open(x);
}

static int sradio_newclick(t_gobj *z, struct _glist *glist,
 int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
  (void)glist;
  (void)dbl;
  if(doit)
    sradio_click((t_sradio *)z, 
      (t_floatarg)xpix, (t_floatarg)ypix, 
      (t_floatarg)shift, 0, (t_floatarg)alt);
  return (1);
}

static void sradio_focus(t_sradio *x, t_floatarg foc)
{
  x->x_focflag = !!(int)foc;
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void sradio_next(t_sradio *x)
{
  if(!x->x_focflag)
    sradio_focus(x,1.0);
  int pval = x->x_foc;
  int nval;
  pval+=1;
  nval = pval%x->x_number;
  sradio_float(x, (t_floatarg)nval);
}

static void sradio_prev(t_sradio *x)
{
  if(!x->x_focflag)
    sradio_focus(x,1.0);
  int pval = x->x_foc;
  int nval;
  pval-=1;
  nval = pval%x->x_number;
  sradio_float(x, (t_floatarg)nval);
}

static void sradio_number(t_sradio *x, t_floatarg num)
{
  int n=(int)num;
  if(n < 1)
    n = 1;
  if(n > IEM_RADIO_MAX)
    n = IEM_RADIO_MAX;
  if(n != x->x_number)
  {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    x->x_number = n;
    sradio_resizer(x,n);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  }
}

static void sradio_size(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  (void)s;
  x->x_gui.x_w = iemgui_clip_size((int)atom_getintarg(0, ac, av));
  x->x_gui.x_h = x->x_gui.x_w;
  iemgui_size((void *)x, &x->x_gui);
}

static void sradio_delta(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{ iemgui_delta((void *)x, &x->x_gui, s, ac, av); }

static void sradio_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{ iemgui_pos((void *)x, &x->x_gui, s, ac, av); }

static void sradio_color(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{ iemgui_color((void *)x, &x->x_gui, s, ac, av); }

static void sradio_send(t_sradio *x, t_symbol *s)
{ iemgui_send(x, &x->x_gui, s); }

static void sradio_receive(t_sradio *x, t_symbol *s)
{ iemgui_receive(x, &x->x_gui, s); }

static void sradio_label(t_sradio *x, t_symbol *s)
{ iemgui_label((void *)x, &x->x_gui, s); }

static void sradio_label_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{ iemgui_label_pos((void *)x, &x->x_gui, s, ac, av); }

static void sradio_label_font(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{ iemgui_label_font((void *)x, &x->x_gui, s, ac, av); }

static void *sradio_new(t_symbol *s, int argc, t_atom *argv)
{
  (void)s;
  t_sradio *x = (t_sradio *)iemgui_new(sradio_class);
  int ldx     =  0;
  int ldy     = -8;
  int keep    =  1;
  int num     =  8;
  int a       =  IEM_GUI_DEFAULTSIZE;
  int fs      =  10;
  t_symbol *asym = gensym("#A");

  IEMGUI_SETDRAWFUNCTIONS(x, sradio);

  //iem_inttosymargs(&x->x_gui.x_isa, 0);
  iem_inttofstyle(&x->x_gui.x_fsf, 0);

  x->x_gui.x_bcol = 0xFCFCFC;
  x->x_gui.x_fcol = 0x00;
  x->x_gui.x_lcol = 0x00;

  if((argc == 13)&&IS_A_FLOAT(argv,0)&&IS_A_FLOAT(argv,1)&&IS_A_FLOAT(argv,2)
     &&(IS_A_SYMBOL(argv,3)||IS_A_FLOAT(argv,3))
     &&(IS_A_SYMBOL(argv,4)||IS_A_FLOAT(argv,4))
     &&(IS_A_SYMBOL(argv,5)||IS_A_FLOAT(argv,5))
     &&IS_A_FLOAT(argv,6)&&IS_A_FLOAT(argv,7)
     &&IS_A_FLOAT(argv,8)&&IS_A_FLOAT(argv,9))
  {
    a     = (int)atom_getintarg(0, argc, argv);
    keep  = (int)atom_getintarg(1, argc, argv);
    num   = (int)atom_getintarg(2, argc, argv);
    iemgui_new_getnames(&x->x_gui, 3, argv);
    ldx   = (int)atom_getintarg(6, argc, argv);
    ldy   = (int)atom_getintarg(7, argc, argv);
    iem_inttofstyle(&x->x_gui.x_fsf, atom_getintarg(8, argc, argv));
    fs    = (int)atom_getintarg(9, argc, argv);
    iemgui_all_loadcolors(&x->x_gui, argv+10, argv+11, argv+12);
  }
  else iemgui_new_getnames(&x->x_gui, 3, 0);

  x->x_gui.x_fsf.x_snd_able = (0 != x->x_gui.x_snd);
  x->x_gui.x_fsf.x_rcv_able = (0 != x->x_gui.x_rcv);
  if(x->x_gui.x_fsf.x_font_style == 1) strcpy(x->x_gui.x_font, "helvetica");
  else if(x->x_gui.x_fsf.x_font_style == 2) strcpy(x->x_gui.x_font, "times");
  else { x->x_gui.x_fsf.x_font_style = 0;
      strcpy(x->x_gui.x_font, sys_font); }

  x->x_keep = (unsigned char) keep;
  if(num < 1)
      num = 1;
  if(num > IEM_RADIO_MAX)
      num = IEM_RADIO_MAX;
  x->x_number = num;
  sradio_resizer(x,num);
  x->x_textbuf.b_binbuf = binbuf_new();
  x->x_textbuf.b_canvas = canvas_getcurrent();
  /* bashily unbind #A -- this would create garbage if #A were
  multiply bound but we believe in this context it's at most
  bound to whichever text_define or array was created most recently */
  asym->s_thing = 0;
  /* and now bind #A to us to receive following messages in the
  saved file or copy buffer */
  pd_bind(&x->x_gui.x_obj.ob_pd, asym);

  if (x->x_gui.x_fsf.x_rcv_able)
    pd_bind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
  x->x_gui.x_ldx = ldx;
  x->x_gui.x_ldy = ldy;
  if(fs < 4)
    fs = 4;
  x->x_gui.x_fontsize = fs;
  x->x_gui.x_w        = iemgui_clip_size(a);
  x->x_gui.x_h        = x->x_gui.x_w;
  iemgui_verify_snd_ne_rcv(&x->x_gui);

  outlet_new(&x->x_gui.x_obj, &s_list);

  return (x);
}

static void sradio_ff(t_sradio *x)
{
  if(x->x_gui.x_fsf.x_rcv_able)
    pd_unbind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
  textbuf_free(&x->x_textbuf);
  freebytes(x->x_onlist, sizeof(int));
  freebytes(x->x_drawnlist, sizeof(int));
  freebytes(x->x_listout, sizeof(t_atom));
  pdgui_stub_deleteforkey(x);
}

void g_sradio_setup(void)
{
  sradio_class = class_new(gensym("sradio"), (t_newmethod)sradio_new,
    (t_method)sradio_ff, sizeof(t_sradio), 0, A_GIMME, 0);

  class_addbang(sradio_class, sradio_bang);
  class_addfloat(sradio_class, sradio_float);
  class_addmethod(sradio_class, (t_method)sradio_click,
    gensym("click"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
  //class_addmethod(sradio_class, (t_method)sradio_motion, gensym("motion"),
          //A_FLOAT, A_FLOAT, 0);
  class_addmethod(sradio_class, (t_method)sradio_dialog, 
    gensym("dialog"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_clear,
    gensym("clear"), A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_focus, 
    gensym("focus"), A_FLOAT, 0);
  class_addmethod(sradio_class, (t_method)sradio_preset,
    gensym("preset"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_set,
    gensym("set"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_keep,
    gensym("keep"), A_FLOAT, 0);
  class_addmethod(sradio_class, (t_method)textbuf_open,
    gensym("open"), 0);
  class_addmethod(sradio_class, (t_method)textbuf_close,
    gensym("close"), 0);
  class_addmethod(sradio_class, (t_method)textbuf_addline,
    gensym("addline"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_next,
    gensym("next"), A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_store,
    gensym("store"), A_DEFFLOAT, A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_recall,
    gensym("recall"), A_DEFFLOAT, A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_flush,
    gensym("flush"), A_SYMBOL, A_NULL);
  class_addmethod(sradio_class, (t_method)textbuf_write,
    gensym("write"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)textbuf_read,
    gensym("read"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_prev,
    gensym("prev"), A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_size,
    gensym("size"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_delta,
    gensym("delta"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_pos,
    gensym("pos"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_color,
    gensym("color"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_send,
    gensym("send"), A_DEFSYM, 0);
  class_addmethod(sradio_class, (t_method)sradio_receive,
    gensym("receive"), A_DEFSYM, 0);
  class_addmethod(sradio_class, (t_method)sradio_label,
    gensym("label"), A_DEFSYM, 0);
  class_addmethod(sradio_class, (t_method)sradio_label_pos,
    gensym("label_pos"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_label_font,
    gensym("label_font"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_number,
    gensym("number"), A_FLOAT, 0);
  class_addmethod(sradio_class, (t_method)iemgui_zoom,
    gensym("zoom"), A_CANT, 0);
  class_addmethod(sradio_class, (t_method)sradio_print,
    gensym("print"), A_NULL);

  sradio_widgetbehavior.w_getrectfn  = sradio_getrect;
  sradio_widgetbehavior.w_displacefn = iemgui_displace;
  sradio_widgetbehavior.w_selectfn   = iemgui_select;
  sradio_widgetbehavior.w_activatefn = NULL;
  sradio_widgetbehavior.w_deletefn   = iemgui_delete;
  sradio_widgetbehavior.w_visfn      = iemgui_vis;
  sradio_widgetbehavior.w_clickfn    = sradio_newclick;
  class_setwidget(sradio_class, &sradio_widgetbehavior);
  class_sethelpsymbol(sradio_class, gensym("sradio"));
  class_setsavefn(sradio_class, sradio_save);
  class_setpropertiesfn(sradio_class, sradio_properties);

  post("[sradio] version 0.3 ~ fdch");

}
