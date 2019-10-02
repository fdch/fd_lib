/* 

Copyright 2019 Fede Camara Halac

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

/* Tuned to PD by Fede Camara Halac www.fedecamarahalac.com */
/* C code taken from "M. Matsumoto and T. Nishimura (1996) */



/* ======== PD Code starts here ========*/
static t_class *pmtwister_class;  

//Object-variables
typedef struct _pmtwister {  
  t_object  x_obj; 
  t_outlet *x_outlet1;
} t_pmtwister;  

//The Counter Method

static void pmtwister_bang(t_pmtwister *x)  
{
	int j; 
	
	sgenrand(time(NULL)*rand());	/* any nonzero integer can be used as a seed */
	
	for (j=0; j<MAXNUM; j++) {
		/* this checks if getrand() is a prime */
		int n, i, flag=0;
 		n = genrand();
  		for(i=2;i<=n/2;++i)
  		{
   			if(n%i==0)
     		{
     			flag=1;
     			break;
     		}
     	}
     	if (flag==0)	/* output float if n is prime */
			outlet_float(x->x_obj.ob_outlet, n);
		else
			outlet_float(x->x_outlet1, n);
	}   
}  

//Constructor
static void *pmtwister_new(t_floatarg f)  
{  
  t_pmtwister *x = (t_pmtwister *)pd_new(pmtwister_class);   
  outlet_new(&x->x_obj, &s_float);  
  x->x_outlet1 = outlet_new(&x->x_obj, gensym("float"));  
 
  return (void *)x;  
}  

//Object-arguments
void pmtwister_setup(void) {  
  pmtwister_class = class_new(gensym("pmtwister"),  (t_newmethod)pmtwister_new,  0, sizeof(t_pmtwister),  CLASS_DEFAULT,  A_DEFFLOAT, 0);  
  class_addbang(pmtwister_class, pmtwister_bang);  
}