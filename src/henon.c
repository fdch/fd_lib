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

static t_class *henon_class;

typedef struct _henon
{
    t_object x_obj;
    t_float x_a, x_b;
    double x_ox, x_oy;
    int x_i;
    t_atom x_ixy[3];
} t_henon;

static void henon_doit(t_henon *x)
{
    double xnew, ynew;
    xnew = x->x_oy - x->x_a * (x->x_ox * x->x_ox) + 1.0;
    ynew = x->x_b * x->x_ox;
    x->x_ox = xnew;
    x->x_oy = ynew;
    SETFLOAT(&x->x_ixy[0], x->x_ox);
    SETFLOAT(&x->x_ixy[1], x->x_oy);
    SETFLOAT(&x->x_ixy[2], x->x_i);
}

static void henon_bang(t_henon *x)
{
    henon_doit(x);
    SETFLOAT(&x->x_ixy[2], 0.);
    outlet_list(x->x_obj.te_outlet, gensym("list"), 3, x->x_ixy);
}

static void henon_print(t_henon *x, t_floatarg f)
{
    if (f <= 0)
        return henon_bang(x);

    x->x_i = 0;
    while (f--)
    {
        henon_doit(x);
        outlet_list(x->x_obj.te_outlet, gensym("list"), 3, x->x_ixy);
        x->x_i++;
    }
}

static void henon_reset(t_henon *x)
{
    x->x_a = 7. / 5.;
    x->x_b = .3;
    x->x_ox = x->x_oy = 0.;
    SETFLOAT(&x->x_ixy[0], 0.);
    SETFLOAT(&x->x_ixy[1], 0.);
    SETFLOAT(&x->x_ixy[2], 0.);
}

static void *henon_new(t_floatarg i, t_floatarg a, t_floatarg b)
{
    t_henon *x = (t_henon *)pd_new(henon_class);
    outlet_new(&x->x_obj, &s_list);
    x->x_i = i ? (int)i : 1000;
    x->x_a = a ? a : (7. / 5.);
    x->x_b = b ? b : .3;
    if (!a)
        floatinlet_new(&x->x_obj, &x->x_a);
    if (!b)
        floatinlet_new(&x->x_obj, &x->x_b);
    return (void *)x;
}

void henon_setup(void)
{
    henon_class =
        class_new(gensym("henon"), (t_newmethod)henon_new, 0, sizeof(t_henon),
                  CLASS_DEFAULT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
    class_addbang(henon_class, henon_bang);
    class_addmethod(henon_class, (t_method)henon_print, gensym("print"),
                    A_FLOAT, A_NULL);
    class_addmethod(henon_class, (t_method)henon_reset, gensym("reset"),
                    A_NULL);
    class_addmethod(henon_class, (t_method)henon_reset, gensym("clear"),
                    A_NULL);
}
