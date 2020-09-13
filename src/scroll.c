/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct scroll {
	t_object x_obj;
	t_symbol *x_sym;
	t_canvas *canvas;
} t_scroll;


static void setcanvassym_scroll(t_scroll *x, t_symbol *s)
{
	t_canvas *canvas;
	if (!(canvas = (t_canvas *)pd_findbyclass(s, canvas_class)))
		pd_error(x, "%s: No such canvas", s->s_name);
	else 
		x->canvas = canvas;
	
}


static void scroll_symbol(t_scroll *x, t_symbol *s)
{
	setcanvassym_scroll(x, s);
}


static void scroll_xaxis(t_scroll *x, t_float f)
{
	if (!(glist_isvisible(x->canvas))) canvas_vis(x->canvas, 1);
	sys_vgui(".x%lx.c xview scroll %d units \n", x->canvas, (int)f);
}


static void scroll_yaxis(t_scroll *x, t_float f)
{
	if (!(glist_isvisible(x->canvas))) canvas_vis(x->canvas, 1);
	sys_vgui(".x%lx.c yview scroll [expr {- (%d)}] units \n", x->canvas, (int)f);
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
	return(x);
}

void scroll_setup()
{
	scroll_class = class_new(gensym("scroll"), (t_newmethod)scroll_new, 0, sizeof(t_scroll), CLASS_DEFAULT, A_DEFSYM, 0);
	class_addsymbol(scroll_class, scroll_symbol);
	class_addmethod(scroll_class, (t_method)scroll_xaxis, gensym("xaxis"), A_FLOAT, 0);
	class_addmethod(scroll_class, (t_method)scroll_yaxis, gensym("yaxis"), A_FLOAT, 0);
}