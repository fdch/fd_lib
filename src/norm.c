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

/* --------------------------- a normalizable value ------------------------- */

/* set the value and the min/max accordingly */
void norm_setval(t_norm *x, t_float fval)
{
    if (fval > x->x_max)
        x->x_max = fval;
    if (fval < x->x_min)
        x->x_min = fval;
    x->x_value = fval;
}

/* reset the value and the max and min */
void norm_reset(t_norm *x, t_float fval)
{
    x->x_max = fval;
    x->x_min = fval;
    x->x_value = fval;
}

/* get the normalized version of the value */
t_float norm_getnorm(t_norm *x)
{
    t_float val, range = x->x_max - x->x_min;
    if (range == 0)
        return 0;
    val = (x->x_value - x->x_min) / range;
    if (val > 1.0)
        return 1.0;
    return val;
}

/* ---------------------------------- [norm] ---------------------------- */

static t_class *norm_obj_class;

typedef struct norm_obj
{
    t_object x_ob;
    t_norm x_x;
} t_norm_obj;

static void norm_obj_float(t_norm_obj *x, t_floatarg fval)
{
    norm_setval(&x->x_x, fval);
    outlet_float(x->x_ob.te_outlet, norm_getnorm(&x->x_x));
}

static void *norm_obj_new()
{
    t_norm_obj *x = (t_norm_obj *)pd_new(norm_obj_class);
    outlet_new(&x->x_ob, gensym("float"));
    return (void *)x;
}

static void norm_obj_reset(t_norm_obj *x) { norm_reset(&x->x_x, 0.0); }

void norm_obj_setup(void)
{
    norm_obj_class = class_new(gensym("norm"), (t_newmethod)norm_obj_new, 0,
                               sizeof(t_norm_obj), CLASS_DEFAULT, A_NULL);
    class_addcreator((t_newmethod)norm_obj_new, gensym("normalize"), A_NULL);
    class_addfloat(norm_obj_class, (t_method)norm_obj_float);
    class_addmethod(norm_obj_class, (t_method)norm_obj_reset, gensym("reset"),
                    A_NULL);
}
