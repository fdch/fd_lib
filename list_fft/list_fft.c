#include "stdlib.h"
#include "math.h"
#include "m_pd.h"
#include "fftw3.h"

#define MAX 512
// choose either FFTW_MEASURE or FFTW_ESTIMATE here.
#define FFTWPLANNERFLAG FFTW_ESTIMATE

typedef struct _list_fft {
  t_object  x_obj;
  t_outlet *real_out, *imag_out;
  //t_outlet *mag_out;
  t_float *x_fftwIn;
  fftwf_complex *x_fftwOut;
  fftwf_plan x_fftwPlan;
  t_atom *x_real, *x_imag;
  //t_atom *x_mag;
  int x_max;
} t_list_fft;

static void list_fft_alloc(t_list_fft *x, int n) {
  int npow = sqrt(n)+1;
  x->x_fftwIn = (t_sample *)t_getbytes(n*sizeof(t_sample));
  x->x_real = (t_atom *)t_getbytes(n*sizeof(t_atom));
  x->x_imag = (t_atom *)t_getbytes(n*sizeof(t_atom));
  //x->x_mag = (t_atom *)t_getbytes(n*sizeof(t_atom));
  x->x_fftwOut = (fftwf_complex *)fftwf_alloc_complex(n);
  x->x_fftwPlan = fftwf_plan_dft_r2c_2d(npow, npow, x->x_fftwIn, x->x_fftwOut, FFTWPLANNERFLAG);
  x->x_max = n;
}

static void list_fft_list(t_list_fft *x, t_symbol *s, int argc, t_atom *argv) {
  int i, n;
  double re=0.0, im=0.0;
  
  if (!argc)
    return;
  if (argc != x->x_max)
    list_fft_alloc(x,argc);
  n=x->x_max;
  
  for(i=0; i<n; i++)
      x->x_fftwIn[i] = argv[i].a_w.w_float;

  fftwf_execute(x->x_fftwPlan);
  
  for (i=0; i<n; i++) {
    re=x->x_fftwOut[i][0];
    im=x->x_fftwOut[i][1];
    SETFLOAT(x->x_real+i, re);
    SETFLOAT(x->x_imag+i, im);
    //SETFLOAT(x->x_mag, log(hypot(x->x_fftwOut[i][0],x->x_fftwOut[i][1])));
  }
  
  //outlet_list(x->mag_out, gensym("list"), n, x->x_mag);
  outlet_list(x->imag_out, gensym("list"), n, x->x_imag);
  outlet_list(x->real_out, gensym("list"), n, x->x_real);
}



static t_class *list_fft_class;

void *list_fft_new(t_floatarg f) {
  t_list_fft *x = (t_list_fft *)pd_new(list_fft_class);
  x->real_out = outlet_new(&x->x_obj, &s_list);
  x->imag_out = outlet_new(&x->x_obj, &s_list);
  //x->mag_out = outlet_new(&x->x_obj, &s_list);
  list_fft_alloc(x,(f>2)?(int)f*f:MAX*MAX);
  return (void *)x;
}

static void list_fft_free(t_list_fft *x) {
  t_freebytes(x->x_imag, (x->x_max)*sizeof(t_atom));
  t_freebytes(x->x_real, (x->x_max)*sizeof(t_atom));
  //t_freebytes(x->x_mag, (x->x_max)*sizeof(t_atom));
  t_freebytes(x->x_fftwIn, (x->x_max)*sizeof(t_float));
  fftwf_free(x->x_fftwOut);
  fftwf_destroy_plan(x->x_fftwPlan);
}

void list_fft_setup(void) {
  list_fft_class = class_new(gensym("list_fft"),(t_newmethod)list_fft_new,(t_method)list_fft_free, sizeof(t_list_fft), CLASS_DEFAULT,A_FLOAT,0);
  class_addlist(list_fft_class, list_fft_list);
}