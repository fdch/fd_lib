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

/* Needed for qsort.
 * See
 * http://w...content-available-to-author-only...s.com/reference/cstdlib/qsort/
 */
int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

t_float factorial(t_float f)
{
    if (f > 1)
        return f * factorial(f - 1);
    return 1;
}

t_float reflectit(t_float f, t_float r)
{
    t_float t = f;
    do
        t = (fabs((fabs(t + r) - r) * -1 + r) - r) * -1;
    while (t < (r * -1));
    if (t == -0)
        t = 0;
    return t;
}

t_float mod(t_float a, t_float n) { return a - n * floor(a / n); }
