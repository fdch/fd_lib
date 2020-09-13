/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct prandom
{
	t_object x_ob;
	t_outlet *x_outlet0;
} t_prandom;

static void prandom_bang(t_prandom *x) {
	float a, b, prand;
	a = 0;
	b = 1;
	prand = rand();
	
    outlet_float(x->x_outlet0, prand);
    
}

static t_class *prandom_class;

static void *prandom_new(t_floatarg f)
{
    t_prandom *x = (t_prandom *)pd_new(prandom_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    	/* just stick the argument in the object structure for later. */
    return (void *)x;
}
    /* this is called once at setup time, when this code is loaded into Pd. */
void prandom_setup(void)
{
    prandom_class = class_new(gensym("prandom"), (t_newmethod)prandom_new, 0, sizeof(t_prandom), 0, 0);
    class_addbang(prandom_class, prandom_bang);  
}