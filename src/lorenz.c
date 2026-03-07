/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "fdLib.h"

/*
 * code for the "lorenz" pd class.
 * [lorenz] runs the equations for the lorenz system as in (https://en.wikipedia.org/wiki/Lorenz_system)
 * developed by Fede Camara Halac (fch226@nyu.edu)
 * @ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
 * Released under the GNU General Public License. 
 */

static t_class *lorenz_class;

typedef struct lorenz
{
  t_object  x_ob;
  t_outlet *x_outlet0;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  double    x_x, x_y, x_z;
  double    x_inita, x_initb, x_initc, x_inith;
  int       x_count, x_type;
  t_atom    x_raw[4];
} t_lorenz;

static void lorenz_doit(t_lorenz *x)
{
  double x0, y0, z0, x1, y1, z1;
  x0 = x->x_x;
  y0 = x->x_y;
  z0 = x->x_z;
  x1 = x0 + x->x_inith * x->x_inita * (y0 - x0);
  y1 = y0 + x->x_inith * (x0 * (x->x_initb - z0) - y0);
  z1 = z0 + x->x_inith * (x0 * y0 - x->x_initc * z0);
  x->x_x = x1;
  x->x_y = y1;
  x->x_z = z1;
  SETFLOAT(&x->x_raw[0], x->x_x);
  SETFLOAT(&x->x_raw[1], x->x_y);
  SETFLOAT(&x->x_raw[2], x->x_z);
  SETFLOAT(&x->x_raw[3], x->x_count++);
}

static void lorenz_bang(t_lorenz *x)
{
  lorenz_doit(x);
  if (x->x_count > LORMAX)
    outlet_list(x->x_outlet0, 0, 4, x->x_raw);
  outlet_float(x->x_outlet2, x->x_count);
}

static void lorenz_print(t_lorenz *x, t_floatarg f)
{
  int max = (int)f;
  for (int i = 0; i < max; i++)
  {
    lorenz_doit(x);
    if (x->x_count > LORMAX)
      outlet_list(x->x_outlet1, 0, 4, x->x_raw);
    outlet_float(x->x_outlet2, x->x_count);
  }
}

static void lorenz_type(t_lorenz *x, t_floatarg ftype)
{
  int type = ftype ? (int)ftype : 0;
  if (type < 0)
    type = 0;
  if (type > 20)
    type = 19;
  x->x_inita = lorenz_types[type][0];
  x->x_initb = lorenz_types[type][1];
  x->x_initc = lorenz_types[type][2];
  x->x_inith = lorenz_types[type][3];
  x->x_x = (double)lorenz_types[type][4];
  x->x_y = (double)lorenz_types[type][5];
  x->x_z = (double)lorenz_types[type][6];
}

static void lorenz_reset(t_lorenz *x)
{
  lorenz_type(x, x->x_type);
  x->x_count = 0;
}

static void lorenz_print_types(t_lorenz *x)
{
  for (int i=0; i<20; i++)
  {
    t_atom params[8];
    SETFLOAT(&params[0], i);
    for (int j=1; j<8; j++)
      SETFLOAT(&params[j], lorenz_types[i][j]);
    outlet_list(x->x_outlet1, gensym("list"), 8, params);
  }
}

static void lorenz_inita(t_lorenz *x, t_floatarg f)
{ x->x_inita = (double)f; }

static void lorenz_initb(t_lorenz *x, t_floatarg f)
{ x->x_initb = (double)f; }

static void lorenz_initc(t_lorenz *x, t_floatarg f)
{ x->x_initc = (double)f; }

static void lorenz_inith(t_lorenz *x, t_floatarg f)
{ x->x_inith = (double)f; }

static void lorenz_initx(t_lorenz *x, t_floatarg f)
{ x->x_x = (double)f; }

static void lorenz_inity(t_lorenz *x, t_floatarg f)
{ x->x_y = (double)f; }

static void lorenz_initz(t_lorenz *x, t_floatarg f)
{ x->x_z = (double)f; }

static void *lorenz_new()
{
  t_lorenz *x = (t_lorenz *)pd_new(lorenz_class);
  x->x_outlet0 = outlet_new(&x->x_ob, gensym("list"));
  x->x_outlet1 = outlet_new(&x->x_ob, gensym("list"));
  x->x_outlet2 = outlet_new(&x->x_ob, gensym("float"));
  x->x_type = 0;
  lorenz_reset(x);
  return (void *)x;
}

void lorenz_setup(void)
{
  lorenz_class = class_new(gensym("lorenz"), (t_newmethod)lorenz_new, 0,
                           sizeof(t_lorenz), CLASS_DEFAULT, A_NULL);

  class_addbang(lorenz_class, lorenz_bang);
  class_addmethod(lorenz_class, (t_method)lorenz_inita,
                  gensym("inita"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_initb,
                  gensym("initb"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_initc,
                  gensym("initc"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_inith,
                  gensym("inith"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_initx,
                  gensym("initx"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_inity,
                  gensym("inity"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_initz,
                  gensym("initz"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_type,
                  gensym("type"), A_DEFFLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_print,
                  gensym("print"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_reset,
                  gensym("reset"),A_NULL);
  class_addmethod(lorenz_class, (t_method)lorenz_print_types,
                  gensym("print-types"),A_NULL);
}
