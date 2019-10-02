/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

/*
code for the "lorenz" pd class. 
[lorenz] runs the equations for the lorenz system as in (https://en.wikipedia.org/wiki/Lorenz_system)
developed by Fede Camara Halac (fch226@nyu.edu)
@ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
Released under the GNU General Public License. 
*/



static t_class *lorenz_class;

typedef struct lorenz {
  t_object x_ob;
  t_outlet *x_outlet0;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  double x, y, z, inita, initb, initc, inith;
  int count;
  t_atom raw[4];
} t_lorenz;

static void lorenz_doit(t_lorenz *x) {
  double x0, y0, z0, x1, y1, z1;
  double h = x->inith;
  double a = x->inita;
  double b = x->initb;
  double c = x->initc;
  x0 = x->x;
  y0 = x->y;
  z0 = x->z;
  x1 = x0 + h * a * (y0 - x0);
  y1 = y0 + h * (x0 * (b - z0) - y0);
  z1 = z0 + h * (x0 * y0 - c * z0);
  x->x = x1;
  x->y = y1;
  x->z = z1;
  SETFLOAT(&x->raw[0], x->x);
  SETFLOAT(&x->raw[1], x->y);
  SETFLOAT(&x->raw[2], x->z);
}

static void lorenz_bang(t_lorenz *x) {
  lorenz_doit(x);
  if (x->count > LORMAX)
    outlet_list(x->x_outlet0, 0, 3, x->raw);
  outlet_float(x->x_outlet2, x->count++);
}
static void lorenz_print(t_lorenz *x, t_floatarg f) {
  int i, max = (int)f;
  for (i = 0; i < max; i++) {
    lorenz_doit(x);
    SETFLOAT(&x->raw[3], i);
    if (x->count > LORMAX)
      outlet_list(x->x_outlet1, 0, 4, x->raw);
    outlet_float(x->x_outlet2, x->count++);
  }
}
static void lorenz_type(t_lorenz *x, t_floatarg f) {
  int tipo=(int)f;
  double prng = rand() % 10;
  switch (tipo) {
  case 1:
    x->x = INITX;x->y = INITY;x->z = INITZ;
    x->inita = 28;x->initb = 46.92;x->initc = 4;
    x->inith = INITH;
    break;
  case 2:  
    x->x = INITX;x->y = INITY;x->z = INITZ;
    x->inita = prng * INITA;x->initb = prng * INITB;x->initc = prng * 4;
    x->inith = INITH;
    break;
  default:
    x->x = INITX;x->y = INITY;x->z = INITZ;
    x->inita = INITA;x->initb = INITB;x->initc = INITC;
    x->inith = INITH;
    break;
  }
}
static void lorenz_reset(t_lorenz *x) {
  x->x = INITX;x->y = INITY;x->z = INITZ;
   x->inita = INITA;x->initb = INITB;x->initc = INITC;
  x->inith = INITH;
  x->count = 0;
}

static void lorenz_inita(t_lorenz *x, t_floatarg f) { x->inita = f; }
static void lorenz_initb(t_lorenz *x, t_floatarg f) { x->initb = f; }
static void lorenz_initc(t_lorenz *x, t_floatarg f) { x->initc = f; }
static void lorenz_inith(t_lorenz *x, t_floatarg f) { x->inith = f; }
static void lorenz_initx(t_lorenz *x, t_floatarg f) { x->x = f; }
static void lorenz_inity(t_lorenz *x, t_floatarg f) { x->y = f; }
static void lorenz_initz(t_lorenz *x, t_floatarg f) { x->z = f; }


static void *lorenz_new() {
  t_lorenz *x = (t_lorenz *)pd_new(lorenz_class);
  x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
  x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
  x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
  lorenz_reset(x);
  return (void *)x;
}
void lorenz_setup(void) {
  lorenz_class = class_new(gensym("lorenz"), (t_newmethod)lorenz_new, 0, sizeof(t_lorenz), CLASS_DEFAULT, 0);
  class_addbang(lorenz_class, lorenz_bang);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_inita, gensym("inita"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_initb, gensym("initb"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_initc, gensym("initc"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_inith, gensym("inith"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_initx, gensym("initx"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_inity, gensym("inity"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_initz, gensym("initz"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_type, gensym("type"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_print, gensym("print"), A_FLOAT, 0);
  class_addmethod(lorenz_class, 
    (t_method)lorenz_reset, gensym("reset"),0);
}