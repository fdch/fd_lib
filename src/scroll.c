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
#include "s_stuff.h" /* for sys_havegui */

static t_class *scroll_class;

typedef struct scroll
{
    t_object x_ob;
    t_canvas *x_canvas;      /* the canvas in which to scroll */
    t_symbol *x_canvas_name; /* the name of the canvas in which to scroll */
    int x_page;              /* define 'page' (1) or 'units' (0) */
    int x_norm;              /* flag to user normalized coordinates or not*/
} t_scroll;

static void scroll_symbol(t_scroll *x, t_symbol *s)
{
    t_canvas *c = (t_canvas *)pd_findbyclass(s, canvas_class);
    if (!c)
        return pd_error(x, "%s: No such canvas", s->s_name);
    x->x_canvas = c;
    x->x_canvas_name = s;
}

static int scroll_valid_canvas(t_scroll *x)
{
    if (!sys_havegui())
        return 0;

    if (x->x_canvas_name && strcmp(x->x_canvas_name->s_name, ""))
        scroll_symbol(x, x->x_canvas_name);

    return x->x_canvas != NULL;
}

static void scroll_page(t_scroll *x, t_floatarg f) { x->x_page = !!(int)f; }

static void scroll_normalize(t_scroll *x, t_floatarg f)
{
    x->x_norm = !!(int)f;
}

static int scroll_get_screenx(t_scroll *x)
{
    return x->x_canvas->gl_screenx2 - x->x_canvas->gl_screenx1;
}

static int scroll_get_screeny(t_scroll *x)
{
    return x->x_canvas->gl_screeny2 - x->x_canvas->gl_screeny1;
}

static void scroll_screenx(t_scroll *x) { scroll_get_screenx(x); }

static void scroll_screeny(t_scroll *x) { scroll_get_screeny(x); }

static void scroll_moveto(t_scroll *x, int d, t_floatarg f)
{
    if (!scroll_valid_canvas(x))
        return;

    if (!(glist_isvisible(x->x_canvas)))
        canvas_vis(x->x_canvas, 1);

    t_float famount = (t_float)f;

    /* allow unnormalized scrolling by normalizing the input */
    t_float max_dim = !d ? scroll_get_screenx(x) : scroll_get_screeny(x);

    /* normalize to max along dimension*/
    if (!x->x_norm)
        famount /= max_dim;

    /* restrict bounds to 0-1 */
    if (famount > 1.0)
        famount = 1.0;
    if (famount < 0.0)
        famount = 0.0;

    char *moveto_direction = d ? "xview" : "yview";
    pdgui_vmess(0, "cr sf", x->x_canvas, moveto_direction, "moveto", famount);
}

static void scroll_xmoveto(t_scroll *x, t_floatarg f)
{
    scroll_moveto(x, 1, f);
}

static void scroll_ymoveto(t_scroll *x, t_floatarg f)
{
    scroll_moveto(x, 0, 1 - f);
}

static void scroll_scroll(t_scroll *x, int d, int f)
{
    if (!scroll_valid_canvas(x))
        return;

    if (!(glist_isvisible(x->x_canvas)))
        canvas_vis(x->x_canvas, 1.0);

    char *scroll_direction = d ? "xview" : "yview";
    char *scroll_type = x->x_page ? "pages" : "units";
    pdgui_vmess(0, "cr sis", x->x_canvas, scroll_direction, "scroll", (int)f,
                scroll_type);
}

static void scroll_xaxis(t_scroll *x, t_floatarg f) { scroll_scroll(x, 1, f); }

static void scroll_yaxis(t_scroll *x, t_floatarg f)
{
    scroll_scroll(x, 0, f * -1);
}

static void *scroll_new(t_symbol *s, int argc, t_atom *argv)
{
    (void)s;
    t_scroll *x = (t_scroll *)pd_new(scroll_class);
    x->x_canvas = canvas_getcurrent();
    x->x_canvas_name = 0;
    if (argc && argv[0].a_type == A_SYMBOL &&
        strcmp(atom_getsymbolarg(0, argc, argv)->s_name, ""))
    {
        scroll_symbol(x, atom_getsymbolarg(0, argc, argv));
        if (argc > 1)
        {
            startpost("Ignoring extra arguments:");
            postatom(argc - 1, argv + 1);
            endpost();
        }
    }
    else
        symbolinlet_new(&x->x_ob, &x->x_canvas_name);
    x->x_page = x->x_norm = 0;
    return x;
}

void scroll_setup()
{
    scroll_class = class_new(gensym("scroll"), (t_newmethod)scroll_new, 0,
                             sizeof(t_scroll), CLASS_DEFAULT, A_GIMME, A_NULL);
    class_addsymbol(scroll_class, scroll_symbol);
    class_addmethod(scroll_class, (t_method)scroll_xaxis, gensym("xaxis"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_yaxis, gensym("yaxis"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_xmoveto, gensym("xpos"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_ymoveto, gensym("ypos"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_page, gensym("page"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_normalize, gensym("norm"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_normalize,
                    gensym("normalize"), A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_screenx, gensym("screenx"),
                    A_FLOAT, A_NULL);
    class_addmethod(scroll_class, (t_method)scroll_screeny, gensym("screeny"),
                    A_FLOAT, A_NULL);
}
