#include "m_pd.h"
#include "math.h"

/*------------------ cantor --------------------*/

typedef struct cantor	{
	t_object x_ob;
	t_outlet *x_outlet0;
	double x;
} t_cantor;

void cantor_float(t_cantor *x, t_floatarg f) {
   if (f <= 0.5) {
   		x->x = 3 * f;
      	outlet_float(x->x_outlet0, x->x);
	}	else if (f > 0.5) {
		x->x = 3 * f * (-1) + 3;
		outlet_float(x->x_outlet0, x->x);
		}
}

t_class *cantor_class;

void *cantor_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_cantor *x = (t_cantor *)pd_new(cantor_class);
	//default values
	x->x = 0;
	//end default values
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

void cantor_setup(void)
{
    cantor_class = class_new(gensym("cantor"), (t_newmethod)cantor_new, 0, sizeof(t_cantor), 0, 0);
    class_addfloat(cantor_class, cantor_float);
}