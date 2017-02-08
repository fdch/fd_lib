#include "m_pd.h"
#include <math.h>
#include <stdio.h>
#include <string.h>



typedef struct _randy
{
    t_object x_obj;
    t_float x_f;
    unsigned int x_state;
} t_randy;


int makeseed(void)
{
    unsigned int randy_nextseed = 1489853723;
    randy_nextseed = randy_nextseed * 435898247 + 938284287;
    return (randy_nextseed & 0x7fffffff);
}

void randy_bang(t_randy *x)
{
    int n = x->x_f, nval;
    int range = (n < 1 ? 1 : n);
    unsigned int randval = x->x_state;
    x->x_state = randval = randval * 472940017 + 832416023;
    nval = ((double)range) * ((double)randval)
        * (1./4294967296.);
    if (nval >= range) nval = range-1;
    outlet_float(x->x_obj.ob_outlet, nval);
}

void randy_seed(t_randy *x, t_float f, t_float glob)
{
    x->x_state = f;
}

t_class *randy_class;

void *randy_new(t_floatarg f)
{
    t_randy *x = (t_randy *)pd_new(randy_class);
    x->x_f = f;
    x->x_state = makeseed();
    floatinlet_new(&x->x_obj, &x->x_f);
    outlet_new(&x->x_obj, &s_float);
    return (x);
}

void randy_setup(void)
{
    randy_class = class_new(gensym("randy"), (t_newmethod)randy_new, 0,
        sizeof(t_randy), 0, A_DEFFLOAT, 0);
    class_addbang(randy_class, randy_bang);
    class_addmethod(randy_class, (t_method)randy_seed,
        gensym("seed"), A_FLOAT, 0);
}