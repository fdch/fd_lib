/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

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


static void *halton_new(t_floatarg f)  {
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