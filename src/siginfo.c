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
    t_outlet *x_outlet0;
    t_outlet *x_outlet1;
    t_outlet *x_outlet2;
    t_particle x_x, x_y, x_z;
    int x_ref, x_order;
} t_siginfo;

static void siginfo_list(t_siginfo *x, t_symbol *s, int argc, t_atom *argv)
{
    (void)s;
    t_atom previous[3], cook[7], raw[4];
    x->x_ref = atom_getfloatarg(0, argc, argv);
    t_float x_val = atom_getfloatarg(1, argc, argv);
    t_float y_val = atom_getfloatarg(2, argc, argv);
    t_float z_val = atom_getfloatarg(3, argc, argv);
    particle_update(&x->x_x, x_val);
    particle_update(&x->x_y, y_val);
    particle_update(&x->x_z, z_val);
    t_float dist =
        sqrt((x->x_x.x_diff * x->x_x.x_diff) + (x->x_y.x_diff * x->x_y.x_diff) +
             (x->x_z.x_diff * x->x_z.x_diff));
    SETFLOAT(&raw[0], (int)x->x_ref);
    SETFLOAT(&raw[1], x_val);
    SETFLOAT(&raw[2], y_val);
    SETFLOAT(&raw[3], z_val);
    SETFLOAT(&cook[0], x->x_x.x_up);
    SETFLOAT(&cook[1], x->x_y.x_up);
    SETFLOAT(&cook[2], x->x_z.x_up);
    SETFLOAT(&cook[3], x->x_x.x_diff);
    SETFLOAT(&cook[4], x->x_y.x_diff);
    SETFLOAT(&cook[5], x->x_z.x_diff);
    SETFLOAT(&cook[6], dist);
    SETFLOAT(&previous[0], x->x_x.x_stored[0]);
    SETFLOAT(&previous[1], x->x_y.x_stored[0]);
    SETFLOAT(&previous[2], x->x_z.x_stored[0]);
    outlet_list(x->x_outlet0, gensym("list"), 4, raw);
    outlet_list(x->x_outlet1, gensym("list"), 7, cook);
    outlet_list(x->x_outlet2, gensym("list"), 3, previous);
}

static void siginfo_reset(t_siginfo *x)
{
    particle_reset(&x->x_x);
    particle_reset(&x->x_y);
    particle_reset(&x->x_z);
}

static void siginfo_order(t_siginfo *x, t_floatarg forder)
{
    x->x_order = forder <= 0.0 ? 1 : (int)forder;
    particle_init(&x->x_x, x->x_order);
    particle_init(&x->x_y, x->x_order);
    particle_init(&x->x_z, x->x_order);
}

static void siginfo_free(t_siginfo *x)
{
    particle_free(&x->x_x);
    particle_free(&x->x_y);
    particle_free(&x->x_z);
}

static void *siginfo_new(t_floatarg forder)
{
    t_siginfo *x = (t_siginfo *)pd_new(siginfo_class);
    x->x_outlet0 = outlet_new(&x->x_ob, gensym("list"));
    x->x_outlet1 = outlet_new(&x->x_ob, gensym("list"));
    x->x_outlet2 = outlet_new(&x->x_ob, gensym("list"));
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
