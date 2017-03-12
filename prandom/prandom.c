#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

typedef struct prandom
{
	t_object x_ob;
	t_outlet *x_outlet0;
} t_prandom;
void prandom_bang(t_prandom *x) {
	float a, b, prand;
	a = 0;
	b = 1;
	prand = rand();
	
    outlet_float(x->x_outlet0, prand);
    
}

t_class *prandom_class;

void *prandom_new(t_floatarg f)
{
    t_prandom *x = (t_prandom *)pd_new(prandom_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    	/* just stick the argument in the object structure for later. */
    return (void *)x;
}
    /* this is called once at setup time, when this code is loaded into Pd. */
void prandom_setup(void)
{
    post("prandom is running");
    prandom_class = class_new(gensym("prandom"), (t_newmethod)prandom_new, 0, sizeof(t_prandom), 0, 0);
    class_addbang(prandom_class, prandom_bang);  
}