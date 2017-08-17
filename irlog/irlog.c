#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

typedef struct _irlog {
  t_object x_obj;
  t_atom *out;
  int isize;
} t_irlog;

static void irlog_bang(t_irlog *x) {
  if (!x->isize) return;
  outlet_list(x->x_obj.ob_outlet, gensym("list"), x->isize, x->out);
}


static void irlog_list(t_irlog *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  float arg;
  double v;
  if (argc) {
    if (argc != x->isize) {
      x->isize = argc;
      x->out = (t_atom *)t_getbytes((argc)*sizeof(t_atom));
    }
    for (i=0; i<argc; i++) {
      arg = atom_getfloatarg(i, argc, argv);
      SETFLOAT(x->out+i, arg==0?0:log(arg));
    }
  }
  irlog_bang(x);
}

static t_class *irlog_class;

void *irlog_new(void) {
  t_irlog *x = (t_irlog *)pd_new(irlog_class);
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void irlog_setup(void) {
  irlog_class = class_new(gensym("irlog"),(t_newmethod)irlog_new,0, sizeof(t_irlog), CLASS_DEFAULT,0);
  class_addlist(irlog_class, irlog_list);
}