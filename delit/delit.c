/*
    delit

    outputs delayed iterations

*/



#include "m_pd.h"

static t_class *delit_class;

typedef struct delit	{
  t_object	x_ob;
  t_clock *x_clock;
  t_int x_initialized;
  double x_d,x_initd;
  t_outlet 	*x_outlet0;
  t_outlet 	*x_outlet1;
  t_int 	x_n,x_i,x_initn;

} t_delit;

static void delit_set(t_delit *x, t_floatarg f, t_floatarg g) { 
	x->x_n=f>=1?(t_int)f:1;
    x->x_d=g>0?(double)g:0.0;
    x->x_i=x->x_n;
    if(!x->x_initialized) {
        x->x_initn=x->x_n;
        x->x_initd=x->x_d;
        x->x_initialized=1;
    }
}

static void delit_reset(t_delit *x){ delit_set(x,x->x_initn,x->x_initd); }


static void delit_tick(t_delit *x) {
    if (x->x_i) {
        clock_delay(x->x_clock, x->x_d);
        outlet_float(x->x_outlet0, x->x_i);
        x->x_i--;
    } else {
        outlet_bang(x->x_outlet1);
        x->x_i=x->x_n;
    }
}

static void delit_bang(t_delit *x) {
    delit_tick(x);
}

static void delit_stop(t_delit *x) {
    clock_unset(x->x_clock);
}

static void delit_float(t_delit *x, t_floatarg f) {
	delit_set(x,f,0);
	delit_bang(x);
}
static void delit_list(t_delit *x, t_symbol *s, int argc, t_atom *argv) {
    int i;
    t_int f;
    double g;
    for (i = 0; i < argc; i++) {
            if (argv[i].a_type == A_FLOAT) {
            if      (i == 0) {f  = (t_int)  argv[i].a_w.w_float;}
            else if (i == 1) {g  = (double)  argv[i].a_w.w_float;}
        }   
    }

    delit_set(x,f,g);
    delit_bang(x);
}
static void delit_free(t_delit *x) {
    clock_free(x->x_clock);
}

void *delit_new(t_floatarg f, t_floatarg g)	{
    t_delit *x = (t_delit *)pd_new(delit_class);
    x->x_clock = clock_new(x, (t_method)delit_tick);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet1 = outlet_new(&x->x_ob, gensym("bang"));
    x->x_initialized=0;
	delit_set(x,f,g);
    return (void *)x;
}

void delit_setup(void)	{
    delit_class = class_new(gensym("delit"),
    	(t_newmethod)delit_new, (t_method)delit_free,
    	sizeof(t_delit), 0, 
    	A_DEFFLOAT,A_DEFFLOAT, 0);

    class_addbang(delit_class, delit_bang);
    class_addfloat(delit_class, delit_float);
    class_addlist(delit_class, delit_list);

    class_addmethod(delit_class, 
    	(t_method)delit_set, 
    	gensym("set"), 
    	A_DEFFLOAT, A_DEFFLOAT, 0);

    class_addmethod(delit_class, 
    	(t_method)delit_reset, 
    	gensym("reset"), 0);
}