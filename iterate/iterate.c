#include "m_pd.h"

static t_class *iterate_class;

typedef struct iterate	{
  t_object	x_ob;
  t_outlet 	*x_outlet0;
  t_outlet 	*x_outlet1;
  t_int 	x_n;

} t_iterate;

static void iterate_set(t_iterate *x, t_floatarg f) { 
	x->x_n=f>=1?(t_int)f:100;
}

static void iterate_reset(t_iterate *x){ iterate_set(x,100); }

static void iterate_bang(t_iterate *x) {


	t_int i;
	for (i=0;i<x->x_n;i++) outlet_float(x->x_outlet0, (t_int)i);

	outlet_bang(x->x_outlet1);
}

static void iterate_float(t_iterate *x, t_floatarg f) {
	iterate_set(x,f);
	iterate_bang(x);
}

void *iterate_new(t_floatarg f)	{
    t_iterate *x = (t_iterate *)pd_new(iterate_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_symbol);
	iterate_set(x,f);
    return (void *)x;
}

void iterate_setup(void)	{
    iterate_class = class_new(gensym("iterate"),
    	(t_newmethod)iterate_new, 0,
    	sizeof(t_iterate), 0, 
    	A_DEFFLOAT, 0);

    class_addbang(iterate_class, iterate_bang);
    class_addfloat(iterate_class, iterate_float);

    class_addmethod(iterate_class, 
    	(t_method)iterate_set, 
    	gensym("set"), 
    	A_DEFFLOAT, 0);

    class_addmethod(iterate_class, 
    	(t_method)iterate_reset, 
    	gensym("reset"), 0);
}