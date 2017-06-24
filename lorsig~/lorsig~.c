#include "m_pd.h"
#include "math.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ lorsig~ ----------------------------- */

static t_class *lorsig_class;

typedef struct _lorsig {
  t_object x_obj;   /* obligatory header */
  t_float x_f;    /* place to hold inlet's value if it's set by message */
  t_outlet *x_outlet1;
  double x, y, z, inita, initb, initc, inith;
  double dist;
} t_lorsig;

static double lorsig_lorenz(t_lorsig *x, double signal) {
  double x0, y0, z0, x1, y1, z1;
  double h = x->inith; //step-size
  double a = x->inita*signal;
  double b = x->initb*signal;
  double c = x->initc;
  double t_dist;

  x0 = x->x;
  y0 = x->y;
  z0 = x->z;
  x1 = x0 + h * a * (y0 - x0);
  y1 = y0 + h * (x0 * (b - z0) - y0);
  z1 = z0 + h * (x0 * y0 - c * z0);

  t_dist = sqrt((x1 * x1) + (y1 * y1)  + (z1 * z1));

  x->x = x1;
  x->y = y1;
  x->z = z1;
  x->dist = t_dist;

  return(sqrt((t_dist*t_dist)+(signal*signal))*0.1);
}
static t_int *lorsig_perform(t_int *w) {
  t_sample *in = (t_sample *)(w[1]);
  t_sample *out = (t_sample *)(w[2]);
  int n = (int)(w[3]);
  t_lorsig *x = (t_lorsig *)(w[4]); 
  while (n--) {
    *out++ = lorsig_lorenz(x, *in++);
   }
   return (w+5);
}
static void lorsig_dsp(t_lorsig *x, t_signal **sp) {
  dsp_add(lorsig_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}
static void lorsig_reset(t_lorsig *x) {
  x->x = 0.1;
  x->y = 0;
  x->z = 0; 
  x->inita = 10.0;
  x->initb = 28.0;
  x->initc = 8.0 / 3.0;
  x->inith = 0.01;  
  //post("default: \n Prandlt = %f\n Rayleigh = %f\n c = %f\n ", x->inita, x->initb, x->initc);
}
static void lorsig_set(t_lorsig *x, t_symbol *s, int argc, t_atom *argv) {
  x->inita = atom_getfloatarg(0,argc, argv);
  x->initb = atom_getfloatarg(1,argc, argv);
  x->initc = atom_getfloatarg(2,argc, argv);
  x->inith = atom_getfloatarg(3,argc, argv);
  x->x = atom_getfloatarg(4,argc, argv);
  x->y = atom_getfloatarg(5,argc, argv);
  x->z = atom_getfloatarg(6,argc, argv);
}
static void lorsig_debug(t_lorsig *x) {
  t_atom raw[8];
  SETFLOAT(&raw[0], (float) x->inita);
  SETFLOAT(&raw[1], (float) x->initb);
  SETFLOAT(&raw[2], (float) x->initc);
  SETFLOAT(&raw[3], (float) x->inith);
  SETFLOAT(&raw[4], (float) x->x);
  SETFLOAT(&raw[5], (float) x->y);
  SETFLOAT(&raw[6], (float) x->z);
  SETFLOAT(&raw[7], (float) x->dist);
  outlet_list(x->x_outlet1, 0, 8, raw);
}
static void *lorsig_new(void) {
  t_lorsig *x = (t_lorsig *)pd_new(lorsig_class);
  outlet_new(&x->x_obj, gensym("signal"));
  x->x_outlet1 = outlet_new(&x->x_obj, &s_list);
  x->x_f = 0;
  x->x = 0.1;
  x->y = 0;
  x->z = 0;
  x->inita = 10.0;
  x->initb = 28.0;
  x->initc = 8.0 / 3.0;
  x->inith = 0.01;
  return (x);
}
void lorsig_tilde_setup(void) {
  lorsig_class = class_new(gensym("lorsig~"), (t_newmethod)lorsig_new, 0, sizeof(t_lorsig), 0, A_DEFFLOAT, 0);
  CLASS_MAINSIGNALIN(lorsig_class, t_lorsig, x_f);
  class_addmethod(lorsig_class, (t_method)lorsig_dsp, gensym("dsp"), 0);
  class_addmethod(lorsig_class, (t_method)lorsig_reset, gensym("reset"), A_NULL, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_set, gensym("set"),A_GIMME, 0);
  class_addmethod(lorsig_class, (t_method)lorsig_debug, gensym("debug"), A_NULL, A_NULL);
}
