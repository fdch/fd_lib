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

/* --------------------------------- particle  -------------------------- */

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

void particle_update(t_particle *x, t_float fvalue)
{
    if (!x->x_size)
        return pd_error(x, "Set order first.");

    for (int i = x->x_size; i > 0; i--)
        x->x_stored[i] = x->x_stored[i - 1];
    x->x_stored[0] = fvalue;
    x->x_diff = particle_nth_diff(x);
    x->x_up = x->x_diff >= 0 ? 1 : 0;
}

void particle_free(t_particle *x)
{
    if (x->x_stored)
        freebytes(x->x_stored, (x->x_size + 1) * sizeof(t_float));
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

void particle_reset(t_particle *x) { particle_allocate(x, x->x_size); }

void particle_init(t_particle *x, int size)
{
    if (size < 1)
        size = 1;
    if (size > 128)
        size = 128;
    particle_allocate(x, size);
}

/* ---------------------------------- [particle] ---------------------------- */

static t_class *particle_obj_class;

typedef struct particle_obj
{
    t_object x_ob;
    t_particle x_x;
} t_particle_obj;

static void particle_float(t_particle_obj *x, t_floatarg f)
{
    particle_update(&x->x_x, f);
    outlet_float(x->x_ob.te_outlet, x->x_x.x_diff);
}

static void particle_obj_order(t_particle_obj *x, t_floatarg forder)
{
    particle_init(&x->x_x, forder);
}

static void *particle_obj_new(t_floatarg forder)
{
    t_particle_obj *x = (t_particle_obj *)pd_new(particle_obj_class);
    outlet_new(&x->x_ob, gensym("float"));
    particle_obj_order(x, forder);
    return (void *)x;
}

static void particle_obj_reset(t_particle_obj *x) { particle_reset(&x->x_x); }

void particle_obj_setup(void)
{
    particle_obj_class =
        class_new(gensym("particle"), (t_newmethod)particle_obj_new, 0,
                  sizeof(t_particle_obj), CLASS_DEFAULT, A_DEFFLOAT, A_NULL);
    class_addfloat(particle_obj_class, (t_method)particle_float);
    class_addmethod(particle_obj_class, (t_method)particle_obj_reset,
                    gensym("reset"), A_NULL);
    class_addmethod(particle_obj_class, (t_method)particle_obj_order,
                    gensym("order"), A_FLOAT, A_NULL);
}
