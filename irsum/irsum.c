#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

#define MAX 65536

typedef struct _irsum {
  t_object  x_obj;
  double imag[MAX], real[MAX];
  int isize;
} t_irsum;

static void irsum_imag(t_irsum *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  if (argc > MAX) {
    post("too big");
    return;
  } else {
    x->isize = argc;
    //post("argc = %d", argc);
    for (i=0; i<argc; i++)
      x->imag[i] = atom_getfloatarg(i, argc, argv);
  }
}

static void irsum_list(t_irsum *x, t_symbol *s, int argc, t_atom *argv) {
  int i, n, mid;
  double want;
  t_atom out[MAX];
  
  if (argc > MAX) {
    post("too big");
    return;
  } else if (0 < x->isize && x->isize == argc) {
    n = argc;
    for (i=0; i<argc; i++)
      x->real[i] = atom_getfloatarg(i, argc, argv);
  } else {
    post("size mismatch");
    return;
  }
  
  
  mid = n/2;
  for (i=0; i<n; i++) {
    if (i==0 || i == mid)
      want = x->real[i];
    else if (i <= mid-1)
      want = x->real[i] - x->imag[i-1];
    else //if (i >= mid+1)
      want = x->real[n-i] - x->imag[n-i-1];
    SETFLOAT(&out[i], want);
  }
  
  outlet_list(x->x_obj.ob_outlet, gensym("list"), x->isize, out);
}


static t_class *irsum_class;

void *irsum_new(t_floatarg i, t_floatarg a, t_floatarg b) {
  t_irsum *x = (t_irsum *)pd_new(irsum_class);
  outlet_new(&x->x_obj, &s_list);
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("imag"), gensym("imag"));
  return (void *)x;
}

void irsum_setup(void) {
  irsum_class = class_new(gensym("irsum"),(t_newmethod)irsum_new,0, sizeof(t_irsum), CLASS_DEFAULT,0);
  class_addmethod(irsum_class, (t_method)irsum_imag, gensym("imag"), A_GIMME,0);
  class_addlist(irsum_class, irsum_list);
}