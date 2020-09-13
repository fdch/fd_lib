/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"


typedef struct fd_lib
{
  t_object t_ob;
} t_fd_lib;

t_class *fd_lib_class;

static void fd_lib_declare_path()
{
	// attempting to load the path to our lib
	t_atom ap[3];
	char libname[]="/fd_lib";
	// the destinations path string with + for dialog
	char ps[MAXPDSTRING];
	ps[0]='+';
	ps[1]='\0';
	// the environment-dependent path string
	char pb[MAXPDSTRING];
	int created=0;
	char pdlibdir[MAXPDSTRING];
	
#ifdef MACOSX
	if (created==0) {
		sprintf(pdlibdir,"~/Documents/Pd/externals");
		created=1;
	}
#endif
#ifdef UNIX
	if (created==0) {
		sprintf(pdlibdir,"/usr/local/lib/pd-externals");
		created=1;
	}
#endif
#ifdef MSW
	if (created==0) {
		sprintf(pdlibdir,"%AppData%/Pd");
		created=1;
	}
#endif
	// expand the path
	sys_expandpath(pdlibdir, pb, MAXPDSTRING);
	// append the lib name
	strcat(pb,libname);
	pb[strlen(pb)]='\0';
	// place it in the destination string
	strcat(ps,pb);
	// make the list to send to add-to-path selector
	SETSYMBOL (ap+0, gensym(ps)); // add it as a symbol
	SETFLOAT (ap+1, 1.0f);
	SETFLOAT (ap+2,0);
	post("Adding path to fd_lib in: %s",pb);
	pd_typedmess(gensym("pd")->s_thing, gensym("add-to-path"), 3, ap);
}


static void *fd_lib_new(void)
{
  t_fd_lib *x = (t_fd_lib *)pd_new(fd_lib_class);
  return (x);
}


void fd_lib_setup(void) 
{
	fd_lib_class = class_new(gensym("fd_lib"), fd_lib_new, 0, sizeof(t_fd_lib), CLASS_PD, 0);

	post("fd_lib version %s", FDLIBVERSION);
	fd_lib_declare_path();


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
	irlog_setup();
	irpow_setup();
	irsqrt_setup();
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
	// g_surface_setup();

}