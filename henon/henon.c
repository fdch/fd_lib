#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

#define N 1000

typedef struct _henon {
  t_object  x_obj;
  t_float  x_a, x_b;
  double x_ox, x_oy;
  int  x_i;
  t_atom x_ixy[3];
} t_henon;

static void henon_doit(t_henon *x) {
  double a, b, ox, oy, xnew, ynew;
  int i=x->x_i, px, py;
  ox = x->x_ox;
  oy = x->x_oy;
  a = x->x_a;
  b = x->x_b;
  xnew = oy - (a*(ox*ox)) + 1.0;
  ynew = b * ox;
  px = (int)((ox + 2.0) * 150.0);
  py = (int)((oy + 0.4) * 400.0);
  x->x_ox = xnew;
  x->x_oy = ynew;
  SETFLOAT(&x->x_ixy[0], ox);
  SETFLOAT(&x->x_ixy[1], oy);
  SETFLOAT(&x->x_ixy[2], i);
}

static void henon_bang(t_henon *x) {
  henon_doit(x);
  SETFLOAT(&x->x_ixy[2], 0);
  outlet_list(x->x_obj.ob_outlet, gensym("list"), 3, x->x_ixy);
}

static void henon_print(t_henon *x, t_floatarg f) {
  if (f > 0) {
    x->x_i = 0;
    while (f--) {
      henon_doit(x);
      outlet_list(x->x_obj.ob_outlet, gensym("list"), 3, x->x_ixy);
      x->x_i++;
    }
  } else
    henon_bang(x);
}

static void henon_reset(t_henon *x) {
  x->x_a = 7./5.;
  x->x_b = .3;
  x->x_ox = x->x_oy = 0;
  SETFLOAT(&x->x_ixy[0], 0);
  SETFLOAT(&x->x_ixy[1], 0);
  SETFLOAT(&x->x_ixy[2], 0);
}

static t_class *henon_class;

void *henon_new(t_floatarg i, t_floatarg a, t_floatarg b) {
  t_henon *x = (t_henon *)pd_new(henon_class);
  x->x_i = i>0?(int)i:N;
  if (!a) {
    floatinlet_new(&x->x_obj, &x->x_a);
  }
  if (!b) {
    floatinlet_new(&x->x_obj, &x->x_b);
  }
  x->x_a = a>0?a:(7./5.);
  x->x_b = b>0?b:.3;
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void henon_setup(void) {
  henon_class = class_new(gensym("henon"),(t_newmethod)henon_new,0, sizeof(t_henon), CLASS_DEFAULT,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);
  class_addbang(henon_class, henon_bang);
  class_addmethod(henon_class, (t_method)henon_print, gensym("print"), A_FLOAT, A_NULL);
  class_addmethod(henon_class, (t_method)henon_reset, gensym("reset"), 0);
  class_addmethod(henon_class, (t_method)henon_reset, gensym("clear"), 0);
}