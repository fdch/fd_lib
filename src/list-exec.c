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

static t_class *list_exec_class;

typedef struct _list_exec t_list_exec;

typedef void (*t_list_exec_fun)(t_list_exec *, t_float, t_float *);

struct _list_exec
{
    t_object x_obj;
    t_atom *x_out;
    t_list_exec_fun x_fun_ptr;
    t_float x_constant;
    int x_size;
};

static void list_exec_log(t_list_exec *x, t_float f, t_float *o)
{
    *o = (f == 0.0 ? 0.0 : log(f)) + x->x_constant;
}

static void list_exec_pow(t_list_exec *x, t_float f, t_float *o)
{
    *o = f * f + x->x_constant;
}

static void list_exec_sqrt(t_list_exec *x, t_float f, t_float *o)
{
    *o = (t_float)sqrt((double)f) + x->x_constant;
}

static void list_exec_sum(t_list_exec *x, t_float f, t_float *o)
{
    *o = f + x->x_constant;
}

static void list_exec_free(t_list_exec *x)
{
    if (!x->x_out)
        return;

    freebytes(x->x_out, x->x_size * sizeof(t_atom));
    x->x_size = 0;
}

static void list_exec_bang(t_list_exec *x)
{
    if (!x->x_size)
        return;
    outlet_list(x->x_obj.te_outlet, gensym("list"), x->x_size, x->x_out);
}

static void list_exec_list(t_list_exec *x, t_symbol *s, int argc, t_atom *argv)
{
    (void)s;
    if (!argc)
        return list_exec_bang(x);

    if (argc != x->x_size)
    {
        list_exec_free(x);
        x->x_out = (t_atom *)t_getbytes(argc * sizeof(t_atom));
        x->x_size = argc;
    }
    for (int i = 0; i < x->x_size; i++)
    {
        const t_float f = atom_getfloatarg(i, argc, argv);
        t_float fout;
        x->x_fun_ptr(x, f, &fout);
        SETFLOAT(&x->x_out[i], fout);
    }
    list_exec_bang(x);
}

static void list_exec_symbol(t_list_exec *x, t_symbol *s)
{
    t_symbol *pow_sym = gensym("pow");
    t_symbol *log_sym = gensym("log");
    t_symbol *sqrt_sym = gensym("sqrt");
    if (s == pow_sym)
        x->x_fun_ptr = list_exec_pow;
    else if (s == log_sym)
        x->x_fun_ptr = list_exec_log;
    else if (s == sqrt_sym)
        x->x_fun_ptr = list_exec_sqrt;
    else
        x->x_fun_ptr = list_exec_sum;
}

static void *list_exec_new(t_symbol *s, t_floatarg f)
{
    t_list_exec *x = (t_list_exec *)pd_new(list_exec_class);
    outlet_new(&x->x_obj, gensym("list"));
    x->x_size = 0;
    x->x_constant = f;
    list_exec_symbol(x, s);
    return (void *)x;
}

void list_exec_setup(void)
{
    list_exec_class = class_new(gensym("list-exec"), (t_newmethod)list_exec_new,
                                (t_method)list_exec_free, sizeof(t_list_exec),
                                CLASS_DEFAULT, A_DEFSYMBOL, A_DEFFLOAT, A_NULL);
    class_addlist(list_exec_class, list_exec_list);
    class_addsymbol(list_exec_class, list_exec_symbol);
}
