/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

/*
code for the "mandelbrot" pd class. 
[mandelbrot] runs the equations for the mandelbrot system as in (https://en.wikipedia.org/wiki/mandelbrot_system)
developed by Fede Camara Halac (fch226@nyu.edu)
@ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
Released under the GNU General Public License. 
*/

static t_class *mandelbrot_class;

typedef struct _mandelbrot
{
    t_object x_obj;
    t_float  cr;
    t_float  ci;
    t_float  k;
    t_float  result;
} t_mandelbrot;


static void *mandelbrot_new(t_floatarg f)
{
    t_mandelbrot *x = (t_mandelbrot *)pd_new(mandelbrot_class);
    x->k=f;
    if (! x->k) {x->k=100;}
    floatinlet_new(&x->x_obj, &x->ci);
    floatinlet_new(&x->x_obj, &x->k);
    outlet_new(&x->x_obj,0);
    return (x);
}



static void mandelbrot_bang(t_mandelbrot *x)
{
   outlet_float(x->x_obj.ob_outlet,x->result); 
}

static void mandelbrot_float(t_mandelbrot *x, t_float f)
{ 
  int i = -2;
  t_float zr = 0;
  t_float zi = 0;
  t_float z2r = 0;
  t_float z2i = 0;
  x->cr = f;
  if ((x->cr < 0 ) && ( x->cr > -0.5) && (x->ci * x->ci < 0.25)){
    x->result = x->k;
  }else{
    while (((z2r + z2i) < 4 ) && (++i < x->k )) {
      z2r = zr * zr;
      z2i = zi * zi;
      zi = 2 * zr * zi + x->ci;
      zr = z2r - z2i + x->cr;
    }
    x->result = i;
  }  
  outlet_float(x->x_obj.ob_outlet,x->result);
}



void mandelbrot_setup(void)
{
    mandelbrot_class = class_new(gensym("mandelbrot"), (t_newmethod)mandelbrot_new, 0,
    	sizeof(t_mandelbrot), 0, A_DEFFLOAT, 0);
    class_addbang(mandelbrot_class, mandelbrot_bang);
    class_addfloat(mandelbrot_class, mandelbrot_float);        
/*    class_addfloat(mandelbrot_class; mandelbrot_float);*/
}
