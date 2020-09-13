/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

/*------------------ parabola --------------------*/

typedef struct parabola  {
  t_object x_obj;
  double x, a;
  int max;
} t_parabola;

static void parabola_bang(t_parabola *x) {
  double x0, x1;
  double a = x->a;
  int f=0;
  while (f < x->max) {
    x0 = x->x;
    x1 = (1 - x0) * a * x0;
    x->x = x1;
    outlet_float(x->x_obj.ob_outlet, x->x);
    f++;
  }
}

static void parabola_float(t_parabola *x, t_floatarg f) {
  x->max = f>=1?(int)f:1;
  parabola_bang(x);
}

static void parabola_a(t_parabola *x, t_floatarg f) {
  x->a = f;
}
static void parabola_x(t_parabola *x, t_floatarg f) {
  x->x = f;
}
static void parabola_reset(t_parabola *x) {
  x->x = 1;
  x->a = 1;
}

static t_class *parabola_class;

static void *parabola_new(t_floatarg f) {
  t_parabola *x = (t_parabola *)pd_new(parabola_class);
  x->x = 1;
  x->a = 1;
  x->max = f>=1?f:1;
  outlet_new(&x->x_obj, &s_float);
  return (void *)x;
}

void parabola_setup(void) {
  parabola_class = class_new(gensym("parabola"), (t_newmethod)parabola_new, 0, sizeof(t_parabola), CLASS_DEFAULT, A_DEFFLOAT, 0);
  class_addbang(parabola_class, parabola_bang);
  class_addfloat(parabola_class, parabola_float);
  class_addmethod(parabola_class, (t_method)parabola_a, gensym("a"), A_FLOAT, A_NULL);
  class_addmethod(parabola_class, (t_method)parabola_x, gensym("x"), A_FLOAT, A_NULL);
  class_addmethod(parabola_class, (t_method)parabola_reset, gensym("reset"), A_NULL, A_NULL);
}