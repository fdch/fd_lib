/*
code for the "clifford" pd class. 
[clifford] runs the equations for the clifford system as in (https://en.wikipedia.org/wiki/Lorenz_system)
developed by Fede Camara Halac (fch226@nyu.edu)
@ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
Released under the GNU General Public License. 
*/
#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>
/*
********************** STRUCTURE *********************
*/
typedef struct clifford
{
	t_object x_ob;
	t_outlet *x_outlet0;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	double x, y, z, inita, initb, initc, initd;
	int gmax, count;
} t_clifford;
/*
********************** BANG *********************
*/
void clifford_bang(t_clifford *x) {

   double x0, y0, x1, y1;
   double a = x->inita;
   double b = x->initb;
   double c = x->initc;
   double d = x->initd;
   
   t_atom raw[2]; //array "raw"
   
   int f; //count
   
   for (f = 0; f < x->gmax; f++)	{ //para [bang( no hace falta que sea un for loop!
   
   	x0 = x->x;
   	y0 = x->y;
   	//xn+1 = sin(a yn) + c cos(a xn)
	//yn+1 = sin(b xn) + d cos(b yn)
	
	x1 = sin(a * y0) + c*cos(a*x0);
	y1 = sin(b * x0) + d*cos(b*y0);
   	
   	x->x = x1;
   	x->y = y1;
   	x->count++;
   	
   	//pone x, y, z en el array "raw"
   	SETFLOAT(&raw[0], (float) x->x);
   	SETFLOAT(&raw[1], (float) x->y);
   	
   	//cortar las 100 primeras iteraciones y sacar el array por outlet 1
   	if (x->count > 100) outlet_list(x->x_outlet0, 0, 2, raw);
   	
   	//outlet 2 para la cuenta de iteraciones
   	outlet_float(x->x_outlet2, x->count);
   	
	}
}
/*
********************** PRINT float *********************
*/
void clifford_print(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->gmax = argvec[i].a_w.w_float; //float que entra es el limite de iteraciones
	    }
    }
   double x0, y0, x1, y1;
   double a = x->inita;
   double b = x->initb;
   double c = x->initc;
   double d = x->initd;

	t_atom cooked[3];
	int f;
   for (f = 0; f < x->gmax; f++)
   {	
   	x0 = x->x;
   	y0 = x->y;
   	//xn+1 = sin(a yn) + c cos(a xn)
	//yn+1 = sin(b xn) + d cos(b yn)
	
	x1 = sin(a * y0) + c*cos(a*x0);
	y1 = sin(b * x0) + d*cos(b*y0);
   	
   	x->x = x1;
   	x->y = y1;
   	x->count++;
      	SETFLOAT(&cooked[0], (float) f);
      	SETFLOAT(&cooked[1], (float) x->x);
      	SETFLOAT(&cooked[2], (float) x->y);  
      	
      	if (x->count > 100) outlet_list(x->x_outlet1, 0, 3, cooked);
      	outlet_float(x->x_outlet2, x->count);
      	//post("%d, %f, %f, %f", f, (float) x->x, (float) x->y, (float) x->z);
	}
}
/*
********************** RESET *********************
*/
void clifford_reset(t_clifford *x, t_symbol *selector)
{ //a = -1.4, b = 1.6, c = 1.0, d = 0.7 
	x->x = 0.1;
   	x->y = 0;
   	x->inita = -1.4;
   	x->initb = 1.6;
   	x->initc = 1.0;
   	x->initd = 0.7;	
 	x->gmax = 1;
 	x->count = 0;
 	post("default: \n a = %f\n b = %f\n c = %f\n d = %f", x->inita, x->initb, x->initc, x->initd);
}


/*
********************** VARIABLES *********************
*/

/*
********************** inita *********************
*/
void clifford_inita(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->inita = argvec[i].a_w.w_float;
	    post("a = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initb *********************
*/
void clifford_initb(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->initb = argvec[i].a_w.w_float;
	    post("b = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initc *********************
*/
void clifford_initc(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
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
/*
********************** initd *********************
*/
void clifford_initd(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->initd = argvec[i].a_w.w_float;
	    post("d = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initx *********************
*/
void clifford_initx(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
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
/*
********************** inity *********************
*/
void clifford_inity(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
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

void clifford_maxval(t_clifford *x, t_symbol *selector, int argcount, t_atom *argvec)
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
/*
********************** PUREDATA *********************
*/
t_class *clifford_class;
/*
********************** setup *********************
*/
void *clifford_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_clifford *x = (t_clifford *)pd_new(clifford_class);
	//default values
   	//a = -1.4, b = 1.6, c = 1.0, d = 0.7 
	x->x = 0.1;
   	x->y = 0;
   	x->inita = -1.4;
   	x->initb = 1.6;
   	x->initc = 1.0;
   	x->initd = 0.7;	
 	x->gmax = 1;
 	x->count = 0;
 	x->gmax = 1; // this is the maximum number of iterations (defaults to 1)
    x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
    x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}
void clifford_setup(void)
{
    clifford_class = class_new(gensym("clifford"), (t_newmethod)clifford_new, 0, sizeof(t_clifford), 0, 0);
    class_addbang(clifford_class, clifford_bang);  
    class_addmethod(clifford_class, (t_method)clifford_inita, gensym("inita"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_initb, gensym("initb"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_initc, gensym("initc"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_initd, gensym("initd"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_initx, gensym("initx"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_inity, gensym("inity"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_maxval, gensym("maxval"), A_GIMME, 0);
    class_addmethod(clifford_class, (t_method)clifford_print, gensym("print"), A_GIMME, 0);
}