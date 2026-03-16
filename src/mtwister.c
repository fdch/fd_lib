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

/* A C-Program for MT19937: Real number version genrand() generates one
 * pseudorandom real number (t_float) which is uniformly distributed on
 * [0,1]-interval, for each call. sgenrand(seed) set initial values to the
 * working area of 624 words. Before genrand(), sgenrand() must be called once.
 * (seed is any 32-bit integer except for 0). Integer generator is obtained by
 * modifying two lines. Coded by Takuji Nishimura, considering the suggestions
 * by Topher Cooper and Marc Rieffel in July-Aug. 1997. Comments should be
 * addressed to: matumoto@math.keio.ac.jp
 */

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
    x->x_seed = !h ? time(NULL) * rand() : (unsigned long)h;
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
