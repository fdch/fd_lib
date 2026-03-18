/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details. You
should have received a copy of the GNU General Public License along with this
program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "fdLib.h"

static t_class *crand_class;

typedef struct crand
{
    t_object x_ob;
    double x_min, x_init_min;
    double x_max, x_init_max;
    double x_result;
    time_t x_time;
    unsigned int x_seed, x_init_seed;
} t_crand;

static void crand_set(t_crand *x, t_floatarg fmin, t_floatarg fmax)
{
    x->x_min = (double)fmin;
    x->x_max = (double)fmax;
}

static void crand_reset(t_crand *x)
{
    x->x_min = x->x_init_min;
    x->x_max = x->x_init_max;
    x->x_result = 0.0;
}

static void crand_bang(t_crand *x)
{
    x->x_result = rand();
    x->x_result *= (x->x_max - x->x_min) / RAND_MAX;
    x->x_result += x->x_min;
    outlet_float(x->x_ob.te_outlet, (t_float)x->x_result);
}

static void crand_seed(t_crand *x, t_floatarg f)
{
    x->x_seed = (unsigned int)f;
    srand(x->x_seed);
}

static void *crand_new(t_floatarg f, t_floatarg g)
{
    t_crand *x = (t_crand *)pd_new(crand_class);
    x->x_init_seed = (unsigned int)time(&x->x_time);
    x->x_min = x->x_init_min = f ? (double)f : 0.;
    x->x_max = x->x_init_max = g ? (double)g : 1.;
    x->x_result = 0.0;
    crand_seed(x, (t_floatarg)x->x_init_seed);
    outlet_new(&x->x_ob, gensym("float"));
    return (void *)x;
}

void crand_setup(void)
{
    crand_class = class_new(gensym("crand"), (t_newmethod)crand_new, 0,
                            sizeof(t_crand), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
    class_addbang(crand_class, crand_bang);
    class_addmethod(crand_class, (t_method)crand_set, gensym("set"), A_FLOAT,
                    A_FLOAT, A_NULL);
    class_addmethod(crand_class, (t_method)crand_reset, gensym("reset"),
                    A_DEFFLOAT, A_DEFFLOAT, A_NULL);
    class_addmethod(crand_class, (t_method)crand_seed, gensym("seed"), A_FLOAT,
                    A_NULL);
}
