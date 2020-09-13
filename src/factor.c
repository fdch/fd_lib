/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct factor {
  t_object x_obj;
  t_float x_f;
} t_factor;

static t_class *factor_class;


static void *factor_new(t_floatarg f) {
  t_factor *x = (t_factor *)pd_new(factor_class);
  outlet_new(&x->x_obj, &s_float);
  if (!f)
    floatinlet_new(&x->x_obj, &x->x_f);
  else
    x->x_f = f;
  return (void *)x;
}



static void factor_bang(t_factor *x) {
  if (x->x_f >= 1)
    outlet_float(x->x_obj.ob_outlet, factorial(x->x_f));
  else
    outlet_float(x->x_obj.ob_outlet, 1);
}


static void factor_float(t_factor *x, t_float f) {
  x->x_f = f >= 1 ? f : 1.;
}

void factor_setup() {
  factor_class = class_new(gensym("factor"), (t_newmethod)factor_new, 0, sizeof(t_factor), CLASS_DEFAULT, A_DEFFLOAT, A_NULL);
  class_addbang(factor_class, factor_bang);
  class_addfloat(factor_class, factor_float);
  class_addcreator((t_newmethod)factor_new, gensym("n!"), A_DEFFLOAT, A_NULL);
}
