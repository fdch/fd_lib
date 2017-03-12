#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ siglor~ ----------------------------- */

static t_class *siglor_class;

typedef struct _siglor
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    t_outlet *x_outlet1;
    float x, y, z, inita, initb, initc, inith;
    float dist;
    int count;
} t_siglor;

float siglor_lorenz(float signal, t_siglor *x) {
	float x0, y0, z0, x1, y1, z1;
   	float h = x->inith; //step-size
   	float a = x->inita;
   	float b = x->initb;
   	float c = x->initc;
    float t_dist;
    float sig = signal;
    float outcome;
    
	x0 = x->x;
   	y0 = x->y;
   	z0 = x->z;
   	
   	x1 = x0 + h * a * (y0 - x0);
   	y1 = y0 + h * (x0 * (b - z0) - y0);
   	z1 = z0 + h * (x0 * y0 - c * z0);
   	
   	
   	t_dist = sqrt((x1 * x1) + (y1 * y1)  + (z1 * z1));
   	
   	x->x = x1;
   	x->y = y1;
   	x->z = z1;
   	x->dist = t_dist;
   	
   	outcome = sqrt((t_dist*t_dist)+(sig*sig));
   	
   	return(outcome);

}
 
static t_int *siglor_perform(t_int *w)
{
	t_siglor *x = (t_siglor *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    while (n--)
    {
    	float f = *in++;
    // *out++ = siglor_lorenz(f, &x);
     *out++ = f / 2;
      return (w+5);
     }
}

static void siglor_dsp(t_siglor *x, t_signal **sp)
{
    dsp_add(siglor_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void siglor_reset(t_siglor *x, t_symbol *selector)
{
	x->x = 0.1;
   	x->y = 0;
   	x->z = 0; 
   	x->inita = 10.0;
   	x->initb = 28.0;
   	x->initc = 8.0 / 3.0;
   	x->inith = 0.01;	
 	post("default: \n Prandlt = %f\n Rayleigh = %f\n c = %f\n ", x->inita, x->initb, x->initc);
}

void siglor_debug(t_siglor *x, t_symbol *selector)
{

   	t_atom raw[4];
   	SETFLOAT(&raw[0], (float) x->x);
    SETFLOAT(&raw[1], (float) x->y);
    SETFLOAT(&raw[2], (float) x->z);  
    SETFLOAT(&raw[3], (float) x->count);  

   	outlet_list(x->x_outlet1, 0, 4, raw);


}

static void *siglor_new(void)
{
    t_siglor *x = (t_siglor *)pd_new(siglor_class);
    outlet_new(&x->x_obj, gensym("signal"));
    outlet_new(&x->x_obj, &s_signal);
  //  x->x_outlet1 = outlet_new(&x->x_obj, &s_list);
    x->x_f = 0;
    x->x = 0.1;
   	x->y = 0;
   	x->z = 0; 
   	x->inita = 10.0; // this is default for sigma (or the Prandlt number) used in dx/dt = sigma * (y-x)
   	x->initb = 28.0; // this is the "Rayleigh number" the default for 'r' value used in dy/dt = rx - y 0 xz
   	x->initc = 8.0 / 3.0; // this is the default for 'b' value used in dz/dt = xy - bz
   	x->inith = 0.01;	 // this is the "step size" or "resolution" of the iterations.
    return (x);
}

void siglor_tilde_setup(void)
{
    siglor_class = class_new(gensym("siglor~"), (t_newmethod)siglor_new, 0,
    	sizeof(t_siglor), 0, A_DEFFLOAT, 0);

    CLASS_MAINSIGNALIN(siglor_class, t_siglor, x_f);

    class_addmethod(siglor_class, (t_method)siglor_dsp, gensym("dsp"), 0);
    class_addmethod(siglor_class, (t_method)siglor_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(siglor_class, (t_method)siglor_debug, gensym("debug"), A_GIMME, 0);
}
