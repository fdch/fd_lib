#include "m_pd.h"
#include "math.h"

/*------------------ cantor1 --------------------*/

typedef struct cantor1	{
	t_object x_ob;
	t_outlet *x_outlet0;
	double x;
} t_cantor1;

void cantor1_float(t_cantor1 *x, t_floatarg f) {
   if (f <= 0.5) {
   		x->x = 3 * f;
      	outlet_float(x->x_outlet0, x->x);
	}	else if (f > 0.5) {
		x->x = 3 * f * (-1) + 3;
		outlet_float(x->x_outlet0, x->x);
		}
}

t_class *cantor1_class;

void *cantor1_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_cantor1 *x = (t_cantor1 *)pd_new(cantor1_class);
	//default values
	x->x = 0;
	//end default values
    x->x_outlet0 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

void cantor1_setup(void)
{
    cantor1_class = class_new(gensym("cantor1"), (t_newmethod)cantor1_new, 0, sizeof(t_cantor1), 0, 0);
    class_addfloat(cantor1_class, cantor1_float);
}