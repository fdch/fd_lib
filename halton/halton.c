#include "m_pd.h"
#include <math.h>

typedef struct halton  {
  t_object x_ob;
  int index, base;
} t_halton;

static void halton_set(t_halton *x, t_floatarg f) {
  x->base = (int)f; 
}

static void halton_float(t_halton *x, t_floatarg f) {
  x->index = f>0?(int)f:1;
  int base = x->base;
  double frac = 1./base;
  double result=0.0;
  int i = x->index+1;
  while(i > 0) {
    result += frac * (i % base);
    i = floor(i / base);
    frac = frac / base;
  }
  outlet_float(x->x_ob.ob_outlet, result);
}

static t_class *halton_class;

void *halton_new(t_floatarg f)  {
  t_halton *x = (t_halton *)pd_new(halton_class);
  outlet_new(&x->x_ob, &s_float);
  if (f) x->base = f; else x->base = 7;
  post("base: %d",x->base);
  return (void *)x;
}

void halton_setup(void)  {
  halton_class = class_new(gensym("halton"), (t_newmethod)halton_new, 0, sizeof(t_halton), CLASS_DEFAULT, A_DEFFLOAT, 0);
  class_addfloat(halton_class, halton_float);
  class_addmethod(halton_class, (t_method)halton_set, gensym("set"), A_FLOAT, 0);
}