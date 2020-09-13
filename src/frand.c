/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct frand	{
  t_object x_ob;
  t_outlet *x_outlet0;
  double a, b, result;
  unsigned long int next;
} t_frand;


static void frand_set(t_frand *x, t_symbol *selector, int argcount, t_atom *argvec) {
	int i;
	for (i = 0; i < argcount; i++)
    {
			if (argvec[i].a_type == A_FLOAT) {
			if 		(i == 0) {x->a	= (double)	argvec[i].a_w.w_float;}
			else if (i == 1) {x->b	= (double)	argvec[i].a_w.w_float;}
		}   
    }
}

static void frand_reset(t_frand *x, t_symbol *selector, t_floatarg f, t_floatarg g){
   	x->a = (f != 0) ? f : 0;
	x->b = (g != 0) ? g : 1;
	x->result = 0.0;}
	

static void frand_bang(t_frand *x) {
	double t_r, t_a, t_b;
	t_a = x->a;
	t_b = x->b;
	// t_r	=	t_a	+ (double) rand() * (t_b-t_a) / RAND_MAX;
	
	unsigned long int next = x->next;

	next = next * 1103515245 + 12345;

	t_r	=	t_a	+ ((next/65536) % 32768) * (t_b-t_a) / 32767;
	
	x->next = next;
	x->result = t_r;
	outlet_float(x->x_outlet0, x->result);
	
}


static void frand_seed(t_frand *x, t_floatarg f) {
	unsigned int seed = (unsigned int) f;
	// srand(seed);
	x->next = seed;
}



static t_class *frand_class;


static void *frand_new(t_floatarg f, t_floatarg g)	{
    t_frand *x = (t_frand *)pd_new(frand_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    // time_t t;
    // srand((unsigned) time(&t));
   	x->a = (f != 0) ? f : 0;
	x->b = (g != 0) ? g : 1;
	x->result = 0.0;
    return (void *)x;
}
void frand_setup(void)	{
    frand_class = class_new(gensym("frand"),
    (t_newmethod)frand_new, 0,
    	sizeof(t_frand), 0, A_DEFFLOAT,A_DEFFLOAT, 0);
    class_addbang(frand_class, frand_bang);
    class_addmethod(frand_class, (t_method)frand_set, gensym("set"), A_GIMME, 0);
    class_addmethod(frand_class, (t_method)frand_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(frand_class, (t_method)frand_seed, gensym("seed"), A_FLOAT, 0);
}