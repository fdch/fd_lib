#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

#define N 10000

typedef struct _oil {  
	t_object  x_obj;
	t_int i_count;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	t_outlet *x_outlet3;
	t_outlet *x_outlet4;
} t_oil;

void oil_bang(t_oil *x)  {
	t_float f = x->i_count;
	int i;
	i = f;
	double x0,y0,z0,x1,y1,z1;
	double h = 0.01;
	double a = 10.0;
	double b = 28.0;
	double c = 8.0 / 3.0;		

	x0 = 0.1;
	y0 = 0;
	z0 = 0;
	
	for (i=0; i<N; i++) {	
		x1 = x0 + h * a * (y0 - x0);
		y1 = y0 + h * (x0 * (b - z0) - y0);
		z1 = z0 + h * (x0 * y0 - c * z0);
		x0 = x1;
		y0 = y1;
		z0 = z1;
    	outlet_float(x->x_outlet1, i);
    	outlet_float(x->x_outlet2, x0);
    	outlet_float(x->x_outlet3, y0);
    	outlet_float(x->x_outlet4, z0);
    }
}  

static t_class *oil_class;  


void *oil_new(t_floatarg f)  {  
	t_oil *x = (t_oil *)pd_new(oil_class);
	x->i_count=f;  
	outlet_new(&x->x_obj, &s_float);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("ft1"));
    x->x_outlet1 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet2 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet3 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet4 = outlet_new(&x->x_obj, gensym("float"));
	return (void *)x;
}  

void oil_setup(void) {  
  oil_class = class_new(gensym("oil"),  
        (t_newmethod)oil_new,  
        0, sizeof(t_oil),  
        CLASS_DEFAULT,  
        A_DEFFLOAT, 0);  
  class_addbang(oil_class, oil_bang);  
}