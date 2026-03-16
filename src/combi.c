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

static t_class *combi_class;

typedef struct _combi
{
    t_object x_ob;
    float *x_arr;
    int x_max, x_r;
} t_combi;

static void combinationUtil(t_combi *x, t_float data[], int start, int end,
                            int index)
{
    if (index == x->x_r)
    {
        for (int i = 0; i < x->x_r; i++)
            outlet_float(x->x_ob.te_outlet, data[i]);
        return;
    }

    for (int i = start; i <= end && end - i + 1 >= x->x_r - index; i++)
    {
        data[index] = x->x_arr[i];
        combinationUtil(x, data, i + 1, end, index + 1);
        while (x->x_arr[i] == x->x_arr[i + 1])
            i++;
    }
}

static void combi_set(t_combi *x, t_symbol *s, t_float f) { x->x_r = f; }

static void combi_alloc(t_combi *x, int n)
{
    if (!n)
        return pd_error(x, "Invalid size.");

    if (x->x_max == n)
        return;

    if (x->x_arr)
        t_freebytes(x->x_arr, n * sizeof(t_float));

    x->x_arr = (t_float *)t_getbytes(n * sizeof(t_float));
    x->x_max = n;
}

static void combi_list(t_combi *x, t_symbol *s, int argc, t_atom *argv)
{
    if (!argc)
        return;

    (void)s;
    t_float data[x->x_r];
    combi_alloc(x, argc);

    for (int i = 0; i < argc; i++)
        x->x_arr[i] = atom_getfloat(argv + i);

    /* Sort array to handle duplicates */
    qsort(x->x_arr, argc, sizeof(float), compare);

    /* Print all combination using temprary array 'data[]' */
    combinationUtil(x, data, 0, argc - 1, 0);
}

static void *combi_new(t_floatarg f)
{
    t_combi *x = (t_combi *)pd_new(combi_class);
    x->x_r = f;
    outlet_new(&x->x_ob, &s_float);
    return x;
}

static void combi_free(t_combi *x)
{
    if (x->x_arr)
        t_freebytes(x->x_arr, x->x_max * sizeof(t_float));
}

void combi_setup(void)
{
    combi_class =
        class_new(gensym("combi"), (t_newmethod)combi_new, (t_method)combi_free,
                  sizeof(t_combi), CLASS_DEFAULT, A_DEFFLOAT, 0);

    class_addlist(combi_class, combi_list);
    class_addmethod(combi_class, (t_method)combi_set, gensym("set"), A_GIMME,
                    0);
}
