/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

#ifdef UNIX
#include <unistd.h>
#endif

#include <sys/stat.h>

#ifdef MSW
#include <io.h>
#include <windows.h>
#endif

#ifdef MSW
# include <malloc.h> /* MSVC or mingw on windows */
#elif defined(UNIX) || defined(MACOSX)
# include <alloca.h> /* linux, mac, mingw, cygwin */
#else
# include <stdlib.h> /* BSDs for example */
#endif


// Needed for qsort.  See http://w...content-available-to-author-only...s.com/reference/cstdlib/qsort/
t_int compare (const void * a, const void * b) { 
    return ( *(int*)a - *(int*)b );  
}

t_float factorial(t_float f) {
  if (f > 1)
    return (f * factorial (f-1));
  else
    return 1;
}

t_float reflectit(t_float f, t_float r) {
	t_float t = f;
	do 
	t = (fabs((fabs(t + r) - r) * -1 + r ) - r) * -1;
	while (t < (r * -1));
	if (t==(-0))t=0;
	return t;
}

t_float mod(t_float a, t_float n)
{
	return a - n*floor(a/n);
}

/* ======== Original C code for genrand() and sgenrand() ======== */
/* ======== with <time.h> as next seed ======== */


unsigned long mt[NN]; /* the array for the state vector */
int mti=NN+1; /* mti==NN+1 means mt[NN] is not initialized */

/*

	genrand() and sgenrand() taken from "M. Matsumoto and T. Nishimura (1996)

*/

/* initializing the array with a NONZERO seed */
void sgenrand(unsigned long seed)
{
	/* setting initial seeds to mt[NN] using			*/
	/* the generator Line 25 of Table 1 in			*/
	/* [KNUTH 1981, The Art of Computer Programming	*/
	/* 		Vol. 2 (2nd Ed.), pp102]				*/
	mt[0]= seed & 0xffffffff;
	for (mti=1; mti<NN; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

unsigned long genrand()
{
	unsigned long y;
	static unsigned long mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A for x=0,1 */
	
	if (mti >= NN) { /* generate NN words at one time */
		int kk;
		
		if (mti == NN+1)		/* if sgrand() has not been called, */
			sgenrand(time(NULL));	/* a default initial seed is used */
		
		for (kk=0; kk<NN-MM;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+MM] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (;kk<NN-1;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(MM-NN)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (mt[NN-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[NN-1] = mt[MM-1] ^ (y >> 1) ^ mag01[y & 0x1];
		
		mti = 0;
	}
	
	y = mt[mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);
	
	/* return ( (double)y / (unsigned long)0xffffffff ); */ /* reals */
	return y;  /* for integer generation */
}

// shamelessly taken from s_path.c
/* expand env vars and ~ at the beginning of a path and make a copy to return */
void sys_expandpath(const char *from, char *to, int bufsize)
{
    if ((strlen(from) == 1 && from[0] == '~') || (strncmp(from,"~/", 2) == 0))
    {
#ifdef MSW
        const char *home = getenv("USERPROFILE");
#else
        const char *home = getenv("HOME");
#endif
        if (home)
        {
            strncpy(to, home, bufsize);
            to[bufsize-1] = 0;
            strncpy(to + strlen(to), from + 1, bufsize - strlen(to));
            to[bufsize-1] = 0;
        }
        else *to = 0;
    }
    else
    {
        strncpy(to, from, bufsize);
        to[bufsize-1] = 0;
    }
#ifdef MSW
    {
        char *buf = alloca(bufsize);
        ExpandEnvironmentStrings(to, buf, bufsize-1);
        buf[bufsize-1] = 0;
        strncpy(to, buf, bufsize);
        to[bufsize-1] = 0;
    }
#endif
}
