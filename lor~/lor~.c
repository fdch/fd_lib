#include "m_pd.h"
/* ---------------- lor~ ----------------- */

typedef struct lorctl
{
    t_sample c_x1;
    t_sample c_x2;
    t_sample c_y1;
    t_sample c_y2;
    t_sample c_z1;
    t_sample c_z2;
    t_sample c_ia;
    t_sample c_ib;
    t_sample c_ic;
    t_sample c_ih;

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
    x->x_f = 0;
    return (x);
}

static t_int *siglor_perform(t_int *w)
{
    //t_sample *in = (t_sample *)(w[1]);
    t_sample *out0 = (t_sample *)(w[1]);
    t_sample *out1 = (t_sample *)(w[2]);
    t_sample *out2 = (t_sample *)(w[3]);
    t_lorctl *c = (t_lorctl *)(w[4]);
    int n = (t_int)(w[5]);
    int i;
    t_sample xlast = c->c_x1;
    t_sample xprev = c->c_x2;
    t_sample ylast = c->c_y1;
    t_sample yprev = c->c_y2;
    t_sample zlast = c->c_z1;
    t_sample zprev = c->c_z2;
    t_sample ia = c->c_ia;
    t_sample ib = c->c_ib;
    t_sample ic = c->c_ic;
    t_sample ih = c->c_ih;
    for (i = 0; i < n; i++)
    {

        xprev = xlast + ih * ia * (ylast - xlast);
        yprev = ylast + ih * (xlast * (ib - zlast) - ylast);
        zprev = zlast + ih * (xlast * ylast - ic * zlast);
        
        *out0++ = xprev;
        *out1++ = yprev;
        *out2++ = zprev;
        
        xlast = xprev;
        ylast = yprev;
        zlast = zprev;
    }
    c->c_x1 = xlast;
    c->c_x2 = xprev;
    c->c_y1 = ylast;
    c->c_y2 = yprev;
    c->c_z1 = zlast;
    c->c_z2 = zprev;
    return (w+6);
}

static void siglor_list(t_siglor *x, t_symbol *s, int argc, t_atom *argv)
{
    c->c_ia = atom_getfloatarg(0, argc, argv);
    c->c_ib = atom_getfloatarg(1, argc, argv);
    c->c_ic = atom_getfloatarg(2, argc, argv);
    c->c_ih = atom_getfloatarg(3, argc, argv);
    t_lorctl *c = x->x_ctl;
}

static void siglor_set(t_siglor *x, t_symbol *s, int argc, t_atom *argv)
{
    t_lorctl *c = x->x_ctl;
    c->c_ia = 0;
    c->c_ib = 0;
    c->c_ic = 0;
    c->c_ih = 0;
    c->c_x1 = 0;
    c->c_x2 = 0;
    c->c_y1 = 0;
    c->c_y2 = 0;
    c->c_z1 = 0;
    c->c_z2 = 0;
}

static void siglor_dsp(t_siglor *x, t_signal **sp)
{
    dsp_add(siglor_perform, 5,
        sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec,
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