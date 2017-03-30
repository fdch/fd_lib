#include "m_pd.h"
#include "stdlib.h"
#include "math.h"

typedef struct _henon2 {  
	t_object  x_obj;
	t_int i_count;
	t_outlet *x_outlet0;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	t_outlet *x_outlet3;
} t_henon2;

void henon2_bang(t_henon2 *x)  {
	int i;
	i = x->i_count;
	x->i_count++;
	double a, b;
	double xnew, ynew;	
	int px, py;
	float ox, oy;
	ox = oy = 0;
	a = 7.0/5.0;
	b = 0.3;
	if (i < 1000) {

		xnew = oy - (a*(ox*ox)) + 1.0;
		ynew = b * ox;

		px = (int)((ox + 2.0) * 150.0);
		py = (int)((oy + 0.4) * 400.0);

		ox = xnew;
		oy = ynew;
		
		outlet_float(x->x_outlet0, i);
		outlet_float(x->x_outlet1, i++);
		outlet_float(x->x_outlet2, ox);
		outlet_float(x->x_outlet3, oy);
	} else {
		i = x->i_count = 0;
	}
}

static t_class *henon2_class;  

void *henon2_new(t_floatarg f)  {  
	t_henon2 *x = (t_henon2 *)pd_new(henon2_class);
	x->i_count=f;  
	x->x_outlet0 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet1 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet2 = outlet_new(&x->x_obj, gensym("float"));
    x->x_outlet3 = outlet_new(&x->x_obj, gensym("float"));
	return (void *)x;
}  

void henon2_setup(void) {  
  henon2_class = class_new(gensym("henon2"),  
        (t_newmethod)henon2_new,  
        0, sizeof(t_henon2),  
        CLASS_DEFAULT,  
        A_DEFFLOAT, 0);  
  class_addbang(henon2_class, henon2_bang);  
}