#include "m_pd.h"
#include <math.h> // for 'fmod()'

static t_class *halton_class;

typedef struct halton  {
  t_object x_ob;
  double x_i;
  double base;
} t_halton;

static void halton_set(t_halton *x, t_floatarg f) {
  x->base=(f>1.0)?(double)f:1.0;
}

static void halton_float(t_halton *x, t_floatarg f) {  
  x->x_i   = (f > 0) ? (double) f : 1.0;
  double i = x->x_i+1;
  double b = x->base;
  double fr = 1.;
  double r = 0.0;

  while(i >= 0) {
    fr /= b;
    r  += fr * fmod(i,b);
    i  /= b;
    i--;
  }

  outlet_float(x->x_ob.ob_outlet, r);
}

void *halton_new(t_floatarg f)  {
  t_halton *x = (t_halton *)pd_new(halton_class);

  halton_set(x,f);
  outlet_new(&x->x_ob, &s_float);

  return (void *)x;
}

void halton_setup(void)  {
    halton_class = class_new(gensym("halton"),
        (t_newmethod)halton_new, 0,
        sizeof(t_halton), 0,
        A_DEFFLOAT, 0);
  // class_addbang(halton_class, halton_bang);
  class_addfloat(halton_class, halton_float);
  class_addmethod(halton_class,
    (t_method)halton_set,
    gensym("set"),
    A_FLOAT, 0);
}