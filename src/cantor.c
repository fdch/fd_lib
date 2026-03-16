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

static t_class *cantor_class;

typedef struct cantor
{
    t_object x_ob;
    int x_d, x_n, x_u, x_i;
    t_float x_f;
} t_cantor;

static int cantor_inner(t_cantor *x, int d, int i)
{
    int in = 1;
    int y = i;
    for (int j = 0; j < d; j++)
    {
        in = in && !((x->x_u * y >= x->x_n) && (x->x_u * y < x->x_f * x->x_n));
        y = (x->x_u * y) % x->x_n;
    }
    return in;
}

static void cantor_doit(t_cantor *x)
{
    t_atom *set = (t_atom *)getbytes(x->x_d * x->x_n * sizeof(t_atom));

    for (int d = 0, idx = 0; d < x->x_d; d++)
        for (int i = 0; i < x->x_n; i++, idx++)
            SETFLOAT(&set[idx], cantor_inner(x, d, i));

    outlet_list(x->x_ob.te_outlet, &s_list, x->x_d * x->x_n, set);

    freebytes(set, x->x_d * x->x_n * sizeof(t_atom));
}

static void cantor_bang(t_cantor *x)
{
    outlet_float(x->x_ob.te_outlet, cantor_inner(x, x->x_d, x->x_i));
    x->x_i++;
}

static void cantor_float(t_cantor *x, t_floatarg f)
{
    x->x_n = (int)f;
    cantor_doit(x);
}
/* LATER restrict these? */
static void cantor_depth(t_cantor *x, t_floatarg f) { x->x_d = (int)f; }

static void cantor_width(t_cantor *x, t_floatarg f) { x->x_u = (int)f; }

static void cantor_coef(t_cantor *x, t_floatarg f) { x->x_f = f; }

static void *cantor_new(t_floatarg f)
{
    t_cantor *x = (t_cantor *)pd_new(cantor_class);
    outlet_new(&x->x_ob, &s_float);
    x->x_n = 80;
    if (f)
        x->x_n = f;
    x->x_d = 5;
    x->x_u = 3;
    x->x_f = 2;
    return (x);
}

void cantor_setup()
{
    cantor_class = class_new(gensym("cantor"), (t_newmethod)cantor_new, 0,
                             sizeof(t_cantor), CLASS_DEFAULT, A_DEFFLOAT, 0);
    class_addbang(cantor_class, cantor_bang);
    class_addfloat(cantor_class, cantor_float);
    class_addmethod(cantor_class, (t_method)cantor_depth, gensym("depth"),
                    A_FLOAT, 0);
    class_addmethod(cantor_class, (t_method)cantor_width, gensym("width"),
                    A_FLOAT, 0);
    class_addmethod(cantor_class, (t_method)cantor_coef, gensym("coef"),
                    A_FLOAT, 0);
}
