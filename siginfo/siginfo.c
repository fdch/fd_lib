#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

typedef struct siginfo
{
	t_object x_ob;
	t_outlet *x_outlet0;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	double xcheckprev, ycheckprev, zcheckprev;
	double xprev, yprev, zprev;
	double xcurr, ycurr, zcurr;
	double dist;
	int ref, xup, yup, zup;
} t_siginfo;



/* -------------------------- parse ------------------------------ */
void siginfo_parse(t_siginfo *x, t_symbol *selector, int argcount, t_atom *argvec)
{
    int i;
    for (i = 0; i < argcount; i++) {
    	if (argvec[i].a_type == A_FLOAT) {
			if 		(i == 0) {x->ref	= (int) argvec[i].a_w.w_float;}
			else if (i == 1) {x->xcurr = argvec[i].a_w.w_float;}
			else if (i == 2) {x->ycurr = argvec[i].a_w.w_float;}
			else if (i == 3) {x->zcurr = argvec[i].a_w.w_float;}
		}   
	}
	double xpre, ypre, zpre, xdiffn, ydiffn, zdiffn, t_dist, t_velo;
	
	xpre = x->xprev;
	ypre = x->yprev;
	zpre = x->zprev;
	
	xdiffn = x->xcurr - xpre;
	ydiffn = x->ycurr - ypre;
	zdiffn = x->zcurr - zpre;
	
	x->xup = (xdiffn >= 0 ) ? 1 : 0;
	x->yup = (ydiffn >= 0 ) ? 1 : 0;
	x->zup = (zdiffn >= 0 ) ? 1 : 0;

	//longitud de la diagonal que pasa por estos 2 puntos en 3d (x1, y1, z1) y (x2, y2, z2)
	t_dist = sqrt( (xdiffn * xdiffn) + (ydiffn * ydiffn) + (zdiffn * zdiffn) );
	





/////////////////////////// change values before leaving /////////////////////////////	
	
	x->xcheckprev = x->xprev;
	x->ycheckprev = x->yprev;
	x->zcheckprev = x->zprev;
	x->xprev = x->xcurr;
	x->yprev = x->ycurr;
	x->zprev = x->zcurr;
	x->dist = t_dist;
	
	
/////////////////////////// OUTLETS /////////////////////////////
	t_atom previous[3];
	
	
    SETFLOAT(&previous[0], (double) x->xcheckprev); 
    SETFLOAT(&previous[1], (double) x->ycheckprev); 
    SETFLOAT(&previous[2], (double) x->zcheckprev); 
	
	outlet_list(x->x_outlet2, 0, 3, previous);
	
	t_atom cook[7];
	
  	SETFLOAT(&cook[0], (int) x->xup);
    SETFLOAT(&cook[1], (int) x->yup);
    SETFLOAT(&cook[2], (int) x->zup);
	SETFLOAT(&cook[3], (double) xdiffn); 
	SETFLOAT(&cook[4], (double) ydiffn); 
    SETFLOAT(&cook[5], (double) zdiffn); 
    SETFLOAT(&cook[6], (double) x->dist);
    
    outlet_list(x->x_outlet1, 0, 7, cook);
    
    t_atom raw[4];
    
    SETFLOAT(&raw[0], (int) x->ref);
    SETFLOAT(&raw[1], (double) x->xcurr);
    SETFLOAT(&raw[2], (double) x->ycurr);
    SETFLOAT(&raw[3], (double) x->zcurr);  
    
    outlet_list(x->x_outlet0, 0, 4, raw);
}

void siginfo_bang(t_siginfo *x)
{
	post("prev xyz: %f, %f, %f\n curr xyz: %f, %f, %f\n dist: %f", x->xcheckprev, x->ycheckprev, x->zcheckprev, x->xcurr,x->ycurr,x->zcurr, x->dist);
}
void siginfo_flush(t_siginfo *x, t_symbol *selector)
{
	//default values
	x->ref = x->xcurr = x->ycurr = x->zcurr = x->dist = 0;
	x->xprev = x->yprev = x->zprev = 0;
	x->xup = x->yup = x->zup = 0;
	x->xcheckprev =	x->ycheckprev = x->zcheckprev = 0;
	//end default values
}

t_class *siginfo_class;
/////////////////////////// "New" routine /////////////////////////////
void *siginfo_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_siginfo *x = (t_siginfo *)pd_new(siginfo_class);
	//default values
	x->ref = x->xcurr = x->ycurr = x->zcurr = x->dist = 0;
	x->xprev = x->yprev = x->zprev = 0;
	x->xcheckprev =	x->ycheckprev = x->zcheckprev = 0;
	//end default values
    x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
    x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
    x->x_outlet2 = outlet_new(&x->x_ob, &s_list);
    return (void *)x;
}
/////////////////////////// Setup Routine /////////////////////////////
void siginfo_setup(void)
{
    siginfo_class = class_new(gensym("siginfo"), (t_newmethod)siginfo_new, 0, sizeof(t_siginfo), 0, 0);
    class_addbang(siginfo_class, siginfo_bang);  
    class_addmethod(siginfo_class, (t_method)siginfo_parse, gensym("parse"), A_GIMME, 0);
    class_addmethod(siginfo_class, (t_method)siginfo_flush, gensym("flush"), A_GIMME, 0);

}