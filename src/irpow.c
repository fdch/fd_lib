/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct _irpow {
  t_object x_obj;
  t_atom *out;
  int isize;
} t_irpow;

static void irpow_bang(t_irpow *x) {
  if (!x->isize) return;
  outlet_list(x->x_obj.ob_outlet, gensym("list"), x->isize, x->out);
}


static void irpow_list(t_irpow *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  float arg;
  double v;
  if (argc) {
    if (argc != x->isize) {
      x->isize = argc;
      x->out = (t_atom *)t_getbytes((argc)*sizeof(t_atom));
    }
    for (i=0; i<argc; i++) {
      arg = atom_getfloatarg(i, argc, argv);
      SETFLOAT(x->out+i, arg*arg);
    }
  }
  irpow_bang(x);
}

static t_class *irpow_class;


static void *irpow_new(void) {
  t_irpow *x = (t_irpow *)pd_new(irpow_class);
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void irpow_setup(void) {
  irpow_class = class_new(gensym("irpow"),(t_newmethod)irpow_new,0, sizeof(t_irpow), CLASS_DEFAULT,0);
  class_addlist(irpow_class, irpow_list);
}