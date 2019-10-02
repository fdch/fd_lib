/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
//#include <stdbool.h>
#include <string.h>	// for memset() and strlen()
#include <limits.h> // for INT_MAX, etc
#include <float.h>
#include "m_pd.h"
#include "g_canvas.h"


#define FDLIBVERSION "0.1"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define LORMAX 100
#define INITA 10.0
#define INITB 28.0
#define INITC 8.0 / 3.0
#define INITH 0.01
#define INITX 0.1
#define INITY 0.0
#define INITZ 0.0
#define NORMMIN 0.0
#define NORMMAX 1000000.0
#define MAXFILEINDEX 8192


/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)	(y >> 11)
#define TEMPERING_SHIFT_S(y) 	(y << 7)
#define TEMPERING_SHIFT_T(y)	(y << 15)
#define TEMPERING_SHIFT_L(y)	(y >> 18)

#define DEF_SEED 4357
#define MAXNUM 1

#define TRACKSMAX 2048

t_int compare(const void * a, const void * b);
t_float factorial(t_float f);
t_float reflectit(t_float f, t_float r);
unsigned long genrand(void);
void sgenrand(unsigned long seed);

t_float mod(t_float a, t_float n);

void sys_expandpath(const char *from, char *to, int bufsize);

void cantor_setup_setup(void);
void clifford_setup_setup(void);
void colormap_setup_setup(void);
void combi_setup_setup(void);
void connect_setup_setup(void);
void counter_setup_setup(void);
void crand_setup_setup(void);
void cuadratic_setup_setup(void);
void delit_setup_setup(void);
void factor_setup_setup(void);
void fdm_setup_setup(void);
void frand_setup_setup(void);
void g_sradio_setup_setup(void);
void glistinfo_setup_setup(void);
void halton_setup_setup(void);
void henon_setup_setup(void);
void irlog_setup_setup(void);
void irpow_setup_setup(void);
void irsqrt_setup_setup(void);
void iterate_setup_setup(void);
void lor_tilde_setup_setup(void);
void lorenz_setup_setup(void);
void lornorm_setup_setup(void);
void lorsig_tilde_setup_setup(void);
void mainpath_setup_setup(void);
void mandelbrot_setup_setup(void);
void minimax_setup_setup(void);
void mtwister_setup_setup(void);
void parabola_setup_setup(void);
void pmtwister_setup_setup(void);
void prandom_setup_setup(void);
void randy_setup_setup(void);
void reflect_setup_setup(void);
void root_setup_setup(void);
void scroll_setup_setup(void);
void siginfo_setup_setup(void);
void tracks_setup_setup(void);
