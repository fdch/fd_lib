#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

float maximum = 100000000;
float minimum = 0;
/* ------------------------ lor4~ ----------------------------- */

/* tilde object to take absolute value. */

static t_class *lor4_class;

typedef struct _lor4
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;
    t_float val;    	/* place to hold inlet's value if it's set by message */

	t_float x, y, z;
	t_float inith, inita, initb, initc, dist;
} t_lor4;

    
static float minval(float f) {
    double min, nmin;
	min = minimum;
	nmin = (f < min) ? f : min;
   	minimum = nmin;
    }
    
 static float maxval(float f) {
    double max, nmax;                       
   	max = maximum;
   	nmax = (f > max) ? f : max;
    maximum = nmax;
    }

static float fd_norm(float f) {
	if (!f)
	return (f / (maxval(f) + abs(minval(f))));
}
static t_int *lor4_perform(t_int *w)
{
    t_lor4 *x = (t_lor4 *)(w[1]);  
    t_float *in = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);
    int n = (int)(w[4]);
    float f = 0;
    
    float x0, y0, z0, x1, y1, z1;
   	float h = x->inith; //step-size
   	float a = x->inita;
   	float b = x->initb;
   	float c = x->initc;
    float t_dist;
    
    while (n--)
    {
    	f = *in++;
		//*out++  = x->prev = (f + x->prev * x->val) / 2;
		x0 = x->x;
   		y0 = x->y;
   		z0 = x->z;
   		
   		x1 = x0 + h * a * (y0 - x0);
   		y1 = y0 + h * (x0 * (b - z0) - y0);
   		z1 = z0 + h * (x0 * y0 - c * z0);   	
   	
		t_dist = fd_norm(sqrt((x1 * x1) + (y1 * y1)  + (z1 * z1)));
   		   	
		*out++ = x->dist  = f * t_dist;
		
		x->x = x1;
   		x->y = y1;
   		x->z = z1;
    }
    return (w+5);
}

void lor4_ft1(t_lor4 *x, t_floatarg g)
{
    x->val = g;
}

    /* called to start DSP.  Here we call Pd back to add our perform
    routine to a linear callback list which Pd in turn calls to grind
    out the samples. */
static void lor4_dsp(t_lor4 *x, t_signal **sp)
{
    dsp_add(lor4_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}



/*
********************** VARIABLES *********************
*/

/*
********************** inita *********************
*/
void lor4_inita(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->inita = argvec[i].a_w.w_float;
	    //post("Prandlt = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initb *********************
*/
void lor4_initb(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->initb = argvec[i].a_w.w_float;
	    //post("Rayleigh = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initc *********************
*/
void lor4_initc(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->initc = argvec[i].a_w.w_float;
	    //post("c = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** inith *********************
*/
void lor4_inith(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->inith = argvec[i].a_w.w_float;
	    //post("h = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initx *********************
*/
void lor4_initx(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->x = argvec[i].a_w.w_float;
	    //post("x = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** inity *********************
*/
void lor4_inity(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->y = argvec[i].a_w.w_float;
	    //post("y = %f", argvec[i].a_w.w_float);
	    }
    }
}
/*
********************** initz *********************
*/
void lor4_initz(t_lor4 *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    	{
    	x->z = argvec[i].a_w.w_float;
	    //post("z = %f", argvec[i].a_w.w_float);
	    }
    }
}

static void *lor4_new(void)
{
    t_lor4 *x = (t_lor4 *)pd_new(lor4_class);
    outlet_new(&x->x_obj, gensym("signal"));
    x->x_f = 0.0;
    x->val = 1;
	x->inith = 0.001;
	x->inita = 10.0;
	x->initb = 28.0;
	x->initc = 8/3;
	x->x = 0.1;
	x->y = 0;
	x->z = 0;
	x->dist = 0.0;
    return (x);
}

    /* this routine, which must have exactly this name (with the "~" replaced
    by "_tilde) is called when the code is first loaded, and tells Pd how
    to build the "class". */
void lor4_tilde_setup(void)
{
    lor4_class = class_new(gensym("lor4~"), (t_newmethod)lor4_new, 0,
    	sizeof(t_lor4), 0, A_DEFFLOAT, 0);
	    /* this is magic to declare that the leftmost, "main" inlet
	    takes signals; other signal inlets are done differently... */
    CLASS_MAINSIGNALIN(lor4_class, t_lor4, x_f);
    	/* here we tell Pd about the "dsp" method, which is called back
	when DSP is turned on. */
    class_addmethod(lor4_class, (t_method)lor4_dsp, gensym("dsp"), 0);
	class_addmethod(lor4_class, (t_method)lor4_ft1, gensym("ft1"), A_FLOAT, 0);
	//variables
    class_addmethod(lor4_class, (t_method)lor4_inita, gensym("inita"), A_GIMME, 0);
    class_addmethod(lor4_class, (t_method)lor4_initb, gensym("initb"), A_GIMME, 0);
    class_addmethod(lor4_class, (t_method)lor4_initc, gensym("initc"), A_GIMME, 0);
    class_addmethod(lor4_class, (t_method)lor4_inith, gensym("inith"), A_GIMME, 0);
   // class_addmethod(lor4_class, (t_method)lor4_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(lor4_class, (t_method)lor4_initx, gensym("initx"), A_GIMME, 0);
    class_addmethod(lor4_class, (t_method)lor4_inity, gensym("inity"), A_GIMME, 0);
    class_addmethod(lor4_class, (t_method)lor4_initz, gensym("initz"), A_GIMME, 0);
}




