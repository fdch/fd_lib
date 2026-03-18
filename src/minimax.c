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

static t_class *minimax_class;

typedef struct minimax
{
    t_object x_ob;
    t_outlet *x_outlet1;
    t_outlet *x_outlet2;
    t_norm x_f;
    int x_norm;
} t_minimax;

static void minimax_float(t_minimax *x, t_floatarg f)
{
    norm_setval(&x->x_f, f);
    outlet_float(x->x_outlet2, x->x_f.x_max);
    outlet_float(x->x_outlet1, x->x_f.x_min);
    outlet_float(x->x_ob.te_outlet, x->x_norm ? norm_getnorm(&x->x_f) : f);
}

static void minimax_flush(t_minimax *x) { norm_reset(&x->x_f, 0.0); }

static void minimax_normalize(t_minimax *x, t_floatarg fnorm)
{
    x->x_norm = !!(int)fnorm;
}

static void *minimax_new(t_floatarg fnorm)
{
    t_minimax *x = (t_minimax *)pd_new(minimax_class);
    outlet_new(&x->x_ob, gensym("float"));
    x->x_outlet1 = outlet_new(&x->x_ob, gensym("float"));
    x->x_outlet2 = outlet_new(&x->x_ob, gensym("float"));
    minimax_flush(x);
    minimax_normalize(x, fnorm);
    return (void *)x;
}

void minimax_setup(void)
{
    minimax_class = class_new(gensym("minimax"), (t_newmethod)minimax_new, 0,
                              sizeof(t_minimax), 0, A_DEFFLOAT, A_NULL);
    class_addfloat(minimax_class, minimax_float);
    class_addmethod(minimax_class, (t_method)minimax_flush, gensym("flush"),
                    A_NULL);
    class_addmethod(minimax_class, (t_method)minimax_normalize,
                    gensym("normalize"), A_FLOAT, A_NULL);
}
