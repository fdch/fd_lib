#include "m_pd.h"
#include <math.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

typedef struct PdCon16 {
	t_object x_obj; 
    t_float x_f;
    t_inlet *in2;
    t_inlet *in3;
    t_inlet *in4;
	t_outlet *out1;
    t_outlet *out2;
    t_outlet *out3;
    t_outlet *out4;
	t_float x, y, z;
} t_PdCon16;

//void PdCon16_bang(t_PdCon16_tilde *x) {
//	post("PdCon16~ NYC2016");
//}

static t_class *PdCon16_class;

static t_int *PdCon16_perform(t_int *w)
{
    t_PdCon16 *x = (t_PdCon16 *)(w[1]);  
    t_sample *in1 = (t_sample *)(w[2]);
    t_sample *in2 = (t_sample *)(w[3]);
    t_sample *in3 = (t_sample *)(w[4]);
    t_sample *in4 = (t_sample *)(w[5]);
    t_sample *out1 = (t_sample *)(w[6]);
    t_sample *out2 = (t_sample *)(w[7]);
    t_sample *out3 = (t_sample *)(w[8]);
    t_sample *out4 = (t_sample *)(w[9]);
    int n = (int)(w[10]);
    
    float x0, y0, z0, x1, y1, z1;
    float h, a, b, c;
    float pdist, cdist;

    while (n--)
    {
    	h= *in1++;
		a= *in2++;
		b=*in3++;
		c=*in4++;
		
		x0 = x->x;
   		y0 = x->y;
   		z0 = x->z;
   		
   		pdist = sqrt((x0 * x0) + (y0 * y0)  + (z0 * z0));
   		
   		x1 = x0 + h * a * (y0 - x0);
   		y1 = y0 + h * (x0 * (b - z0) - y0);
   		z1 = z0 + h * (x0 * y0 - c * z0);   	
   	
		
		cdist = sqrt((x1 * x1) + (y1 * y1)  + (z1 * z1));
	 	
	 	
	    *out1++ = x->x = x1;
		*out2++ = x->y = y1;
	    *out3++ = x->z = z1;
	    *out4++ = sqrt((pdist*pdist)+(cdist*cdist));

    }
    return (w+11);
}
static void PdCon16_dsp(t_PdCon16 *x, t_signal **sp)
{
    dsp_add(PdCon16_perform, 10,  x, 
    sp[0]->s_vec, 
    sp[1]->s_vec,
    sp[2]->s_vec,
    sp[3]->s_vec,
    sp[4]->s_vec,
    sp[5]->s_vec,
    sp[6]->s_vec,
    sp[7]->s_vec,
    sp[0]->s_n);
}

void PdCon16_free(t_PdCon16 *x)
{
	inlet_free(x->in2);
	inlet_free(x->in3);
	inlet_free(x->in4);
	outlet_free(x->out1);
	outlet_free(x->out2);
	outlet_free(x->out3);
	outlet_free(x->out4);
}

void *PdCon16_new()
{
	t_PdCon16 *x = (t_PdCon16 *)pd_new(PdCon16_class);
	x->in2 = inlet_new(&x->x_obj,&x->x_obj.ob_pd, &s_signal,&s_signal);
    x->in3 = inlet_new(&x->x_obj,&x->x_obj.ob_pd, &s_signal,&s_signal);
    x->in4 = inlet_new(&x->x_obj,&x->x_obj.ob_pd, &s_signal,&s_signal);
    x->out1 = outlet_new(&x->x_obj, &s_signal);
    x->out2 = outlet_new(&x->x_obj, &s_signal);
    x->out3 = outlet_new(&x->x_obj, &s_signal);
    x->out4 = outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0.0;
	x->x = 0.1;
	x->y = 0;
	x->z = 0;
	return (x);
}
void PdCon16_tilde_setup(void)
{
	PdCon16_class = class_new(gensym("PdCon16~"), (t_newmethod)PdCon16_new, 0, sizeof(t_PdCon16), 0, 0);
	CLASS_MAINSIGNALIN(PdCon16_class, t_PdCon16, x_f);
	class_addmethod(PdCon16_class, (t_method)PdCon16_dsp, gensym("dsp"), 0);

	//class_addbang(PdCon16_tilde_class, PdCon16_tilde_bang);
}
