#include "m_pd.h"
#include "math.h"

/*------------------ cuadratic --------------------*/

typedef struct cuadratic	{
	t_object x_ob;
	t_outlet *x_outlet0;
	double x, initc;
	int gmax;
} t_cuadratic;

void cuadratic_bang(t_cuadratic *x) {
   double x0, x1;
   double c = x->initc;
   int f;
   for (f = 0; f < x->gmax; f++)
   {	
   		x0 = x->x;
		x1 = (x0 * x0) + c;
      	x->x = x1;
      	outlet_float(x->x_outlet0, x->x);
	}
}

void cuadratic_initc(t_cuadratic *x, t_symbol *selector, int argcount, t_atom *argvec) {
	int i;
	for (i = 0; i < argcount; i++) {
		if (argvec[i].a_type == A_FLOAT)
    	{
    		x->initc = argvec[i].a_w.w_float;
	    	post("c = %f", argvec[i].a_w.w_float);
	    }
	}
}
void cuadratic_initx(t_cuadratic *x, t_symbol *selector, int argcount, t_atom *argvec) {
	int i;
	for (i = 0; i < argcount; i++) {
		if (argvec[i].a_type == A_FLOAT)
    	{
    		x->x = argvec[i].a_w.w_float;
	    	post("x = %f", argvec[i].a_w.w_float);
	    }
	}
}
void cuadratic_reset(t_cuadratic *x, t_symbol *selector)
{
	x->x = 1.97;
	x->initc = -2;
 	post("x = %f, c = %f", x->x, x->initc);
}
t_class *cuadratic_class;

void *cuadratic_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_cuadratic *x = (t_cuadratic *)pd_new(cuadratic_class);
	//default values
	x->x = 1.97;
	x->initc = -2;
	x->gmax = 1;
	//end default values
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

void cuadratic_setup(void)
{
    cuadratic_class = class_new(gensym("cuadratic"), (t_newmethod)cuadratic_new, 0, sizeof(t_cuadratic), 0, 0);
    class_addbang(cuadratic_class, cuadratic_bang);
    class_addmethod(cuadratic_class, (t_method)cuadratic_initc, gensym("initc"), A_GIMME, 0);
    class_addmethod(cuadratic_class, (t_method)cuadratic_initx, gensym("initx"), A_GIMME, 0);
    class_addmethod(cuadratic_class, (t_method)cuadratic_reset, gensym("reset"), A_GIMME, 0);
}