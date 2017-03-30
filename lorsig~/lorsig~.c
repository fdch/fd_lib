#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ lorsig~ ----------------------------- */

/* tilde object to take absolute value. */

static t_class *lorsig_class;

typedef struct _lorsig
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    t_outlet *x_outlet1;
    float x, y, z, inita, initb, initc, inith;
    float dist;
    int count;
} t_lorsig;

float lorsig_lorenz(float signal, t_lorsig *x) {
	float x0, y0, z0, x1, y1, z1;
   	float h = x->inith; //step-size
   	float a = x->inita;
   	float b = x->initb;
   	float c = x->initc;
    float t_dist;
    int i;
   
    
    
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
   	
   	 i++;
   	
   	x->count = i;
   	if(i>=100)
   	return(sqrt((t_dist*t_dist)+(signal*signal)));
   	else if(i>=128)i=100;
}
    /* this is the actual performance routine which acts on the samples.
    It's called with a single pointer "w" which is our location in the
    DSP call list.  We return a new "w" which will point to the next item
    after us.  Meanwhile, w[0] is just a pointer to dsp-perform itself
    (no use to us), w[1] and w[2] are the input and output vector locations,
    and w[3] is the number of points to calculate. */
static t_int *lorsig_perform(t_int *w, t_lorsig *x) //t_lorsig *x (pass pointer)
{
    t_float *in = (t_float *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);
    while (n--)
    {
    	float f = *in;
     *out = lorsig_lorenz(f, &x);
      return (w+4);
     }
}

    /* called to start DSP.  Here we call Pd back to add our perform
    routine to a linear callback list which Pd in turn calls to grind
    out the samples. */
static void lorsig_dsp(t_lorsig *x, t_signal **sp)
{
    dsp_add(lorsig_perform, 3, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}
/*
********************** RESET *********************
*/
void lorsig_reset(t_lorsig *x, t_symbol *selector)
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

void lorsig_debug(t_lorsig *x, t_symbol *selector)
{

   	t_atom raw[4];
   	SETFLOAT(&raw[0], (float) x->x);
    SETFLOAT(&raw[1], (float) x->y);
    SETFLOAT(&raw[2], (float) x->z);  
    SETFLOAT(&raw[3], (float) x->count);  

   	outlet_list(x->x_outlet1, 0, 4, raw);


}

static void *lorsig_new(void)
{
    t_lorsig *x = (t_lorsig *)pd_new(lorsig_class);
    outlet_new(&x->x_obj, gensym("signal"));
    x->x_outlet1 = outlet_new(&x->x_obj, &s_list);
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

    /* this routine, which must have exactly this name (with the "~" replaced
    by "_tilde) is called when the code is first loaded, and tells Pd how
    to build the "class". */
void lorsig_tilde_setup(void)
{
    lorsig_class = class_new(gensym("lorsig~"), (t_newmethod)lorsig_new, 0,
    	sizeof(t_lorsig), 0, A_DEFFLOAT, 0);
	    /* this is magic to declare that the leftmost, "main" inlet
	    takes signals; other signal inlets are done differently... */
    CLASS_MAINSIGNALIN(lorsig_class, t_lorsig, x_f);
    	/* here we tell Pd about the "dsp" method, which is called back
	when DSP is turned on. */
    class_addmethod(lorsig_class, (t_method)lorsig_dsp, gensym("dsp"), 0);
    class_addmethod(lorsig_class, (t_method)lorsig_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(lorsig_class, (t_method)lorsig_debug, gensym("debug"), A_GIMME, 0);
}
