#include "m_pd.h"
#include "math.h"
#include "stdlib.h"

typedef struct reflect	{
  t_object x_ob;
  t_float f;
  t_outlet *x_outlet0;
  double range;
} t_reflect;

static double reflectit(float f, double range) {
	double temp = f;
	do 
	temp = (fabs((fabs(temp + range) - range) * -1 + range ) - range) * -1;	
	while (temp < (range * -1));
	return temp;
}
void reflect_float(t_reflect *x, t_floatarg f) {
	outlet_float(x->x_outlet0, reflectit(f,x->range));
}

t_class *reflect_class;

void *reflect_new(t_floatarg f)	{
    t_reflect *x = (t_reflect *)pd_new(reflect_class);
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
   	x->range =  (f != 0) ? f : 100;
    return (void *)x;
}
void reflect_setup(void)	{
    reflect_class = class_new(gensym("reflect"),
    (t_newmethod)reflect_new, 0,
    	sizeof(t_reflect), 0, A_DEFFLOAT,A_DEFFLOAT, 0);
    class_addfloat(reflect_class, reflect_float);
}