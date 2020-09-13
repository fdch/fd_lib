/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"
 
typedef struct minimax
{
  t_object x_ob;
  t_outlet *x_outlet0;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  double minimum, maximum;
} t_minimax;


static void minimax_float(t_minimax *x, t_floatarg f)	{ 
	double min, max, nmin, nmax;                       
 
	min = x->minimum;
   	max = x->maximum;
   	 
   	nmax = (f > max) ? f : max;
	nmin = (f < min) ? f : min;
   			
   	x->minimum = nmin;
    x->maximum = nmax;
    
    outlet_float(x->x_outlet2, x->maximum); 
	outlet_float(x->x_outlet1, x->minimum); 
	outlet_float(x->x_outlet0, f);
}


static void minimax_flush(t_minimax *x, t_symbol *selector)	{
	x->maximum = NORMMIN;
	x->minimum = NORMMAX;
}

t_class *minimax_class;


static void *minimax_new(t_floatarg f)	{
    t_minimax *x = (t_minimax *)pd_new(minimax_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
	x->maximum = NORMMIN;
	x->minimum = NORMMAX;
    return (void *)x;
}

void minimax_setup(void)	{
    minimax_class = class_new(gensym("minimax"), (t_newmethod)minimax_new, 0, sizeof(t_minimax), 0, 0);
    class_addfloat(minimax_class, minimax_float);
    class_addmethod(minimax_class, (t_method)minimax_flush, gensym("flush"), A_GIMME, 0);
}