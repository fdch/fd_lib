/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

static t_class *lornorm_class;
typedef struct lornorm {
  t_object x_ob;
  t_outlet *x_outlet0;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  double x, y, z, inita, initb, initc, inith;
  double xmin, ymin, zmin, xmax, ymax, zmax;
  int count;
  t_atom raw[4];
} t_lornorm;

static void lornorm_doit(t_lornorm *x) {
  double x0, y0, z0, x1, y1, z1;
  double h = x->inith;
  double a = x->inita;
  double b = x->initb;
  double c = x->initc;
  double xmin = x->xmin;
  double ymin = x->ymin;
  double zmin = x->zmin;
  double xmax = x->xmax;
  double ymax = x->ymax;
  double zmax = x->zmax;
  double nxmin, nymin, nzmin, nxmax, nymax, nzmax;
  x0 = x->x;
  y0 = x->y;
  z0 = x->z;
  x1 = x0 + h * a * (y0 - x0);
  y1 = y0 + h * (x0 * (b - z0) - y0);
  z1 = z0 + h * (x0 * y0 - c * z0);
  nxmax = (x1 > xmax) ? x1 : xmax;
  nymax = (y1 > ymax) ? y1 : ymax;
  nzmax = (z1 > zmax) ? z1 : zmax;
  nxmin = (x1 < xmin) ? x1 : xmin;
  nymin = (y1 < ymin) ? y1 : ymin;
  nzmin = (z1 < zmin) ? z1 : zmin;
  x->xmin = nxmin;
  x->ymin = nymin;
  x->zmin = nzmin;
  x->xmax = nxmax;
  x->ymax = nymax;
  x->zmax = nzmax;
  x->x = x1;
  x->y = y1;
  x->z = z1;
  SETFLOAT(&x->raw[0], x1/(fabs(nxmin)+nxmax));
  SETFLOAT(&x->raw[1], y1/(fabs(nymin)+nymax));
  SETFLOAT(&x->raw[2], z1/(fabs(nzmin)+nzmax));
}

static void lornorm_bang(t_lornorm *x) {
  lornorm_doit(x);
  if (x->count > LORMAX)
    outlet_list(x->x_outlet0, 0, 3, x->raw);
  outlet_float(x->x_outlet2, x->count++);
}
static void lornorm_print(t_lornorm *x, t_floatarg f) {
  int i, max = (int)f;
  for (i = 0; i < max; i++) {
    lornorm_doit(x);
    SETFLOAT(&x->raw[3], i);
    if (x->count > LORMAX)
      outlet_list(x->x_outlet1, 0, 4, x->raw);
    outlet_float(x->x_outlet2, x->count++);
  }
}
static void lornorm_type(t_lornorm *x, t_floatarg f) {
  int tipo=(int)f;
  double prng = rand() % 10;
  switch (tipo) {
  case 1:
    x->x = 0.1;x->y = 0;x->z = 0; 
    x->inita = 28;x->initb = 46.92;x->initc = 4;
    x->inith = 0.01;
    break;
  case 2:  
    x->x = 0.1;x->y = 0;x->z = 0;
    x->inita = prng * 10;x->initb = prng * 28.0;x->initc = prng * 4;
    x->inith = 0.01;
    break;
  default:
    x->x = 0.1;x->y = 0;x->z = 0; 
    x->inita = 10.0;x->initb = 28.0;x->initc = 8.0 / 3.0;
    x->inith = 0.01;
    break;
  }
}
static void lornorm_reset(t_lornorm *x) {
  x->x = 0.1;x->y = 0;x->z = 0;
  x->inita = 10.0;x->initb = 28.0;x->initc = 8.0 / 3.0;
  x->inith = 0.01;
  x->count = 0;
  x->xmin = x->ymin = x->zmin = 0.0;
  x->xmax = x->ymax = x->zmax = 1000000.0;
}

static void lornorm_inita(t_lornorm *x, t_floatarg f) { x->inita = f; }
static void lornorm_initb(t_lornorm *x, t_floatarg f) { x->initb = f; }
static void lornorm_initc(t_lornorm *x, t_floatarg f) { x->initc = f; }
static void lornorm_inith(t_lornorm *x, t_floatarg f) { x->inith = f; }
static void lornorm_initx(t_lornorm *x, t_floatarg f) { x->x = f; }
static void lornorm_inity(t_lornorm *x, t_floatarg f) { x->y = f; }
static void lornorm_initz(t_lornorm *x, t_floatarg f) { x->z = f; }


static void *lornorm_new() {
  t_lornorm *x = (t_lornorm *)pd_new(lornorm_class);
  x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
  x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
  x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
  lornorm_reset(x);
  return (void *)x;
}
void lornorm_setup(void) {
  lornorm_class = class_new(gensym("lornorm"), (t_newmethod)lornorm_new, 0, sizeof(t_lornorm), CLASS_DEFAULT, 0);
  class_addbang(lornorm_class, lornorm_bang);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_inita, gensym("inita"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_initb, gensym("initb"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_initc, gensym("initc"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_inith, gensym("inith"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_initx, gensym("initx"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_inity, gensym("inity"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_initz, gensym("initz"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_type, gensym("type"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_print, gensym("print"), A_FLOAT, 0);
  class_addmethod(lornorm_class, 
    (t_method)lornorm_reset, gensym("reset"),0);
}