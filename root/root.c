#include "m_pd.h"
#include "g_canvas.h"
 
typedef struct root
{
  t_object x_ob;
  t_glist *x_glist;
} t_root;

t_class *root_class;

static void root_bang(t_root *x) {
  char buf[MAXPDSTRING];
  if(x->x_glist->gl_owner)
    snprintf(buf, MAXPDSTRING-1, ".x%lx", (t_int)canvas_getrootfor(x->x_glist->gl_owner));
  else
    snprintf(buf, MAXPDSTRING-1, ".x%lx", (t_int)x->x_glist->gl_name);
  buf[MAXPDSTRING-1]=0;
  outlet_symbol(x->x_ob.ob_outlet, gensym(buf));
}

void *root_new() {
  t_root *x = (t_root *)pd_new(root_class);
  x->x_glist = canvas_getcurrent();
  outlet_new(&x->x_ob, &s_symbol);
  return (void *)x;
}

void root_setup(void) {
    root_class = class_new(gensym("root"), (t_newmethod)root_new, 0, sizeof(t_root), 0, 0);
    class_addbang(root_class, root_bang);
}