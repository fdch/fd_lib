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
