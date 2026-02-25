/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

static t_class *colormap_class;

typedef struct _colormap
{
  t_object  x_obj;
  int       x_size;
  double   *x_r, *x_g, *x_b;
} t_colormap;

static void make_curves(t_colormap *x)
{
  int i, size;
  double r,b,g;
  x->x_r = (double *)t_getbytes(x->x_size * sizeof(double));
  x->x_g = (double *)t_getbytes(x->x_size * sizeof(double));
  x->x_b = (double *)t_getbytes(x->x_size * sizeof(double));

  size = x->x_size;

  for(i = 0; size--; i++)
  {
    b = (double)i / (double)x->x_size;
    r = sqrt(b);
    g = (-1.0*r) + 1.0;
    x->x_b[i] = (double)b;
    x->x_r[i] = (double)r;
    x->x_g[size] = (double)g;
  }

  x->x_b[0] = 0.0;
  x->x_r[0] = 0.0;
  x->x_g[0] = 0.0;
  x->x_b[x->x_size-1] = 1.0;
  x->x_r[x->x_size-1] = 1.0;
  x->x_g[x->x_size-1] = 1.0;
}

static void colormap_float(t_colormap *x, t_floatarg f)
{
  t_atom out[3];
  int i = (int)f;

  if (f < 1)
    i = 0;

  if (f > x->x_size-1)
    i = x->x_size-1;

  SETFLOAT(out+0, x->x_r[i]);
  SETFLOAT(out+1, x->x_g[i]);
  SETFLOAT(out+2, x->x_b[i]);
  outlet_list(x->x_obj.ob_outlet, gensym("list"), 3, out);
}

static void colormap_free(t_colormap *x)
{
  t_freebytes(x->x_r, (x->x_size)*sizeof(double));
  t_freebytes(x->x_g, (x->x_size)*sizeof(double));
  t_freebytes(x->x_b, (x->x_size)*sizeof(double));
}

void *colormap_new(t_floatarg f)
{
  t_colormap *x = (t_colormap *)pd_new(colormap_class);
  outlet_new(&x->x_obj, &s_list);

  x->x_size = (int)f;
  if (f < 128)
    x->x_size = 128;

  make_curves(x);
  return x;
}

void colormap_setup(void) {
  colormap_class = class_new(gensym("colormap"),(t_newmethod)colormap_new,
                             (t_method)colormap_free, sizeof(t_colormap),
                             CLASS_DEFAULT, A_DEFFLOAT, 0);
  class_addfloat(colormap_class, colormap_float);
}
