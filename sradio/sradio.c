/* Copyright (c) 1997-1999 Miller Puckette.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution. */

/* g_7_guis.c written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001 */
/* thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja */

/* name change to sradio by MSP and changed to
put out a "float" as in sliders, toggles, etc. */

/* hacked to behave like an array and store/recall t_atoms*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "m_pd.h"
#include "g_canvas.h"

#include "g_all_guis.h"
#include <math.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/*This should be in g_all_guis.h*/
typedef struct _sradio
{
    t_iemgui x_gui;
    int      x_on;
    t_int      *x_onlist;
    t_int      *x_drawnlist;
    t_atom   *x_listout;
    int      x_on_old;  /* LATER delete this; it's used for old version */
    int      x_change;
    int      x_number;
    int      x_drawn,x_pos, x_val;
    t_float  x_fval;
    double x_k;
    t_atom   x_at[2];
    //t_outlet *x_out1;
} t_sradio;

/* ------------- hdl     gui-horicontal dial ---------------------- */

t_widgetbehavior sradio_widgetbehavior;
static t_class *sradio_class;


void sradio_resizer(t_sradio *x, int nsize) {
  int i =0;
  int d=0;
  freebytes(x->x_onlist, sizeof(&x->x_onlist));
  freebytes(x->x_drawnlist, sizeof(&x->x_drawnlist));
  freebytes(x->x_listout, sizeof(&x->x_listout));
  x->x_onlist = (t_int *)malloc(nsize * sizeof(int));
  x->x_drawnlist = (t_int *)malloc(nsize * sizeof(int));
  x->x_listout = (t_atom *)malloc(nsize * sizeof(t_atom));
  while (nsize--) {
    x->x_onlist[i] = x->x_drawnlist[i]= d;
    SETFLOAT(&x->x_listout[i], d);
    i++;
  }
}

/* widget helper functions */

void sradio_draw_update(t_gobj *client, t_glist *glist)
{
    t_sradio *x = (t_sradio *)client;
    int n=x->x_number, i=0;
    //post("before update");
    if(glist_isvisible(glist))
    { 
      t_canvas *canvas=glist_getcanvas(glist);
      while (n--) {
        if (x->x_onlist[i]==1)
        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%06x -outline #%06x\n",
                 canvas, x, i,//draw
                 x->x_gui.x_fcol, x->x_gui.x_fcol);
        else 
        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%06x -outline #%06x\n",
                 canvas, x, i,//undraw
                 x->x_gui.x_bcol, x->x_gui.x_bcol);
        
        x->x_drawnlist[i] = x->x_onlist[i];
        i++;
      }
      
    }
}

void sradio_draw_new(t_sradio *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i, dx=x->x_gui.x_w, s4=dx/4;
    int yy11=text_ypix(&x->x_gui.x_obj, glist), yy12=yy11+dx;
    int yy21=yy11+s4, yy22=yy12-s4;
    int xx11b=text_xpix(&x->x_gui.x_obj, glist), xx11=xx11b, xx21=xx11b+s4;
    int xx22=xx11b+dx-s4;
    int zoomlabel =
        1 + (IEMGUI_ZOOM(x)-1) * (x->x_gui.x_ldx >= 0 && x->x_gui.x_ldy >= 0);


    for(i=0; i<n; i++)
    {
        sys_vgui(".x%lx.c create rectangle %d %d %d %d -width %d -fill #%06x -tags %lxBASE%d\n",
                 canvas, xx11, yy11, xx11+dx, yy12, IEMGUI_ZOOM(x),
                 x->x_gui.x_bcol, x, i);
        sys_vgui(".x%lx.c create oval %d %d %d %d -fill #%06x -outline #%06x -tags %lxBUT%d\n",
                 canvas, xx21, yy21, xx22, yy22,
                 (x->x_onlist[i]==1)?x->x_gui.x_fcol:x->x_gui.x_bcol,
                 (x->x_onlist[i]==1)?x->x_gui.x_fcol:x->x_gui.x_bcol, x, i);
        xx11 += dx;
        xx21 += dx;
        xx22 += dx;
        //x->x_drawn= x->x_on;
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
        sys_vgui(".x%lx.c create rectangle %d %d %d %d -fill black -tags [list %lxOUT%d outlet]\n",
             canvas, xx11b, yy12+1-2*IEMGUI_ZOOM(x), xx11b + IOWIDTH, yy12, x, 0);
    if(!x->x_gui.x_fsf.x_rcv_able)
        sys_vgui(".x%lx.c create rectangle %d %d %d %d -fill black -tags [list %lxIN%d inlet]\n",
             canvas, xx11b, yy11, xx11b + IOWIDTH, yy11-1+2*IEMGUI_ZOOM(x), x, 0);

}

void sradio_draw_move(t_sradio *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i, dx=x->x_gui.x_w, s4=dx/4;
    int yy11=text_ypix(&x->x_gui.x_obj, glist), yy12=yy11+dx;
    int yy21=yy11+s4, yy22=yy12-s4;
    int xx11b=text_xpix(&x->x_gui.x_obj, glist), xx11=xx11b, xx21=xx11b+s4;
    int xx22=xx11b+dx-s4;
    int zoomlabel =
        1 + (IEMGUI_ZOOM(x)-1) * (x->x_gui.x_ldx >= 0 && x->x_gui.x_ldy >= 0);

    xx11 = xx11b;
    xx21=xx11b+s4;
    xx22=xx11b+dx-s4;
    for(i=0; i<n; i++)
    {
        sys_vgui(".x%lx.c coords %lxBASE%d %d %d %d %d\n",
                 canvas, x, i, xx11, yy11, xx11+dx, yy12);
        sys_vgui(".x%lx.c coords %lxBUT%d %d %d %d %d\n",
                 canvas, x, i, xx21, yy21, xx22, yy22);
        xx11 += dx;
        xx21 += dx;
        xx22 += dx;
    }
    sys_vgui(".x%lx.c coords %lxLABEL %d %d\n",
             canvas, x, xx11b+x->x_gui.x_ldx * zoomlabel,
             yy11+x->x_gui.x_ldy * zoomlabel);
    if(!x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%lx.c coords %lxOUT%d %d %d %d %d\n",
             canvas, x, 0, xx11b, yy12+1-2*IEMGUI_ZOOM(x), xx11b + IOWIDTH, yy12);
    if(!x->x_gui.x_fsf.x_rcv_able)
        sys_vgui(".x%lx.c coords %lxIN%d %d %d %d %d\n",
             canvas, x, 0, xx11b, yy11, xx11b + IOWIDTH, yy11-1+2*IEMGUI_ZOOM(x));
}

void sradio_draw_erase(t_sradio* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i;

    for(i=0; i<n; i++)
    {
        sys_vgui(".x%lx.c delete %lxBASE%d\n", canvas, x, i);
        sys_vgui(".x%lx.c delete %lxBUT%d\n", canvas, x, i);
    }
    sys_vgui(".x%lx.c delete %lxLABEL\n", canvas, x);
    if(!x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%lx.c delete %lxOUT%d\n", canvas, x, 0);
    if(!x->x_gui.x_fsf.x_rcv_able)
    sys_vgui(".x%lx.c delete %lxIN%d\n", canvas, x, 0);
}

void sradio_draw_config(t_sradio* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i;

    sys_vgui(".x%lx.c itemconfigure %lxLABEL -font {{%s} -%d %s} -fill #%06x -text {%s} \n",
             canvas, x, x->x_gui.x_font, x->x_gui.x_fontsize, sys_fontweight,
             x->x_gui.x_fsf.x_selected?IEM_GUI_COLOR_SELECTED:x->x_gui.x_lcol,
             strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"");
    for(i=0; i<n; i++)
    {
        sys_vgui(".x%lx.c itemconfigure %lxBASE%d -fill #%06x\n", canvas, x, i,
                 x->x_gui.x_bcol);
        sys_vgui(".x%lx.c itemconfigure %lxBUT%d -fill #%06x -outline #%06x\n", canvas, x, i,
                 (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol,
                 (x->x_on==i)?x->x_gui.x_fcol:x->x_gui.x_bcol);
    }
}

void sradio_draw_io(t_sradio* x, t_glist* glist, int old_snd_rcv_flags)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int xpos=text_xpix(&x->x_gui.x_obj, glist);
    int ypos=text_ypix(&x->x_gui.x_obj, glist);

    if((old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && !x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%lx.c create rectangle %d %d %d %d -tags %lxOUT%d\n",
                 canvas,
                 xpos, ypos + x->x_gui.x_w-1,
                 xpos + IOWIDTH, ypos + x->x_gui.x_w,
                 x, 0);
    if(!(old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%lx.c delete %lxOUT%d\n", canvas, x, 0);
    if((old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && !x->x_gui.x_fsf.x_rcv_able)
        sys_vgui(".x%lx.c create rectangle %d %d %d %d -tags %lxIN%d\n",
                 canvas,
                 xpos, ypos,
                 xpos + IOWIDTH, ypos+1, x, 0);
    if(!(old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && x->x_gui.x_fsf.x_rcv_able)
        sys_vgui(".x%lx.c delete %lxIN%d\n", canvas, x, 0);
}

void sradio_draw_select(t_sradio* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int n=x->x_number, i;

    if(x->x_gui.x_fsf.x_selected)
    {
        for(i=0; i<n; i++)
        {
            sys_vgui(".x%lx.c itemconfigure %lxBASE%d -outline #%06x\n", canvas, x, i,
                     IEM_GUI_COLOR_SELECTED);
        }
        sys_vgui(".x%lx.c itemconfigure %lxLABEL -fill #%06x\n", canvas, x, IEM_GUI_COLOR_SELECTED);
    }
    else
    {
        for(i=0; i<n; i++)
        {
            sys_vgui(".x%lx.c itemconfigure %lxBASE%d -outline #%06x\n", canvas, x, i,
                     IEM_GUI_COLOR_NORMAL);
        }
        sys_vgui(".x%lx.c itemconfigure %lxLABEL -fill #%06x\n", canvas, x,
                 x->x_gui.x_lcol);
    }
}

void sradio_draw(t_sradio *x, t_glist *glist, int mode)
{
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

static void sradio_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1, int *xp2, int *yp2)
{
    t_sradio *x = (t_sradio *)z;

    *xp1 = text_xpix(&x->x_gui.x_obj, glist);
    *yp1 = text_ypix(&x->x_gui.x_obj, glist);
    *xp2 = *xp1 + x->x_gui.x_w*x->x_number;
    *yp2 = *yp1 + x->x_gui.x_h;
}
//later revise this so that presets get saved with a 'keep' flag
static void sradio_save(t_gobj *z, t_binbuf *b)
{
    t_sradio *x = (t_sradio *)z;
    t_symbol *bflcol[3];
    t_symbol *srl[3];

    iemgui_save(&x->x_gui, srl, bflcol);
    binbuf_addv(b, "ssiisiiiisssiiiisssf", gensym("#X"),gensym("obj"),
                (int)x->x_gui.x_obj.te_xpix, (int)x->x_gui.x_obj.te_ypix,
                gensym("sradio"),
                x->x_gui.x_w,
                x->x_change, iem_symargstoint(&x->x_gui.x_isa), x->x_number,
                srl[0], srl[1], srl[2],
                x->x_gui.x_ldx, x->x_gui.x_ldy,
                iem_fstyletoint(&x->x_gui.x_fsf), x->x_gui.x_fontsize,
                bflcol[0], bflcol[1], bflcol[2], x->x_fval);
    binbuf_addv(b, ";");
}

static void sradio_properties(t_gobj *z, t_glist *owner)
{
    t_sradio *x = (t_sradio *)z;
    char buf[800];
    t_symbol *srl[3];
    int hchange=-1;

    iemgui_properties(&x->x_gui, srl);
    sprintf(buf, "pdtk_iemgui_dialog %%s |sradio| \
            ----------dimensions(pix):----------- %d %d size: 0 0 empty \
            empty 0.0 empty 0.0 empty %d \
            %d new-only new&old %d %d number: %d \
            %s %s \
            %s %d %d \
            %d %d \
            #%06x #%06x #%06x\n",
            x->x_gui.x_w, IEM_GUI_MINSIZE,
            0,/*no_schedule*/
            hchange, x->x_gui.x_isa.x_loadinit, -1, x->x_number,
            srl[0]->s_name, srl[1]->s_name,
            srl[2]->s_name, x->x_gui.x_ldx, x->x_gui.x_ldy,
            x->x_gui.x_fsf.x_font_style, x->x_gui.x_fontsize,
            0xffffff & x->x_gui.x_bcol, 0xffffff & x->x_gui.x_fcol, 0xffffff & x->x_gui.x_lcol);
    gfxstub_new(&x->x_gui.x_obj.ob_pd, x, buf);
}

static void sradio_dialog(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol *srl[3];
    int a = (int)atom_getintarg(0, argc, argv);
    int chg = (int)atom_getintarg(4, argc, argv);
    int num = (int)atom_getintarg(6, argc, argv);
    int sr_flags, i;
    int *dummy;

    if(chg != 0) chg = 1;
    x->x_change = chg;
    sr_flags = iemgui_dialog(&x->x_gui, srl, argc, argv);
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    if(x->x_number != num)
    {
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_ERASE);
        
        sradio_resizer(x,num);
        
        x->x_number = num;
        /*
        if(x->x_on >= x->x_number)
        {
            x->x_on = x->x_number - 1;
            x->x_on_old = x->x_on;
        }
        */
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

static void sradio_set(t_sradio *x, t_floatarg f)
{
    int i=(int)f;
    int old=x->x_on_old;

    x->x_fval = f;
    if(i < 0)
        i = 0;
    if(i >= x->x_number)
        i = x->x_number-1;
    if(x->x_on != x->x_on_old)
    {
        old = x->x_on_old;
        x->x_on_old = x->x_on;
        x->x_on = i;
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        x->x_on_old = old;
    }
    else
    {
        x->x_on = i;
        (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    }
}

static void sradio_bang(t_sradio *x)
{

outlet_list(x->x_gui.x_obj.ob_outlet,0,x->x_number,x->x_listout);
   /*
        float outval = x->x_fval;
        outlet_float(x->x_gui.x_obj.ob_outlet, outval);
        if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
            pd_float(x->x_gui.x_snd->s_thing, outval);
    */

}

static void sradio_fout(t_sradio *x, t_floatarg f)
{
    int i=(int)f;
    if(i < 0)
        i = 0;
    if(i >= x->x_number)
        i = x->x_number-1;
    x->x_onlist[i]= x->x_drawnlist[i]==1?0:1;
    SETFLOAT(&x->x_listout[i], (int)x->x_onlist[i]);
    
    
        //
            //pd_pointer(x->x_gui.x_snd->s_thing, &gp);
            //
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);

}

static void sradio_float(t_sradio *x, t_floatarg f)
{
  int i=(int)f;
  if(i < 0)
      i = 0;
  outlet_float(x->x_gui.x_obj.ob_outlet, x->x_onlist[i%x->x_number]);
  if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
    pd_float(x->x_gui.x_snd->s_thing, x->x_onlist[i%x->x_number]);
}

/*
static void sradio_motion(t_sradio *x, t_floatarg dx, t_floatarg f)
{
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
static void sradio_click(t_sradio *x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
    int xx = (int)xpos - (int)text_xpix(&x->x_gui.x_obj, x->x_gui.x_glist);
    float f = (t_float)(xx / x->x_gui.x_w);
    sradio_fout(x, f);
    //glist_grab(x->x_gui.x_glist, &x->x_gui.x_obj.te_g, (t_glistmotionfn)sradio_motion, 0, xpos, f);
}

static int sradio_newclick(t_gobj *z, struct _glist *glist, int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    if(doit)
        sradio_click((t_sradio *)z, (t_floatarg)xpix, (t_floatarg)ypix, (t_floatarg)shift, 0, (t_floatarg)alt);
    return (1);
}

static void sradio_loadbang(t_sradio *x, t_floatarg action)
{
    //if (action == LB_LOAD && x->x_gui.x_isa.x_loadinit)
        //sradio_bang(x);
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
{iemgui_delta((void *)x, &x->x_gui, s, ac, av);}

static void sradio_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{iemgui_pos((void *)x, &x->x_gui, s, ac, av);}

static void sradio_color(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{iemgui_color((void *)x, &x->x_gui, s, ac, av);}

static void sradio_send(t_sradio *x, t_symbol *s)
{iemgui_send(x, &x->x_gui, s);}

static void sradio_receive(t_sradio *x, t_symbol *s)
{iemgui_receive(x, &x->x_gui, s);}

static void sradio_label(t_sradio *x, t_symbol *s)
{iemgui_label((void *)x, &x->x_gui, s);}

static void sradio_label_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{iemgui_label_pos((void *)x, &x->x_gui, s, ac, av);}

static void sradio_label_font(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{iemgui_label_font((void *)x, &x->x_gui, s, ac, av);}

static void sradio_init(t_sradio *x, t_floatarg f)
{
    x->x_gui.x_isa.x_loadinit = (f==0.0)?0:1;
}

static void sradio_double_change(t_sradio *x)
{x->x_change = 1;}

static void sradio_single_change(t_sradio *x)
{x->x_change = 0;}

static void *sradio_donew(t_symbol *s, int argc, t_atom *argv, int old)
{
    t_sradio *x = (t_sradio *)pd_new(sradio_class);
    int a=IEM_GUI_DEFAULTSIZE, on = 0, f=0;
    int ldx=0, ldy=-8, chg=1, num=8;
    int fs=10;
    int ftbreak=IEM_BNG_DEFAULTBREAKFLASHTIME, fthold=IEM_BNG_DEFAULTHOLDFLASHTIME;
    char str[144];
    float fval = 0;

    iem_inttosymargs(&x->x_gui.x_isa, 0);
    iem_inttofstyle(&x->x_gui.x_fsf, 0);

    x->x_gui.x_bcol = 0xFCFCFC;
    x->x_gui.x_fcol = 0x00;
    x->x_gui.x_lcol = 0x00;

    if((argc == 15)&&IS_A_FLOAT(argv,0)&&IS_A_FLOAT(argv,1)&&IS_A_FLOAT(argv,2)
       &&IS_A_FLOAT(argv,3)
       &&(IS_A_SYMBOL(argv,4)||IS_A_FLOAT(argv,4))
       &&(IS_A_SYMBOL(argv,5)||IS_A_FLOAT(argv,5))
       &&(IS_A_SYMBOL(argv,6)||IS_A_FLOAT(argv,6))
       &&IS_A_FLOAT(argv,7)&&IS_A_FLOAT(argv,8)
       &&IS_A_FLOAT(argv,9)&&IS_A_FLOAT(argv,10)&&IS_A_FLOAT(argv,14))
    {
        a = (int)atom_getintarg(0, argc, argv);
        chg = (int)atom_getintarg(1, argc, argv);
        iem_inttosymargs(&x->x_gui.x_isa, atom_getintarg(2, argc, argv));
        num = (int)atom_getintarg(3, argc, argv);
        iemgui_new_getnames(&x->x_gui, 4, argv);
        ldx = (int)atom_getintarg(7, argc, argv);
        ldy = (int)atom_getintarg(8, argc, argv);
        iem_inttofstyle(&x->x_gui.x_fsf, atom_getintarg(9, argc, argv));
        fs = (int)atom_getintarg(10, argc, argv);
        iemgui_all_loadcolors(&x->x_gui, argv+11, argv+12, argv+13);
        fval = atom_getfloatarg(14, argc, argv);
    }
    else iemgui_new_getnames(&x->x_gui, 4, 0);
    x->x_gui.x_draw = (t_iemfunptr)sradio_draw;
    x->x_gui.x_fsf.x_snd_able = 1;
    x->x_gui.x_fsf.x_rcv_able = 1;
    x->x_gui.x_glist = (t_glist *)canvas_getcurrent();
    if (!strcmp(x->x_gui.x_snd->s_name, "empty"))
        x->x_gui.x_fsf.x_snd_able = 0;
    if (!strcmp(x->x_gui.x_rcv->s_name, "empty"))
        x->x_gui.x_fsf.x_rcv_able = 0;
    if(x->x_gui.x_fsf.x_font_style == 1) strcpy(x->x_gui.x_font, "helvetica");
    else if(x->x_gui.x_fsf.x_font_style == 2) strcpy(x->x_gui.x_font, "times");
    else { x->x_gui.x_fsf.x_font_style = 0;
        strcpy(x->x_gui.x_font, sys_font); }
    if(num < 1)
        num = 1;
    if(num > IEM_RADIO_MAX)
        num = IEM_RADIO_MAX;
    x->x_number = num;
    
    sradio_resizer(x,num);
    x->x_fval = fval;
    on = fval;
    if(on < 0)
        on = 0;
    if(on >= x->x_number)
        on = x->x_number - 1;
    if(x->x_gui.x_isa.x_loadinit)
        x->x_on = on;
    else
        x->x_on = 0;
    x->x_on_old = x->x_on;
    x->x_change = (chg==0)?0:1;
    if (x->x_gui.x_fsf.x_rcv_able)
        pd_bind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    x->x_gui.x_ldx = ldx;
    x->x_gui.x_ldy = ldy;
    if(fs < 4)
        fs = 4;
    x->x_gui.x_fontsize = fs;
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_verify_snd_ne_rcv(&x->x_gui);
    outlet_new(&x->x_gui.x_obj, &s_list);
    post("value in x->onlist[num-1] is: %d", x->x_onlist[num-1]);
    return (x);
}

static void *sradio_new(t_symbol *s, int argc, t_atom *argv)
{
    return (sradio_donew(s, argc, argv, 0));
}

static void sradio_ff(t_sradio *x)
{
    if(x->x_gui.x_fsf.x_rcv_able)
        pd_unbind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    freebytes(x->x_onlist, sizeof(&x->x_onlist));
    gfxstub_deleteforkey(x);
}

void g_sradio_setup(void)
{
    sradio_class = class_new(gensym("sradio"), (t_newmethod)sradio_new,
        (t_method)sradio_ff, sizeof(t_sradio), 0, A_GIMME, 0);
    class_addbang(sradio_class, sradio_bang);
    class_addfloat(sradio_class, sradio_float);
    class_addmethod(sradio_class, (t_method)sradio_click, gensym("click"),
                    A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    //class_addmethod(sradio_class, (t_method)sradio_motion, gensym("motion"),
                    //A_FLOAT, A_FLOAT, 0);
    class_addmethod(sradio_class, (t_method)sradio_dialog, gensym("dialog"),
                    A_GIMME, 0);
    class_addmethod(sradio_class, (t_method)sradio_loadbang,
        gensym("loadbang"), A_DEFFLOAT, 0);
    class_addmethod(sradio_class, (t_method)sradio_set,
        gensym("set"), A_FLOAT, 0);
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
    class_addmethod(sradio_class, (t_method)sradio_init,
        gensym("init"), A_FLOAT, 0);
    class_addmethod(sradio_class, (t_method)sradio_number,
        gensym("number"), A_FLOAT, 0);
    class_addmethod(sradio_class, (t_method)sradio_single_change,
        gensym("single_change"), 0);
    class_addmethod(sradio_class, (t_method)sradio_double_change,
        gensym("double_change"), 0);
    class_addmethod(sradio_class, (t_method)iemgui_zoom, gensym("zoom"),
        A_CANT, 0);
    sradio_widgetbehavior.w_getrectfn = sradio_getrect;
    sradio_widgetbehavior.w_displacefn = iemgui_displace;
    sradio_widgetbehavior.w_selectfn = iemgui_select;
    sradio_widgetbehavior.w_activatefn = NULL;
    sradio_widgetbehavior.w_deletefn = iemgui_delete;
    sradio_widgetbehavior.w_visfn = iemgui_vis;
    sradio_widgetbehavior.w_clickfn = sradio_newclick;
    class_setwidget(sradio_class, &sradio_widgetbehavior);
    class_sethelpsymbol(sradio_class, gensym("sradio"));
    class_setsavefn(sradio_class, sradio_save);
    class_setpropertiesfn(sradio_class, sradio_properties);

}

//needed to setup

void sradio_setup(void) {
  g_sradio_setup();
  }
