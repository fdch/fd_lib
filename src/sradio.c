/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

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

#include "m_imp.h"
#include "s_stuff.h"    /* just for sys_hostfontsize, phooey */
#include "g_all_guis.h"


#ifdef _WIN32
#include <io.h>
#endif

static t_class *sradio_class;

#ifdef _WIN32
# include <malloc.h> /* MSVC or mingw on windows */
#elif defined(__linux__) || defined(__APPLE__)
# include <alloca.h> /* linux, mac, mingw, cygwin */
#else
# include <stdlib.h> /* BSDs for example */
#endif

#ifndef HAVE_ALLOCA     /* can work without alloca() but we never need it */
#define HAVE_ALLOCA 1
#endif
#define TEXT_NGETBYTE 100 /* bigger that this we use alloc, not alloca */
#if HAVE_ALLOCA
#define ATOMS_ALLOCA(x, n) ((x) = (t_atom *)((n) < TEXT_NGETBYTE ?  \
        alloca((n) * sizeof(t_atom)) : getbytes((n) * sizeof(t_atom))))
#define ATOMS_FREEA(x, n) ( \
    ((n) < TEXT_NGETBYTE || (freebytes((x), (n) * sizeof(t_atom)), 0)))
#else
#define ATOMS_ALLOCA(x, n) ((x) = (t_atom *)getbytes((n) * sizeof(t_atom)))
#define ATOMS_FREEA(x, n) (freebytes((x), (n) * sizeof(t_atom)))
#endif

/* Default settings */

#define DEF_KEEP 1
#define DEF_NUMBER 8
#define DEF_FONTSIZE 10

typedef struct _textbuf
{
    t_object b_ob;
    t_binbuf *b_binbuf;
    t_canvas *b_canvas;
    t_guiconnect *b_guiconnect;
    t_int b_status;
} t_textbuf;

/*This should be in g_all_guis.h*/
typedef struct _sradio
{
  t_iemgui  x_gui;
  t_textbuf x_textbuf;
  t_int    *x_onlist;
  t_int    *x_drawnlist;
  t_atom   *x_listout;
  t_symbol *x_bindsym;
  int    x_number, x_drawn, x_pos, x_val, x_foc, x_old, x_focflag;
  unsigned char x_keep;
} t_sradio;

/*--------Shamelessly Taken from x_text--------*/

/* --- common code for text define, textfile, and qlist for storing text -- */


static void textbuf_init(t_textbuf *x)
{
  x->b_binbuf = binbuf_new();
  x->b_canvas = canvas_getcurrent();
}

static void textbuf_senditup(t_textbuf *x)
{
  int i, ntxt;
  char *txt;
  if (!x->b_guiconnect)
  {
    return;
  }
  binbuf_gettext(x->b_binbuf, &txt, &ntxt);
  sys_vgui("pdtk_textwindow_clear .x%lx\n", x);
  for (i = 0; i < ntxt; )
  {
    char *j = strchr(txt+i, '\n');
    if (!j)
    {
      j = txt + ntxt;
    }
    sys_vgui("pdtk_textwindow_append .x%lx {%.*s\n}\n",
        x, j-txt-i, txt+i);
    // post("%.*s",j-txt-i, txt+i);
    i = (int)((j-txt)+1);
  }
  sys_vgui("pdtk_textwindow_setdirty .x%lx 0\n", x);
  t_freebytes(txt, ntxt);
}

static void textbuf_read(t_sradio *z, t_symbol *s, int argc, t_atom *argv)
{  
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  int cr = 0;
  t_symbol *filename;
  while (argc && argv->a_type == A_SYMBOL &&
      *argv->a_w.w_symbol->s_name == '-')
  {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-c"))
    {
      cr = 1;
    }
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
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  int cr = 0;
  t_symbol *filename;
  char buf[MAXPDSTRING];
  while (argc && argv->a_type == A_SYMBOL &&
      *argv->a_w.w_symbol->s_name == '-')
  {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-c"))
    {
      cr = 1;
    }
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

  canvas_makefilename(x->b_canvas, filename->s_name, buf, MAXPDSTRING);

  if (binbuf_write(x->b_binbuf, buf, "", cr))
    pd_error(x, "%s: write failed", filename->s_name);
}

static void textbuf_free(t_textbuf *x)
{
  t_pd *x2;
  binbuf_free(x->b_binbuf);
  if (x->b_guiconnect)
  {
    sys_vgui("destroy .x%lx\n", x);
    guiconnect_notarget(x->b_guiconnect, 1000);
  }
  
  /* just in case we're still bound to #A from loading... */
  while ((x2 = pd_findbyclass(gensym("#A"), sradio_class)))
  {
    pd_unbind(x2, gensym("#A"));
  }
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
      while (j < n && vec[j].a_type != A_SEMI && vec[j].a_type != A_COMMA)
      {
        j++;
      }
      *startp = i;
      *endp = j;
      return (1);
    }
    else if (vec[i].a_type == A_SEMI || vec[i].a_type == A_COMMA)
    {
      cnt++;
    }
  }
  return (0);
}

/*--------End from x_text--------*/

static void textbuf_open(t_sradio *z)
{
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  x->b_status = 1;
  if (x->b_guiconnect)
  {
    sys_vgui("wm deiconify .x%lx\n", x);
    sys_vgui("raise .x%lx\n", x);
    sys_vgui("focus .x%lx.text\n", x);
  }
  else
  {
    char buf[40];
    sys_vgui("pdtk_textwindow_open .x%lx %dx%d {%s} %d\n", x, 200, 340,"text",
      sys_hostfontsize(glist_getfont(x->b_canvas),glist_getzoom(x->b_canvas)));
    sprintf(buf, ".x%lx", (unsigned long)x);
    x->b_guiconnect = guiconnect_new(&z->x_gui.x_obj.ob_pd, gensym(buf));
    textbuf_senditup(x);
  }
}

static void textbuf_close(t_sradio *z)
{
  t_textbuf *x = (t_textbuf *)&z->x_textbuf;
  if(x->b_status)
  {
    sys_vgui("pdtk_textwindow_doclose .x%lx\n", x);
    if (x->b_guiconnect)
    {
      guiconnect_notarget(x->b_guiconnect, 1000);
      x->b_guiconnect = 0;
      x->b_status = 0;
    }
  }
}


static void sradio_resizer(t_sradio *x, int nsize)
{
  int i,j;
  int size = (int)nsize;//fd:NEVER USE VALUES DIRECTLY FROM FUNCTION ARGS!
  freebytes(x->x_onlist,    sizeof(&x->x_onlist));
  freebytes(x->x_drawnlist, sizeof(&x->x_drawnlist));
  freebytes(x->x_listout,   sizeof(&x->x_listout));
  x->x_onlist    = (t_int  *)malloc(size * sizeof(t_int));
  x->x_drawnlist = (t_int  *)malloc(size * sizeof(t_int));
  x->x_listout   = (t_atom *)malloc(size * sizeof(t_atom));
  for (i=0,j=0;i<=size-1;i++,j++)
  {
    x->x_onlist[i]    = 0;
    x->x_drawnlist[j] = 0;
    SETFLOAT(&x->x_listout[i], 0);
  }
}

/*Shamelessly taken from hradio*/


static t_widgetbehavior sradio_widgetbehavior;

/* widget helper functions */

static void sradio_draw_update(t_gobj *client, t_glist *glist)
{
  t_sradio *x = (t_sradio *)client;
  int n=x->x_number,i=0,j=0;
  int val = x->x_foc;
  // post("before update");
  if(glist_isvisible(glist))
  { 
    t_canvas *canvas=glist_getcanvas(glist);
    while (n--)
    {
      if (x->x_focflag)
      {
        sys_vgui(".x%lx.c itemconfigure %lxFOC%d -outline #%06x\n",
          canvas, x, i, (i==val)?x->x_gui.x_fcol:x->x_gui.x_bcol);
      }
      // post("onlist %d : %d",i,x->x_onlist[i]);
      if (x->x_onlist[i])
      {
        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%06x -outline #%06x\n",
          canvas, x, i, x->x_gui.x_fcol, x->x_gui.x_fcol);
      }
      else
      {
        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%06x -outline #%06x\n",
          canvas, x, i, x->x_gui.x_bcol, x->x_gui.x_bcol);
      }
      x->x_drawnlist[i++] = x->x_onlist[j++];
    }
  }
}


static void sradio_draw_new(t_sradio *x, t_glist *glist)
{
  t_canvas *canvas = glist_getcanvas(glist);
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
  
  int zoomlabel = 
    1 + (IEMGUI_ZOOM(x)-1) * (x->x_gui.x_ldx >= 0 && x->x_gui.x_ldy >= 0);
  
  for(i=0; i<n; i++)
  {
    sys_vgui(".x%lx.c create rectangle %d %d %d %d \
      -width %d -fill #%06x -tags %lxBASE%d\n",
      canvas, xx11, yy11, xx11+dx, yy12, IEMGUI_ZOOM(x),
      x->x_gui.x_bcol, x, i);
    sys_vgui(".x%lx.c create oval %d %d %d %d \
      -fill #%06x -outline #%06x -tags %lxBUT%d\n",
      canvas, xx21, yy21, xx22, yy22,
      (x->x_onlist[i]==1)?x->x_gui.x_fcol:x->x_gui.x_bcol,
      (x->x_onlist[i]==1)?x->x_gui.x_fcol:x->x_gui.x_bcol, x, i);
    sys_vgui(".x%lx.c create rectangle %d %d %d %d \
      -outline #%06x -tags %lxFOC%d\n",
      canvas, xx31, yy31, xx32, yy32,
      (x->x_foc==i)?x->x_gui.x_fcol:x->x_gui.x_bcol, x, i);
    xx11 += dx;
    xx21 += dx;
    xx22 += dx;
    xx31 += dx;
    xx32 += dx;
    x->x_drawnlist[i] = x->x_onlist[i];
  }

  sys_vgui(".x%lx.c create text %d %d -text {%s} -anchor w \
    -font {{%s} -%d %s} -fill #%06x -tags [list %lxLABEL label text]\n",
    canvas, xx11b+x->x_gui.x_ldx * zoomlabel,
    yy11+x->x_gui.x_ldy * zoomlabel,
    strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"",
    x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
    x->x_gui.x_lcol, x);

  if(!x->x_gui.x_fsf.x_snd_able)
  {
    sys_vgui(".x%lx.c create rectangle %d %d %d %d \
      -fill black -tags [list %lxOUT%d outlet]\n",
      canvas, xx11b, yy12+1-2*IEMGUI_ZOOM(x), xx11b + IOWIDTH, yy12, x, 0);
  }

  if(!x->x_gui.x_fsf.x_rcv_able)
  {
    sys_vgui(".x%lx.c create rectangle %d %d %d %d \
      -fill black -tags [list %lxIN%d inlet]\n",
      canvas, xx11b, yy11, xx11b + IOWIDTH, yy11-1+2*IEMGUI_ZOOM(x), x, 0);
  }
}


static void sradio_draw_move(t_sradio *x, t_glist *glist)
{
  t_canvas *canvas = glist_getcanvas(glist);
  int n=x->x_number, i, dx=x->x_gui.x_w, s4=dx/4, s6=dx/6;
  int yy11  = text_ypix(&x->x_gui.x_obj, glist);
  int yy12  = yy11+dx;
  int yy21  = yy11+s4;
  int yy22  = yy12-s4;
  int xx11b = text_xpix(&x->x_gui.x_obj, glist);
  int xx11  = xx11b;
  int xx21  = xx11b+s4;
  int xx22  = xx11b+dx-s4;
  int xx31  = xx11b+s6;
  int yy31  = yy11+s6;
  int xx32  = xx11b+dx-s6;
  int yy32  = yy12-s6;
  int zoomlabel =
    1 + (IEMGUI_ZOOM(x)-1) * (x->x_gui.x_ldx >= 0 && x->x_gui.x_ldy >= 0);

  xx11 = xx11b;
  xx21 = xx11b+s4;
  xx22 = xx11b+dx-s4;
  for(i=0; i<n; i++)
  {
    sys_vgui(".x%lx.c coords %lxBASE%d %d %d %d %d\n",
      canvas, x, i, xx11, yy11, xx11+dx, yy12);
    sys_vgui(".x%lx.c coords %lxBUT%d %d %d %d %d\n",
      canvas, x, i, xx21, yy21, xx22, yy22);
    sys_vgui(".x%lx.c coords %lxFOC%d %d %d %d %d\n",
      canvas, x, i, xx31, yy31, xx32, yy32);
    xx11 += dx;
    xx21 += dx;
    xx22 += dx;
    xx31 += dx;
    xx32 += dx;
  }
  sys_vgui(".x%lx.c coords %lxLABEL %d %d\n",
    canvas, x, xx11b+x->x_gui.x_ldx * zoomlabel,
    yy11+x->x_gui.x_ldy * zoomlabel);
  if(!x->x_gui.x_fsf.x_snd_able)
  {
    sys_vgui(".x%lx.c coords %lxOUT%d %d %d %d %d\n",
      canvas, x, 0, xx11b, yy12+1-2*IEMGUI_ZOOM(x), xx11b + IOWIDTH, yy12);
  }
  if(!x->x_gui.x_fsf.x_rcv_able)
  {
    sys_vgui(".x%lx.c coords %lxIN%d %d %d %d %d\n",
      canvas, x, 0, xx11b, yy11, xx11b + IOWIDTH, yy11-1+2*IEMGUI_ZOOM(x));
  }
}


static void sradio_draw_erase(t_sradio* x, t_glist* glist)
{
  t_canvas *canvas = glist_getcanvas(glist);
  int n=x->x_number, i;

  for(i=0; i<n; i++)
  {
    sys_vgui(".x%lx.c delete %lxBASE%d\n", canvas, x, i);
    sys_vgui(".x%lx.c delete %lxBUT%d\n", canvas, x, i);
    sys_vgui(".x%lx.c delete %lxFOC%d\n", canvas, x, i);
  }
  sys_vgui(".x%lx.c delete %lxLABEL\n", canvas, x);
  if(!x->x_gui.x_fsf.x_snd_able)
  {
    sys_vgui(".x%lx.c delete %lxOUT%d\n", canvas, x, 0);
  }
  if(!x->x_gui.x_fsf.x_rcv_able)
  {
    sys_vgui(".x%lx.c delete %lxIN%d\n", canvas, x, 0);
  }
}


static void sradio_draw_config(t_sradio* x, t_glist* glist)
{
  t_canvas *canvas = glist_getcanvas(glist);
  int n=x->x_number, i;

  sys_vgui(".x%lx.c itemconfigure %lxLABEL -font {{%s} -%d %s} \
    -fill #%06x -text {%s} \n",
    canvas, x, x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
    x->x_gui.x_fsf.x_selected?IEM_GUI_COLOR_SELECTED:x->x_gui.x_lcol,
    strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"");
  for(i=0; i<n; i++)
  {
    sys_vgui(".x%lx.c itemconfigure %lxBASE%d -fill #%06x\n", canvas, x, i,
      x->x_gui.x_bcol);
    sys_vgui(".x%lx.c itemconfigure %lxBUT%d \
      -fill #%06x -outline #%06x\n", canvas, x, i,
      (x->x_onlist[i]==1)?x->x_gui.x_fcol:x->x_gui.x_bcol,
      (x->x_onlist[i]==1)?x->x_gui.x_fcol:x->x_gui.x_bcol);
    sys_vgui(".x%lx.c itemconfigure %lxFOC%d -outline #%06x\n", canvas, x, i,
      (x->x_foc==i)?x->x_gui.x_fcol:x->x_gui.x_bcol);
  }
}


static void sradio_draw_io(t_sradio* x, t_glist* glist, int old_snd_rcv_flags)
{
  t_canvas *canvas = glist_getcanvas(glist);
  int xpos = text_xpix(&x->x_gui.x_obj, glist);
  int ypos = text_ypix(&x->x_gui.x_obj, glist);

  if((old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && !x->x_gui.x_fsf.x_snd_able)
  {
    sys_vgui(".x%lx.c create rectangle %d %d %d %d -tags %lxOUT%d\n",
      canvas,
      xpos, ypos + x->x_gui.x_w-1,
      xpos + IOWIDTH, ypos + x->x_gui.x_w,
      x, 0);
  }
  if(!(old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && x->x_gui.x_fsf.x_snd_able)
  {
    sys_vgui(".x%lx.c delete %lxOUT%d\n", canvas, x, 0);
  }
  if((old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && !x->x_gui.x_fsf.x_rcv_able)
  {
    sys_vgui(".x%lx.c create rectangle %d %d %d %d -tags %lxIN%d\n",
      canvas,
      xpos, ypos,
      xpos + IOWIDTH, ypos+1, x, 0);
  }
  if(!(old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && x->x_gui.x_fsf.x_rcv_able)
  {
    sys_vgui(".x%lx.c delete %lxIN%d\n", canvas, x, 0);
  }
}


static void sradio_draw_select(t_sradio* x, t_glist* glist)
{
  t_canvas *canvas = glist_getcanvas(glist);
  int n=x->x_number, i;
  if(x->x_gui.x_fsf.x_selected)
  {
    for(i=0; i<n; i++)
    {
      sys_vgui(".x%lx.c itemconfigure %lxBASE%d \
        -outline #%06x\n", canvas, x, i,
        IEM_GUI_COLOR_SELECTED);
    }
    sys_vgui(".x%lx.c itemconfigure %lxLABEL \
      -fill #%06x\n", canvas, x, IEM_GUI_COLOR_SELECTED);
  }
  else
  {
    for(i=0; i<n; i++)
    {
      sys_vgui(".x%lx.c itemconfigure %lxBASE%d \
        -outline #%06x\n", canvas, x, i, IEM_GUI_COLOR_NORMAL);
    }
    sys_vgui(".x%lx.c itemconfigure %lxLABEL \
      -fill #%06x\n", canvas, x, x->x_gui.x_lcol);
  }
}


static void sradio_draw(t_sradio *x, t_glist *glist, int mode) {
  if(mode == IEM_GUI_DRAW_MODE_UPDATE)
    sys_queuegui(x, glist, sradio_draw_update);
  else if(mode == IEM_GUI_DRAW_MODE_MOVE)
    sradio_draw_move(x, glist);
  else if(mode == IEM_GUI_DRAW_MODE_NEW)
    sradio_draw_new(x, glist);
  else if(mode == IEM_GUI_DRAW_MODE_SELECT)
    sradio_draw_select(x, glist);
  else if(mode == IEM_GUI_DRAW_MODE_ERASE)
    sradio_draw_erase(x, glist);
  else if(mode == IEM_GUI_DRAW_MODE_CONFIG)
    sradio_draw_config(x, glist);
  else if(mode >= IEM_GUI_DRAW_MODE_IO)
    sradio_draw_io(x, glist, mode - IEM_GUI_DRAW_MODE_IO);
}

/* ------------------------ hdl widgetbehaviour----------------------------- */

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
  {
    return;
  }
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
  t_sradio *x = (t_sradio *)z;
  char buf[800];
  t_symbol *srl[3];
  iemgui_properties(&x->x_gui, srl);
  sprintf(buf, "pdtk_iemgui_dialog %%s |sradio| \
    ----------dimensions(pix):----------- %d %d \
    size: 0 0 empty empty 0.0 empty 0.0 empty 0 %d \
    dont keep -1 -1 number: %d %s %s %s %d %d %d %d #%06x #%06x #%06x\n",
    x->x_gui.x_w, IEM_GUI_MINSIZE,
    (int)x->x_keep, x->x_number,
    srl[0]->s_name, srl[1]->s_name,
    srl[2]->s_name, x->x_gui.x_ldx, x->x_gui.x_ldy,
    x->x_gui.x_fsf.x_font_style, x->x_gui.x_fontsize,
    0xffffff & x->x_gui.x_bcol, 
    0xffffff & x->x_gui.x_fcol, 
    0xffffff & x->x_gui.x_lcol);
  gfxstub_new(&x->x_gui.x_obj.ob_pd, x, buf);
}

static void sradio_dialog(t_sradio *x, t_symbol *s, int argc, t_atom *argv) 
{
  t_symbol *srl[3];
  int a    = (int)atom_getintarg(0, argc, argv);
  int keep = (int)atom_getintarg(4, argc, argv);
  int num  = (int)atom_getintarg(6, argc, argv);
  int sr_flags;
  int *dummy;

  if(keep != 0)
  {
    keep = 1;
  }
  x->x_keep    = (unsigned char)keep;
  sr_flags     = iemgui_dialog(&x->x_gui, srl, argc, argv);
  x->x_gui.x_w = iemgui_clip_size(a);
  x->x_gui.x_h = x->x_gui.x_w;
  
  if(x->x_number != num)
  {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    
    sradio_resizer(x,num);
    
    x->x_number = num;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  }
  else
  {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO + sr_flags);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_MOVE);
    canvas_fixlinesfor(x->x_gui.x_glist, (t_text*)x);
  }
}

static void sradio_keep(t_sradio *x, t_floatarg f)
{
  x->x_keep = (unsigned char) (f>0?1:0);
}

static void sradio_preset(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
  if(argc)
  {
    int n;
    if (argc > x->x_number && argc < IEM_RADIO_MAX)
    {
      n = x->x_number;
    }
    if (argc <= x->x_number)
    {
      n = argc;
    } 
    int i = 0;
    while (n--) {
      if (!IS_A_FLOAT(argv, i))
      {
        goto error;
      }
    }
    binbuf_restore(x->x_textbuf.b_binbuf, argc, argv);
    textbuf_senditup(&x->x_textbuf); 
  }
  else
  {
    error:
      pd_error(x,"Can't parse:");
      postatom(argc,argv);
      endpost();
  }
}
//'addline' is needed because pdtk_textwindow uses that method
static void textbuf_addline(t_sradio *x, t_symbol *s, int argc, t_atom *argv) 
{
  sradio_preset(x,gensym("preset"),argc,argv);
}

static void sradio_set(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
  if (argc)
  {
    int n;
    if (argc > x->x_number && argc < IEM_RADIO_MAX)
    {
      n = x->x_number;
    }
    if (argc <= x->x_number)
    {
      n = argc;
    } 
    int i = 0;
    while (n--)
    {
      if (IS_A_FLOAT(argv, i))
      {
        x->x_onlist[i] = ((int)atom_getintarg(i,argc,argv)==0)?0:1;
        SETFLOAT(&x->x_listout[i], ((int)atom_getintarg(i,argc,argv)==0)?0:1);
        i++;
      }
      else
      {
        pd_error(x,"sradio_set: index %d is not a number", i);
      }
    }
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
  }
  else
  {
    pd_error(x,"Can't parse:");
    postatom(argc,argv);
    endpost();
  }
  
}

static void sradio_store(t_sradio *x, t_floatarg ff)
{
  t_binbuf *b = (t_binbuf *) x->x_textbuf.b_binbuf;
  int start, end, n, i, lineno;
  int f = (int)ff;
  t_atom *vec;
  if (!b)
  {
    return;
  }
  vec = binbuf_getvec(b);
  n   = binbuf_getnatom(b);
  if (f && ((lineno = f<0?0:f)>=0) && text_nthline(n, vec, lineno, &start, &end))
  {
    post("n:%d, lineno:%d,start:%d,end:%d",n,lineno,start,end);
    int oldn = n;
    n = n + (x->x_number - (end-start));
    if (n > oldn)
    {
      (void)binbuf_resize(b, n);
    }
    vec = binbuf_getvec(b);
    memmove(&vec[start + x->x_number], &vec[end], sizeof(*vec) * (oldn - end));
    // post("after memmove");
    if (n < oldn)
    {
      (void)binbuf_resize(b, n);
      vec = binbuf_getvec(b);
    }
    // post("before loop");
    for (i=0; i<x->x_number; i++)
    {
      vec[start+i] = x->x_listout[i];
      // post("loop:%d",n);
    }
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
  {
    return;
  }
  vec = binbuf_getvec(b);
  n = binbuf_getnatom(b);

  if (text_nthline(n, vec, f, &start, &end))
  {
    int outc = end - start, k;
    t_atom *outv;
    ATOMS_ALLOCA(outv, outc);
    for (k = 0; k < outc; k++)
    {
      outv[k] = vec[start+k];
    }
    sradio_set(x, gensym("set"),outc, outv);
    ATOMS_FREEA(outv, outc);
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
    {
      return;
    }
    vec = binbuf_getvec(b);
    n   = binbuf_getnatom(b);
    while (text_nthline(n, vec, i, &start, &end))
    {
      int outc = end - start, k;
      t_atom *outv;
      ATOMS_ALLOCA(outv, outc);
      for (k = 0; k < outc; k++)
      {
        outv[k] = vec[start+k];
      }
      pd_list(s->s_thing, gensym("list"),outc, outv);
      ATOMS_FREEA(outv, outc);
      i++;
    }
  }
}

static void sradio_clear(t_sradio *x)
{
  int n = x->x_number;
  int i=0, d=0;
  while (n--)
  {
    x->x_onlist[i] = x->x_drawnlist[i]= d;
    SETFLOAT(&x->x_listout[i], d);
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
  {
    pd_list(x->x_gui.x_snd->s_thing, 0,x->x_number, x->x_listout);
  }
}

static void sradio_fout(t_sradio *x, t_floatarg f)
{
  int i=(int)f;
  if(i < 0)
  {
    i = 0;
  }
  if(i >= x->x_number)
  {
    i = x->x_number-1;
  }
  x->x_onlist[i] = x->x_drawnlist[i]==1?0:1;
  SETFLOAT(&x->x_listout[i], (int)x->x_onlist[i]);
  (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);

}

static void sradio_float(t_sradio *x, t_floatarg f)
{
  int i=((int)f)%x->x_number;
  if(i < 0)
  {
    i = 0;
  }
  outlet_float(x->x_gui.x_obj.ob_outlet, x->x_onlist[i]);
  if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
  {
    pd_float(x->x_gui.x_snd->s_thing, x->x_onlist[i]);
  }
  if (x->x_focflag == 1)
  {
    x->x_foc = i;
    if (x->x_foc != x->x_old )
    {
      (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    }
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
  n   = binbuf_getnatom(b);
  
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
      t_atom *outv;
      ATOMS_ALLOCA(outv, outc);
      for (k = 0; k < outc; k++)
      {
        outv[k] = vec[start+k];
      }
      postatom(outc, outv);
      endpost();
      ATOMS_FREEA(outv, outc);
      i++;
    }
  }
}

/*
static void sradio_motion(t_sradio *x, t_floatarg dx, t_floatarg f) {

  //int xx = (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
  
  int old = x->x_val;
  int size = x->x_gui.x_w;
  int width = (int)size*x->x_number;
  x->x_pos += size*(int)dx+f;
  x->x_val = x->x_pos;
  if(x->x_val > width)
  {
    x->x_val = width;
    x->x_pos = (int)width/2;
  }
  if(x->x_val < 0)
  {
    x->x_val = 0;
    x->x_pos = (int)width/2;
  }
   // x->x_fval = sradio_getfval(x);
  if (old != x->x_val)
  {
    //(*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    //hslider_bang(x);
    post("xval %d",x->x_val);
    //sradio_fout(x, x->x_val);
  }
}
*/
static void sradio_click(t_sradio *x, 
  t_floatarg xpos, t_floatarg ypos, 
  t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
  int xx = (int)xpos - (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
  float f = (t_float)(xx / x->x_gui.x_w);
  if (!shift)
  {
    sradio_fout(x, f);
  }
  else
  {
    textbuf_open(x);
  }
  // post("f:%f",f);
  // post("xpos: %d, ypos: %d,  shift: %d, 
  // ctrl: %d,  alt: %d", xpos,  ypos,  shift,  ctrl,  alt);
  // glist_grab(x->x_gui.x_glist,
  // &x->x_gui.x_obj.te_g, (t_glistmotionfn)sradio_motion, 0, xpos, f);
}

static int sradio_newclick(t_gobj *z, struct _glist *glist,
 int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
  // post("xpix: %d, ypix: %d, shift: %d, alt: %d, dbl: %d, doit: %d",
  //   xpix, ypix, shift, alt, dbl, doit);
  if(doit)
  {
    sradio_click((t_sradio *)z, 
      (t_floatarg)xpix, (t_floatarg)ypix, 
      (t_floatarg)shift, 0, (t_floatarg)alt);
  }
  return (1);
}

static void sradio_focus(t_sradio *x, t_floatarg foc)
{
  t_canvas *canvas=glist_getcanvas(x->x_gui.x_glist);
  int i, n=x->x_number;
  if (foc)
  {
    x->x_focflag = 1;
  }
  else
  {
    x->x_focflag = 0;
    for (i=0;i<n;i++)
    {
      sys_vgui(".x%lx.c itemconfigure %lxFOC%d \
      -outline #%06x\n", canvas, x, i, x->x_gui.x_bcol);
    }
  }
}

static void sradio_next(t_sradio *x)
{
  if(x->x_focflag==0) sradio_focus(x,1);
  int pval = x->x_foc;
  int nval;
  pval+=1;
  nval = pval%x->x_number;
  sradio_float(x,nval);
}

static void sradio_prev(t_sradio *x)
{
  if(x->x_focflag==0)
  {
    sradio_focus(x,1);
  }
  int pval = x->x_foc;
  int nval;
  pval-=1;
  nval = pval%x->x_number;
  sradio_float(x,nval);
}

static void sradio_number(t_sradio *x, t_floatarg num)
{
  int n=(int)num;
  if(n < 1)
  {
    n = 1;
  }
  if(n > IEM_RADIO_MAX)
  {
    n = IEM_RADIO_MAX;
  }
  if(n != x->x_number)
  {
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
    sradio_resizer(x,n);
    x->x_number = n;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_NEW);
  }
}

static void sradio_size(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  x->x_gui.x_w = iemgui_clip_size((int)atom_getintarg(0, ac, av));
  x->x_gui.x_h = x->x_gui.x_w;
  iemgui_size((void *)x, &x->x_gui);
}

static void sradio_delta(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  iemgui_delta((void *)x, &x->x_gui, s, ac, av);
}

static void sradio_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  iemgui_pos((void *)x, &x->x_gui, s, ac, av);
}

static void sradio_color(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  iemgui_color((void *)x, &x->x_gui, s, ac, av);
}

static void sradio_send(t_sradio *x, t_symbol *s)
{
  iemgui_send(x, &x->x_gui, s);
}

static void sradio_receive(t_sradio *x, t_symbol *s)
{
  iemgui_receive(x, &x->x_gui, s);
}

static void sradio_label(t_sradio *x, t_symbol *s)
{
  iemgui_label((void *)x, &x->x_gui, s);
}

static void sradio_label_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  iemgui_label_pos((void *)x, &x->x_gui, s, ac, av);
}

static void sradio_label_font(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
  iemgui_label_font((void *)x, &x->x_gui, s, ac, av);
}

static void *sradio_donew(t_symbol *s, int argc, t_atom *argv, int old)
{
  t_sradio *x = (t_sradio *)pd_new(sradio_class);
  int f       =  0;
  int ldx     =  0;
  int ldy     = -8;
  int keep    =  DEF_KEEP;
  int num     =  DEF_NUMBER;
  int a       =  IEM_GUI_DEFAULTSIZE;
  int fs      =  DEF_FONTSIZE;
  int ftbreak =  IEM_BNG_DEFAULTBREAKFLASHTIME;
  int fthold  =  IEM_BNG_DEFAULTHOLDFLASHTIME;
  char str[144];
  t_symbol *asym = gensym("#A");

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
  else
  { 
    iemgui_new_getnames(&x->x_gui, 4, 0);
  }
  x->x_keep                 = (unsigned char) keep;
  x->x_gui.x_draw           = (t_iemfunptr)sradio_draw;
  x->x_gui.x_fsf.x_snd_able = 1;
  x->x_gui.x_fsf.x_rcv_able = 1;
  x->x_gui.x_glist          = (t_glist *)canvas_getcurrent();
  if (!strcmp(x->x_gui.x_snd->s_name, "empty"))
  {
    x->x_gui.x_fsf.x_snd_able = 0;
  }
  if (!strcmp(x->x_gui.x_rcv->s_name, "empty"))
  {
    x->x_gui.x_fsf.x_rcv_able = 0;
  }
  if(x->x_gui.x_fsf.x_font_style == 1)
  {
    strcpy(x->x_gui.x_font, "helvetica");
  }
  else if(x->x_gui.x_fsf.x_font_style == 2)
  {
    strcpy(x->x_gui.x_font, "times");
  }
  else
  {
    x->x_gui.x_fsf.x_font_style = 0;
    strcpy(x->x_gui.x_font, sys_font);
  }
  if(num < 1)
  {
    num = 1;
  }
  if(num > IEM_RADIO_MAX)
  {
    num = IEM_RADIO_MAX;
  }
  x->x_number = num;
  sradio_resizer(x,num);
  textbuf_init(&x->x_textbuf);
  /* bashily unbind #A -- this would create garbage if #A were
  multiply bound but we believe in this context it's at most
  bound to whichever text_define or array was created most recently */
  asym->s_thing = 0;
  /* and now bind #A to us to receive following messages in the
  saved file or copy buffer */
  pd_bind(&x->x_gui.x_obj.ob_pd, asym);
  
  if (x->x_gui.x_fsf.x_rcv_able)
  {
    pd_bind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
  }
  x->x_gui.x_ldx = ldx;
  x->x_gui.x_ldy = ldy;
  if(fs < 4)
  {
    fs = 4;
  }
  x->x_gui.x_fontsize = fs;
  x->x_gui.x_w        = iemgui_clip_size(a);
  x->x_gui.x_h        = x->x_gui.x_w;
  iemgui_verify_snd_ne_rcv(&x->x_gui);

  outlet_new(&x->x_gui.x_obj, &s_list);

  return (x);
}

static void *sradio_new(t_symbol *s, int argc, t_atom *argv)
{
  return (sradio_donew(s, argc, argv, 0));
}

static void sradio_ff(t_sradio *x)
{
  textbuf_free(&x->x_textbuf);
  if(x->x_gui.x_fsf.x_rcv_able)
  {
    pd_unbind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
  }
  freebytes(x->x_onlist,    sizeof(&x->x_onlist));
  freebytes(x->x_drawnlist, sizeof(&x->x_drawnlist));
  freebytes(x->x_listout,   sizeof(&x->x_listout));
  gfxstub_deleteforkey(x);
}

void g_sradio_setup(void)
{
  sradio_class = class_new(gensym("sradio"), (t_newmethod)sradio_new,
    (t_method)sradio_ff, sizeof(t_sradio), 0, A_GIMME, 0);

  class_addbang(sradio_class,   sradio_bang);
  class_addfloat(sradio_class,  sradio_float);
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

  // post("[sradio] version 0.2 ~ fdch");

}
