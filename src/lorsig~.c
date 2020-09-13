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

/* ------------------------ lorsig~ ----------------------------- */

static t_class *lorsig_class;

typedef struct _lorsig {
  t_object x_obj;   /* obligatory header */
  t_float x_f;    /* place to hold inlet's value if it's set by message */
  t_outlet *x_outlet1;
  double x, y, z, inita, initb, initc, inith;
  double dist;
  double xmin, ymin, zmin, xmax, ymax, zmax, xnr, ynr, znr;
} t_lorsig;

static double lorsig_lorenz(t_lorsig *x) {
  double x0, y0, z0, x1, y1, z1;
  double h = x->inith; //step-size
  double a = x->inita;
  double b = x->initb;
  double c = x->initc;
  double t_dist;
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
  x->xnr = x1/(fabs(nxmin)+nxmax);
  x->ynr = y1/(fabs(nymin)+nymax);
  x->znr = z1/(fabs(nzmin)+nzmax);
  t_dist = sqrt((x->xnr * x->xnr) + (x->ynr * x->ynr)  + (x->znr * x->znr));
  x->dist = t_dist;

  return t_dist;
}
static t_int *lorsig_perform(t_int *w) {
  t_lorsig *x = (t_lorsig *)(w[1]);
  t_sample *out = (t_sample *)(w[2]);
  int n = (int)(w[3]);
  while (n--) {
    *out++ = lorsig_lorenz(x);
   }
   return (w+4);
}
static void lorsig_dsp(t_lorsig *x, t_signal **sp) {
  dsp_add(lorsig_perform, 3, x, sp[1]->s_vec, sp[0]->s_n);
}
static void lorsig_reset(t_lorsig *x) {
  x->x     = INITX;
  x->y     = INITY;
  x->z     = INITZ;
  x->inita = INITA;
  x->initb = INITB;
  x->initc = INITC;
  x->inith = INITH;
  //post("default: \n Prandlt = %f\n Rayleigh = %f\n c = %f\n ", x->inita, x->initb, x->initc);
}
static void lorsig_set(t_lorsig *x, t_symbol *s, int argc, t_atom *argv) {
  x->inita = atom_getfloatarg(0,argc, argv);
  x->initb = atom_getfloatarg(1,argc, argv);
  x->initc = atom_getfloatarg(2,argc, argv);
  x->inith = atom_getfloatarg(3,argc, argv);
  x->x = atom_getfloatarg(4,argc, argv);
  x->y = atom_getfloatarg(5,argc, argv);
  x->z = atom_getfloatarg(6,argc, argv);
}
static void lorsig_debug(t_lorsig *x) {
  t_atom raw[8];
  SETFLOAT(&raw[0], (float) x->inita);
  SETFLOAT(&raw[1], (float) x->initb);
  SETFLOAT(&raw[2], (float) x->initc);
  SETFLOAT(&raw[3], (float) x->inith);
  SETFLOAT(&raw[4], (float) x->xnr);
  SETFLOAT(&raw[5], (float) x->ynr);
  SETFLOAT(&raw[6], (float) x->znr);
  SETFLOAT(&raw[7], (float) x->dist);
  outlet_list(x->x_outlet1, 0, 8, raw);
}
static void *lorsig_new(void) {
  t_lorsig *x = (t_lorsig *)pd_new(lorsig_class);
  outlet_new(&x->x_obj, gensym("signal"));
  x->x_outlet1 = outlet_new(&x->x_obj, &s_list);
  x->x_f = 0;
  x->x     = INITX;
  x->y     = INITY;
  x->z     = INITZ;
  x->inita = INITA;
  x->initb = INITB;
  x->initc = INITC;
  x->inith = INITH;
  x->xmin = x->ymin = x->zmin = NORMMIN;
  x->xmax = x->ymax = x->zmax = NORMMAX;
  return (x);
}
void lorsig_tilde_setup(void) {
  lorsig_class = class_new(gensym("lorsig~"), (t_newmethod)lorsig_new, 0, sizeof(t_lorsig), 0, A_DEFFLOAT, 0);
  CLASS_MAINSIGNALIN(lorsig_class, t_lorsig, x_f);
  class_addmethod(lorsig_class, (t_method)lorsig_dsp, gensym("dsp"), 0);
  class_addmethod(lorsig_class, (t_method)lorsig_reset, gensym("reset"), A_NULL, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_set, gensym("set"),A_GIMME, 0);
  class_addmethod(lorsig_class, (t_method)lorsig_debug, gensym("debug"), A_NULL, A_NULL);
}
