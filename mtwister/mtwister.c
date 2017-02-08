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

/* Tuned to PD by Fede Camara Halac www.fedecamarahalac.com */
/* C code taken from "M. Matsumoto and T. Nishimura (1996) */

#include "m_pd.h"  
#include <stdio.h>
#include <time.h>


/* ======== Original C code for genrand() and sgenrand() ======== */
/* ======== with <time.h> as next seed ======== */

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


static unsigned long mt[N]; /* the array for the state vector */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializing the array with a NONZERO seed */
void
sgenrand(seed)
	unsigned long seed;
{
	/* setting initial seeds to mt[N] using			*/
	/* the generator Line 25 of Table 1 in			*/
	/* [KNUTH 1981, The Art of Computer Programming	*/
	/* 		Vol. 2 (2nd Ed.), pp102]				*/
	mt[0]= seed & 0xffffffff;
	for (mti=1; mti<N; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

double /* generating reals */
/*unsigned long */ /* for integer generation */
genrand()
{
	unsigned long y;
	static unsigned long mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A for x=0,1 */
	
	if (mti >= N) { /* generate N words at one time */
		int kk;
		
		if (mti == N+1)		/* if sgrand() has not been called, */
			sgenrand(time(NULL));	/* a default initial seed is used */
		
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
	
	return ( (double)y / (unsigned long)0xffffffff ); /* reals */
	/*return y; */ /* for integer generation */
}

/* ======== PD Code starts here ========*/
static t_class *mtwister_class;  

//Object-variables
typedef struct _mtwister {  
  t_object  x_obj;  
} t_mtwister;  

//The Counter Method
void mtwister_bang(t_mtwister *x)  
{
	int j; 
	
	sgenrand(time(NULL)* rand());	/* any nonzero integer can be used as a seed */
	
	for (j=0; j<MAXNUM; j++) {
	
		outlet_float(x->x_obj.ob_outlet, genrand());
	}   
}  

//Constructor
void *mtwister_new(t_floatarg f)  
{  
  t_mtwister *x = (t_mtwister *)pd_new(mtwister_class);   
  outlet_new(&x->x_obj, &s_float);  
 
  return (void *)x;  
}  

//Object-arguments
void mtwister_setup(void) {  
  mtwister_class = class_new(gensym("mtwister"),  (t_newmethod)mtwister_new,  0, sizeof(t_mtwister),  CLASS_DEFAULT,  A_DEFFLOAT, 0);  
  class_addbang(mtwister_class, mtwister_bang);  
}