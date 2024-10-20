/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"
#include "s_stuff.h"

typedef struct scroll {
	t_object x_obj;
	t_symbol *x_sym; // the canvas name
	t_canvas *canvas; // the canvas on which to scroll
    t_float x_page; // define 'page' (1) or 'units' (0) (default=0)
    t_float x_norm; // flag to user normalized coordinates (1) or not (0) (default=0)
} t_scroll;

static void setcanvassym_scroll(t_scroll *x, t_symbol *s)
{
	t_canvas *canvas;
	if (!(canvas = (t_canvas *)pd_findbyclass(s, canvas_class)))
		pd_error(x, "%s: No such canvas", s->s_name);
	else 
		x->canvas = canvas;
}

static void scroll_page(t_scroll *x, t_float f)
{
    x->x_page = (int)f > 0 ? 1 : 0;
}

static void scroll_norm(t_scroll *x, t_float f)
{
    x->x_norm = (int)f > 0 ? 1 : 0;
}

static void scroll_symbol(t_scroll *x, t_symbol *s)
{
	setcanvassym_scroll(x, s);
}

static int scroll_get_screenx(t_scroll *x)
{
    return x->canvas->gl_screenx2 - x->canvas->gl_screenx1;
}

static int scroll_get_screeny(t_scroll *x)
{
    return x->canvas->gl_screeny2 - x->canvas->gl_screeny1;
}

static void scroll_moveto(t_scroll *x, int d, float f)
{
    if (!sys_havegui())return;
	if (!(glist_isvisible(x->canvas))) canvas_vis(x->canvas, 1);
    // allow unnormalized scrolling by normalizing the input
    if (!x->x_norm) {
        // adjust to x or y axis
        f /= d == 0 ? scroll_get_screenx(x) : scroll_get_screeny(x);
    }
    // restrict bounds to 0-1
    float t = f > 1 ? 1 : f < 0 ? 0 : f;
	sys_vgui(".x%lx.c %sview moveto %f \n", x->canvas, d == 0 ? "x" : "y", t);
}

static void scroll_xmoveto(t_scroll *x, t_float f)
{
    scroll_moveto(x, 0, (float) f);
}

static void scroll_ymoveto(t_scroll *x, t_float f)
{
    scroll_moveto(x, 1, 1 - (float) f );
}

static void scroll_scroll(t_scroll *x, int d, int f)
{
    if (!sys_havegui())return;
	if (!(glist_isvisible(x->canvas))) canvas_vis(x->canvas, 1);
	sys_vgui(".x%lx.c %sview scroll %d %s \n", x->canvas, d == 0 ? "x" : "y", (int)f, x->x_page == 1 ? "pages" : "units");
}

static void scroll_xaxis(t_scroll *x, t_float f)
{
    scroll_scroll(x, 0, (int)f);
}


static void scroll_yaxis(t_scroll *x, t_float f)
{
    scroll_scroll(x, 1, (int)f * -1);
}

static t_class *scroll_class;


static void *scroll_new(t_symbol *s)
{
	t_scroll *x = (t_scroll *)pd_new(scroll_class);
	t_canvas *canvas = (t_canvas *)pd_findbyclass(s, canvas_class);
	if(!canvas) {
		x->canvas = canvas_getcurrent();
	} else
		setcanvassym_scroll(x, s);
    x->x_page = 0; // default to units
    x->x_norm = 0; // default to unnormalized scrolling
	return(x);
}

void scroll_setup()
{
	scroll_class = class_new(gensym("scroll"), (t_newmethod)scroll_new, 0, sizeof(t_scroll), CLASS_DEFAULT, A_DEFSYM, 0);
	class_addsymbol(scroll_class, scroll_symbol);
	class_addmethod(scroll_class, (t_method)scroll_xaxis, gensym("xaxis"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_yaxis, gensym("yaxis"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_xmoveto, gensym("xpos"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_ymoveto, gensym("ypos"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_page, gensym("page"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_norm, gensym("norm"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_get_screenx, gensym("screenx"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_get_screeny, gensym("screeny"), A_FLOAT, 0);
}