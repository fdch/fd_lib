/*

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "fdLib.h"
#include "m_pd.h"

/*
 * code for the "lorenz" pd class.
 * [lorenz] runs the equations for the lorenz system as in (https://en.wikipedia.org/wiki/Lorenz_system)
 * developed by Fede Camara Halac (fch226@nyu.edu)
 * @ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
 * Released under the GNU General Public License. 
 */

#define MAXLORTYPES 20
#define MAXLORPARAMS 7

#define LVAL(n) ((x->x_norm) ? norm_getnorm(n) : (n)->x_value)

  /* inita, initb, initc, inith, x, y, z */
t_float lorenz_types[MAXLORTYPES][MAXLORPARAMS] = {
  /* stable */
{10,28,8./3.,0.01,0.1,0,0},
{10,20,8./3.,0.005,1,1,10},
{11,20,2.8,0.005,3,5,8},
{8,15,8./3.,0.01,4,4,4},
{13,22,2.7,0.005,3,1,9},
{9,16,2.4,0.01,3,6,8},
{7,20,2.9,0.005,5,2,10},
{10,22,8./3.,0.005,1,1,20},
  /* unstable */
{5,12,2.2,0.01,6,3,2},
{10,10,8./3.,0.01,1,2,3},
{10,15,8./3.,0.01,2,3,4},
{8,12,3,0.01,3,4,5},
{12,18,3,0.005,5,5,5},
{14,16,2.5,0.005,4,3,2},
{6,10,2,0.01,2,3,1},
{7,14,2.5,0.01,1,4,6},
{9,18,3,0.005,5,2,7},
{6,8,2,0.01,2,2,2},
{10,18,8./3.,0.005,7,5,3},
{12,14,3,0.01,2,7,4},
};

/* --------------------------- a normalizable value ------------------------- */

  /* keep a normalizable value to use as state variable */
typedef struct norm
{
  t_float x_value;
  t_float x_min;
  t_float x_max;
} t_norm;

  /* set the value and the min/max accordingly */
static void norm_setval(t_norm *x, t_float fval)
{
  if (fval > x->x_max)
    x->x_max = fval;
  if (fval < x->x_min)
    x->x_min = fval;
  x->x_value = fval;
}

  /* reset the value and the max and min */
static void norm_reset(t_norm *x, t_float fval)
{
  x->x_max = fval;
  x->x_min = fval;
  x->x_value = fval;
}

  /* get the normalized version of the value */
static t_float norm_getnorm(t_norm *x)
{
  t_float val, range = x->x_max - x->x_min;
  if(range == 0)
      return 0;
  val = (x->x_value - x->x_min) / range;
  if (val > 1.0)
    return 1.0;
  return val;
}

/* --------------------------- a base lorenz class -------------------------- */

static t_class *lorenz_class;

typedef struct lorenz
{
  t_pd    x_ob;
  t_norm  x_x, x_y, x_z;
  t_float x_inita, x_initb, x_initc, x_inith;
  int     x_type, x_norm;
  t_atom  x_raw[3];
} t_lorenz;

static void lorenz_loop(t_lorenz *x, t_norm *nx, t_norm *ny, t_norm *nz,
                        t_float a, t_float b, t_float c, t_float h)
{
  t_float x0 = nx->x_value, y0 = ny->x_value, z0 = nz->x_value;
  t_float x1 = x0 + h * a * (y0 - x0);
  t_float y1 = y0 + h * (x0 * (b - z0) - y0);
  t_float z1 = z0 + h * (x0 * y0 - c * z0);
  norm_setval(&x->x_x, x1);
  norm_setval(&x->x_y, y1);
  norm_setval(&x->x_z, z1);
  SETFLOAT(&x->x_raw[0], LVAL(&x->x_x));
  SETFLOAT(&x->x_raw[1], LVAL(&x->x_y));
  SETFLOAT(&x->x_raw[2], LVAL(&x->x_z));
}

static void lorenz_doit(t_lorenz *x)
{
  lorenz_loop(x, &x->x_x, &x->x_y, &x->x_z,
              x->x_inita, x->x_initb, x->x_initc, x->x_inith);
}

static void lorenz_type(t_lorenz *x, t_floatarg ftype)
{
  int type = ftype ? (int)ftype : 0;
  if (type < 0)
    type = 0;
  if (type > MAXLORTYPES)
    type = MAXLORTYPES - 1;
  x->x_inita = lorenz_types[type][0];
  x->x_initb = lorenz_types[type][1];
  x->x_initc = lorenz_types[type][2];
  x->x_inith = lorenz_types[type][3];
  x->x_x.x_value = lorenz_types[type][4];
  x->x_y.x_value = lorenz_types[type][5];
  x->x_z.x_value = lorenz_types[type][6];
}

static void lorenz_reset(t_lorenz *x)
{
  lorenz_type(x, x->x_type);
}

static void lorenz_inita(t_lorenz *x, t_floatarg f)
{ x->x_inita = f; }

static void lorenz_initb(t_lorenz *x, t_floatarg f)
{ x->x_initb = f; }

static void lorenz_initc(t_lorenz *x, t_floatarg f)
{ x->x_initc = f; }

static void lorenz_inith(t_lorenz *x, t_floatarg f)
{ x->x_inith = f; }

static void lorenz_initx(t_lorenz *x, t_floatarg f)
{ norm_reset(&x->x_x, f); }

static void lorenz_inity(t_lorenz *x, t_floatarg f)
{ norm_reset(&x->x_y, f); }

static void lorenz_initz(t_lorenz *x, t_floatarg f)
{ norm_reset(&x->x_z, f); }

static void lorenz_print_types(t_outlet *outlet)
{
  if (!outlet)
    return;

  for (int i = 0; i < MAXLORTYPES; i++)
  {
    t_atom params[MAXLORPARAMS + 1];
    SETFLOAT(&params[0], i);
    for (int j = 0; j < MAXLORPARAMS; j++)
      SETFLOAT(&params[j + 1], lorenz_types[i][j]);
    outlet_list(outlet, gensym("list"), MAXLORPARAMS + 1, params);
  }
}

static void lorenz_normalize(t_lorenz *x, t_floatarg fnorm)
{ x->x_norm = !!(int)fnorm; }

static t_lorenz *lorenz_new(t_floatarg fnorm)
{
  t_lorenz *x = (t_lorenz *)pd_new(lorenz_class);
  lorenz_normalize(x, fnorm);
  lorenz_reset(x);
  return x;
}

/* ----------------------------------- [lorenz] ----------------------------- */

static t_class *lorenz_obj_class;

typedef struct lorenz_obj
{
  t_object  x_ob;
  t_lorenz *x_lorenz;
  t_outlet *x_outlet1, *x_outlet2;
} t_lorenz_obj;

static void lorenz_obj_bang(t_lorenz_obj *x)
{
  lorenz_doit(x->x_lorenz);
  outlet_list(x->x_ob.te_outlet, 0, 3, x->x_lorenz->x_raw);
  outlet_bang(x->x_outlet2);
}

static void lorenz_obj_print(t_lorenz_obj *x, t_floatarg f)
{
  int max = (int)f;
  for (int i = 0; i < max; i++)
  {
    lorenz_doit(x->x_lorenz);
    outlet_list(x->x_outlet1, 0, 3, x->x_lorenz->x_raw);
    outlet_bang(x->x_outlet2);
  }
}

static void lorenz_obj_inita(t_lorenz_obj *x, t_floatarg f)
{ lorenz_inita(x->x_lorenz, f); }

static void lorenz_obj_initb(t_lorenz_obj *x, t_floatarg f)
{ lorenz_initb(x->x_lorenz, f); }

static void lorenz_obj_initc(t_lorenz_obj *x, t_floatarg f)
{ lorenz_initc(x->x_lorenz, f); }

static void lorenz_obj_inith(t_lorenz_obj *x, t_floatarg f)
{ lorenz_inith(x->x_lorenz, f); }

static void lorenz_obj_initx(t_lorenz_obj *x, t_floatarg f)
{ lorenz_initx(x->x_lorenz, f); }

static void lorenz_obj_inity(t_lorenz_obj *x, t_floatarg f)
{ lorenz_inity(x->x_lorenz, f); }

static void lorenz_obj_initz(t_lorenz_obj *x, t_floatarg f)
{ lorenz_initz(x->x_lorenz, f); }

static void lorenz_obj_type(t_lorenz_obj *x, t_floatarg f)
{ lorenz_type(x->x_lorenz, f); }

static void lorenz_obj_reset(t_lorenz_obj *x)
{ lorenz_reset(x->x_lorenz); }

static void lorenz_obj_print_types(t_lorenz_obj *x)
{ lorenz_print_types(x->x_outlet1); }

static void lorenz_obj_normalize(t_lorenz_obj *x, t_floatarg f)
{ lorenz_normalize(x->x_lorenz, f); }

static void *lorenz_obj_new(t_floatarg fnorm)
{
  t_lorenz_obj *x = (t_lorenz_obj *)pd_new(lorenz_obj_class);
  x->x_lorenz = lorenz_new(fnorm);
  outlet_new(&x->x_ob, gensym("list"));
  x->x_outlet1 = outlet_new(&x->x_ob, gensym("list"));
  x->x_outlet2 = outlet_new(&x->x_ob, gensym("bang"));
  x->x_lorenz->x_type = 0;
  lorenz_reset(x->x_lorenz);
  return (void *)x;
}

/* ---------------------------------- [lorsig~] ----------------------------- */

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

static t_class *lorsig_class;

typedef struct _lorsig
{
  t_object  x_ob;
  t_lorenz *x_lorenz;
  t_outlet *x_outlet1;
  t_float   x_f;
  t_norm    x_dist;
} t_lorsig;

static t_float lorsig_lorenz(t_lorsig *x)
{
  t_float nx, ny, nz;
  lorenz_doit(x->x_lorenz);

  if(x->x_lorenz->x_norm)
  {
    nx = norm_getnorm(&x->x_lorenz->x_x);
    ny = norm_getnorm(&x->x_lorenz->x_y);
    nz = norm_getnorm(&x->x_lorenz->x_z);
  }
  else
  {
    nx = x->x_lorenz->x_x.x_value;
    ny = x->x_lorenz->x_y.x_value;
    nz = x->x_lorenz->x_z.x_value;
  }
  norm_setval(&x->x_dist, q8_sqrt((nx*nx)+(ny*ny)+(nz*nz)));
  return norm_getnorm(&x->x_dist);
}

static t_int *lorsig_perform(t_int *w)
{
  t_lorsig *x = (t_lorsig *)w[1];
  t_sample *out = (t_sample *)w[2];
  int n = (int)w[3];
  while (n--)
    *out++ = lorsig_lorenz(x) * 2 - 0.5;
  return w + 4;
}

static void lorsig_dsp(t_lorsig *x, t_signal **sp)
{ dsp_add(lorsig_perform, 3, x, sp[1]->s_vec, sp[0]->s_n); }

static void lorsig_inita(t_lorsig *x, t_floatarg f)
{ lorenz_inita(x->x_lorenz, f); }

static void lorsig_initb(t_lorsig *x, t_floatarg f)
{ lorenz_initb(x->x_lorenz, f); }

static void lorsig_initc(t_lorsig *x, t_floatarg f)
{ lorenz_initc(x->x_lorenz, f); }

static void lorsig_inith(t_lorsig *x, t_floatarg f)
{ lorenz_inith(x->x_lorenz, f); }

static void lorsig_initx(t_lorsig *x, t_floatarg f)
{ lorenz_initx(x->x_lorenz, f); }

static void lorsig_inity(t_lorsig *x, t_floatarg f)
{ lorenz_inity(x->x_lorenz, f); }

static void lorsig_initz(t_lorsig *x, t_floatarg f)
{ lorenz_initz(x->x_lorenz, f); }

static void lorsig_type(t_lorsig *x, t_floatarg f)
{ lorenz_type(x->x_lorenz, f); }

static void lorsig_reset(t_lorsig *x)
{ lorenz_reset(x->x_lorenz); }

static void lorsig_print_types(t_lorsig *x)
{ lorenz_print_types(x->x_outlet1); }

static void lorsig_normalize(t_lorsig *x, t_floatarg f)
{ lorenz_normalize(x->x_lorenz, f); }

static void *lorsig_new()
{
  t_lorsig *x = (t_lorsig *)pd_new(lorsig_class);
    /* default to normalized values */
  x->x_lorenz = lorenz_new(1.0);
  outlet_new(&x->x_ob, gensym("signal"));
  x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
  x->x_f = 0;
  x->x_lorenz->x_type = 0;
  norm_setval(&x->x_dist, 0.0);
  lorenz_reset(x->x_lorenz);
  return (void *)x;
}

/* ------------------------ [lor~] ----------------------------- */

static t_class *lor_class;

typedef struct _lor
{
  t_object  x_ob;
  t_lorenz *x_lorenz;
  t_float   x_f;
  t_inlet  *in2, *in3;
  t_outlet *out1, *out2, *out3;
} t_lor;

static t_float lor_mix(t_float x1, t_float x2)
{ return x1 + x2 * 0.001; }

static t_int *lor_perform(t_int *w)
{
  t_lor *x = (t_lor *)(w[1]);
  t_lorenz *x_l = x->x_lorenz;
  t_sample *in1 = (t_sample *)(w[2]);
  t_sample *in2 = (t_sample *)(w[3]);
  t_sample *in3 = (t_sample *)(w[4]);
  t_sample *out1 = (t_sample *)(w[5]);
  t_sample *out2 = (t_sample *)(w[6]);
  t_sample *out3 = (t_sample *)(w[7]);
  int n = (int)(w[8]);

  while (n--)
  {
    norm_setval(&x_l->x_x, lor_mix(x_l->x_x.x_value, *in1++));
    norm_setval(&x_l->x_y, lor_mix(x_l->x_y.x_value, *in2++));
    norm_setval(&x_l->x_z, lor_mix(x_l->x_z.x_value, *in3++));
    lorenz_doit(x_l);
    *out1++ = norm_getnorm(&x_l->x_x) * 2 - 0.5;
    *out2++ = norm_getnorm(&x_l->x_y) * 2 - 0.5;
    *out3++ = norm_getnorm(&x_l->x_z) * 2 - 0.5;
  }
  return w + 9;
}

static void lor_tilde_free(t_lor *x)
{
  inlet_free(x->in2);
  inlet_free(x->in3);
  outlet_free(x->out1);
  outlet_free(x->out2);
  outlet_free(x->out3);
}

static void lor_dsp(t_lor *x, t_signal **sp)
{
  dsp_add(lor_perform, 8,  x,
    sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,
    sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
    (int)sp[0]->s_n
  );
}
static void lor_inita(t_lor *x, t_floatarg f)
{ lorenz_inita(x->x_lorenz, f); }

static void lor_initb(t_lor *x, t_floatarg f)
{ lorenz_initb(x->x_lorenz, f); }

static void lor_initc(t_lor *x, t_floatarg f)
{ lorenz_initc(x->x_lorenz, f); }

static void lor_inith(t_lor *x, t_floatarg f)
{ lorenz_inith(x->x_lorenz, f); }

static void lor_initx(t_lor *x, t_floatarg f)
{ lorenz_initx(x->x_lorenz, f); }

static void lor_inity(t_lor *x, t_floatarg f)
{ lorenz_inity(x->x_lorenz, f); }

static void lor_initz(t_lor *x, t_floatarg f)
{ lorenz_initz(x->x_lorenz, f); }

static void lor_type(t_lor *x, t_floatarg f)
{ lorenz_type(x->x_lorenz, f); }

static void lor_reset(t_lor *x)
{ lorenz_reset(x->x_lorenz); }

static void lor_normalize(t_lor *x, t_floatarg f)
{ lorenz_normalize(x->x_lorenz, f); }

static void *lor_new(void)
{
  t_lor *x = (t_lor *)pd_new(lor_class);
    /* default to normalized values */
  x->x_lorenz = lorenz_new(1.0);
  t_symbol *ss = gensym("signal");
  x->in2 = inlet_new(&x->x_ob, &x->x_ob.ob_pd, ss, ss);
  x->in3 = inlet_new(&x->x_ob, &x->x_ob.ob_pd, ss, ss);
  x->out1 = outlet_new(&x->x_ob, ss);
  x->out2 = outlet_new(&x->x_ob, ss);
  x->out3 = outlet_new(&x->x_ob, ss);
  x->x_f = 0.0;
  x->x_lorenz->x_type = 0;
  lorenz_reset(x->x_lorenz);
  return (x);
}

/* ------------------------------- global setup ----------------------------- */

void lorenz_global_setup(void)
{
  lorenz_class = class_new(gensym("lorenz_base"), 0, 0,
                           sizeof(t_lorenz), CLASS_PD, A_DEFFLOAT, A_NULL);

  lorenz_obj_class = class_new(gensym("lorenz"), (t_newmethod)lorenz_obj_new, 0,
                           sizeof(t_lorenz_obj), CLASS_DEFAULT, A_DEFFLOAT, A_NULL);

  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_inita,
                  gensym("inita"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_initb,
                  gensym("initb"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_initc,
                  gensym("initc"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_inith,
                  gensym("inith"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_initx,
                  gensym("initx"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_inity,
                  gensym("inity"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_initz,
                  gensym("initz"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_type,
                  gensym("type"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_normalize,
                  gensym("normalize"), A_FLOAT, A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_reset,
                  gensym("reset"),A_NULL);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_print_types,
                  gensym("print-types"),A_NULL);

  class_addbang(lorenz_obj_class, lorenz_obj_bang);
  class_addmethod(lorenz_obj_class, (t_method)lorenz_obj_print,
                  gensym("print"), A_FLOAT, A_NULL);

  lorsig_class = class_new(gensym("lorsig~"), (t_newmethod)lorsig_new, 0,
                           sizeof(t_lorsig),CLASS_PATCHABLE, A_NULL);

  CLASS_MAINSIGNALIN(lorsig_class, t_lorsig, x_f);
  class_addmethod(lorsig_class, (t_method)lorsig_dsp,
                  gensym("dsp"), A_NULL);

  class_addmethod(lorsig_class, (t_method)lorsig_inita,
                  gensym("inita"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_initb,
                  gensym("initb"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_initc,
                  gensym("initc"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_inith,
                  gensym("inith"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_initx,
                  gensym("initx"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_inity,
                  gensym("inity"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_initz,
                  gensym("initz"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_type,
                  gensym("type"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_normalize,
                  gensym("normalize"), A_FLOAT, A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_reset,
                  gensym("reset"),A_NULL);
  class_addmethod(lorsig_class, (t_method)lorsig_print_types,
                  gensym("print-types"),A_NULL);

  lor_class = class_new(gensym("lor~"), (t_newmethod)lor_new,
                        (t_method)lor_tilde_free, sizeof(t_lor),
                        0, A_NULL);

  CLASS_MAINSIGNALIN(lor_class, t_lor, x_f);
  class_addmethod(lor_class, (t_method)lor_dsp,
                  gensym("dsp"), A_NULL);

  class_addmethod(lor_class, (t_method)lor_inita,
                  gensym("inita"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_initb,
                  gensym("initb"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_initc,
                  gensym("initc"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_inith,
                  gensym("inith"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_initx,
                  gensym("initx"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_inity,
                  gensym("inity"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_initz,
                  gensym("initz"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_type,
                  gensym("type"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_normalize,
                  gensym("normalize"), A_FLOAT, A_NULL);
  class_addmethod(lor_class, (t_method)lor_reset,
                  gensym("reset"),A_NULL);
}
