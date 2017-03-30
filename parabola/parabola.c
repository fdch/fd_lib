#include "m_pd.h"
#include "math.h"

/*------------------ parabola --------------------*/

typedef struct parabola	{
	t_object x_ob;
	t_outlet *x_outlet0;
	double x, inita;
	int gmax;
} t_parabola;

void parabola_bang(t_parabola *x) {
   double x0, x1;
   double a = x->inita;
   int f;
   for (f = 0; f < x->gmax; f++)
   {	
   		x0 = x->x;
		x1 = (1 - x0) * a * x0;
      	x->x = x1;
      	outlet_float(x->x_outlet0, x->x);
	}
}

void parabola_inita(t_parabola *x, t_symbol *selector, int argcount, t_atom *argvec) {
	int i;
	for (i = 0; i < argcount; i++) {
		if (argvec[i].a_type == A_FLOAT)
    	{
    		x->inita = argvec[i].a_w.w_float;
	    	post("a = %f", argvec[i].a_w.w_float);
	    }
	}
}
void parabola_initx(t_parabola *x, t_symbol *selector, int argcount, t_atom *argvec) {
	int i;
	for (i = 0; i < argcount; i++) {
		if (argvec[i].a_type == A_FLOAT)
    	{
    		x->x = argvec[i].a_w.w_float;
	    	post("x = %f", argvec[i].a_w.w_float);
	    }
	}
}
void parabola_reset(t_parabola *x, t_symbol *selector)
{
	x->x = 1;
	x->inita = 1;
 	post("x = %f, a = %f", x->x, x->inita);
}
t_class *parabola_class;

void *parabola_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_parabola *x = (t_parabola *)pd_new(parabola_class);
	//default values
	x->x = 1;
	x->inita = 1;
	x->gmax = 1;
	//end default values
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

void parabola_setup(void)
{
    parabola_class = class_new(gensym("parabola"), (t_newmethod)parabola_new, 0, sizeof(t_parabola), 0, 0);
    class_addbang(parabola_class, parabola_bang);
    class_addmethod(parabola_class, (t_method)parabola_inita, gensym("inita"), A_GIMME, 0);
    class_addmethod(parabola_class, (t_method)parabola_initx, gensym("initx"), A_GIMME, 0);
    class_addmethod(parabola_class, (t_method)parabola_reset, gensym("reset"), A_GIMME, 0);
}