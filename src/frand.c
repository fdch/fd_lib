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

static t_class *frand_class;

typedef struct frand
{
    t_object x_ob;
    double x_min, x_init_min;
    double x_max, x_init_max;
    double x_result;
    unsigned int x_seed, x_init_seed;
} t_frand;

static void frand_set(t_frand *x, t_floatarg fmin, t_floatarg fmax)
{
    x->x_min = (double)fmin;
    x->x_max = (double)fmax;
}

static void frand_reset(t_frand *x)
{
    x->x_min = x->x_init_min;
    x->x_max = x->x_init_max;
    x->x_result = 0.0;
}

static void frand_bang(t_frand *x)
{
    x->x_seed = x->x_seed * 1103515245 + 12345;
    x->x_result = ((x->x_seed / 65536) % 32768) * (x->x_max - x->x_min) / 32767;
    x->x_result += x->x_min;
    outlet_float(x->x_ob.te_outlet, x->x_result);
}

static void frand_seed(t_frand *x, t_floatarg f)
{
    x->x_seed = (unsigned int)f;
}

static void *frand_new(t_floatarg f, t_floatarg g)
{
    t_frand *x = (t_frand *)pd_new(frand_class);
    outlet_new(&x->x_ob, gensym("float"));
    x->x_seed = x->x_init_seed = 42;
    x->x_min = x->x_init_min = f ? (double)f : 0.;
    x->x_max = x->x_init_max = g ? (double)g : 1.;
    x->x_result = 0.0;
    return (void *)x;
}

void frand_setup(void)
{
    frand_class = class_new(gensym("frand"), (t_newmethod)frand_new, 0,
                            sizeof(t_frand), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);

    class_addbang(frand_class, frand_bang);
    class_addmethod(frand_class, (t_method)frand_set, gensym("set"), A_FLOAT,
                    A_FLOAT, A_NULL);
    class_addmethod(frand_class, (t_method)frand_reset, gensym("reset"),
                    A_DEFFLOAT, A_DEFFLOAT, A_NULL);
    class_addmethod(frand_class, (t_method)frand_seed, gensym("seed"), A_FLOAT,
                    A_NULL);
}
