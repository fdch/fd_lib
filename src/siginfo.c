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

/* --------------------------------- siginfo  -------------------------- */

static t_class *siginfo_class;

typedef struct siginfo
{
    t_object x_ob;
    t_outlet *x_outlet1;
    t_outlet *x_outlet2;
    t_outlet *x_outlet3;
    t_particle *x_particles;
    int x_num_parts, x_order;
} t_siginfo;

static void siginfo_free(t_siginfo *x)
{
    if (x->x_particles)
        freebytes(x->x_particles, x->x_num_parts * sizeof(t_particle));
}

static void siginfo_init_particles(t_siginfo *x)
{
    for (int i = 0; i < x->x_num_parts; i++)
        particle_init(x->x_particles + i, x->x_order);
}

static void siginfo_order(t_siginfo *x, t_floatarg forder)
{
    x->x_order = forder <= 0.0 ? 1 : (int)forder;
}

static void siginfo_allocate_particles(t_siginfo *x, t_floatarg fnumparts)
{
    if (fnumparts == x->x_num_parts)
        return;

    siginfo_free(x);
    x->x_particles = (t_particle *)getbytes(fnumparts * sizeof(t_particle));
    x->x_num_parts = fnumparts;
    siginfo_init_particles(x);
}

static void siginfo_list(t_siginfo *x, t_symbol *s, int argc, t_atom *argv)
{
    (void)s;

    if (argc < 1)
        return logpost(x, PD_DEBUG, "At least 1 argument must be passed");

    siginfo_allocate_particles(x, argc);

    // t_atom previous[3], cook[7], raw[4];
    int cook_size = x->x_num_parts * 2 + 1;

    t_atom *previous = (t_atom *)getbytes(x->x_num_parts * sizeof(t_atom));
    t_atom *cook = (t_atom *)getbytes(cook_size * sizeof(t_atom));
    t_atom *raw = (t_atom *)getbytes(x->x_num_parts * sizeof(t_atom));

    t_float square_sum = 0.0;
    for (int i = 0; i < argc; i++)
    {
        t_float val = atom_getfloatarg(i, argc, argv);
        SETFLOAT(raw + i, val);
        particle_update(x->x_particles + i, val);
        SETFLOAT(previous + i, x->x_particles[i].x_stored[0]);
        SETFLOAT(cook + i, x->x_particles[i].x_up);
        SETFLOAT(cook + i + x->x_num_parts, x->x_particles[i].x_diff);
        square_sum += (x->x_particles[i].x_diff * x->x_particles[i].x_diff);
    }
    SETFLOAT(cook + cook_size - 1, sqrt(square_sum));

    outlet_list(x->x_outlet1, gensym("list"), x->x_num_parts, raw);
    outlet_list(x->x_outlet2, gensym("list"), cook_size, cook);
    outlet_list(x->x_outlet3, gensym("list"), x->x_num_parts, previous);

    freebytes(previous, x->x_num_parts * sizeof(t_atom));
    freebytes(cook, cook_size * sizeof(t_atom));
    freebytes(raw, x->x_num_parts * sizeof(t_atom));
}

static void siginfo_reset(t_siginfo *x)
{
    for (int i = 0; i < x->x_num_parts; i++)
        particle_reset(x->x_particles + i);
}

static void *siginfo_new(t_floatarg forder)
{
    t_siginfo *x = (t_siginfo *)pd_new(siginfo_class);
    x->x_outlet1 = outlet_new(&x->x_ob, gensym("list"));
    x->x_outlet2 = outlet_new(&x->x_ob, gensym("list"));
    x->x_outlet3 = outlet_new(&x->x_ob, gensym("list"));
    x->x_num_parts = 0;
    siginfo_order(x, forder);
    return (void *)x;
}

void siginfo_setup(void)
{
    siginfo_class = class_new(gensym("siginfo"), (t_newmethod)siginfo_new,
                              (t_method)siginfo_free, sizeof(t_siginfo),
                              CLASS_DEFAULT, A_DEFFLOAT, A_NULL);
    class_addlist(siginfo_class, siginfo_list);
    class_addmethod(siginfo_class, (t_method)siginfo_reset, gensym("reset"),
                    A_NULL);
    class_addmethod(siginfo_class, (t_method)siginfo_order, gensym("order"),
                    A_FLOAT, A_NULL);
}
