#include "m_pd.h"
#include "math.h"
#include "stdlib.h"

typedef struct crand	{
  t_object x_ob;
  t_outlet *x_outlet0;
  double a, b, result;
} t_crand;

void crand_set(t_crand *x, t_symbol *selector, int argcount, t_atom *argvec) {
	int i;
	for (i = 0; i < argcount; i++)
    {
			if (argvec[i].a_type == A_FLOAT) {
			if 		(i == 0) {x->a	= (double)	argvec[i].a_w.w_float;}
			else if (i == 1) {x->b	= (double)	argvec[i].a_w.w_float;}
		}   
    }
}
void crand_reset(t_crand *x, t_symbol *selector, t_floatarg f, t_floatarg g){
   	x->a = (f != 0) ? f : 0;
	x->b = (g != 0) ? g : 1;
	x->result = 0.0;}
	
void crand_bang(t_crand *x) {
	double t_r, t_a, t_b;
	t_a = x->a;
	t_b = x->b;
	t_r	=	t_a	+ (double) rand() * (t_b-t_a) / RAND_MAX;
	x->result = t_r;
	outlet_float(x->x_outlet0, x->result);
}

t_class *crand_class;

void *crand_new(t_floatarg f, t_floatarg g)	{
    t_crand *x = (t_crand *)pd_new(crand_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
   	x->a = (f != 0) ? f : 0;
	x->b = (g != 0) ? g : 1;
	x->result = 0.0;
    return (void *)x;
}
void crand_setup(void)	{
    crand_class = class_new(gensym("crand"),
    (t_newmethod)crand_new, 0,
    	sizeof(t_crand), 0, A_DEFFLOAT,A_DEFFLOAT, 0);
    class_addbang(crand_class, crand_bang);
    class_addmethod(crand_class, (t_method)crand_set, gensym("set"), A_GIMME, 0);
    class_addmethod(crand_class, (t_method)crand_reset, gensym("reset"), A_GIMME, 0);
}