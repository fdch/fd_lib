#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static t_class *wave_class;

typedef struct _wave
{
  t_object x_obj;
  t_float x_f;
  t_outlet *out;
  t_float p[64][64], c[64][64], f[64][64];
  t_float x, y, z;
  t_float speed, dt, dx, r, stopTime, n, t;
  int size, grab_size, row, col;
  int *grab_array;
} t_wave;


static void wave_bang(t_wave *x)
{
  int i,j;
/*

for i = s/2 - s/16 : s/2 + s/16 


c(i, s/2 - s/16 : s/2 + s/16) = 

- 2 * cos(pi / (s/8) * [s/2 - s/16 : s/2 + s/16]) * cos(pi / (s/8) * i);

*/

  for(i=x->size/2-x->size/16;i<=x->size/2+x->size/16;i++)
    for(j=x->size/2-x->size/16;j<=x->size/2+x->size/16;j++)
      x->c[i][j] = -2 * 
      cos(M_PI / (x->size/8) * x->c[i][j]) * cos(M_PI / (x->size/8) * i);

  for(i=0;i<x->size;i++)
    for(j=0;j<x->size;j++)
      x->p[i][j] = x->c[i][j];
      
  x->t = 0.0;
}

static void wave_row(t_wave *x, t_floatarg f)
{
  x->row = f>0?(int)f:0;
}

static void wave_col(t_wave *x, t_floatarg f)
{
  x->col = f>0?(int)f:0;
}

static void wave_list(t_wave *x, t_symbol *s, int argc, t_atom *argv)
{
  int i=0;
  if (!argc || 1==argc) {
    x->grab_size = 0;
    return;
  } else if (argc != x->grab_size) {
    x->grab_array = (int *)t_getbytes(argc*sizeof(int));
    x->grab_size = argc;
  }
  for(i=0; i<argc; i++)
    x->grab_array[i] = (int)argv[i].a_w.w_float;
  post("%f",x->f[(int)argv[i].a_w.w_float][(int)argv[i+1].a_w.w_float]);
}

static t_int *wave_perform(t_int *w)
{
  t_wave   *x   = (t_wave   *)(w[1]);  
  t_sample *in  = (t_sample *)(w[2]);
  t_sample *out = (t_sample *)(w[3]);
  int n = (int)(w[4]);
  int i = 0;
  int j = 0;
  //int g = 0;


   // wave equation
   
   for(i=1;i<=x->size-1;i++)
     for(j=1;j<=x->size-1;j++)
       x->f[i][j] = 2 * 
       x->c[i][j] - x->p[i][j] + 
       pow(x->r, 2 * (
                    x->c[i-1][j] + x->c[i+1][j] + 
                    x->c[i][j-1] + x->c[i][j+1] - 4 * 
                    x->c[i][j]));
/*
    f(s/2+s/4-1 : s/2+s/4+1 , 1:2) = 1.5 * sin(t*n);
	f(s/2-s/4-1 : s/2-s/4+1 , 1:2) = 1.5 * sin(t*n);
	f(2 : s-1,1:2) = 0;
*/
   for(i=x->size/2+x->size/4-1;i<=x->size/2+x->size/4+1;i++)
     for(j=0;j<=1;j++)
       x->f[i][j] = 1.5 * sin(x->t*(x->n));
   for(i=x->size/2-x->size/4-1;i<=x->size/2-x->size/4+1;i++)
     for(j=0;j<=1;j++)
       x->f[i][j] = 1.5 * sin(x->t*(x->n));
       
   for(i=1;i<=x->size-1;i++)
     for(j=0;j<=1;j++)
       x->f[i][j] = 0;

   /*
	//grab values
    if(2>=x->grab_size       && 
       g<=x->grab_size-2     &&
       i==x->grab_array[g])
    {
	  *out++ = (*in++) * x->f[i][x->grab_array[g+1]];
	  g+=2;
	}
	*/
	
	/*
    f(1,1) = 
    
    ( 2 * 
          c(1,1) + (r-1) * 
          p(1,1) + 2*r*r* 
          
          ( c(2,1) + c(1,2) - 2 * c(1,1) ) 
          
    ) / (1+r);   % X:1 ; Y:1
    
    
    f(s,s) = ( 2 * c(s,s) + (r-1) * p(s,s) + 2*r*r* ( c(s-1,s) + c(s,s-1) - 2*c(s,s))) / (1+r);   % X:s ; Y:s
    
    f(1,s) = ( 2 * c(1,s) + (r-1) * p(1,s) + 2*r*r* ( c(2,s)   + c(1,s-1) - 2*c(1,s))) / (1+r);   % X:1 ; Y:s
    
    f(s,1) = ( 2 * c(s,1) + (r-1) * p(s,1) + 2*r*r* ( c(s-1,1) + c(s,2)   - 2*c(s,1))) / (1+r);   % X:s ; Y:1
  
	*/
	
	//corners
	
	x->f[0][0] = ( 2 * x->c[0][0] + (x->r - 1) * x->p[0][0] + 2*x->r*x->r* (x->c[1][0] + x->c[0][1] - 2 * x->c[0][0])) / (1 + x->r);
	
	x->f[x->size][x->size] = ( 2 * x->c[x->size][x->size] + (x->r - 1) * x->p[x->size][x->size] + 2*x->r*x->r* (x->c[x->size-1][x->size] + x->c[x->size][x->size-1] - 2 * x->c[x->size][x->size])) / (1 + x->r);
	
	x->f[0][x->size] = ( 2 * x->c[0][x->size] + (x->r - 1) * x->p[0][x->size] + 2*x->r*x->r* (x->c[2][x->size] + x->c[0][x->size-1] - 2 * x->c[0][x->size])) / (1 + x->r);
	
	x->f[x->size][0] = ( 2 * x->c[x->size][0] + (x->r - 1) * x->p[x->size][0] + 2*x->r*x->r* (x->c[x->size-1][0] + x->c[x->size][2] - 2 * x->c[x->size][0])) / (1 + x->r);

  i=j=0;
  while(n--) {	
  //main pd signal loop

	//*out++ = *in++ * x->c [x->row==0?i++:x->row][x->col?x->col:j++];
    *out++ = *(in++) * x->c[x->row>=64?63:x->row<=0?0:x->row][j++];
    //*out++ = *in++ * x->c[x->row>64?64:x->row<=0?0:x->row][x->col>64?64:x->col<=0?0:x->col];
    
    x->t++;
  
  }
	//update values
  for(i=0;i<=x->size;i++)
    for(j=0;j<x->size;j++)
      x->p[i][j] = x->c[i][j];
      
  for(i=0;i<=x->size;i++)
    for(j=0;j<x->size;j++)
      x->c[i][j] = x->f[i][j];
	
  
  
  return (w+5);
}

void wave_tilde_free(t_wave *x)
{
 outlet_free(x->out);
}

static void wave_dsp(t_wave *x, t_signal **sp)
{
  dsp_add(wave_perform, 4,  x, 
  sp[0]->s_vec, 
  sp[1]->s_vec,
  sp[0]->s_n);
}

void wave_speed(t_wave *x,  t_floatarg f)
{
	x->speed = f;
}
void wave_dt(t_wave *x,  t_floatarg f)
{
	x->dt = f;
}
void wave_dx(t_wave *x,  t_floatarg f)
{
	x->dx = f;
}

int* allocate_mem(int*** arr, int n, int m)
{
  *arr = (int**)malloc(n * sizeof(int*));
  int *arr_data = malloc( n * m * sizeof(int));
  for(int i=0; i<n; i++)
     (*arr)[i] = arr_data + i * m ;
  return arr_data; //free point
} 

void deallocate_mem(int*** arr, int* arr_data)
{
  free(arr_data);
  free(*arr);
}

static void *wave_new(t_floatarg *f)
{
  t_wave *x = (t_wave *)pd_new(wave_class);
  x->out = outlet_new(&x->x_obj, &s_signal);

  
  x->size = (f>1?(int)f:64);
  //int size = x->size; 
  /*
  if((x->p = (t_float *)t_getbytes(size*sizeof(t_float)))   &&
     (x->c = (t_float *)t_getbytes(size*sizeof(t_float)))   &&
     (x->f = (t_float *)t_getbytes(size*sizeof(t_float)))     )
    x->size = size;
  else
    pd_error(x,"Something went wrong.");
  */
  //allocate_mem(*x->p,size,size);
  //allocate_mem(*x->c,size,size);
  //allocate_mem(*x->f,size,size);
  
  x->speed = 10;  //%propagation speed
  x->dt = 0.001; //%timestep (small!!) (time resolution of the simulation)
  x->dx = 0.1; 	//%distance between elements
  x->r = x->speed * x->dt / x->dx; 
  x->stopTime = 10; //%stop time
  //%for dynamic boundary
  x->n = 300;
  x->row = 0;
  x->col = 0;
  x->t = 0.0;
  wave_bang(x);
  return (x);
}

void wave_tilde_setup(void)
{
  wave_class = class_new(gensym("wave~"),
  (t_newmethod)wave_new, 0,
    sizeof(t_wave), 0, A_DEFFLOAT, 0);

  CLASS_MAINSIGNALIN(wave_class, t_wave, x_f);

  class_addmethod(wave_class,(t_method)wave_dsp, gensym("dsp"), 0);
  class_addbang(wave_class,(t_method)wave_bang);
  class_addlist(wave_class,(t_method)wave_list);
  class_addmethod(wave_class,(t_method)wave_speed, gensym("speed"), A_FLOAT, 0);
  class_addmethod(wave_class,(t_method)wave_dx, gensym("dx"), A_FLOAT, 0);
  class_addmethod(wave_class,(t_method)wave_dt, gensym("dt"), A_FLOAT, 0);
  class_addmethod(wave_class,(t_method)wave_row, gensym("row"), A_FLOAT, 0);
  class_addmethod(wave_class,(t_method)wave_col, gensym("col"), A_FLOAT, 0);
}