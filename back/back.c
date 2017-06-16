/* This is a hack to the x_scalar.c object in Pd-0.47-1, with extra stuff by Fede Camara Halac 2017 */

#include "m_pd.h"
#include "g_canvas.h"
#include <string.h>
#include <stdio.h>

extern t_pd *newest;
/* Dont know where to put this */
static t_symbol *x_fname, *x_toname=NULL, *x_prevname=NULL;
static int backcount = 0;
static t_class *back_define_class;

/* Taken from g_readwrite.c */
static void glist_back_write(t_glist *x, t_symbol *filename, t_symbol *format)
{
  int cr = 0;
  t_binbuf *b;
  char buf[MAXPDSTRING];
  t_canvas *canvas = glist_getcanvas(x);
  canvas_makefilename(canvas, filename->s_name, buf, MAXPDSTRING);
  if (!strcmp(format->s_name, "cr"))
    cr = 1;
  else if (*format->s_name)
    error("qlist_read: unknown flag: %s", format->s_name);
  b = glist_writetobinbuf(x, 1);
  if (b)
  {
    if (binbuf_write(b, buf, "", cr))
      error("%s: write failed", filename->s_name);
    binbuf_free(b);
  }
}
static void putscalar(t_glist *x, t_symbol *templatesym)
{
  t_template *template;
  t_scalar *sc;
  template = template_findbyname(canvas_makebindsym(templatesym));
  if (!template)
  {
    pd_error(x, "back define: couldn't find template %s", templatesym->s_name);
    return;
  }
  sc = scalar_new(x, canvas_makebindsym(templatesym));
  if (!sc)
  {
    pd_error(x, "%s: couldn't create scalar", templatesym->s_name);
    return;
  }
  //gpointer_unset(x_gp);
  sc->sc_gobj.g_next = 0;
  x->gl_list = &sc->sc_gobj;
  //gpointer_setglist(x_gp, x, sc);
}

/* Taken from x_scalar.c */
static void *back_define_new(t_symbol *s, int argc, t_atom *argv)
{   
    
  t_atom a[9];
  t_canvas *x, *z = canvas_getcurrent();
  t_symbol *templatesym = &s_float;
  int keep = 0;
  
  while (argc && argv->a_type == A_SYMBOL &&
  *argv->a_w.w_symbol->s_name == '-')
  {
    if (!strcmp(argv->a_w.w_symbol->s_name, "-k"))
      keep = 1;
    else
    {
      error("back define: unknown flag ...");
      postatom(argc, argv);
    }
    argc--; argv++;
  }
  if (argc && argv->a_type == A_SYMBOL)
  {
    templatesym = argv->a_w.w_symbol;
    x_fname = templatesym; //for binding this object, see below
    argc--; argv++;
  }
  if (argc)
  {
    post("warning: back define ignoring extra argument: ");
    postatom(argc, argv);
  }
  /* make a canvas... */
  SETFLOAT(a, 0);
  SETFLOAT(a+1, 50);
  SETFLOAT(a+2, 600);
  SETFLOAT(a+3, 400);
  SETSYMBOL(a+4, s);
  SETFLOAT(a+5, 0);
  x = canvas_new(0, 0, 6, a);
  
  x->gl_owner = z;
  x->gl_private = keep;
  
  putscalar(x, templatesym);
  canvas_rename(x, x_fname, 0);
  
  newest = &x->gl_pd;
  pd_bind(newest, x_fname);
  pd_popsym(&x->gl_pd);
  
  
  x->gl_loading = 0;
  x->gl_obj.ob_pd = back_define_class;

  
  return (x);
}

/* send a pointer to the scalar to whomever is bound to the symbol */
static void back_define_send(t_glist *x, t_symbol *s)
{
  if (!s->s_thing)
    pd_error(x, "back_define_send: %s: no such object", s->s_name);
  else if (x->gl_list && pd_class(&x->gl_list->g_pd) == scalar_class)
  {
    t_gpointer gp;
    gpointer_init(&gp);
    gpointer_setglist(&gp, x, (t_scalar *)&x->gl_list->g_pd);
    pd_pointer(s->s_thing, &gp);
    gpointer_unset(&gp);
  }
  else //"Consistency check error"
    bug("back_define_send");
}

static int back_vnext(t_glist *x, t_symbol *s, t_gpointer *x_gp)
{
  t_gobj *gobj;
  t_gpointer *gp = x_gp;
  t_gstub *gs = gp->gp_stub;
  t_glist *glist;

  if (!gs)
  {
    pd_error(x, "ptrobj_next: no current pointer");
    return 0;
  }
  if (gs->gs_which != GP_GLIST)
  {
    pd_error(x, "ptrobj_next: lists only, not arrays");
    return 0;
  }
  glist = gs->gs_un.gs_glist;
  if (glist->gl_valid != gp->gp_valid)
  {
    pd_error(x, "ptrobj_next: stale pointer");
    return 0;
  }
  gobj = &gp->gp_un.gp_scalar->sc_gobj;

  if (!gobj) gobj = glist->gl_list;
  else gobj = gobj->g_next;
  while (gobj && ((pd_class(&gobj->g_pd) != scalar_class)))
      gobj = gobj->g_next;

  if (gobj)
  {
    t_scalar *sc = (t_scalar *)gobj;

    gp->gp_un.gp_scalar = sc;
    
    pd_pointer(s->s_thing, gp);
    return 1;
  }
  else
    gpointer_unset(gp);
  return 0;
}

static void back_define_flush(t_glist *x, t_symbol *s)
{
  int i = 0;
  int f = 1;
  if (!s->s_thing)
  pd_error(x, "back_define_flush: %s: no such object", s->s_name);
  else if (x->gl_list && pd_class(&x->gl_list->g_pd) == scalar_class)
  {
    t_gpointer gp;
    gpointer_init(&gp);
    gpointer_setglist(&gp, x, (t_scalar *)&x->gl_list->g_pd);
    back_vnext(x, s, &gp); /* go to the head of the list*/
    while(f) { /* iterate through all found objects*/
      f = back_vnext(x, s, &gp);
      i++;
    }
    gpointer_unset(&gp);
    post("Found %d elements in %s.", i, x_fname->s_name);
  }
  else //"Consistency check error"
    bug("back_define_flush");
  
}

/* Removed until I figure out how to use ... */
/*
static void back_define_set(t_glist *x, t_symbol *s, int argc, t_atom *argv)
{
  if (x->gl_list && pd_class(&x->gl_list->g_pd) == scalar_class)
  {
    t_binbuf *b = binbuf_new();
    int nextmsg = 0, natoms;
    t_atom *vec;
    glist_clear(x);
    binbuf_restore(b, argc, argv);
    natoms = binbuf_getnatom(b);
    vec = binbuf_getvec(b);
    canvas_readscalar(x, natoms, vec, &nextmsg, 0);
    binbuf_free(b);
  }
  else bug("back_define_set");
}

static void back_define_save(t_gobj *z, t_binbuf *bb)
{
  t_glist *x = (t_glist *)z;
  binbuf_addv(bb, "ssff", &s__X, gensym("obj"),
    (float)x->gl_obj.te_xpix, (float)x->gl_obj.te_ypix);
  binbuf_addbinbuf(bb, x->gl_obj.ob_binbuf);
  binbuf_addsemi(bb);
  if (x->gl_private && x->gl_list &&
    pd_class(&x->gl_list->g_pd) == scalar_class)
  {
    t_binbuf *b2 = binbuf_new();
    t_scalar *sc = (t_scalar *)(x->gl_list);
    binbuf_addv(bb, "ss", gensym("#A"), gensym("set"));
    canvas_writescalar(sc->sc_template, sc->sc_vec, b2, 0);
    binbuf_addbinbuf(bb, b2);
    binbuf_addsemi(bb);
    binbuf_free(b2);
  }

}
*/

/* overall creator for "scalar" objects - dispatch to "scalar define" etc */
static void *scalarobj_new(t_symbol *s, int argc, t_atom *argv) {
  if (!argc || argv[0].a_type != A_SYMBOL)
  {/* backname creation from x_array.c's 'tabname'*/
  char  backname[255];
  t_symbol *t = gensym("back");
  sprintf(backname, "%s-%d", t->s_name, backcount++);
  s = gensym(backname); argc = 1; SETSYMBOL(argv, s);
  newest = back_define_new(s, argc, argv);
  if (newest || backcount == 254) 
  post("New back object created with id: %s",s->s_name);
  else bug("scalarobj_new");
  }
  else
  {
    char *str = argv[0].a_w.w_symbol->s_name;
    if (!strcmp(str, "d") || !strcmp(str, "define"))
    {
      newest = back_define_new(s, argc-1, argv+1);
    }
    else
    {
      error("back %s: unknown function", str);
      newest = 0;
    }
  }

  return (newest);
}

static void scroll_xaxis(t_glist *x, t_float f)
{
  if (!(glist_isvisible(x))) canvas_vis(x, 1);
  if (x)
  sys_vgui(".x%lx.c xview scroll %d units \n", x, (int)f);
}

static void scroll_yaxis(t_glist *x, t_float f)
{
  if (!(glist_isvisible(x))) canvas_vis(x, 1);
  if (x)
  sys_vgui(".x%lx.c yview scroll [expr {- (%d)}] units \n", x, (int)f);
}

/* ---------------- gui temporary functions -------------------- */
static void back_define_cursor(t_glist *x, t_symbol *s)
{
  canvas_vis(x,1);
  sys_vgui(".x%lx.c configure -cursor %s\n",x,s->s_name);
}

static void back_define_background(t_glist *x, t_symbol *s)
{
  canvas_vis(x,1);
  sys_vgui(".x%lx.c configure -background %s\n", x,s->s_name);
}

static void back_define_text(t_glist *x, t_floatarg xpos, t_floatarg ypos, t_symbol *s)
{
  canvas_vis(x,1);
  sys_vgui (".x%lx.c create text %f %f -text %s\n", x,xpos, ypos, s->s_name);
}

static void back_define_shape(t_glist *x, t_symbol *s, t_floatarg x1, t_floatarg y1, t_floatarg x2, t_floatarg y2)
{
  canvas_vis(x,1);
  sys_vgui (".x%lx.c create %s %f %f %f %f\n", x,s->s_name,x1,y1,x2,y2);
}

static void back_define_delete(t_glist *x)
{
  canvas_vis(x,1);
  sys_vgui(".x%lx.c delete all\n", x);
}

/* ---------------- canvas functions -------------------- */
static void back_define_vis(t_glist *x, t_floatarg f){canvas_vis(x, f);}

static void back_define_clear(t_glist *x) {
  t_symbol *templatesym = &s_float;
  glist_clear(x);
  putscalar(x, templatesym);
}

static void back_define_sort(t_glist *x) {
  glist_sort(x);
}

static void back_define_bind(t_glist *x, t_symbol *s) {
if (!s->s_thing) {
  post("Couldn't find %s",s->s_name);
} else if (x_prevname && !strcmp(x_toname->s_name, x_prevname->s_name)) {
  post("I'm bound to %s, use 'unbind'", x_prevname->s_name);
} else {
  x_toname = s;
  x_prevname = x_toname;
}
}

static void back_define_unbind(t_glist *x) {
    x_toname = gensym("dummy");
}

static void back_define_motion(t_float eh, t_float xpos, t_float ypos, t_float which) {
  t_symbol *s;
if (x_toname && x_toname->s_thing) {
  s = x_toname;
} else
  return;
if(!s->s_thing) {
  post("Couln't find %s", s->s_name);
} else {
  t_atom motion[3];
  SETFLOAT(&motion[0], (int)xpos);
  SETFLOAT(&motion[1], (int)ypos);
  SETFLOAT(&motion[2], (int)which);
  pd_list(x_toname->s_thing, gensym("list"), 3, motion);
}
}

static void back_define_getbounds(t_glist *x, t_symbol *s) {
if(s->s_thing) {
  t_atom xy[4];
  SETFLOAT(&xy[0], (int)x->gl_screenx1);
  SETFLOAT(&xy[1], (int)x->gl_screeny1);
  SETFLOAT(&xy[2], (int)x->gl_screenx2);
  SETFLOAT(&xy[3], (int)x->gl_screeny2);
  pd_list(s->s_thing, gensym("list"), 4, xy);
} else
  post("Couln't find %s", s->s_name);
}

/* ---------------- read, write, merge -------------------- */
static void back_define_read(t_glist *x, t_symbol *filename, t_symbol *format)
{
  glist_read(x, filename, format);
}

static void back_define_mergefile(t_glist *x, t_symbol *filename, t_symbol *format)
{
  glist_mergefile(x, filename, format);
}

static void back_define_write(t_glist *x, t_symbol *filename, t_symbol *format)
{
  glist_back_write(x, filename, format);
}

/* ---------------- memory -------------------- */
static void back_define_free(t_glist *x)
{
  pd_unbind(&x->gl_obj.ob_pd,x_fname);
  canvas_free(x);
  canvas_destroy_editor(x);
  //gpointer_unset(x_gp);
}

/* ---------------- class -------------------- */
void canvas_add_for_class(t_class *c);

/* ---------------- setup function -------------------- */
void back_setup(void ) {
  back_define_class = class_new(gensym("back define"), 0, (t_method)back_define_free, sizeof(t_canvas), CLASS_NOINLET, 0);
  canvas_add_for_class(back_define_class);
  //class_addbang(back_define_class, back_define_bang);
  class_addmethod(back_define_class, (t_method)back_define_bind, gensym("bind"), A_SYMBOL, 0);
  class_addmethod(back_define_class, (t_method)back_define_unbind, gensym("unbind"), A_NULL);
  class_addmethod(back_define_class, (t_method)back_define_getbounds, gensym("getbounds"), A_SYMBOL, 0);
  class_addmethod(back_define_class, (t_method)back_define_motion, gensym("motion"), A_FLOAT, A_FLOAT, A_FLOAT, 0);
  class_addmethod(back_define_class, (t_method)back_define_send, gensym("send"), A_SYMBOL, 0);
  class_addmethod(back_define_class, (t_method)back_define_flush, gensym("flush"), A_SYMBOL, 0);
  //class_addmethod(back_define_class, (t_method)back_define_set, gensym("set"), A_GIMME, 0);
  class_sethelpsymbol(back_define_class, gensym("back-object"));
  //class_setsavefn(back_define_class, back_define_save);
  class_addcreator((t_newmethod)scalarobj_new, gensym("back"), A_GIMME, 0);
  class_addmethod(back_define_class, (t_method)back_define_write, gensym("write"), A_SYMBOL, A_DEFSYM, 0);
  class_addmethod(back_define_class, (t_method)back_define_read, gensym("read"), A_SYMBOL, A_DEFSYM, 0);
  class_addmethod(back_define_class, (t_method)back_define_mergefile, gensym("mergefile"), A_SYMBOL, A_DEFSYM, 0);
  class_addmethod(back_define_class, (t_method)back_define_cursor, gensym("cursor"), A_SYMBOL, 0);
  class_addmethod(back_define_class, (t_method)back_define_background, gensym("background"), A_SYMBOL, 0);
  class_addmethod(back_define_class, (t_method)back_define_text, gensym("text"), A_FLOAT,A_FLOAT,A_SYMBOL, 0);
  class_addmethod(back_define_class, (t_method)back_define_shape, gensym("shape"), A_SYMBOL,A_FLOAT,A_FLOAT,A_FLOAT,A_FLOAT, 0);
  class_addmethod(back_define_class, (t_method)back_define_vis, gensym("vis"), A_FLOAT, 0);
  class_addmethod(back_define_class, (t_method)back_define_clear, gensym("clear"), 0, 0);
  class_addmethod(back_define_class, (t_method)back_define_sort, gensym("sort"), 0, 0);
  class_addmethod(back_define_class, (t_method)back_define_delete, gensym("delete"), 0, 0);
  class_addmethod(back_define_class, (t_method)scroll_xaxis, gensym("xaxis"), A_FLOAT, 0);
  class_addmethod(back_define_class, (t_method)scroll_yaxis, gensym("yaxis"), A_FLOAT, 0);
}