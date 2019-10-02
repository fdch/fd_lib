/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

typedef struct clifford
{
	t_object x_ob;
	t_outlet *x_outlet0, *x_outlet1, *x_outlet2;
	double x, y, z, inita, initb, initc, initd;
	int gmax, count;
} t_clifford;

void clifford_bang(t_clifford *x)
{
	double x0, y0, x1, y1;
	double a = x->inita;
	double b = x->initb;
	double c = x->initc;
	double d = x->initd;
	t_atom raw[2];
	x0 = x->x;
	y0 = x->y;
	//xn+1 = sin(a yn) + c cos(a xn)
	//yn+1 = sin(b xn) + d cos(b yn)
	
	x1 = sin(a * y0) + c*cos(a*x0);
	y1 = sin(b * x0) + d*cos(b*y0);
	
	x->x = x1;
	x->y = y1;
	x->count++;
	
	SETFLOAT(&raw[0], (float) x->x);
	SETFLOAT(&raw[1], (float) x->y);
	
	outlet_list(x->x_outlet0, 0, 2, raw);
	
	//outlet 2 para la cuenta de iteraciones
	outlet_float(x->x_outlet2, x->count);
}
void clifford_print(t_clifford *x, t_float g)
{
	double x0, y0, x1, y1;
	double a = x->inita;
	double b = x->initb;
	double c = x->initc;
	double d = x->initd;
	int f;

	t_atom cooked[3];

	x->gmax = g;
	
	for (f = 0; f < x->gmax; f++)
	{
		x0 = x->x;
		y0 = x->y;
		//xn+1 = sin(a yn) + c cos(a xn)
		//yn+1 = sin(b xn) + d cos(b yn)
		
		x1 = sin(a * y0) + c*cos(a*x0);
		y1 = sin(b * x0) + d*cos(b*y0);
		
		x->x = x1;
		x->y = y1;
		x->count++;
		
		SETFLOAT(&cooked[0], (float) x->x);
		SETFLOAT(&cooked[1], (float) x->y);
		SETFLOAT(&cooked[2], (float) f);
		
		outlet_list(x->x_outlet1, 0, 3, cooked);
		outlet_float(x->x_outlet2, x->count);
	}
}
void clifford_reset(t_clifford *x)
{
	x->x = 0.1;
	x->y = 0;
	x->inita = -1.4;
	x->initb = 1.6;
	x->initc = 1.0;
	x->initd = 0.7;	
	x->gmax = 1;
	x->count = 0;
	post("default: \n a = %f\n b = %f\n c = %f\n d = %f", x->inita, x->initb, x->initc, x->initd);
}

void clifford_a(t_clifford *x, t_float f){x->inita = f;}
void clifford_b(t_clifford *x, t_float f){x->initb = f;}
void clifford_c(t_clifford *x, t_float f){x->initc = f;}
void clifford_d(t_clifford *x, t_float f){x->initd = f;}
void clifford_x(t_clifford *x, t_float f){x->x = f;}
void clifford_y(t_clifford *x, t_float f){x->y = f;}
void clifford_maxval(t_clifford *x, t_float f){x->gmax = f;}

t_class *clifford_class;

void *clifford_new()
{
	t_clifford *x = (t_clifford *)pd_new(clifford_class);
	x->x = 0.1;
	x->y = 0;
	x->inita = -1.4;
	x->initb = 1.6;
	x->initc = 1.0;
	x->initd = 0.7;
	x->gmax = 1;
	x->count = 0;
	x->gmax = 1;
	x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
	x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
	x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
	return (void *)x;
}
void clifford_setup(void)
{
	clifford_class = class_new(gensym("clifford"), (t_newmethod)clifford_new, 0, sizeof(t_clifford), CLASS_DEFAULT, 0);
	class_addbang(clifford_class, clifford_bang);
	class_addmethod(clifford_class, (t_method)clifford_a, gensym("a"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_b, gensym("b"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_c, gensym("c"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_d, gensym("d"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_reset, gensym("reset"), A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_x, gensym("x"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_y, gensym("y"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_maxval, gensym("maxval"), A_FLOAT, A_NULL);
	class_addmethod(clifford_class, (t_method)clifford_print, gensym("print"), A_FLOAT, A_NULL);
}