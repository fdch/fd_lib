/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ lor~ ----------------------------- */


static t_class *lor_class;

typedef struct _lor
{
  t_object x_obj;   /* obligatory header */
  t_float x_f;
  t_inlet *in2;
  t_inlet *in3;
  t_inlet *in4;
  t_outlet *out1;
  t_outlet *out2;
  t_outlet *out3;
  t_float x, y, z;
} t_lor;


static t_int *lor_perform(t_int *w)
{
  t_lor *x = (t_lor *)(w[1]);
  t_sample *in1 = (t_sample *)(w[2]);
  t_sample *in2 = (t_sample *)(w[3]);
  t_sample *in3 = (t_sample *)(w[4]);
  t_sample *in4 = (t_sample *)(w[5]);
  t_sample *out1 = (t_sample *)(w[6]);
  t_sample *out2 = (t_sample *)(w[7]);
  t_sample *out3 = (t_sample *)(w[8]);
  int n = (int)(w[9]);

  float x0, y0, z0, x1, y1, z1;
  float h, a, b, c;

  while (n--)
  {
    h = *in1++;
    a = *in2++;
    b = *in3++;
    c = *in4++;

    // get initial conditions
    x0 = x->x;
    y0 = x->y;
    z0 = x->z;

    // euler's method for differentiation
    x1 = x0 + h * a * (y0 - x0);
    y1 = y0 + h * (x0 * (b - z0) - y0);
    z1 = z0 + h * (x0 * y0 - c * z0);

    // feed the out buffer and the position with the new position
    *out1++ = x->x = x1;
    *out2++ = x->y = y1;
    *out3++ = x->z = z1;
  }
  return (w+10);
}


void lor_tilde_free(t_lor *x)
{
  inlet_free(x->in2);
  inlet_free(x->in3);
  inlet_free(x->in4);
  outlet_free(x->out1);
  outlet_free(x->out2);
  outlet_free(x->out3);
}


static void lor_dsp(t_lor *x, t_signal **sp)
{
  dsp_add(lor_perform, 9,  x,
    sp[0]->s_vec,
    sp[1]->s_vec,
    sp[2]->s_vec,
    sp[3]->s_vec,
    sp[4]->s_vec,
    sp[5]->s_vec,
    sp[6]->s_vec,
    sp[0]->s_n
  );
}


void lor_initx(t_lor *x,  int argcount, t_atom *argvec)
{
  int i;
  for (i = 0; i < argcount; i++)
  {
    if (argvec[i].a_type == A_FLOAT)
    {
      x->x = argvec[i].a_w.w_float;
    }
  }
}
void lor_inity(t_lor *x,  int argcount, t_atom *argvec)
{
  int i;
  for (i = 0; i < argcount; i++)
  {
    if (argvec[i].a_type == A_FLOAT)
    {
      x->y = argvec[i].a_w.w_float;
    }
  }
}
void lor_initz(t_lor *x,  int argcount, t_atom *argvec)
{
  int i;
  for (i = 0; i < argcount; i++)
  {
    if (argvec[i].a_type == A_FLOAT)
    {
      x->z = argvec[i].a_w.w_float;
    }
  }
}

static void *lor_new(void)
{
  t_lor *x = (t_lor *)pd_new(lor_class);
  x->in2 = inlet_new(&x->x_obj,&x->x_obj.ob_pd, &s_signal,&s_signal);
  x->in3 = inlet_new(&x->x_obj,&x->x_obj.ob_pd, &s_signal,&s_signal);
  x->in4 = inlet_new(&x->x_obj,&x->x_obj.ob_pd, &s_signal,&s_signal);
  x->out1 = outlet_new(&x->x_obj, &s_signal);
  x->out2 = outlet_new(&x->x_obj, &s_signal);
  x->out3 = outlet_new(&x->x_obj, &s_signal);
  x->x_f = 0.0;
  x->x = INITX;
  x->y = INITY;
  x->z = INITZ;
  /*
  x->x1max = 10000000000.0;
  x->x1min = 0.0;
  */
  return (x);
}

void lor_tilde_setup(void)
{
  lor_class = class_new(gensym("lor~"),
  (t_newmethod)lor_new, 0, sizeof(t_lor), 0, A_DEFFLOAT, 0);

  CLASS_MAINSIGNALIN(lor_class, t_lor, x_f);

  class_addmethod(lor_class, (t_method)lor_dsp, gensym("dsp"), 0);
  class_addmethod(lor_class, (t_method)lor_initx, gensym("initx"), A_GIMME, 0);
  class_addmethod(lor_class, (t_method)lor_inity, gensym("inity"), A_GIMME, 0);
  class_addmethod(lor_class, (t_method)lor_initz, gensym("initz"), A_GIMME, 0);
}
