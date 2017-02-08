#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>


/*------------------ lorenz --------------------*/


typedef struct lorenz
{
	t_object x_ob;
	t_outlet *x_outlet0;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	double x, y, z, inita, initb, initc, inith;
	int gmax, count;
} t_lorenz;

void lorenz_bang(t_lorenz *x) {

   double x0, y0, z0, x1, y1, z1;
   double h = x->inith;
   double a = x->inita;
   double b = x->initb;
   double c = x->initc;
   
   t_atom raw[3];
	int f;
   for (f = 0; f < x->gmax; f++)
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
  		x->count++;
  	  	SETFLOAT(&raw[0], (float) x->x);
      	SETFLOAT(&raw[1], (float) x->y);
      	SETFLOAT(&raw[2], (float) x->z);  
      	
      	if (x->count > 100) outlet_list(x->x_outlet0, 0, 3, raw);
      	outlet_float(x->x_outlet2, x->count);
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
	    post("Prandlt = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_initb(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->initb = argvec[i].a_w.w_float;
	    post("Rayleigh = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_initc(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->initc = argvec[i].a_w.w_float;
	    post("c = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_inith(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->inith = argvec[i].a_w.w_float;
	    post("h = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_initx(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->x = argvec[i].a_w.w_float;
	    post("x = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_inity(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->y = argvec[i].a_w.w_float;
	    post("y = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_initz(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->z = argvec[i].a_w.w_float;
	    post("z = %f", argvec[i].a_w.w_float);
	    }
    }
}
void lorenz_maxval(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->gmax = argvec[i].a_w.w_float;
	    post("nmax = %d", x->gmax);
	    }
    }
}
void lorenz_print(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->gmax = argvec[i].a_w.w_float;
	    }
    }
   double x0, y0, z0, x1, y1, z1;
   double h = x->inith;
   double a = x->inita;
   double b = x->initb;
   double c = x->initc;

	t_atom cooked[4];
	int f;
   for (f = 0; f < x->gmax; f++)
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
      	x->count++;
      	SETFLOAT(&cooked[0], (float) f);
      	SETFLOAT(&cooked[1], (float) x->x);
      	SETFLOAT(&cooked[2], (float) x->y);  
      	SETFLOAT(&cooked[3], (float) x->z); 
      	
      	if (x->count > 100) outlet_list(x->x_outlet1, 0, 4, cooked);
      	outlet_float(x->x_outlet2, x->count);
      	//post("%d, %f, %f, %f", f, (float) x->x, (float) x->y, (float) x->z);
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
 	x->gmax = 1;
 	x->count = 0;
 	post("default: \n Prandlt = %f\n Rayleigh = %f\n c = %f\n nmax = %d", x->inita, x->initb, x->initc, x->gmax);
}

void lorenz_type(t_lorenz *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	double prng;
	prng = rand() % 10;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    		int tipo;
    		tipo = argvec[i].a_w.w_float;
    		switch (tipo) 
    		{	
    			case 1:
    				x->x = 0.1;
   					x->y = 0;
   					x->z = 0; 
   					x->inita = 28;
   					x->initb = 46.92;
   					x->initc = 4;
   					x->inith = 0.01;	
 					x->gmax = 1;
    				post("type %d:\n Prandlt = %f\n Rayleigh = %f\n c = %f\n nmax = %d", tipo, x->inita, x->initb, x->initc, x->gmax);
    				break;
    			case 2:	
    				x->x = 0.1;
   					x->y = 0;
   					x->z = 0; 
   					x->inita = prng * 10;
   					x->initb = prng * 28.0;
   					x->initc = prng * 4;
   					x->inith = 0.01;	
 					x->gmax = 1;
    				post("type %d: random \n Prandlt = %f\n Rayleigh = %f\n c = %f\n nmax = %d", tipo, x->inita, x->initb, x->initc, x->gmax);
    				break;
    			default:
    				x->x = 0.1;
 				  	x->y = 0;
  				 	x->z = 0; 
  				 	x->inita = 10.0;
  				 	x->initb = 28.0;
  				 	x->initc = 8.0 / 3.0;
  				 	x->inith = 0.01;	
 					x->gmax = 1;
 					post("default values: \n Prandlt = %d\n Rayleigh = %d\n c = %f\n nmax = %d", x->inita, x->initb, x->initc, x->gmax);
 					break;
    		}
	    }
    }
}

t_class *lorenz_class;

void *lorenz_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_lorenz *x = (t_lorenz *)pd_new(lorenz_class);
	//default values
	x->count = 0;
    x->x = 0.1;
   	x->y = 0;
   	x->z = 0; 
   	x->inita = 10.0; // this is default for sigma (or the Prandlt number) used in dx/dt = sigma * (y-x)
   	x->initb = 28.0; // this is the "Rayleigh number" the default for 'r' value used in dy/dt = rx - y 0 xz
   	x->initc = 8.0 / 3.0; // this is the default for 'b' value used in dz/dt = xy - bz
   	x->inith = 0.01;	 // this is the "step size" or "resolution" of the iterations.
 	x->gmax = 1; // this is the maximum number of iterations (defaults to 1)
    x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
    x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}



void lorenz_setup(void)
{
    lorenz_class = class_new(gensym("lorenz"), (t_newmethod)lorenz_new, 0, sizeof(t_lorenz), 0, 0);
    class_addbang(lorenz_class, lorenz_bang);  
    class_addmethod(lorenz_class, (t_method)lorenz_inita, gensym("inita"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_initb, gensym("initb"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_initc, gensym("initc"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_inith, gensym("inith"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_initx, gensym("initx"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_inity, gensym("inity"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_initz, gensym("initz"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_maxval, gensym("maxval"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_type, gensym("type"), A_GIMME, 0);
    class_addmethod(lorenz_class, (t_method)lorenz_print, gensym("print"), A_GIMME, 0);
}