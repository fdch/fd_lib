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

static t_class *connect_class;

typedef struct connect
{
    t_object x_ob;
    t_symbol *x_tgtsym;
    t_canvas *x_tgtcnv;
} t_connect;

static int connect_settargetcanvas(t_connect *x, t_symbol *s)
{
    if (!s)
        return 0;
    t_canvas *tgt_canvas = (t_canvas *)pd_findbyclass(s, canvas_class);
    if (!tgt_canvas)
        return 0;
    x->x_tgtcnv = glist_getcanvas(tgt_canvas);
    if (!x->x_tgtcnv)
    {
        bug("connect: connect_settargetcanvas failed to get canvas.");
        return 0;
    }
    x->x_tgtsym = s;
    return 1;
}

static void connect_connect(t_connect *x, t_floatarg fwhoout, t_floatarg foutno,
                            t_floatarg fwhoin, t_floatarg finno)
{
    if (!connect_settargetcanvas(x, x->x_tgtsym))
        return;
    if (!glist_isvisible(x->x_tgtcnv))
        canvas_vis(x->x_tgtcnv, 1.0);
    canvas_connect(x->x_tgtcnv, fwhoout, foutno, fwhoin, finno);
}

static void connect_disconnect(t_connect *x, t_floatarg index1,
                               t_floatarg outno, t_floatarg index2,
                               t_floatarg inno)
{
    if (!connect_settargetcanvas(x, x->x_tgtsym))
        return;
    if (!glist_isvisible(x->x_tgtcnv))
        canvas_vis(x->x_tgtcnv, 1.0);
    canvas_disconnect(x->x_tgtcnv, index1, outno, index2, inno);
}

static void *connect_new(t_symbol *s)
{
    t_connect *x = (t_connect *)pd_new(connect_class);
    symbolinlet_new(&x->x_ob, &x->x_tgtsym);
    if (!connect_settargetcanvas(x, s))
        pd_error(x, "%s: could not set canvas.", s->s_name);
    return (x);
}

void connect_setup()
{
    connect_class =
        class_new(gensym("connect"), (t_newmethod)connect_new, 0,
                  sizeof(t_connect), CLASS_DEFAULT, A_DEFSYM, A_NULL);
    class_addmethod(connect_class, (t_method)connect_disconnect,
                    gensym("disconnect"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT,
                    A_NULL);
    class_addmethod(connect_class, (t_method)connect_connect, gensym("connect"),
                    A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
}
