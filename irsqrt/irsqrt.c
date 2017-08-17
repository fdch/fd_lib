#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

typedef struct _irsqrt {
  t_object x_obj;
  t_atom *out;
  int isize;
} t_irsqrt;

static void irsqrt_bang(t_irsqrt *x) {
  if (!x->isize) return;
  outlet_list(x->x_obj.ob_outlet, gensym("list"), x->isize, x->out);
}


static void irsqrt_list(t_irsqrt *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  float arg;
  double v;
  if (argc) {
    if (argc != x->isize) {
      x->isize = argc;
      post("allocating %d",argc);
      x->out = (t_atom *)t_getbytes((argc)*sizeof(t_atom));
    }
    for (i=0; i<argc; i++)
      SETFLOAT(x->out+i, sqrt(atom_getfloatarg(i, argc, argv)));
  }
  irsqrt_bang(x);
}

static t_class *irsqrt_class;

void *irsqrt_new(void) {
  t_irsqrt *x = (t_irsqrt *)pd_new(irsqrt_class);
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void irsqrt_setup(void) {
  irsqrt_class = class_new(gensym("irsqrt"),(t_newmethod)irsqrt_new,0, sizeof(t_irsqrt), CLASS_DEFAULT,0);
  class_addlist(irsqrt_class, irsqrt_list);
}