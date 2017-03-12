#include "m_pd.h"
#include <stdio.h>
#include <string.h>

//taken from pd's clock objects (x_time.c)

/* -------------------------- spline ------------------------------ */
#define DEFAULTLINEGRAIN 20

t_class *spline_class;

typedef struct _spline
{
    t_object x_obj;
    t_clock *x_clock;
    double x_targettime;
    t_float x_targetval;
    double x_prevtime;
    t_float x_setval;
    int x_gotinlet;
    t_float x_grain;
    double x_1overtimediff;
    double x_in1val;
} t_spline;

void spline_tick(t_spline *x)
{
    double timenow = clock_getsystime();
    double msectogo = - clock_gettimesince(x->x_targettime);
    if (msectogo < 1E-9)
    {
        outlet_float(x->x_obj.ob_outlet, x->x_targetval);
    }
    else
    {
        outlet_float(x->x_obj.ob_outlet,
            x->x_setval + x->x_1overtimediff * (timenow - x->x_prevtime)
                * (x->x_targetval - x->x_setval));
        if (x->x_grain <= 0)
            x->x_grain = DEFAULTLINEGRAIN;
        clock_delay(x->x_clock,
            (x->x_grain > msectogo ? msectogo : x->x_grain));
    }
}

void spline_float(t_spline *x, t_float f)
{
    double timenow = clock_getsystime();
    if (x->x_gotinlet && x->x_in1val > 0)
    {
        if (timenow > x->x_targettime) x->x_setval = x->x_targetval;
        else x->x_setval = x->x_setval + x->x_1overtimediff *
            (timenow - x->x_prevtime)
            * (x->x_targetval - x->x_setval);
        x->x_prevtime = timenow;
        x->x_targettime = clock_getsystimeafter(x->x_in1val);
        x->x_targetval = f;
        spline_tick(x);
        x->x_gotinlet = 0;
        x->x_1overtimediff = 1./ (x->x_targettime - timenow);
        if (x->x_grain <= 0)
            x->x_grain = DEFAULTLINEGRAIN;
        clock_delay(x->x_clock,
            (x->x_grain > x->x_in1val ? x->x_in1val : x->x_grain));

    }
    else
    {
        clock_unset(x->x_clock);
        x->x_targetval = x->x_setval = f;
        outlet_float(x->x_obj.ob_outlet, f);
    }
    x->x_gotinlet = 0;
}

void spline_ft1(t_spline *x, t_floatarg g)
{
    x->x_in1val = g;
    x->x_gotinlet = 1;
}

void spline_stop(t_spline *x)
{
    x->x_targetval = x->x_setval;
    clock_unset(x->x_clock);
}

void spline_set(t_spline *x, t_floatarg f)
{
    clock_unset(x->x_clock);
    x->x_targetval = x->x_setval = f;
}
 
 void spline_free(t_spline *x)
{
    clock_free(x->x_clock);
}

void *spline_new(t_floatarg f, t_floatarg grain)
{
    t_spline *x = (t_spline *)pd_new(spline_class);
    x->x_targetval = x->x_setval = f;
    x->x_gotinlet = 0;
    x->x_1overtimediff = 1;
    x->x_clock = clock_new(x, (t_method)spline_tick);
    x->x_targettime = x->x_prevtime = clock_getsystime();
    x->x_grain = grain;
    outlet_new(&x->x_obj, gensym("float"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("ft1"));
    floatinlet_new(&x->x_obj, &x->x_grain);
    return (x);
}

void spline_setup(void)
{
    spline_class = class_new(gensym("spline"), (t_newmethod)spline_new,
        (t_method)spline_free, sizeof(t_spline), 0, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(spline_class, (t_method)spline_ft1,
        gensym("ft1"), A_FLOAT, 0);
    class_addmethod(spline_class, (t_method)spline_stop,
        gensym("stop"), 0);
    class_addmethod(spline_class, (t_method)spline_set,
        gensym("set"), A_FLOAT, 0);
    class_addfloat(spline_class, (t_method)spline_float);
}
