#include "m_pd.h"
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>

#define G_FLOOR 0.01

/*------------------ grieta --------------------*/

typedef struct grieta
{
	t_object x_ob;
	t_outlet *x_outlet0;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	t_outlet *x_outlet3;
	float min,max,size,totalsize;
	int samplerate;
} t_grieta;

void grieta_float(t_grieta *x, t_float f) {

    x->min = f;
    
}
void grieta_bang(t_grieta *x) {

	x->size = x->max - x->min;
	
	if (x->size <= G_FLOOR) {
		
		x->max = x->min + G_FLOOR;
		if (x->max >= x->totalsize)
			x->max = x->totalsize;
		if (x->min <= 0)
			x->min = 0;
	}
	
	float speed;
	speed = x->samplerate / x->size;
	
	
	
	outlet_float(x->x_outlet0, x->size);
	outlet_float(x->x_outlet1, x->min);
	outlet_float(x->x_outlet2, x->max);
	outlet_float(x->x_outlet3, speed);
	
}

void grieta_samplerate(t_grieta *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    		x->samplerate = argvec[i].a_w.w_float;
    }
}
void grieta_totalsize(t_grieta *x, t_symbol *selector, int argcount, t_atom *argvec)
{
	int i;
	for (i = 0; i < argcount; i++)
    {
    	if (argvec[i].a_type == A_FLOAT)
    		x->totalsize = argvec[i].a_w.w_float;
    }
}

t_class *grieta_class;

void *grieta_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_grieta *x = (t_grieta *)pd_new(grieta_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
    x->x_outlet3 = outlet_new(&x->x_ob, &s_float);
    x->min = x->max = x->size = 0; 
    x->totalsize = 48000;
    x->samplerate = 48000;
    floatinlet_new(&x->x_ob, &x->max);
    return (void *)x;
}

void grieta_setup(void)
{

    grieta_class = class_new(gensym("grieta"),
    (t_newmethod)grieta_new, 0,
    	sizeof(t_grieta), 0, A_DEFFLOAT,A_DEFFLOAT, 0);

    class_addbang(grieta_class, grieta_bang);

    class_addfloat(grieta_class, grieta_float);
	
    class_addmethod(grieta_class, (t_method)grieta_samplerate, gensym("samplerate"), A_GIMME, 0);
    class_addmethod(grieta_class, (t_method)grieta_totalsize, gensym("totalsize"), A_GIMME, 0);
    
    
}
