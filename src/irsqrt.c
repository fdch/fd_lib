/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct _irsqrt {
  t_object x_obj;
  t_atom *out;
  int isize;
} t_irsqrt;

static void irsqrt_bang(t_irsqrt *x) {
  if (!x->isize) return;
  outlet_list(x->x_obj.ob_outlet, gensym("list"), x->isize, x->out);
}


static void irsqrt_list(t_irsqrt *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  float arg;
  double v;
  if (argc) {
    if (argc != x->isize) {
      x->isize = argc;
      post("allocating %d",argc);
      x->out = (t_atom *)t_getbytes((argc)*sizeof(t_atom));
    }
    for (i=0; i<argc; i++)
      SETFLOAT(x->out+i, sqrt(atom_getfloatarg(i, argc, argv)));
  }
  irsqrt_bang(x);
}

static t_class *irsqrt_class;


static void *irsqrt_new(void) {
  t_irsqrt *x = (t_irsqrt *)pd_new(irsqrt_class);
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void irsqrt_setup(void) {
  irsqrt_class = class_new(gensym("irsqrt"),(t_newmethod)irsqrt_new,0, sizeof(t_irsqrt), CLASS_DEFAULT,0);
  class_addlist(irsqrt_class, irsqrt_list);
}