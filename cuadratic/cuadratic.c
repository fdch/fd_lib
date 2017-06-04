#include "m_pd.h"
#include "math.h"

typedef struct cuadratic  {
  t_object x_obj;
  double x, c;
} t_cuadratic;

static double cuadratic_step(double x, double c) {
  double x0, x1;
  x0 = x;
  x1 = (x0 * x0) + c;
  return x1;
}

void cuadratic_print(t_cuadratic *x, t_float max) {
  if (1 > (t_int)max)
    pd_error(x, "print message argument must be greater than 0");
  else {
    t_atom xy[(t_int)max];
    for (t_int f = 0; f < max; f++) {
      x->x = cuadratic_step(x->x, x->c);
      t_float tx = (t_float)x->x;
      SETFLOAT(&xy[f], (t_float)tx);
    }
    outlet_list(x->x_obj.ob_outlet, &s_list, max, xy);
    freebytes(&xy, sizeof(&xy)*(t_int)max);
  }
}

void cuadratic_bang(t_cuadratic *x) {
  outlet_float(x->x_obj.ob_outlet, (x->x = cuadratic_step(x->x, x->c)));
}

void cuadratic_c(t_cuadratic *x, t_float f) { x->c = (double)f; }
void cuadratic_x(t_cuadratic *x, t_float f) { x->x = (double)f; }
void cuadratic_reset(t_cuadratic *x) { x->x = 1.97, x->c = -2.;}

t_class *cuadratic_class;

void *cuadratic_new() {
  t_cuadratic *x = (t_cuadratic *)pd_new(cuadratic_class);
  x->x = 1.97, x->c = -2.;
  outlet_new(&x->x_obj, &s_float);
  return (void *)x;
}

void cuadratic_setup(void) {
  cuadratic_class = class_new(gensym("cuadratic"), (t_newmethod)cuadratic_new, 0, sizeof(t_cuadratic), CLASS_DEFAULT, 0);
  class_addbang(cuadratic_class, cuadratic_bang);
  class_addmethod(cuadratic_class, (t_method)cuadratic_c, gensym("c"), A_FLOAT, A_NULL);
  class_addmethod(cuadratic_class, (t_method)cuadratic_x, gensym("x"), A_FLOAT, A_NULL);
  class_addmethod(cuadratic_class, (t_method)cuadratic_reset, gensym("reset"), A_NULL);
  class_addmethod(cuadratic_class, (t_method)cuadratic_print, gensym("print"), A_FLOAT, A_NULL);
}