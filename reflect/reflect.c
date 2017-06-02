#include "m_pd.h"
#include <math.h>

typedef struct reflect {
	t_object x_obj;
	t_float r, f, o;
} t_reflect;

static t_float reflectit(t_float f, t_float r)
{
	t_float t = f;
	do 
	t = (fabsf((fabsf(t + r) - r) * -1 + r ) - r) * -1;
	while (t < (r * -1));
	return t;
}
void reflect_float(t_reflect *x, t_float f)
{
	if (0 < x->r)
		outlet_float(x->x_obj.ob_outlet, reflectit(f,x->r)+x->o);
	else
		pd_error(x,"range must be greater than 0");
}
void reflect_set(t_reflect *x, t_float f, t_float g)
{
	x->r = fabsf(f);
	x->o = g;
}
t_class *reflect_class;

void *reflect_new(t_symbol *s, t_int argc, t_atom *argv) {
	t_reflect *x = (t_reflect *)pd_new(reflect_class);
	outlet_new(&x->x_obj, &s_float);
	if (!argc)
		floatinlet_new(&x->x_obj, &x->r);
	else if (argc == 1)
		x->r = atom_getfloatarg(0,argc,argv);
	else if (argc == 2)
		reflect_set(x, atom_getfloatarg(0,argc,argv), atom_getfloatarg(1,argc,argv));
	else
		pd_error(x,"reflect: only accepts 2 arguments"), postatom(argc,argv), endpost();
	return (void *)x;
}
void reflect_setup(void)	{
	reflect_class = class_new(gensym("reflect"), (t_newmethod)reflect_new, 0, sizeof(t_reflect), CLASS_DEFAULT, A_GIMME, 0);
	class_addfloat(reflect_class, reflect_float);
	class_addmethod(reflect_class, (t_method)reflect_set, gensym("set"), A_FLOAT,A_FLOAT,0);
}