#include "m_pd.h"  
 
static t_class *lorenzc_class;  

//Object-variables
typedef struct _lorenzc {  
  t_object  x_obj;  
  t_int i_count; 
  t_outlet *x_outlet;
  t_outlet *x_outlet1;
  t_outlet *x_outlet2;
  t_outlet *x_outlet3;
  t_outlet *x_outlet4;
} t_lorenzc;  

//The Counter Method
void lorenzc_bang(t_lorenzc *x)  
{  
  t_int i;
  i = x->i_count;  
  x->i_count++; 
 
   while ((i=0) && (i<1000)) {
   
   
     
   outlet_float(x->x_outlet, i);
   
   
   double x0,y0,z0,x1,y1,z1;
   double h = 0.01;
   double a = 10.0;
   double b = 28.0;
   double c = 8.0 / 3.0;

   x0 = 0.1;
   y0 = 0;
   z0 = 0;
   
      x1 = x0 + h * a * (y0 - x0);
      y1 = y0 + h * (x0 * (b - z0) - y0);
      z1 = z0 + h * (x0 * y0 - c * z0);
      x0 = x1;
      y0 = y1;
      z0 = z1;
      if (i > 100)
      outlet_float(x->x_outlet1, i);
      outlet_float(x->x_outlet2, x0);
      outlet_float(x->x_outlet3, y0);
      outlet_float(x->x_outlet4, z0);
   }

}  

//Constructor
void *lorenzc_new(t_floatarg f)  
{  
  t_lorenzc *x = (t_lorenzc *)pd_new(lorenzc_class);  
 
  x->i_count=f;  
  x->x_outlet = outlet_new(&x->x_obj, gensym("float"));
 x->x_outlet1 = outlet_new(&x->x_obj, gensym("float"));
 x->x_outlet2 = outlet_new(&x->x_obj, gensym("float"));
 x->x_outlet3 = outlet_new(&x->x_obj, gensym("float"));
 x->x_outlet4 = outlet_new(&x->x_obj, gensym("float"));  
 
  return (void *)x;  
}  

//Object-arguments
void lorenzc_setup(void) {  
  lorenzc_class = class_new(gensym("lorenzc"),  
        (t_newmethod)lorenzc_new,  
        0, sizeof(t_lorenzc),  
        CLASS_DEFAULT,  
        A_DEFFLOAT, 0);  
  class_addbang(lorenzc_class, lorenzc_bang);  
}