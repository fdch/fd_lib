/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct reflect {
	t_object x_obj;
	t_float r, f, o;
} t_reflect;

static void reflect_float(t_reflect *x, t_float f)
{
	if (0 < x->r)
		outlet_float(x->x_obj.ob_outlet, reflectit(f,x->r)+x->o);
	else
		pd_error(x,"range must be greater than 0");
}

static void reflect_set(t_reflect *x, t_float f, t_float g)
{
	x->r = fabsf(f);
	x->o = g;
}

static t_class *reflect_class;


static void *reflect_new(t_symbol *s, t_int argc, t_atom *argv) {
	t_reflect *x = (t_reflect *)pd_new(reflect_class);
	outlet_new(&x->x_obj, &s_float);
	if (!argc)
		floatinlet_new(&x->x_obj, &x->r);
	else if (argc == 1)
		x->r = atom_getfloatarg(0,argc,argv);
	else if (argc == 2)
		reflect_set(x, atom_getfloatarg(0,argc,argv), atom_getfloatarg(1,argc,argv));
	else
		pd_error(x,"reflect: only accepts 2 arguments"), postatom(argc,argv), endpost();
	return (void *)x;
}
void reflect_setup(void)	{
	reflect_class = class_new(gensym("reflect"), (t_newmethod)reflect_new, 0, sizeof(t_reflect), CLASS_DEFAULT, A_GIMME, 0);
	class_addfloat(reflect_class, reflect_float);
	class_addmethod(reflect_class, (t_method)reflect_set, gensym("set"), A_FLOAT,A_FLOAT,0);
}