#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_math.h>


/*------------------ tbessel --------------------*/


typedef struct tbessel
{
	t_object x_ob;
	double x;
	t_outlet *x_outlet0;
} t_tbessel;


void tbessel_print(t_tbessel *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->x = argvec[i].a_w.w_float;
	    }
    }

	double y = gsl_sf_bessel_J0 (x->x);

	t_atom cooked[2];
	
	
    SETFLOAT(&cooked[0], (double) x->x);
    SETFLOAT(&cooked[1], (double) y);
    outlet_list(x->x_outlet0, 0, 2, cooked);
}

t_class *tbessel_class;

void *tbessel_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_tbessel *x = (t_tbessel *)pd_new(tbessel_class);
	//default values
	x->x = 5.0;
    x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
    return (void *)x;
}



void tbessel_setup(void)
{
    tbessel_class = class_new(gensym("tbessel"), (t_newmethod)tbessel_new, 0, sizeof(t_tbessel), 0, 0);
    class_addmethod(tbessel_class, (t_method)tbessel_print, gensym("print"), A_GIMME, 0);
}
