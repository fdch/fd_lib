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

static t_class *prandom_class;

typedef struct prandom
{
    t_object x_ob;
    t_norm x_result;
    int x_norm;
} t_prandom;

static void prandom_norm(t_prandom *x, t_floatarg fnorm)
{
    x->x_norm = !!(int)fnorm;
}

static void prandom_reset(t_prandom *x) { norm_reset(&x->x_result, 1.0); }

static void prandom_bang(t_prandom *x)
{
    norm_setval(&x->x_result, rand());
    outlet_float(x->x_ob.te_outlet,
                 x->x_norm ? norm_getnorm(&x->x_result) : x->x_result.x_value);
}

static void *prandom_new()
{
    t_prandom *x = (t_prandom *)pd_new(prandom_class);
    x->x_norm = 0;
    outlet_new(&x->x_ob, gensym("float"));
    return (void *)x;
}

void prandom_setup(void)
{
    prandom_class = class_new(gensym("prandom"), (t_newmethod)prandom_new, 0,
                              sizeof(t_prandom), 0, A_NULL);
    class_addbang(prandom_class, prandom_bang);
    class_addmethod(prandom_class, (t_method)prandom_norm, gensym("normalize"),
                    A_FLOAT, A_NULL);
    class_addmethod(prandom_class, (t_method)prandom_reset, gensym("reset"),
                    A_NULL);
}
