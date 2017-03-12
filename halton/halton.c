#include "m_pd.h"

float halton_function(int n, int b) {
	double r,f,fn,res, resn;
	int n1, n0;
	n0 = n;
	res = 0;
	f = 1;
    while(n0 > 0)
    {
    	fn = f / b;
    	f = fn;
        resn = res + f * (n0 % b);
    	res = resn;
        n1 = floor(n0 / b);
        n0 = n1;
        return res;
    }
}

typedef struct halton	{
  t_object x_ob;
  t_outlet *x_outlet0;
  t_int index, base;
  double result;
} t_halton;

void halton_set(t_halton *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT) {
			x->base = (int) argvec[i].a_w.w_float;
			//post("base (prime) = %d", x->base);
		}   
    }
}

void halton_float(t_halton *x, t_floatarg f) {
	x->index = f;
	double resultado;
	resultado = 0;
	if ( x->index > 0 ) {
	resultado = halton_function(x->index, x->base);
	}

	outlet_float(x->x_outlet0, resultado);
}

t_class *halton_class;

void *halton_new(t_floatarg f)	{
    t_halton *x = (t_halton *)pd_new(halton_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
	x->base = 7;
    return (void *)x;
}

void halton_setup(void)	{
    halton_class = class_new(gensym("halton"), (t_newmethod)halton_new, 0, sizeof(t_halton), 0, 0);
    class_addfloat(halton_class, halton_float);
    class_addmethod(halton_class, (t_method)halton_set, gensym("set"), A_GIMME, 0);
}