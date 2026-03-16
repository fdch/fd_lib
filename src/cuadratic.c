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

static t_class *cuadratic_class;

typedef struct cuadratic
{
    t_object x_ob;
    double x_x, x_c;
} t_cuadratic;

static void cuadratic_step(double *x, double c) { *x = ((*x) * (*x)) + c; }

static void cuadratic_float(t_cuadratic *x, t_floatarg fsize)
{
    int size = fsize > 0.0 ? (int)fsize : 1;
    t_atom *xy = (t_atom *)t_getbytes(size * sizeof(t_atom));

    for (int i = 0; i < size; i++)
    {
        cuadratic_step(&x->x_x, x->x_c);
        SETFLOAT(&xy[i], (t_float)x->x_x);
    }
    outlet_list(x->x_ob.te_outlet, gensym("list"), size, xy);
    t_freebytes(xy, size * sizeof(t_atom));
}

static void cuadratic_bang(t_cuadratic *x)
{
    cuadratic_step(&x->x_x, x->x_c);
    outlet_float(x->x_ob.te_outlet, x->x_x);
}

static void cuadratic_c(t_cuadratic *x, t_float f) { x->x_c = (double)f; }

static void cuadratic_x(t_cuadratic *x, t_float f) { x->x_x = (double)f; }

static void cuadratic_reset(t_cuadratic *x)
{
    x->x_x = 1.97;
    x->x_c = -2.0;
}

static void *cuadratic_new(t_floatarg fx, t_floatarg fc)
{
    t_cuadratic *x = (t_cuadratic *)pd_new(cuadratic_class);
    outlet_new(&x->x_ob, &s_float);
    cuadratic_reset(x);
    if (fx)
        x->x_x = fx;
    if (fc)
        x->x_c = fc;
    floatinlet_new(&x->x_ob, (t_float *)&x->x_x);
    floatinlet_new(&x->x_ob, (t_float *)&x->x_c);
    return (void *)x;
}

void cuadratic_setup(void)
{
    cuadratic_class = class_new(gensym("cuadratic"), (t_newmethod)cuadratic_new,
                                0, sizeof(t_cuadratic), CLASS_DEFAULT,
                                A_DEFFLOAT, A_DEFFLOAT, A_NULL);
    class_addbang(cuadratic_class, cuadratic_bang);
    class_addfloat(cuadratic_class, cuadratic_float);
    class_addmethod(cuadratic_class, (t_method)cuadratic_c, gensym("c"),
                    A_FLOAT, A_NULL);
    class_addmethod(cuadratic_class, (t_method)cuadratic_x, gensym("x"),
                    A_FLOAT, A_NULL);
    class_addmethod(cuadratic_class, (t_method)cuadratic_reset, gensym("reset"),
                    A_NULL);
}
