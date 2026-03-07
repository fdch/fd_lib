/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "fdLib.h"
#include "m_pd.h"

t_class *fd_lib_class;

typedef struct fd_lib
{
  t_object t_ob;
} t_fd_lib;

static void *fd_lib_new(void)
{
  t_fd_lib *x = (t_fd_lib *)pd_new(fd_lib_class);
  return x;
}

void fd_lib_setup(void)
{
	fd_lib_class = class_new(gensym("fd_lib"), (t_newmethod)fd_lib_new, 0,
                          sizeof(t_fd_lib), CLASS_NOINLET, A_NULL);
  class_sethelpsymbol(fd_lib_class, gensym("fd_lib"));

	cantor_setup();
	clifford_setup();
	colormap_setup();
	combi_setup();
	connect_setup();
	counter_setup();
	crand_setup();
	cuadratic_setup();
	delit_setup();
	factor_setup();
	fdm_setup();
	frand_setup();
	g_sradio_setup();
	glistinfo_setup();
	halton_setup();
	henon_setup();
	list_exec_setup();
	iterate_setup();
	lor_tilde_setup();
	lorenz_setup();
	lornorm_setup();
	lorsig_tilde_setup();
	mainpath_setup();
	mandelbrot_setup();
	minimax_setup();
	mtwister_setup();
	parabola_setup();
	pmtwister_setup();
	prandom_setup();
	randy_setup();
	reflect_setup();
	root_setup();
	scroll_setup();
	siginfo_setup();
	tracks_setup();

	post("fd_lib version %s", FDLIBVERSION);
}
