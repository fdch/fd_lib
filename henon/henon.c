#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

#define N 5000

typedef struct _henon {  
	t_object  x_obj;
	t_int i_count;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	t_outlet *x_outlet3;
} t_henon;

void henon_bang(t_henon *x)  {
	double a, b;
	double xnew, ynew;
	t_float i=x->i_count;
	int px, py;
	float ox, oy;
	ox = oy = 0;
	a = 7.0/5.0;
	b = 0.3;

	for (i = 0; i < N; i++)
	{
		xnew = oy - (a*(ox*ox)) + 1.0;
		ynew = b * ox;

		px = (int)((ox + 2.0) * 150.0);
		py = (int)((oy + 0.4) * 400.0);

		ox = xnew;
		oy = ynew;
		
		outlet_float(x->x_outlet1, i);
		outlet_float(x->x_outlet2, ox);
		outlet_float(x->x_outlet3, oy);
	}
}

static t_class *henon_class;  

void *henon_new(t_floatarg f)  {  
	t_henon *x = (t_henon *)pd_new(henon_class);
	x->i_count=f;  
	outlet_new(&x->x_obj, &s_float);
    x->x_outlet1 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet2 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet3 = outlet_new(&x->x_obj, gensym("float"));
	return (void *)x;
}  

void henon_setup(void) {  
  henon_class = class_new(gensym("henon"),  
        (t_newmethod)henon_new,  
        0, sizeof(t_henon),  
        CLASS_DEFAULT,  
        A_DEFFLOAT, 0);  
  class_addbang(henon_class, henon_bang);  
}