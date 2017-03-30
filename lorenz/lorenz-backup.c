#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

typedef struct lorenz
{
	t_object x_ob;
	t_int i_count;
	t_outlet *x_outlet0;
	double x, y, z, inita, initb, initc, inith;
} t_lorenz;

void lorenz_bang(t_lorenz *x) {

   t_float f = x->i_count;
   
   double x0, y0, z0, x1, y1, z1;
   double h = x->inith;
   double a = x->inita;
   double b = x->initb;
   double c = x->initc;
   
   t_atom dedo[4];

   for (f = 0; f < 1; f++)
   {	
   		x0 = x->x;
   		y0 = x->y;
   		z0 = x->z;
		x1 = x0 + h * a * (y0 - x0);
		y1 = y0 + h * (x0 * (b - z0) - y0);
	    z1 = z0 + h * (x0 * y0 - c * z0);
      	x->x = x1;
      	x->y = y1;
      	x->z = z1;
    
	  	SETFLOAT(&dedo[0], (float) x->i_count);
  	  	SETFLOAT(&dedo[1], (float) x->x);
      	SETFLOAT(&dedo[2], (float) x->y);
      	SETFLOAT(&dedo[3], (float) x->z);  
      	
      	outlet_list(x->x_outlet0, 0, 4, dedo);
	}
}

void lorenz_inita(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->inita = argvec[i].a_w.w_float;
	    post("double a has been changed to: %f", argvec[i].a_w.w_float);
	    }
    	else if (argvec[i].a_type == A_SYMBOL)
    	{
	    post("i need a number");
	    }
    }
}

void lorenz_reset(t_lorenz *x, t_symbol *selector)
{
	x->x = 0.1;
   	x->y = 0;
   	x->z = 0; 
   	x->inita = 10.0;
   	x->initb = 28.0;
   	x->initc = 8.0 / 3.0;
   	x->inith = 0.01;		 
}

t_class *lorenz_class;

void *lorenz_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_lorenz *x = (t_lorenz *)pd_new(lorenz_class);
	x->i_count=f;
    x->x = 0.1;
   	x->y = 0;
   	x->z = 0; 
   	x->inita = 10.0;
   	x->initb = 28.0;
   	x->initc = 8.0 / 3.0;
   	x->inith = 0.01;	
    x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
    return (void *)x;
}
void lorenz_setup(void)
{
    lorenz_class = class_new(gensym("lorenz"), (t_newmethod)lorenz_new, 0, sizeof(t_lorenz), 0, 0);
    class_addbang(lorenz_class, lorenz_bang);  
    class_addmethod(lorenz_class, (t_method)lorenz_inita, gensym("inita"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_reset, gensym("reset"), A_GIMME, 0);
}