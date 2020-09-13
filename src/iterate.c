/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

static t_class *iterate_class;

typedef struct iterate	{
  t_object	x_ob;
  t_outlet 	*x_outlet0;
  t_outlet 	*x_outlet1;
  t_int 	x_n;

} t_iterate;

static void iterate_set(t_iterate *x, t_floatarg f) { 
	x->x_n=f>=1?(t_int)f:100;
}

static void iterate_reset(t_iterate *x){ iterate_set(x,100); }

static void iterate_bang(t_iterate *x) {


	t_int i;
	for (i=0;i<x->x_n;i++) outlet_float(x->x_outlet0, (t_int)i);

	outlet_bang(x->x_outlet1);
}

static void iterate_float(t_iterate *x, t_floatarg f) {
	iterate_set(x,f);
	iterate_bang(x);
}


static void *iterate_new(t_floatarg f)	{
    t_iterate *x = (t_iterate *)pd_new(iterate_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_symbol);
	iterate_set(x,f);
    return (void *)x;
}

void iterate_setup(void)	{
    iterate_class = class_new(gensym("iterate"),
    	(t_newmethod)iterate_new, 0,
    	sizeof(t_iterate), 0, 
    	A_DEFFLOAT, 0);

    class_addbang(iterate_class, iterate_bang);
    class_addfloat(iterate_class, iterate_float);

    class_addmethod(iterate_class, 
    	(t_method)iterate_set, 
    	gensym("set"), 
    	A_DEFFLOAT, 0);

    class_addmethod(iterate_class, 
    	(t_method)iterate_reset, 
    	gensym("reset"), 0);
}