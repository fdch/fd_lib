#include "m_pd.h"
#include <math.h>
/* ---------------- lor~ - raw lor filter ----------------- */

typedef struct lorctl
{
    t_sample c_x1;
    t_sample c_x2;
    t_sample c_y1;
    t_sample c_y2;
    t_sample c_z1;
    t_sample c_z2;
    t_sample c_d1;
    t_sample c_d2;
    t_sample c_ia;
    t_sample c_ib;
    t_sample c_ic;
    t_sample c_ih;
    t_sample min;
    t_sample max;
    //t_sample c_ff3;
} t_lorctl;

typedef struct siglor
{
    t_object x_obj;
    t_float x_f;
    t_lorctl x_cspace;
    t_lorctl *x_ctl;
} t_siglor;

t_class *siglor_class;

static void siglor_list(t_siglor *x, t_symbol *s, int argc, t_atom *argv);

static void *siglor_new(t_symbol *s, int argc, t_atom *argv)
{
    t_siglor *x = (t_siglor *)pd_new(siglor_class);
    outlet_new(&x->x_obj, &s_signal);
    x->x_ctl = &x->x_cspace;
    x->x_cspace.c_x1 = x->x_cspace.c_x2 = 0;
    siglor_list(x, s, argc, argv);
    x->x_cspace.min = 0;
    x->x_cspace.max = 10000000000;
    x->x_f = 0;
    return (x);
}

static t_int *siglor_perform(t_int *w)
{
    t_sample *in = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    t_lorctl *c = (t_lorctl *)(w[3]);
    int n = (t_int)(w[4]);
    int i;
    t_sample xlast = c->c_x1;
    t_sample xprev = c->c_x2;
    t_sample ylast = c->c_y1;
    t_sample yprev = c->c_y2;
    t_sample zlast = c->c_z1;
    t_sample zprev = c->c_z2;
    t_sample last = c->c_d1;
    t_sample prev = c->c_d2;
    t_sample ia = c->c_ia;
    t_sample ib = c->c_ib;
    t_sample ic = c->c_ic;
    t_sample ih = c->c_ih;
    t_sample min = c->min;
    t_sample max = c->max;
    t_sample nmax, nmin, range;
    //t_sample ff3 = c->c_ff3;
    for (i = 0; i < n; i++)
    {
        t_sample output;
        t_sample outt = *out;
        xprev = xlast + ih * ia * (ylast - xlast);
        yprev = ylast + ih * (xlast * (ib - zlast) - ylast);
        zprev = zlast + ih * (xlast * ylast - ic * zlast);
        prev = sqrt((xlast * xlast) + (ylast * ylast)  + (zlast * zlast));
        
        output =  *in++ + ia * last + ib * prev;
        if (PD_BIGORSMALL(output))
            output = 0;
        outt = (((ih * output + ic * last + prev) / range) * 2. + 1. )* 0.5;
        xprev = xlast;
        yprev = ylast;
        zprev = zlast;
        prev = last;
        last = output;	
        nmax = (outt > max) ? outt : max;
        nmin = (outt < min) ? outt : min;
        range = fabsf(min) + max;
        outt++;
    }
    c->c_d1 = last;
    c->c_d2 = prev;
    c->c_x1 = xlast;
    c->c_x2 = xprev;
    c->c_y1 = ylast;
    c->c_y2 = yprev;
    c->c_z1 = zlast;
    c->c_z2 = zprev;
    c->min = nmin;
    c->max = nmax;
    return (w+5);
}

static void siglor_list(t_siglor *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float ia = atom_getfloatarg(0, argc, argv);
    t_float ib = atom_getfloatarg(1, argc, argv);
    t_float ic = atom_getfloatarg(2, argc, argv);
    t_float ih = atom_getfloatarg(3, argc, argv);
   // t_float discriminant = fb1 * fb1 + 4 * fb2;
    t_lorctl *c = x->x_ctl;
    //if (discriminant < 0) /* imaginary roots -- resonant filter */
    //{
            /* they're conjugates so we just check that the product
            is less than one */
        //if (fb2 >= -1.0f) goto stable;
    //}
    //else    /* real roots */
    //{
            /* check that the parabola 1 - fb1 x - fb2 x^2 has a
                vertex between -1 and 1, and that it's nonnegative
                at both ends, which implies both roots are in [1-,1]. */
        //if (fb1 <= 2.0f && fb1 >= -2.0f &&
            //1.0f - fb1 -fb2 >= 0 && 1.0f + fb1 - fb2 >= 0)
                //goto stable;
    //}
        /* if unstable, just bash to zero */
    //fb1 = fb2 = ff1 = ff2 = ff3 = 0;
//stable:
    c->c_ia = ia;
    c->c_ib = ib;
    c->c_ic = ic;
    c->c_ih = ih;
    //c->c_ff3 = ff3;
}

static void siglor_set(t_siglor *x, t_symbol *s, int argc, t_atom *argv)
{
    t_lorctl *c = x->x_ctl;
    c->c_d1 = atom_getfloatarg(0, argc, argv);
    c->c_d2 = atom_getfloatarg(1, argc, argv);
    c->min = 0;
    c->max = 10000000000;
}

static void siglor_dsp(t_siglor *x, t_signal **sp)
{
    dsp_add(siglor_perform, 4,
        sp[0]->s_vec, sp[1]->s_vec,
            x->x_ctl, sp[0]->s_n);

}

void lor_tilde_setup(void)
{
    siglor_class = class_new(gensym("lor~"), (t_newmethod)siglor_new,
        0, sizeof(t_siglor), 0, A_GIMME, 0);
    CLASS_MAINSIGNALIN(siglor_class, t_siglor, x_f);
    class_addmethod(siglor_class, (t_method)siglor_dsp,
        gensym("dsp"), A_CANT, 0);
    class_addlist(siglor_class, siglor_list);
    class_addmethod(siglor_class, (t_method)siglor_set, gensym("set"),
        A_GIMME, 0);
    class_addmethod(siglor_class, (t_method)siglor_set, gensym("clear"),
        A_GIMME, 0);
}