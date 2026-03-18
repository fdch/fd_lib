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

/* Period parameters */
#define NN 624
#define MM 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y) (y >> 11)
#define TEMPERING_SHIFT_S(y) (y << 7)
#define TEMPERING_SHIFT_T(y) (y << 15)
#define TEMPERING_SHIFT_L(y) (y >> 18)

/* A C-Program for MT19937: Real number version genrand() generates one
 * pseudorandom real number (t_float) which is uniformly distributed on
 * [0,1]-interval, for each call. sgenrand(seed) set initial values to the
 * working area of 624 words. Before genrand(), sgenrand() must be called once.
 * (seed is any 32-bit integer except for 0). Integer generator is obtained by
 * modifying two lines. Coded by Takuji Nishimura, considering the suggestions
 * by Topher Cooper and Marc Rieffel in July-Aug. 1997. Comments should be
 * addressed to: matumoto@math.keio.ac.jp
 */

/* ======== Original C code for genrand() and sgenrand() ======== */
/* ======== with <time.h> as next seed ======== */

unsigned long mt[NN]; /* the array for the state vector */
int mti = NN + 1;     /* mti==NN+1 means mt[NN] is not initialized */

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
    mt[0] = seed & 0xffffffff;
    for (mti = 1; mti < NN; mti++)
        mt[mti] = (69069 * mt[mti - 1]) & 0xffffffff;
}

unsigned long genrand()
{
    unsigned long y;
    static unsigned long mag01[2] = {0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A for x=0,1 */

    if (mti >= NN)
    { /* generate NN words at one time */
        int kk;

        if (mti == NN + 1)        /* if sgrand() has not been called, */
            sgenrand(time(NULL)); /* a default initial seed is used */

        for (kk = 0; kk < NN - MM; kk++)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + MM] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (; kk < NN - 1; kk++)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + (MM - NN)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[NN - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[NN - 1] = mt[MM - 1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }

    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    /* return ( (double)y / (unsigned long)0xffffffff ); */ /* reals */
    return y; /* for integer generation */
}

static t_class *mtwister_class;

typedef struct _mtwister
{
    t_object x_ob;
    unsigned long x_seed;
    t_norm x_result;
    int x_norm;
} t_mtwister;

static void mtwister_norm(t_mtwister *x, t_floatarg fnorm)
{
    x->x_norm = !!(int)fnorm;
}

static void mtwister_output_result(t_mtwister *x)
{
    outlet_float(x->x_ob.te_outlet,
                 x->x_norm ? norm_getnorm(&x->x_result) : x->x_result.x_value);
}

static void mtwister_bang(t_mtwister *x)
{
    norm_setval(&x->x_result, genrand());
    mtwister_output_result(x);
}

static void mtwister_seed(t_mtwister *x, t_floatarg h)
{
    x->x_seed = !h ? (unsigned long)time(NULL) * rand() : (unsigned long)h;
    sgenrand(x->x_seed);
}

static void mtwister_prime(t_mtwister *x, t_floatarg ftryouts)
{
    int max_tryouts = (!ftryouts || ftryouts < 1) ? 1 : (int)ftryouts;
    while (max_tryouts--)
    {
        int n = genrand();
        int prime = 1;

        if (n < 2)
            prime = 0;

        for (int i = 2; i * i <= n; i++)
            if (n % i == 0)
            {
                prime = 0;
                break;
            }

        if (prime)
        {
            norm_setval(&x->x_result, n);
            return mtwister_output_result(x);
        }
    }
    outlet_bang(x->x_ob.te_outlet);
}

static void mtwister_reset(t_mtwister *x) { norm_reset(&x->x_result, 1.0); }

static void *mtwister_new(t_floatarg fseed)
{
    t_mtwister *x = (t_mtwister *)pd_new(mtwister_class);
    outlet_new(&x->x_ob, gensym("float"));
    x->x_norm = 1;
    mtwister_seed(x, fseed);
    return (void *)x;
}

void mtwister_setup(void)
{
    mtwister_class = class_new(gensym("mtwister"), (t_newmethod)mtwister_new, 0,
                               sizeof(t_mtwister), CLASS_DEFAULT, A_DEFFLOAT,
                               A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addbang(mtwister_class, mtwister_bang);
    class_addmethod(mtwister_class, (t_method)mtwister_norm,
                    gensym("normalize"), A_FLOAT, A_NULL);
    class_addmethod(mtwister_class, (t_method)mtwister_seed, gensym("seed"),
                    A_DEFFLOAT, A_NULL);
    class_addmethod(mtwister_class, (t_method)mtwister_prime, gensym("prime"),
                    A_DEFFLOAT, A_NULL);
    class_addmethod(mtwister_class, (t_method)mtwister_reset, gensym("reset"),
                    A_NULL);
}
