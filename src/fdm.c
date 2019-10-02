/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

static t_class *plusplus_class, *multmult_class;

typedef struct plusplus {
  t_object x_obj;
  t_float x_sum;
} t_plusplus;

typedef struct multmult {
  t_object x_obj;
  t_float x_sum;
} t_multmult;

static void plusplus_float(t_plusplus *x, t_float f) {
  outlet_float(x->x_obj.ob_outlet, x->x_sum += f);
}
static void multmult_float(t_multmult *x, t_float f) {
  outlet_float(x->x_obj.ob_outlet, x->x_sum *= (f)?f:1);
}
static void plusplus_clear(t_plusplus *x) {
  x->x_sum = 0;
}
static void multmult_clear(t_plusplus *x) {
  x->x_sum = 1.;
}
static void *multmult_new() {
  t_multmult *x = (t_multmult *)pd_new(multmult_class);
  outlet_new(&x->x_obj, &s_float);
  multmult_clear(x);
  return (x);
}
static void *plusplus_new() {
  t_plusplus *x = (t_plusplus *)pd_new(plusplus_class);
  outlet_new(&x->x_obj, &s_float);
  plusplus_clear(x);
  return (x);
}

void fdm_setup(void) {
  t_symbol *fdm_help = gensym("fdm-help");
  plusplus_class = class_new(gensym("fdm"), (t_newmethod)plusplus_new, 0,
    sizeof(t_plusplus), CLASS_DEFAULT, 0);
  class_addcreator((t_newmethod)plusplus_new, gensym("++"),0);
  class_addfloat(plusplus_class, plusplus_float);
  class_sethelpsymbol(plusplus_class, fdm_help);
  class_addmethod(plusplus_class, (t_method)plusplus_clear, gensym("clear"), 0);
  
  multmult_class = class_new(gensym("fdm"), (t_newmethod)multmult_new, 0,
    sizeof(t_multmult), CLASS_DEFAULT, 0);
  class_addcreator((t_newmethod)multmult_new, gensym("**"),0);
  class_addfloat(multmult_class, multmult_float);
  class_sethelpsymbol(multmult_class, fdm_help);
  class_addmethod(multmult_class, (t_method)multmult_clear, gensym("clear"), 0);
}