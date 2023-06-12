/* 

Copyright 2017-2023 Fede Camara Halac - ffddcchh

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
#include "g_all_guis.h" // for radio
#include "g_radio.c" 
#include "x_text.c" // for text functions

#define x_obj x_obj // get rid of text's definition

static t_class *sradio_class;

typedef struct _sradio
{
  t_iemgui  x_gui;
  t_textbuf x_textbuf; // the text buffer 
  t_radio  *x_radio; // the new radio
  t_int    *x_onlist;
  t_int    *x_drawnlist;
  t_atom   *x_listout;
  t_symbol *x_bindsym;
  t_int   x_pos, x_val, x_foc, x_old, x_focflag;
  unsigned char x_keep;
} t_sradio;

static t_widgetbehavior sradio_widgetbehavior;

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

static void sradio_draw_io(t_sradio* x, t_glist* glist, int old_snd_rcv_flags)
{
    radio_draw_io(&x->x_radio, glist, old_snd_rcv_flags);
}

static void sradio_draw_config(t_sradio* x, t_glist* glist)
{
    radio_draw_config(&x->x_radio, glist);
}

static void sradio_draw_new(t_sradio* x, t_glist* glist)
{
    radio_draw_new(&x->x_radio, glist);
}

static void sradio_draw_select(t_sradio* x, t_glist* glist)
{
    radio_draw_select(&x->x_radio, glist);
}

static void sradio_draw_update(t_gobj *client, t_glist *glist)
{
    radio_draw_update(client, glist);
}

static void sradio_dialog(t_sradio *x, t_symbol *s, int argc, t_atom *argv) 
{
    radio_dialog(&x->x_radio, s, argc, argv);
}

static void sradio_keep(t_sradio *x, t_floatarg f)
{
  x->x_keep = (unsigned char) (f > 0 ? 1 : 0);
}

static void sradio_preset(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
    t_radio *z= (t_radio *)&x->x_radio;
    t_textbuf *t= (t_textbuf *)&x->x_textbuf;
    if(argc)
    {
        int n;
        if (argc > z->x_number && argc < IEM_RADIO_MAX)
            n = z->x_number;
        if (argc <= z->x_number)
            n = argc;
        int i = 0;
        while (n--) {
        if (!IS_A_FLOAT(argv, i))
        {
            goto error;
        }
        }
        binbuf_restore(t->b_binbuf, argc, argv);
        textbuf_senditup(t); 
    }
    else
    {
        error:
            pd_error(x, "Can't parse:");
            postatom(argc, argv);
            endpost();
    }
}

static void sradio_set(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
    t_radio *z= (t_radio *)&x->x_radio;
    if (argc)
    {
        int n;
        if (argc > z->x_number && argc < IEM_RADIO_MAX)
            n = z->x_number;
        if (argc <= z->x_number)
            n = argc;
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
        (*z->x_gui.x_draw)(z, z->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
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
    t_radio *z= (t_radio *)&x->x_radio;
    t_textbuf *t= (t_textbuf *)&x->x_textbuf;
    t_binbuf *b= (t_binbuf *)&t->b_binbuf;
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
        n = n + (z->x_number - (end-start));
        if (n > oldn)
        {
        (void)binbuf_resize(b, n);
        }
        vec = binbuf_getvec(b);
        memmove(&vec[start + z->x_number], &vec[end], sizeof(*vec) * (oldn - end));
        // post("after memmove");
        if (n < oldn)
        {
        (void)binbuf_resize(b, n);
        vec = binbuf_getvec(b);
        }
        // post("before loop");
        for (i=0; i<z->x_number; i++)
        {
        vec[start+i] = x->x_listout[i];
        // post("loop:%d",n);
        }
    }
    else
    {
        binbuf_restore(b, z->x_number, x->x_listout);
        binbuf_addsemi(b);
        textbuf_senditup(t);
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
    t_radio *z= (t_radio *)&x->x_radio;
    t_textbuf *t= (t_textbuf *)&x->x_textbuf;
    int n = z->x_number;
    int i=0, d=0;
    while (n--)
    {
        x->x_onlist[i] = x->x_drawnlist[i]= d;
        SETFLOAT(&x->x_listout[i], d);
        i++;
    }
    (*z->x_gui.x_draw)(z, z->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    binbuf_clear(&t->b_binbuf);
    textbuf_senditup(t);
}

static void sradio_bang(t_sradio *x)
{
    t_radio *z= (t_radio *)&x->x_radio;
    outlet_list(z->x_gui.x_obj.ob_outlet, 0, z->x_number, x->x_listout);
    if(z->x_gui.x_fsf.x_snd_able && z->x_gui.x_snd->s_thing)
        pd_list(z->x_gui.x_snd->s_thing, 0, z->x_number, x->x_listout);
}

static void sradio_fout(t_sradio *x, t_floatarg f)
{
    t_radio *z= (t_radio *)&x->x_radio;
    int i=(int)f;
    if(i < 0)
        i = 0;
    if(i >= z->x_number)
        i = z->x_number - 1;
    x->x_onlist[i] = x->x_drawnlist[i] == 1 ? 0 : 1;
    SETFLOAT(&x->x_listout[i], (int)x->x_onlist[i]);
    (*z->x_gui.x_draw)(z, z->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);

}

static void sradio_float(t_sradio *x, t_floatarg f)
{
    t_radio *z= (t_radio *)&x->x_radio;
    int i= ( (int) f ) % z->x_number;
    if(i < 0)
        i = 0;
    outlet_float(z->x_gui.x_obj.ob_outlet, x->x_onlist[i]);
    if(z->x_gui.x_fsf.x_snd_able && z->x_gui.x_snd->s_thing)
        pd_float(z->x_gui.x_snd->s_thing, x->x_onlist[i]);
    if (x->x_focflag == 1)
    {
        x->x_foc = i;
        if (x->x_foc != x->x_old )
            (*z->x_gui.x_draw)(z, z->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
        x->x_old = x->x_foc;
    }
}

static void sradio_print(t_sradio *x)
{
    t_radio *z= (t_radio *)&x->x_radio;
    post("------------------------[sradio info]------------------------");
    post("num: %d\ndrawn: %d\npos: %d\nval: %d\nfoc: %d\nold: %d\nfflag: %d\nkeep:%d",
    z->x_number, z->x_drawn, x->x_pos, x->x_val, 
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
        postatom(z->x_number, x->x_listout);
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
                outv[k] = vec[start+k];
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
    t_radio *z= (t_radio *)&x->x_radio;
    int xx = (int)xpos - (int)text_xpix(&z->x_gui.x_obj, z->x_gui.x_glist);
    float f = (t_float)(xx / z->x_gui.x_w);
    if (!shift)
    {
        sradio_fout(x, f);
    }
    else
    {
        textbuf_open(&x->x_textbuf);
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
    t_radio *z= (t_radio *)&x->x_radio;
    t_canvas *canvas=glist_getcanvas(z->x_gui.x_glist);
    int i, n = z->x_number;
    if (foc)
        x->x_focflag = 1;
    else
    {
        x->x_focflag = 0;
        for (i=0;i<n;i++)
            sys_vgui(".x%lx.c itemconfigure %lxFOC%d -outline #%06x\n", canvas, z, i, z->x_gui.x_bcol);
    }
}

static void sradio_step(t_sradio *x, int f)
{
    t_radio *z= (t_radio *)&x->x_radio;
    if(x->x_focflag==0)
        sradio_focus(x,1);
    int pval = x->x_foc;
    int nval;
    pval += (int) f;
    nval = pval % z->x_number;
    sradio_float(x, nval);
}

static void sradio_next(t_sradio *x)
{
    sradio_step(x, 1);
}

static void sradio_prev(t_sradio *x)
{
    sradio_step(x, -1);
}

static void sradio_number(t_sradio *x, t_floatarg num)
{
    t_radio *z= (t_radio *)&x->x_radio;
    int n = (int) num;
    n = n < 1 ? 1 : n > IEM_RADIO_MAX ? IEM_RADIO_MAX : n;
    radio_number(z, n);
    if(n != z->x_number)
        sradio_resizer(x, n);
}

static void sradio_orientation(t_sradio *x, t_floatarg forient)
{
    radio_orientation(&x->x_radio, forient);
}

static void sradio_size(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
    radio_size(&x->x_radio, s, ac, av);
}

static void sradio_delta(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
    radio_delta(&x->x_radio, s, ac, av);
}

static void sradio_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
    radio_pos(&x->x_radio, s, ac, av);
}

static void sradio_color(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
    radio_color(&x->x_radio, s, ac, av);
}

static void sradio_send(t_sradio *x, t_symbol *s)
{
    radio_send(&x->x_radio, s);
}

static void sradio_receive(t_sradio *x, t_symbol *s)
{
    radio_receive(&x->x_radio, s);
}

static void sradio_label(t_sradio *x, t_symbol *s)
{
    radio_label(&x->x_radio, s);
}

static void sradio_label_pos(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
    radio_label_pos(&x->x_radio, s, ac, av);
}

static void sradio_label_font(t_sradio *x, t_symbol *s, int ac, t_atom *av)
{
    radio_label_font(&x->x_radio, s, ac, av);
}

static void *sradio_donew(t_symbol *s, int argc, t_atom *argv, int old)
{
    t_sradio *x = (t_sradio *)pd_new(sradio_class);
    x->x_radio = radio_donew(s, argc, argv, old);
    t_radio *z= (t_radio *)&x->x_radio;
    x->x_keep = 1;
    sradio_resizer(x, z->x_number);

    // text
    t_symbol *asym = gensym("#A");
    textbuf_init(&x->x_textbuf, gensym("textfile"));
    /* bashily unbind #A -- this would create garbage if #A were
    multiply bound but we believe in this context it's at most
    bound to whichever text_define or array was created most recently */
    asym->s_thing = 0;
    /* and now bind #A to us to receive following messages in the
    saved file or copy buffer */
    pd_bind(&z->x_gui.x_obj.ob_pd, asym);
    
    return (x);
}

static void *sradio_new(t_symbol *s, int argc, t_atom *argv)
{
    return (sradio_donew(s, argc, argv, 0));
}

static void sradio_free(t_sradio *x)
{
    radio_free(&x->x_radio);
    textbuf_free(&x->x_textbuf);
    freebytes(x->x_onlist,    sizeof(&x->x_onlist));
    freebytes(x->x_drawnlist, sizeof(&x->x_drawnlist));
    freebytes(x->x_listout,   sizeof(&x->x_listout));
}

static void sradio_open(t_sradio *x)
{
    textbuf_open(&x->x_textbuf);
}

static void sradio_close(t_sradio *x)
{
    textbuf_close(&x->x_textbuf);
}
static void sradio_write(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
    textbuf_write(&x->x_textbuf, s, argc, argv);
}
static void sradio_read(t_sradio *x, t_symbol *s, int argc, t_atom *argv)
{
    textbuf_read(&x->x_textbuf, s, argc, argv);
}

static void sradio_addline(t_sradio *x, t_symbol *s, int argc, t_atom *argv) 
{
    sradio_preset(x, gensym("preset"), argc, argv);
}

void g_sradio_setup(void)
{
  sradio_class = class_new(gensym("sradio"), (t_newmethod)sradio_new,
    (t_method)sradio_free, sizeof(t_sradio), 0, A_GIMME, 0);

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
  class_addmethod(sradio_class, (t_method)sradio_open,
    gensym("open"), 0);
  class_addmethod(sradio_class, (t_method)sradio_close,
    gensym("close"), 0);
  class_addmethod(sradio_class, (t_method)sradio_addline,
    gensym("addline"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_next,
    gensym("next"), A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_store,
    gensym("store"), A_DEFFLOAT, A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_recall,
    gensym("recall"), A_DEFFLOAT, A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_flush,
    gensym("flush"), A_SYMBOL, A_NULL);
  class_addmethod(sradio_class, (t_method)sradio_write,
    gensym("write"), A_GIMME, 0);
  class_addmethod(sradio_class, (t_method)sradio_read,
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

  sradio_widgetbehavior.w_getrectfn  = radio_getrect;
  sradio_widgetbehavior.w_displacefn = iemgui_displace;
  sradio_widgetbehavior.w_selectfn   = iemgui_select;
  sradio_widgetbehavior.w_activatefn = NULL;
  sradio_widgetbehavior.w_deletefn   = iemgui_delete;
  sradio_widgetbehavior.w_visfn      = iemgui_vis;
  sradio_widgetbehavior.w_clickfn    = sradio_newclick;
  class_setwidget(sradio_class, &sradio_widgetbehavior);
  class_sethelpsymbol(sradio_class, gensym("sradio"));
  class_setsavefn(sradio_class, radio_save);
  class_setpropertiesfn(sradio_class, radio_properties);

  post("[sradio] version 0.3 ~ fdch");

}
