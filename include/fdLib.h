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
#ifndef FDLIB_H
#define FDLIB_H

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h> // for memset() and strlen()
#include <limits.h> // for INT_MAX, etc
#include <float.h>
#include "m_pd.h"
#include "g_canvas.h"

/* this line is automatically updated by config.sh */
#define FDLIBVERSION "0.4.0"
/* end lines automatically updated by config.sh */

int compare(const void *a, const void *b);
t_float factorial(t_float f);
t_float reflectit(t_float f, t_float r);
t_float mod(t_float a, t_float n);

/* normalizable value defined in norm.c */

/* keep a normalizable value to use as state variable */
typedef struct norm
{
    t_float x_value;
    t_float x_min;
    t_float x_max;
} t_norm;

void norm_setval(t_norm *x, t_float fval);
void norm_reset(t_norm *x, t_float fval);
t_float norm_getnorm(t_norm *x);

/* particle to calculate nth order derivatives defined in particle.c */

typedef struct particle
{
    t_float *x_stored;
    t_float x_diff;
    int x_up, x_size;
} t_particle;

void particle_update(t_particle *x, t_float fvalue);
void particle_free(t_particle *x);
void particle_reset(t_particle *x);
void particle_init(t_particle *x, int size);

/* setup functions */

void cantor_setup(void);
void clifford_setup(void);
void colormap_setup(void);
void combi_setup(void);
void connect_setup(void);
void counter_setup(void);
void crand_setup(void);
void cuadratic_setup(void);
void delit_setup(void);
void factor_setup(void);
void fdm_setup(void);
void frand_setup(void);
void g_sradio_setup(void);
void glistinfo_setup(void);
void halton_setup(void);
void henon_setup(void);
void list_exec_setup(void);
void lorenz_global_setup(void);
void iterate_setup(void);
void mandelbrot_setup(void);
void minimax_setup(void);
void mtwister_setup(void);
void norm_obj_setup(void);
void parabola_setup(void);
void prandom_setup(void);
void randy_setup(void);
void reflect_setup(void);
void root_setup(void);
void scroll_setup(void);
void siginfo_setup(void);
void particle_obj_setup(void);
void tracks_setup(void);

#endif
