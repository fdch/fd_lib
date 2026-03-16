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
#include "m_pd.h"

static t_class *counter_class;

typedef struct _counter
{
    t_object x_ob;
    int x_m, x_r;
    t_float x_i, x_count;
} t_counter;

static void counter_bang(t_counter *x)
{
    if (x->x_m != 0)
        x->x_count = mod(x->x_count + 1, x->x_m);
    else if (x->x_i != 0.0)
        x->x_count += x->x_i;
    else if (x->x_r != 0)
        x->x_count = reflectit(x->x_count + 1, x->x_r);
    else
        x->x_count++;

    outlet_float(x->x_ob.te_outlet, x->x_count);
}

static void counter_reset(t_counter *x) { x->x_count = 0; }

static void counter_float(t_counter *x, t_floatarg f) { x->x_count = f; }

static void counter_m(t_counter *x, t_floatarg f) { x->x_m = (int)f; }

static void counter_r(t_counter *x, t_floatarg f) { x->x_r = (int)fabsf(f); }

static void counter_i(t_counter *x, t_floatarg f) { x->x_i = f; }

static void *counter_new(t_symbol *s, int argc, t_atom *argv)
{
    (void)s;
    t_counter *x = (t_counter *)pd_new(counter_class);
    while (argc && argv->a_type == A_SYMBOL &&
           *argv->a_w.w_symbol->s_name == '-')
    {
        if (!strcmp(argv->a_w.w_symbol->s_name, "-m") && argc >= 2 &&
            argv[1].a_type == A_FLOAT)
            counter_m(x, atom_getfloatarg(1, argc, argv)), argc -= 1, argv += 1;
        else if (!strcmp(argv->a_w.w_symbol->s_name, "-r") && argc >= 2 &&
                 argv[1].a_type == A_FLOAT)
            counter_r(x, atom_getfloatarg(1, argc, argv)), argc -= 1, argv += 1;
        else if (!strcmp(argv->a_w.w_symbol->s_name, "-i") && argc >= 2 &&
                 argv[1].a_type == A_FLOAT)
            counter_i(x, atom_getfloatarg(1, argc, argv)), argc -= 1, argv += 1;
        else
            pd_error(x, "counter: unknown flag ..."), postatom(argc, argv),
                endpost();
        argc--;
        argv++;
    }
    if (argc && argv->a_type == A_FLOAT)
        counter_float(x, atom_getfloatarg(0, argc, argv));
    floatinlet_new(&x->x_ob, &x->x_i);
    outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

void counter_setup(void)
{
    counter_class = class_new(gensym("counter"), (t_newmethod)counter_new, 0,
                              sizeof(t_counter), CLASS_DEFAULT, A_GIMME, 0);

    class_addcreator((t_newmethod)counter_new, gensym("c"), A_GIMME, A_NULL);
    class_addbang(counter_class, counter_bang);
    class_addmethod(counter_class, (t_method)counter_reset, gensym("reset"),
                    A_NULL);
    class_addfloat(counter_class, counter_float);
    class_addmethod(counter_class, (t_method)counter_m, gensym("m"), A_FLOAT,
                    A_NULL);
    class_addmethod(counter_class, (t_method)counter_r, gensym("r"), A_FLOAT,
                    A_NULL);
    class_addmethod(counter_class, (t_method)counter_i, gensym("i"), A_FLOAT,
                    A_NULL);
}
