#include "m_pd.h"
#include <stdlib.h>

typedef struct cantor {
  t_object x_obj;
  t_int x_d, x_n, x_u, x_i;
  t_float x_f;
} t_cantor;

static int cantor_inner(t_cantor *x, t_int d, t_int i) {
  int in = 1;
  int y = i;
  for (int j = 0; j < d; j++) {
    in = in && !((x->x_u)*(y) >= x->x_n && (x->x_u)*(y) < (x->x_f)*(x->x_n));
     y = ((x->x_u)*y) % x->x_n;
  }
  return ((int)in);
}

static void cantor_doit(t_cantor *x) {
  t_atom set[MAXPDSTRING];
  for (int d = 0; d < x->x_d; d++)
    for (int i = 0; i < x->x_n; i++)
      SETFLOAT(&set[i], cantor_inner(x, d, i)); 
  outlet_list(x->x_obj.ob_outlet, &s_list, x->x_n, set);
}

void cantor_bang(t_cantor *x) {
  int i = x->x_i;
  outlet_float(x->x_obj.ob_outlet, cantor_inner(x, x->x_d, i));
  x->x_i++;
}

void cantor_float(t_cantor *x, t_float f) {
  int nsize = (int)f;
  x->x_n = nsize;
  cantor_doit(x);
}

void cantor_depth(t_cantor *x, t_float f) {
  x->x_d = (int)f;
}

void cantor_width(t_cantor *x, t_float f) {
  x->x_u = (int)f;
}

void cantor_coef(t_cantor *x, t_float f) {
  x->x_f = f;
}

static t_class *cantor_class;

void *cantor_new(t_floatarg f) {
  t_cantor *x = (t_cantor *)pd_new(cantor_class);
  outlet_new(&x->x_obj, &s_float);
  if (f) x->x_n = f;
  else x->x_n = 80;
  x->x_d = 5;
  x->x_u = 3;
  x->x_f = 2;
  return (void *)x;
}

void cantor_setup() {
  cantor_class = class_new(gensym("cantor"), (t_newmethod)cantor_new, 0, sizeof(t_cantor), CLASS_DEFAULT, A_DEFFLOAT, 0);
  class_addbang(cantor_class, cantor_bang);
  class_addfloat(cantor_class, cantor_float);
  class_addmethod(cantor_class, (t_method)cantor_depth, gensym("depth"), A_FLOAT, 0);
  class_addmethod(cantor_class, (t_method)cantor_width, gensym("width"), A_FLOAT, 0);
  class_addmethod(cantor_class, (t_method)cantor_coef, gensym("coef"), A_FLOAT, 0);
}