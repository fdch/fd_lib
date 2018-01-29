#include "stdlib.h"
#include "math.h"
#include "m_pd.h"

typedef struct _colormap {
  t_object  x_obj;
  int x_size;
  float *x_r, *x_g, *x_b;
} t_colormap;

static void make_curves(t_colormap *x)
{
  int i, size;
  double r,b,g;
  size = x->x_size;
  x->x_r = (float *)t_getbytes(size*sizeof(float));
  x->x_g = (float *)t_getbytes(size*sizeof(float));
  x->x_b = (float *)t_getbytes(size*sizeof(float));
  
  for(i=0;i<size;i++){
    b=(float)i/(float)size;
    r=sqrt(b);
    g=-r+1;
    x->x_b[i]=(float)b;
    x->x_r[i]=(float)r;
    x->x_g[x->x_size-i]=(float)g;
  }

  //post("size=%d",size);
}

static void colormap_float(t_colormap *x, t_floatarg f)
{
  int i;
  if (f<1)
   i=0;
  if (f>x->x_size)
   i=x->x_size;

  t_atom out[3];
  SETFLOAT(out,  x->x_r[i]);
  SETFLOAT(out+1,x->x_g[i]);
  SETFLOAT(out+2,x->x_b[i]);
  outlet_list(x->x_obj.ob_outlet, gensym("list"), 3, out);
}

static t_class *colormap_class;

void *colormap_new(t_floatarg f) {
  t_colormap *x = (t_colormap *)pd_new(colormap_class);
  outlet_new(&x->x_obj, &s_list);
  x->x_size = f<128?128:f;
  make_curves(x);
  return (void *)x;
}

static void colormap_free(t_colormap *x)
{
  t_freebytes(x->x_r, (x->x_size)*sizeof(t_float));
  t_freebytes(x->x_g, (x->x_size)*sizeof(t_float));
  t_freebytes(x->x_b, (x->x_size)*sizeof(t_float));
}

void colormap_setup(void) {
  colormap_class = class_new(gensym("colormap"),(t_newmethod)colormap_new,(t_method)colormap_free, sizeof(t_colormap), CLASS_DEFAULT,A_FLOAT,0);
  class_addfloat(colormap_class, colormap_float);
}