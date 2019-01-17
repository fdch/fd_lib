/* A C-Program for MT19937: Real number version						*/
/* genrand() generates one pseudorandom real number (double)		*/
/* which is uniformly distributed on [0,1]-interval, for each		*/
/* call. sgenrand(seed) set initial values to the working area		*/
/* of 624 words. Before genrand(), sgenrand() must be				*/
/* called once. (seed is any 32-bit integer except for 0).			*/
/* Integer generator is obtained by modifying two lines.			*/
/* Coded by Takuji Nishimura, considering the suggestions by		*/
/* Topher Cooper and Marc Rieffel in July-Aug. 1997. Comments		*/
/* should be addressed to: matumoto@math.keio.ac.jp 				*/

/* ======== Original C code for genrand() and sgenrand() ======== */
/* ======== with <time.h> as next seed ======== */

#include <stdio.h>
#include <time.h>

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

// #define DEF_SEED 4357


static unsigned long mt[N]; /* the array for the state vector */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* 

	Fede Camara Halac
	fdch.github.io

	genrand() and sgenrand() taken from "M. Matsumoto and T. Nishimura (1996)

*/

#include "m_pd.h"  

static t_class *mtwister_class;  

typedef struct _mtwister { 
	t_object		x_obj;
	double 			x_rng;
	double	 		x_off;
	unsigned long 	x_see;
} t_mtwister;  

static void mtwister_sgenrand(t_mtwister *x, unsigned long sd) {

	unsigned long seed = sd;
	
	/* setting initial seeds to mt[N] using			*/
	/* the generator Line 25 of Table 1 in			*/
	/* [KNUTH 1981, The Art of Computer Programming	*/
	/* 		Vol. 2 (2nd Ed.), pp102]				*/
	
	/* initializing the array with a NONZERO seed */
	mt[0]= seed & 0xffffffff;
	
	for (mti=1; mti<N; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

static double mtwister_rand(t_mtwister *x) {
	unsigned long y;

	static unsigned long mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A for x=0,1 */
	
	if (mti >= N) { /* generate N words at one time */
		int kk;
		/* if sgrand() has not been called,  a new seed is used*/
		if (mti == N+1)	mtwister_sgenrand(x, time(NULL));
		
		for (kk=0; kk<N-M;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (;kk<N-1;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		
		y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
		
		mti = 0;
	}
	
	y = mt[mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);
	
	return ( (double)y / (unsigned long)0xffffffff );
}


static void mtwister_set(t_mtwister *x, t_symbol *s, int argc, t_atom *argv) {
	int i;
	for (i = 0; i < argc; i++) {
			if (argv[i].a_type == A_FLOAT) {
			if 		(i == 0) {x->x_rng	= (double)	argv[i].a_w.w_float;}
			else if (i == 1) {x->x_off	= (double)	argv[i].a_w.w_float;}
		}   
    }
}

static void mtwister_bang(t_mtwister *x) {
	outlet_float(x->x_obj.ob_outlet, 
		mtwister_rand(x) * ( x->x_rng - x->x_off ) + x->x_off);
}  

static void mtwister_seed(t_mtwister *x, t_floatarg h) {
	x->x_see=h==0?(time(NULL)*rand()):(unsigned long)h;	//	store the seed
	mtwister_sgenrand(x,x->x_see);	//	set the actual seed
}

static void mtwister_print(t_mtwister *x) {
	post("Range:%f\nOffset:%f\nSeed:%d", x->x_rng,x->x_off,x->x_see);
}

void *mtwister_new(t_floatarg f, t_floatarg g, t_floatarg h) {  

	t_mtwister *x = (t_mtwister *)pd_new(mtwister_class);
	/* set range and min to map value */
	x->x_rng=f==0?1.0:(double)f;
	x->x_off=g==0?0.0:(double)g;	
	mtwister_seed(x,h);		//	set new seed
	outlet_new(&x->x_obj, &s_float);
	return (void *)x;  
}  

void mtwister_setup(void) {  
 	mtwister_class = class_new(gensym("mtwister"), 
  		(t_newmethod)mtwister_new, 0, 
  		sizeof(t_mtwister), 
  		CLASS_DEFAULT, 
  		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);  
	class_addbang(mtwister_class, mtwister_bang);
	class_addmethod(mtwister_class,
		(t_method)mtwister_set,
		gensym("set"),
		A_GIMME, 0);
	class_addmethod(mtwister_class,
		(t_method)mtwister_seed,
		gensym("seed"),
		A_DEFFLOAT, 0);
	class_addmethod(mtwister_class, 
		(t_method)mtwister_print,
		gensym("print"),0);
}