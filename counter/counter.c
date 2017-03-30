#include "m_pd.h"  
 
static t_class *counter_class;  

//Object-variables
typedef struct _counter {  
  t_object  x_obj;  
  t_int i_count;  
} t_counter;  

//The Counter Method
void counter_bang(t_counter *x)  
{  
  t_float f=x->i_count;  
  x->i_count++;  
  outlet_float(x->x_obj.ob_outlet, f);  
}  

//Constructor
void *counter_new(t_floatarg f)  
{  
  t_counter *x = (t_counter *)pd_new(counter_class);  
 
  x->i_count=f;  
  outlet_new(&x->x_obj, &s_float);  
 
  return (void *)x;  
}  

//Object-arguments
void counter_setup(void) {  
  counter_class = class_new(gensym("counter"),  
        (t_newmethod)counter_new,  
        0, sizeof(t_counter),  
        CLASS_DEFAULT,  
        A_DEFFLOAT, 0);  
  class_addbang(counter_class, counter_bang);  
}