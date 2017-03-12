#include "m_pd.h"

typedef struct PdCon16_tilde
{
	t_object x_ob;
	t_outlet *x_outlet0;
	t_outlet *x_outlet1;
	t_outlet *x_outlet2;
	double x, y, z, inita, initb, initc, inith;
	int gmax, count;
} t_PdCon16_tilde;

void PdCon16_tilde_bang(t_PdCon16_tilde *x) {
	post("PdCon16~ NYC2016");
}

t_class *PdCon16_tilde_class;

void *PdCon16_tilde_new(t_symbol *selector, int argcount, t_floatarg f)
{
	t_PdCon16_tilde *x = (t_PdCon16_tilde *)pd_new(PdCon16_tilde_class);
    return (void *)x;
}
void PdCon16_tilde_setup(void)
{
    PdCon16_tilde_class = class_new(gensym("PdCon16~"), (t_newmethod)PdCon16_tilde_new, 0, sizeof(t_PdCon16_tilde), 0, 0);
    class_addbang(PdCon16_tilde_class, PdCon16_tilde_bang);  
}