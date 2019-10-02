/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

static t_class *siginfo_class;

typedef struct siginfo {
  t_object x_ob;
  t_outlet *x_outlet0;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  double xcheckprev, ycheckprev, zcheckprev;
  double xprev, yprev, zprev;
  double xcurr, ycurr, zcurr;
  double dist;
  int ref, xup, yup, zup;
} t_siginfo;

static void siginfo_list(t_siginfo *x, t_symbol *s, int argc, t_atom *argv) {
  double xpre, ypre, zpre, xdiffn, ydiffn, zdiffn, t_dist;
  t_atom previous[3], cook[7], raw[4];
  x->ref = atom_getfloatarg(0,argc,argv);
  x->xcurr = atom_getfloatarg(1,argc,argv);
  x->ycurr = atom_getfloatarg(2,argc,argv);
  x->zcurr = atom_getfloatarg(3,argc,argv);
  xpre = x->xprev;
  ypre = x->yprev;
  zpre = x->zprev;
  xdiffn = x->xcurr - xpre;
  ydiffn = x->ycurr - ypre;
  zdiffn = x->zcurr - zpre;
  x->xup = (xdiffn >= 0 ) ? 1 : 0;
  x->yup = (ydiffn >= 0 ) ? 1 : 0;
  x->zup = (zdiffn >= 0 ) ? 1 : 0;
  t_dist = sqrt( (xdiffn * xdiffn) + (ydiffn * ydiffn) + (zdiffn * zdiffn) );
  x->xcheckprev = x->xprev;
  x->ycheckprev = x->yprev;
  x->zcheckprev = x->zprev;
  x->xprev = x->xcurr;
  x->yprev = x->ycurr;
  x->zprev = x->zcurr;
  x->dist = t_dist;
  SETFLOAT(&previous[0], (double) x->xcheckprev); 
  SETFLOAT(&previous[1], (double) x->ycheckprev); 
  SETFLOAT(&previous[2], (double) x->zcheckprev); 
  SETFLOAT(&cook[0], (int) x->xup);
  SETFLOAT(&cook[1], (int) x->yup);
  SETFLOAT(&cook[2], (int) x->zup);
  SETFLOAT(&cook[3], (double) xdiffn);
  SETFLOAT(&cook[4], (double) ydiffn);
  SETFLOAT(&cook[5], (double) zdiffn);
  SETFLOAT(&cook[6], (double) x->dist);
  SETFLOAT(&raw[0], (int) x->ref);
  SETFLOAT(&raw[1], (double) x->xcurr);
  SETFLOAT(&raw[2], (double) x->ycurr);
  SETFLOAT(&raw[3], (double) x->zcurr);
  outlet_list(x->x_outlet0, 0, 4, raw);
  outlet_list(x->x_outlet1, 0, 7, cook);
  outlet_list(x->x_outlet2, 0, 3, previous);
  
}


static void siginfo_bang(t_siginfo *x) {
  post("prev xyz: %f, %f, %f\ncurr xyz: %f, %f, %f\ndist: %f", x->xcheckprev, x->ycheckprev, x->zcheckprev, x->xcurr,x->ycurr,x->zcurr, x->dist);
}

static void siginfo_reset(t_siginfo *x) {
  x->ref = x->xcurr = x->ycurr = x->zcurr = x->dist = 0;
  x->xprev = x->yprev = x->zprev = 0;
  x->xup = x->yup = x->zup = 0;
  x->xcheckprev =  x->ycheckprev = x->zcheckprev = 0;
}


static void *siginfo_new() {
  t_siginfo *x = (t_siginfo *)pd_new(siginfo_class);
  x->ref = x->xcurr = x->ycurr = x->zcurr = x->dist = 0;
  x->xprev = x->yprev = x->zprev = 0;
  x->xcheckprev =  x->ycheckprev = x->zcheckprev = 0;
  x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
  x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
  x->x_outlet2 = outlet_new(&x->x_ob, &s_list);
  return (void *)x;
}
void siginfo_setup(void) {
  siginfo_class = class_new(gensym("siginfo"), (t_newmethod)siginfo_new, 0, sizeof(t_siginfo), CLASS_DEFAULT, 0);
  class_addbang(siginfo_class, siginfo_bang);  
  class_addlist(siginfo_class, siginfo_list);
  class_addmethod(siginfo_class, (t_method)siginfo_reset, gensym("reset"), 0);
}