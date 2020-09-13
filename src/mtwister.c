/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

/* A C-Program for MT19937: Real number version						*/
/* genrand() generates one pseudorandom real number (double)		*/
/* which is uniformly distributed on [0,1]-interval, for each		*/
/* call. sgenrand(seed) set initial values to the working area		*/
/* of 624 words. Before genrand(), sgenrand() must be				*/
/* called once. (seed is any 32-bit integer except for 0).			*/
/* Integer generator is obtained by modifying two lines.			*/
/* Coded by Takuji Nishimura, considering the suggestions by		*/
/* Topher Cooper and Marc Rieffel in July-Aug. 1997. Comments		*/
/* should be addressed to: matumoto@math.keio.ac.jp 				*/

#include "m_pd.h"  

static t_class *mtwister_class;  

typedef struct _mtwister { 
	t_object		x_obj;
	double 			x_rng;
	double	 		x_off;
	unsigned long 	x_see;
} t_mtwister;  

static void mtwister_set(t_mtwister *x, t_symbol *s, int argc, t_atom *argv) {
	int i;
	for (i = 0; i < argc; i++) {
			if (argv[i].a_type == A_FLOAT) {
			if 		(i == 0) {x->x_rng	= (double)	argv[i].a_w.w_float;}
			else if (i == 1) {x->x_off	= (double)	argv[i].a_w.w_float;}
		}   
    }
}

static void mtwister_bang(t_mtwister *x) {
	outlet_float(x->x_obj.ob_outlet, 
		genrand() * ( x->x_rng - x->x_off ) + x->x_off);
}  

static void mtwister_seed(t_mtwister *x, t_floatarg h) {
	x->x_see=h==0?(time(NULL)*rand()):(unsigned long)h;	//	store the seed
	sgenrand(x->x_see);	//	set the actual seed
}

static void mtwister_print(t_mtwister *x) {
	post("Range:%f\nOffset:%f\nSeed:%d", x->x_rng,x->x_off,x->x_see);
}

static void *mtwister_new(t_floatarg f, t_floatarg g, t_floatarg h) {  

	t_mtwister *x = (t_mtwister *)pd_new(mtwister_class);
	/* set range and min to map value */
	x->x_rng=f==0?1.0:(double)f;
	x->x_off=g==0?0.0:(double)g;	
	mtwister_seed(x,h);		//	set new seed
	outlet_new(&x->x_obj, &s_float);
	return (void *)x;  
}  

void mtwister_setup(void) {  
 	mtwister_class = class_new(gensym("mtwister"), 
  		(t_newmethod)mtwister_new, 0, 
  		sizeof(t_mtwister), 
  		CLASS_DEFAULT, 
  		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);  
	class_addbang(mtwister_class, mtwister_bang);
	class_addmethod(mtwister_class,
		(t_method)mtwister_set,
		gensym("set"),
		A_GIMME, 0);
	class_addmethod(mtwister_class,
		(t_method)mtwister_seed,
		gensym("seed"),
		A_DEFFLOAT, 0);
	class_addmethod(mtwister_class, 
		(t_method)mtwister_print,
		gensym("print"),0);
}