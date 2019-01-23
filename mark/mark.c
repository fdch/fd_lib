/*
    mark

    outputs a markov sequence

*/

#include "m_pd.h"
#include "math.h"
#include "stdlib.h"
#include <time.h>

#define DEFSTATES 3

static t_class *mark_class;

typedef struct prob {
    t_int       p_prev;
    t_int       p_state;    
} t_prob;

typedef struct mark	{
    t_object	x_ob;
    t_outlet    *x_out1;
    t_int       x_state;
    t_prob      *x_prob;
    t_atom      *x_probs;
    t_int       x_nprob;
    double      x_r;
} t_mark;

static void mark_sprob(t_mark *x) {}

static void mark_set(t_mark *x, t_symbol *s, int argc, t_atom *argv) { 
	double remainder;
    for (int i=0; i<argc; i++) {
        if (argv[i].a_type == A_FLOAT) {
            SETFLOAT(x->x_probs+i, (double) argv[i].a_w.w_float);
            remainder+=argv[i].a_w.w_float;
        }
    }
    if (remainder>=1) {
        post("Warning: sum is greater than 1");
    } else {
        SETFLOAT(x->x_probs+argc,1-remainder);
    }
}

static void mark_reset(t_mark *x) {}



// #define SWAP(x0,x) {float *tmp=x0; x0=x; x=tmp;}

// #define MAX(a, b) ((a)>(b)? (a) : (b))


static void mark_bang(t_mark *x) {
    t_atom list[2];
    double r = (double) rand() / RAND_MAX;





/*


if len(p) == 2

    len(s) = len(p)+1  --> (0,1,2)
    s[0] = if (a[0] >  f) 
    s[1] = if (a[0] <= f && a[1] > f)
    s[2] = if (a[1] <= f) 

if len(p) == 3
    
    len(s) = len(p)+1 --> (0,1,2,3)
    

*/






    // SETFLOAT(list,r);
    // SETFLOAT(list+1,state);
    outlet_float(x->x_ob.ob_outlet, r);
    // outlet_list(x->x_ob.ob_outlet, gensym("list"), 2, list);
}

static void mark_float(t_mark *x, t_floatarg f) {}

static void mark_list(t_mark *x, t_symbol *s, int argc, t_atom *argv) {}

void mark_seed(t_mark *x, t_floatarg f) {
    unsigned int seed = (unsigned int) f;
    srand(seed);
}

static void mark_free(t_mark *x) {
    t_freebytes(x->x_probs, (x->x_nprob)*sizeof(t_atom));
}

void *mark_new(t_symbol *s, int argc, t_atom *argv)	{
    int i;
    t_float def_prob=1.0/DEFSTATES;
    t_mark *x = (t_mark *)pd_new(mark_class);
    time_t t;
    srand((unsigned) time(&t));
    if(argc) {
        x->x_probs = (t_atom *)t_getbytes((argc+1)*sizeof(t_atom));
        x->x_nprob = argc+1;
        mark_set(x,s,argc,argv);
    } else {
        x->x_probs = (t_atom *)t_getbytes(DEFSTATES*sizeof(t_atom));
        x->x_nprob = DEFSTATES;
        for (i=0;i<x->x_nprob;i++) SETFLOAT(x->x_probs+i, def_prob);
    }
    x->x_out1 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

void mark_setup(void)	{
    mark_class = class_new(gensym("mark"),
    	(t_newmethod)mark_new, (t_method)mark_free,
    	sizeof(t_mark), 0, 
    	A_GIMME, 0);
    class_addbang(mark_class, mark_bang);
    class_addfloat(mark_class, mark_float);
    class_addlist(mark_class, mark_list);
    class_addmethod(mark_class, 
    	(t_method)mark_set, 
    	gensym("set"), 
    	A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(mark_class, 
    	(t_method)mark_reset, 
    	gensym("reset"), 0);
    class_addmethod(mark_class, 
        (t_method)mark_seed, 
        gensym("seed"), 
        A_FLOAT, 0);
}