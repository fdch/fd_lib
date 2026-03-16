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

static t_class *siginfo_class;

typedef struct particle
{
    t_float *x_stored;
    t_float x_diff;
    int x_up, x_size;
} t_particle;

static int particle_binom(t_particle *x, int k)
{
    int r = 1;
    for (int i = 1; i <= k; i++)
        r = r * (x->x_size - k + i) / i;
    return r;
}

static t_float particle_nth_diff(t_particle *x)
{
    t_float sum = 0;

    for (int k = 0; k <= x->x_size; k++)
    {
        int sign = (k % 2) ? -1 : 1;
        sum += sign * particle_binom(x, k) * x->x_stored[k];
    }

    return sum;
}

static void particle_free(t_particle *x)
{
    if (x->x_stored)
        freebytes(x->x_stored, (x->x_size + 1) * sizeof(t_float));
}

static void particle_update(t_particle *x, t_float fvalue)
{
    if (!x->x_size)
        return pd_error(x, "Set order first.");

    for (int i = x->x_size; i > 0; i--)
        x->x_stored[i] = x->x_stored[i - 1];
    x->x_stored[0] = fvalue;
    x->x_diff = particle_nth_diff(x);
    x->x_up = x->x_diff >= 0 ? 1 : 0;
}

static void particle_allocate(t_particle *x, t_float forder)
{
    if (x->x_stored && (x->x_size == (int)forder))
        return;
    particle_free(x);
    x->x_size = (int)forder;
    x->x_stored = (t_float *)getbytes((x->x_size + 1) * sizeof(t_float));
    memset(x->x_stored, 0.0, x->x_size + 1);
}

static void particle_reset(t_particle *x) { particle_allocate(x, x->x_size); }

static int particle_init(t_particle *x, int size)
{
    if (size <= 0)
        return 0;
    particle_allocate(x, size);
    return x->x_stored != NULL;
}

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
