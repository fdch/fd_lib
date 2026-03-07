/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "fdLib.h"

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

static t_class *lorsig_class;

typedef struct _lorsig
{
  t_object  x_obj;
  t_outlet *x_outlet1;
  t_float   x_f;
  double    x_inita, x_initb, x_initc, x_inith;
  double    x_x, x_y, x_z;
  double    x_dist;
  double    x_xmin, x_ymin, x_zmin;
  double    x_xmax, x_ymax, x_zmax;
  double    x_xnr, x_ynr, x_znr;
  int x_type;
} t_lorsig;

static double lorsig_lorenz(t_lorsig *x)
{
  double x0, y0, z0, x1, y1, z1;
  double t_dist;
  double xmin = x->x_xmin;
  double ymin = x->x_ymin;
  double zmin = x->x_zmin;
  double xmax = x->x_xmax;
  double ymax = x->x_ymax;
  double zmax = x->x_zmax;
  double nxmin, nymin, nzmin, nxmax, nymax, nzmax;

  x0 = x->x_x;
  y0 = x->x_y;
  z0 = x->x_z;
  x1 = x0 + x->x_inith * x->x_inita * (y0 - x0);
  y1 = y0 + x->x_inith * (x0 * (x->x_initb - z0) - y0);
  z1 = z0 + x->x_inith * (x0 * y0 - x->x_initc * z0);

  nxmax = (x1 > xmax) ? x1 : xmax;
  nymax = (y1 > ymax) ? y1 : ymax;
  nzmax = (z1 > zmax) ? z1 : zmax;
  nxmin = (x1 < xmin) ? x1 : xmin;
  nymin = (y1 < ymin) ? y1 : ymin;
  nzmin = (z1 < zmin) ? z1 : zmin;
  x->x_xmin = nxmin;
  x->x_ymin = nymin;
  x->x_zmin = nzmin;
  x->x_xmax = nxmax;
  x->x_ymax = nymax;
  x->x_zmax = nzmax;
  x->x_x = x1;
  x->x_y = y1;
  x->x_z = z1;
  x->x_xnr = x1/(fabs(nxmin)+nxmax);
  x->x_ynr = y1/(fabs(nymin)+nymax);
  x->x_znr = z1/(fabs(nzmin)+nzmax);
  t_dist = sqrt((x->x_xnr * x->x_xnr) + (x->x_ynr * x->x_ynr)  + (x->x_znr * x->x_znr));
  x->x_dist = t_dist;

  return t_dist;
}

static void lorsig_type(t_lorsig *x, t_floatarg ftype)
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

static t_int *lorsig_perform(t_int *w)
{
  t_lorsig *x = (t_lorsig *)(w[1]);
  t_sample *out = (t_sample *)(w[2]);
  int n = (int)(w[3]);
  while (n--)
    *out++ = lorsig_lorenz(x);
  return w + 4;
}

static void lorsig_dsp(t_lorsig *x, t_signal **sp)
{ dsp_add(lorsig_perform, 3, x, sp[1]->s_vec, sp[0]->s_n); }

static void lorsig_reset(t_lorsig *x)
{ lorsig_type(x, x->x_type); }

static void lorsig_set(t_lorsig *x, t_symbol *s, int argc, t_atom *argv)
{
  (void)s;
  x->x_inita = atom_getfloatarg(0,argc, argv);
  x->x_initb = atom_getfloatarg(1,argc, argv);
  x->x_initc = atom_getfloatarg(2,argc, argv);
  x->x_inith = atom_getfloatarg(3,argc, argv);
  x->x_x = atom_getfloatarg(4,argc, argv);
  x->x_y = atom_getfloatarg(5,argc, argv);
  x->x_z = atom_getfloatarg(6,argc, argv);
}

static void lorsig_debug(t_lorsig *x)
{
  t_atom raw[8];
  SETFLOAT(&raw[0], (float) x->x_inita);
  SETFLOAT(&raw[1], (float) x->x_initb);
  SETFLOAT(&raw[2], (float) x->x_initc);
  SETFLOAT(&raw[3], (float) x->x_inith);
  SETFLOAT(&raw[4], (float) x->x_xnr);
  SETFLOAT(&raw[5], (float) x->x_ynr);
  SETFLOAT(&raw[6], (float) x->x_znr);
  SETFLOAT(&raw[7], (float) x->x_dist);
  outlet_list(x->x_outlet1, 0, 8, raw);
}

static void *lorsig_new(void)
{
  t_lorsig *x = (t_lorsig *)pd_new(lorsig_class);
  outlet_new(&x->x_obj, gensym("signal"));
  x->x_outlet1 = outlet_new(&x->x_obj, &s_list);
  x->x_f = 0;
  x->x_xmin = x->x_ymin = x->x_zmin = NORMMIN;
  x->x_xmax = x->x_ymax = x->x_zmax = NORMMAX;
  x->x_type = 0;
  lorsig_reset(x);
  return (void *)x;
}

void lorsig_tilde_setup(void)
{
  lorsig_class = class_new(gensym("lorsig~"), (t_newmethod)lorsig_new, 0,
                           sizeof(t_lorsig), 0, A_DEFFLOAT, 0);

  CLASS_MAINSIGNALIN(lorsig_class, t_lorsig, x_f);
  class_addmethod(lorsig_class, (t_method)lorsig_dsp,
                  gensym("dsp"), 0);
  class_addmethod(lorsig_class, (t_method)lorsig_reset,
                  gensym("reset"), A_NULL, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_set,
                  gensym("set"),A_GIMME, 0);
  class_addmethod(lorsig_class, (t_method)lorsig_debug,
                  gensym("debug"), A_NULL, A_NULL);
}
