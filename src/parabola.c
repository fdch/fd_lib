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

/*------------------ parabola --------------------*/

static t_class *parabola_class;

typedef struct parabola
{
    t_object x_ob;
    t_float x_alpha;
    t_norm x_x;
    int x_norm;
} t_parabola;

static void parabola_bang(t_parabola *x)
{
    const t_float x0 = x->x_x.x_value;
    norm_setval(&x->x_x, (1.0 - x0) * x->x_alpha * x0);
    return outlet_float(x->x_ob.te_outlet,
                        x->x_norm ? norm_getnorm(&x->x_x) : x->x_x.x_value);
}

static void parabola_alpha(t_parabola *x, t_floatarg falpha)
{
    x->x_alpha = !falpha ? 1.0 : falpha;
}

static void parabola_float(t_parabola *x, t_floatarg fval)
{
    norm_setval(&x->x_x, fval);
    parabola_bang(x);
}

static void parabola_reset(t_parabola *x)
{
    norm_reset(&x->x_x, 1.0);
    parabola_alpha(x, 1.0);
}

static void parabola_normalize(t_parabola *x, t_floatarg fnorm)
{
    x->x_norm = !!(int)fnorm;
}

static void *parabola_new(t_floatarg falpha)
{
    t_parabola *x = (t_parabola *)pd_new(parabola_class);
    x->x_norm = 0;
    norm_setval(&x->x_x, 1.0);
    parabola_alpha(x, falpha);
    if (!falpha)
        floatinlet_new(&x->x_ob, &x->x_alpha);

    outlet_new(&x->x_ob, gensym("float"));
    return (void *)x;
}

void parabola_setup(void)
{
    parabola_class =
        class_new(gensym("parabola"), (t_newmethod)parabola_new, 0,
                  sizeof(t_parabola), CLASS_DEFAULT, A_DEFFLOAT, A_NULL);
    class_addbang(parabola_class, parabola_bang);
    class_addfloat(parabola_class, parabola_float);
    class_addmethod(parabola_class, (t_method)parabola_alpha, gensym("alpha"),
                    A_FLOAT, A_NULL);
    class_addmethod(parabola_class, (t_method)parabola_normalize,
                    gensym("normalize"), A_FLOAT, A_NULL);
    class_addmethod(parabola_class, (t_method)parabola_reset, gensym("reset"),
                    A_NULL);
}
