/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct cuadratic  {
  t_object x_obj;
  double x, c;
  t_atom *xy;
  int x_max;
} t_cuadratic;

static void cuadratic_free(t_cuadratic *x) {
  t_freebytes(x->xy, (x->x_max)*sizeof(t_atom));
}


static double cuadratic_step(double x, double c) {
  double x0, x1;
  x0 = x;
  x1 = (x0 * x0) + c;
  return x1;
}

static void cuadratic_float(t_cuadratic *x, t_floatarg f) {
  int i=0,max = f>0?(int)f:1;
  if(x->x_max!=max) {
    cuadratic_free(x);
    x->xy = (t_atom *)t_getbytes(max*sizeof(t_atom));
    x->x_max = max;
  }
  for(i=0;i<max;i++)
    SETFLOAT(&x->xy[i], (x->x = cuadratic_step(x->x, x->c)));  
  outlet_list(x->x_obj.ob_outlet, 0, x->x_max, x->xy);
}

static void cuadratic_bang(t_cuadratic *x) {
  outlet_float(x->x_obj.ob_outlet, (x->x = cuadratic_step(x->x, x->c)));
}

static void cuadratic_c(t_cuadratic *x, t_float f) { x->c = (double)f; }
static void cuadratic_x(t_cuadratic *x, t_float f) { x->x = (double)f; }
static void cuadratic_reset(t_cuadratic *x) { x->x = 1.97, x->c = -2.0;}

static void cuadratic_print(t_cuadratic *x) { 
  post("c: %f, x: %f", (t_float)x->c, (t_float)x->x);
  if(x->x_max) {
    post("xy:");
    postatom(x->x_max,x->xy);
    endpost();
  }

}


static t_class *cuadratic_class;


static void *cuadratic_new() {
  t_cuadratic *x = (t_cuadratic *)pd_new(cuadratic_class);
  x->x = 1.97, x->c = -2.;
  x->x_max = 0;
  outlet_new(&x->x_obj, &s_float);
  return (void *)x;
}

void cuadratic_setup(void) {
  cuadratic_class = class_new(gensym("cuadratic"), (t_newmethod)cuadratic_new, 
    (t_method)cuadratic_free, sizeof(t_cuadratic), CLASS_DEFAULT, 0);
  class_addbang(cuadratic_class, cuadratic_bang);
  class_addfloat(cuadratic_class, cuadratic_float);
  class_addmethod(cuadratic_class, (t_method)cuadratic_c, gensym("c"), A_FLOAT, 0);
  class_addmethod(cuadratic_class, (t_method)cuadratic_x, gensym("x"), A_FLOAT, 0);
  class_addmethod(cuadratic_class, (t_method)cuadratic_reset, gensym("reset"), 0);
  class_addmethod(cuadratic_class, (t_method)cuadratic_print, gensym("print"), 0);
}