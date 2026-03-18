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

static t_class *delit_class;

typedef struct delit
{
    t_object x_ob;
    t_outlet *x_outlet0;
    t_outlet *x_outlet1;
    t_clock *x_clock;
    double x_delaytime, x_initd;
    int x_n, x_i, x_initn;
} t_delit;

static void delit_set(t_delit *x, t_floatarg f, t_floatarg g)
{
    x->x_n = f >= 1 ? (int)f : 1;
    x->x_delaytime = g > 0.0 ? (double)g : 0.0;
    x->x_i = x->x_n;
}

static void delit_reset(t_delit *x)
{
    x->x_n = x->x_initn;
    x->x_delaytime = x->x_initd;
}

static void delit_tick(t_delit *x)
{
    if (x->x_i)
    {
        clock_delay(x->x_clock, x->x_delaytime);
        outlet_float(x->x_outlet0, x->x_i);
        x->x_i--;
    }
    else
    {
        outlet_bang(x->x_outlet1);
        x->x_i = x->x_n;
    }
}

static void delit_bang(t_delit *x) { delit_tick(x); }

static void delit_stop(t_delit *x) { clock_unset(x->x_clock); }

static void delit_float(t_delit *x, t_floatarg f)
{
    delit_set(x, f, 0.0);
    delit_bang(x);
}
static void delit_list(t_delit *x, t_symbol *s, int argc, t_atom *argv)
{
    if (!argc)
        return;

    (void)s;
    t_float f = 1.0, g = 0.0;
    if (argc && argv->a_type == A_FLOAT)
    {
        f = atom_getfloat(argv);
        argv++, argc--;
    }
    if (argc && argv->a_type == A_FLOAT)
    {
        g = atom_getfloat(argv);
        argv++, argc--;
    }
    if (argc)
    {
        startpost("Ignoring extra args:");
        postatom(argc, argv);
        endpost();
    }

    delit_set(x, f, g);
    delit_bang(x);
}

static void delit_free(t_delit *x) { clock_free(x->x_clock); }

static void *delit_new(t_floatarg f, t_floatarg g)
{
    t_delit *x = (t_delit *)pd_new(delit_class);
    x->x_clock = clock_new(x, (t_method)delit_tick);
    x->x_outlet0 = outlet_new(&x->x_ob, gensym("float"));
    x->x_outlet1 = outlet_new(&x->x_ob, gensym("bang"));
    delit_set(x, f, g);
    x->x_initn = x->x_n;
    x->x_initd = x->x_delaytime;
    return (void *)x;
}

void delit_setup(void)
{
    delit_class =
        class_new(gensym("delit"), (t_newmethod)delit_new, (t_method)delit_free,
                  sizeof(t_delit), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
    class_addbang(delit_class, delit_bang);
    class_addfloat(delit_class, delit_float);
    class_addlist(delit_class, delit_list);

    class_addmethod(delit_class, (t_method)delit_set, gensym("set"), A_DEFFLOAT,
                    A_DEFFLOAT, A_NULL);
    class_addmethod(delit_class, (t_method)delit_reset, gensym("reset"),
                    A_NULL);
    class_addmethod(delit_class, (t_method)delit_stop, gensym("stop"), A_NULL);
}
