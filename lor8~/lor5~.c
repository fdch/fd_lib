#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

float maximum = 100000000;
float minimum = 0;
/* ------------------------ lor5~ ----------------------------- */

/* tilde object to take absolute value. */

static t_class *lor5_class;

typedef struct _lor5
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;
    t_float val;    	/* place to hold inlet's value if it's set by message */

	t_outlet *out1;
    t_outlet *out2;
   t_outlet *out3;
	t_float x, y, z;
	t_float inith, inita, initb, initc, dist;
} t_lor5;

    
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
	return (f / (maxval(f) + fabsf(minval(f))));
}
static t_int *lor5_perform(t_int *w)
{
    t_lor5 *x = (t_lor5 *)(w[1]);  
    t_float *in = (t_float *)(w[2]);
    t_float *out1 = (t_float *)(w[3]);
    t_float *out2 = (t_float *)(w[4]);
    t_float *out3 = (t_float *)(w[5]);
    int n = (int)(w[6]);

    
    float x0, y0, z0, x1, y1, z1;
   	float h = x->inith; //step-size
   	float a = x->inita;
   	float b = x->initb;
   	float c = x->initc;
   	float f = 0;
    float t_dist;
    
    while (n--)
    {
    	//f = *in++;
		//*out++  = x->prev = (f + x->prev * x->val) / 2;
		x0 = x->x;
   		y0 = x->y;
   		z0 = x->z;
   		
   		x1 = x0 + h * a * (y0 - x0);
   		y1 = y0 + h * (x0 * (b - z0) - y0);
   		z1 = z0 + h * (x0 * y0 - c * z0);   	
   	
		//t_dist = fd_norm(sqrt((x1 * x1) + (y1 * y1)  + (z1 * z1)));
   		   	
		//*out1++ = x->x = fd_norm(f * x1);
		//*out2++ = x->y  = fd_norm(f * y1);
	    //*out3++ = x->z  = fd_norm(f * z1);
	    *out1++ = x->x = fd_norm(x1);
		*out2++ = x->y  = fd_norm(y1);
	    *out3++ = x->z  = fd_norm(z1);
		
	//	x->x = x1;
   // x->y = y1;
   	//	x->z = z1;
    }
    return (w+7);
}


void lor5_tilde_free(t_lor5 *x)
{
 outlet_free(x->out1);
 outlet_free(x->out2);
 outlet_free(x->out3);
}



    /* called to start DSP.  Here we call Pd back to add our perform
    routine to a linear callback list which Pd in turn calls to grind
    out the samples. */
static void lor5_dsp(t_lor5 *x, t_signal **sp)
{
    dsp_add(lor5_perform, 6,  x, 
    sp[0]->s_vec, 
    sp[1]->s_vec,
    sp[2]->s_vec,
    sp[3]->s_vec,
    sp[0]->s_n);
}



/*
********************** VARIABLES *********************
*/

/*
********************** inita *********************
*/
void lor5_inita(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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
void lor5_initb(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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
void lor5_initc(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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
void lor5_inith(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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
void lor5_initx(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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
void lor5_inity(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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
void lor5_initz(t_lor5 *x, t_symbol *selector, int argcount, t_atom *argvec)
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

static void *lor5_new(void)
{
    t_lor5 *x = (t_lor5 *)pd_new(lor5_class);
    x->out1 = outlet_new(&x->x_obj, gensym("signal"));
    x->out2 = outlet_new(&x->x_obj, gensym("signal"));
   x->out3 = outlet_new(&x->x_obj, gensym("signal"));
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
void lor5_tilde_setup(void)
{
    lor5_class = class_new(gensym("lor5~"), (t_newmethod)lor5_new, 0,
    	sizeof(t_lor5), 0, A_DEFFLOAT, 0);
	    /* this is magic to declare that the leftmost, "main" inlet
	    takes signals; other signal inlets are done differently... */
    CLASS_MAINSIGNALIN(lor5_class, t_lor5, x_f);
    	/* here we tell Pd about the "dsp" method, which is called back
	when DSP is turned on. */
    class_addmethod(lor5_class, (t_method)lor5_dsp, gensym("dsp"), 0);
	//class_addmethod(lor5_class, (t_method)lor5_ft1, gensym("ft1"), A_FLOAT, 0);
	//variables
    class_addmethod(lor5_class, (t_method)lor5_inita, gensym("inita"), A_GIMME, 0);
    class_addmethod(lor5_class, (t_method)lor5_initb, gensym("initb"), A_GIMME, 0);
    class_addmethod(lor5_class, (t_method)lor5_initc, gensym("initc"), A_GIMME, 0);
    class_addmethod(lor5_class, (t_method)lor5_inith, gensym("inith"), A_GIMME, 0);
   // class_addmethod(lor5_class, (t_method)lor5_reset, gensym("reset"), A_GIMME, 0);
    class_addmethod(lor5_class, (t_method)lor5_initx, gensym("initx"), A_GIMME, 0);
    class_addmethod(lor5_class, (t_method)lor5_inity, gensym("inity"), A_GIMME, 0);
    class_addmethod(lor5_class, (t_method)lor5_initz, gensym("initz"), A_GIMME, 0);
}




