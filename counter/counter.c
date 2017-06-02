#include "m_pd.h"
#include <string.h>
#include <math.h>

static t_class *counter_class;

typedef struct _counter {
	t_object x_obj;
	t_int count, m, r;
	t_float i, i_count;
} t_counter;

static t_float reflectit(t_float f, t_float r) {
	t_float t = f;
	do 
	t = (fabs((fabs(t + r) - r) * -1 + r ) - r) * -1;
	while (t < (r * -1));
	if (t==(-0))t=0;
	return t;
}
static t_float mod(float a, float N){return a - N*floor(a/N);} 

void counter_bang(t_counter *x)
{
	t_float f = x->count;
	x->count++;
	
	if (x->m != 0)
		outlet_float(x->x_obj.ob_outlet, mod(f,x->m));
	else if (x->i != 0) {
		t_float i = x->i;
		x->i_count += i;
		outlet_float(x->x_obj.ob_outlet, x->i_count);
	} else if (x->r != 0) {
		outlet_float(x->x_obj.ob_outlet, reflectit(x->count, x->r));
	} else
		outlet_float(x->x_obj.ob_outlet, f);
}
void counter_reset(t_counter *x)
{
	x->i_count = 0;
	x->count = 0;
}
void counter_float(t_counter *x, t_floatarg f)
{
	x->count = (int) f;
	x->i_count = f;
}
void counter_m(t_counter *x, t_float f)
{
	x->m = (int)f;
}
void counter_r(t_counter *x, t_float f)
{
	x->r = (int)fabsf(f);
}
void counter_i(t_counter *x, t_float f)
{
	x->i = f;
}
void *counter_new(t_symbol *s, t_int argc, t_atom *argv)
{
	t_counter *x = (t_counter *)pd_new(counter_class);
	while (argc && argv->a_type == A_SYMBOL && *argv->a_w.w_symbol->s_name == '-')
	{
		if (!strcmp(argv->a_w.w_symbol->s_name, "-m") && argc >= 2 && argv[1].a_type == A_FLOAT)
			counter_m(x,atom_getfloatarg(1, argc, argv)), argc -= 1, argv += 1;
		else if (!strcmp(argv->a_w.w_symbol->s_name, "-r") && argc >= 2 && argv[1].a_type == A_FLOAT)
			counter_r(x,atom_getfloatarg(1, argc, argv)), argc -= 1, argv += 1;
		else if (!strcmp(argv->a_w.w_symbol->s_name, "-i") && argc >= 2 && argv[1].a_type == A_FLOAT)
			counter_i(x,atom_getfloatarg(1, argc, argv)), argc -= 1, argv += 1;
		else
			error("counter: unknown flag ..."), postatom(argc, argv), endpost();
		argc--; argv++;
	}
	if (argv->a_type == A_FLOAT)
		counter_float(x,atom_getfloatarg(0, argc, argv));
	outlet_new(&x->x_obj, &s_float);
	return (void *)x;
}


void counter_setup(void)
{
	counter_class = class_new(gensym("counter"), (t_newmethod)counter_new, 0, sizeof(t_counter), CLASS_DEFAULT, A_GIMME, 0);

	class_addcreator((t_newmethod)counter_new, gensym("c"), A_GIMME, 0);
	class_addbang(counter_class, counter_bang);
	class_addmethod(counter_class, (t_method)counter_reset, gensym("reset"),0);
	class_addfloat(counter_class, counter_float);
	class_addmethod(counter_class, (t_method)counter_m, gensym("m"),A_FLOAT,0);
	class_addmethod(counter_class, (t_method)counter_r, gensym("r"),A_FLOAT,0);
	class_addmethod(counter_class, (t_method)counter_i, gensym("i"),A_FLOAT,0);
}