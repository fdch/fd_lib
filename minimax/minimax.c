#include "m_pd.h"
 
typedef struct minimax
{
  t_object x_ob;
  t_outlet *x_outlet0;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  double minimum, maximum;
} t_minimax;

void minimax_float(t_minimax *x, t_floatarg f)	{ 
	double min, max, nmin, nmax;                       
 
	min = x->minimum;
   	max = x->maximum;
   	 
   	nmax = (f > max) ? f : max;
	nmin = (f < min) ? f : min;
   			
   	x->minimum = nmin;
    x->maximum = nmax;
    
    outlet_float(x->x_outlet2, x->maximum); 
	outlet_float(x->x_outlet1, x->minimum); 
	outlet_float(x->x_outlet0, f);
}

void minimax_flush(t_minimax *x, t_symbol *selector)	{
	x->maximum = 0.0;
	x->minimum = 1000000.0;
}

t_class *minimax_class;

void *minimax_new(t_floatarg f)	{
    t_minimax *x = (t_minimax *)pd_new(minimax_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
	x->maximum = 0.0;
	x->minimum = 1000000.0;
    return (void *)x;
}

void minimax_setup(void)	{
    minimax_class = class_new(gensym("minimax"), (t_newmethod)minimax_new, 0, sizeof(t_minimax), 0, 0);
    class_addfloat(minimax_class, minimax_float);
    class_addmethod(minimax_class, (t_method)minimax_flush, gensym("flush"), A_GIMME, 0);
}