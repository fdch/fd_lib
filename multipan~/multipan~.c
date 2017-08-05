#include "m_pd.h"
#include <string.h>

static t_class *multipan_tilde_class;

typedef struct _multipan_tilde {
  t_object  x_obj;
  t_sample f_pan;
  t_sample f;

  t_inlet *x_in2;
  t_inlet *x_in3;
  t_outlet*x_out;
} t_multipan_tilde;
 
t_int *multipan_tilde_perform(t_int *w) {
  t_multipan_tilde *x = (t_multipan_tilde *)(w[1]);
  t_sample *in1 = (t_sample *)(w[2]);
  t_sample *out = (t_sample *)(w[3]);
  t_sample *out = (t_sample *)(w[4]);
  int n = (int)(w[5]);
  t_sample f_pan = (x->f_pan<0)?0.0:(x->f_pan>1)?1.0:x->f_pan;

  while (n--) *out++ = (*in1++)*(1-f_pan)+(*in2++)*f_pan;

  return (w+6);
}
 
void multipan_tilde_dsp(t_multipan_tilde *x, t_signal **sp) {
  dsp_add(multipan_tilde_perform, 5, x,
          sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}
 
void multipan_tilde_free(t_multipan_tilde *x) {
  inlet_free(x->x_in2);
  inlet_free(x->x_in3);
  outlet_free(x->x_out);
}
 
void *multipan_tilde_new(t_symbol *s, int argc, t_atom *argv) {
  t_multipan_tilde *x = (t_multipan_tilde *)pd_new(multipan_tilde_class);
 
  x->f_pan = atom_getfloatarg(0,argc,argv);
 
  x->x_in2=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  x->x_in3=floatinlet_new (&x->x_obj, &x->f_pan);
  x->x_out=outlet_new(&x->x_obj, &s_signal);
  int i = 0;
  if (argc)
    while (argc--)
      outlet_new(&x->x_obj, gensym("signal"));
  return (void *)x;
}
void multipan_tilde_setup(void) {
  multipan_tilde_class = class_new(gensym("pan~"),
        (t_newmethod)multipan_tilde_new,
        0, sizeof(t_multipan_tilde),
        CLASS_DEFAULT,
        A_GIMME, 0);
 
  class_addmethod(multipan_tilde_class,
        (t_method)multipan_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(multipan_tilde_class, t_multipan_tilde, f);
}