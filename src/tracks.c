/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


This is [tracks] from Jaime Oliver 

  Changes by Fede Camara Halac:
  
  Changed dimensions to a switch statement;
  Changed cbrt to sqrt in 3 dimension distance check;
  Changed elements to double;
  Elements 1 and 2 used to be int, they are now double;
  Added a TRACKSMAX definition for maximum number of points/frames;
  Added a 'bang' method, to callback tracks_spit;
  Added a 'list' method, to callback tracks_input;
  max_dist and dimen methods are now A_FLOAT instead of A_GIMME;
  'clear' method now also clears all the elements;
  Improved some printing and added a 'print' method for debugging;
  Removed some comments and reindented;
  
  TODO: elements should really be structures, with proper memory management;
*/

#include "fdLib.h"

static t_class *tracks_class;

typedef struct tracks
{
  t_object x_ob;
  t_int x_size;
  t_int x1_i[TRACKSMAX], x1_s[TRACKSMAX];
  t_int x2_i[TRACKSMAX], x2_s[TRACKSMAX];
  double x1_elem01[TRACKSMAX], x1_elem02[TRACKSMAX], x1_elem03[TRACKSMAX], x1_elem04[TRACKSMAX], x1_elem05[TRACKSMAX], x1_elem06[TRACKSMAX], x1_elem07[TRACKSMAX], x1_elem08[TRACKSMAX], x1_elem09[TRACKSMAX], x1_elem10[TRACKSMAX], x1_elem11[TRACKSMAX], x1_elem12[TRACKSMAX], x1_elem13[TRACKSMAX], x1_elem14[TRACKSMAX], x1_elem15[TRACKSMAX], x1_elem16[TRACKSMAX];
  double x2_elem01[TRACKSMAX], x2_elem02[TRACKSMAX], x2_elem03[TRACKSMAX], x2_elem04[TRACKSMAX], x2_elem05[TRACKSMAX], x2_elem06[TRACKSMAX], x2_elem07[TRACKSMAX], x2_elem08[TRACKSMAX], x2_elem09[TRACKSMAX], x2_elem10[TRACKSMAX], x2_elem11[TRACKSMAX], x2_elem12[TRACKSMAX], x2_elem13[TRACKSMAX], x2_elem14[TRACKSMAX], x2_elem15[TRACKSMAX], x2_elem16[TRACKSMAX];
  t_float i_reg1[TRACKSMAX], i_reg2[TRACKSMAX];
  t_float w[10], w_sum, max_dist;
  t_int prev_pts, pts, report, dimen;
  t_outlet *out01, *out02, *out03;
} t_tracks;

static void tracks_weights(t_tracks *x, t_symbol *s, int argc, t_atom *argv)
{
  int i;
  for (i = 0; i < argc; i++)
  {
    if (argv[i].a_type == A_FLOAT)
    {
      if (i == 0) x->w[i] = atom_getfloatarg(i,argc,argv);
      else if (i == 1) x->w[i] = atom_getfloatarg(i,argc,argv);
      else if (i == 2) x->w[i] = atom_getfloatarg(i,argc,argv);
    }
  }
  x->w_sum = 0;
  for (i = 0; i < x->dimen; i++)
  {
    x->w_sum += x->w[i];
  }
  post("[tracks] dimension = %d - total weight = %d",x->dimen,x->w_sum);

  for (i = 0; i < x->dimen; i++)
  {
    post("[tracks] w%d = %d (%.2f)", i+1, x->w[i], x->w[i]/x->w_sum);
  }
}


static void tracks_input(t_tracks *x, t_symbol *s, int argc, t_atom *argv)
{
  int i;
  for (i = 0; i < argc; i++)
  {
    if (argv[i].a_type == A_FLOAT)
    {
      if (i == 0) x->x2_i[x->pts] = (int) argv[i].a_w.w_float;
      else if (i == 1) x->x2_elem01[x->pts] = argv[i].a_w.w_float;
      else if (i == 2) x->x2_elem02[x->pts] = argv[i].a_w.w_float;
      else if (i == 3) x->x2_elem03[x->pts] = argv[i].a_w.w_float;
      else if (i == 4) x->x2_elem04[x->pts] = argv[i].a_w.w_float;
      else if (i == 5) x->x2_elem05[x->pts] = argv[i].a_w.w_float;
      else if (i == 6) x->x2_elem06[x->pts] = argv[i].a_w.w_float;
      else if (i == 7) x->x2_elem07[x->pts] = argv[i].a_w.w_float;
      else if (i == 8) x->x2_elem08[x->pts] = argv[i].a_w.w_float;
      else if (i == 9) x->x2_elem09[x->pts] = argv[i].a_w.w_float;
      else if (i == 10) x->x2_elem10[x->pts] = argv[i].a_w.w_float;
      else if (i == 11) x->x2_elem11[x->pts] = argv[i].a_w.w_float;
      else if (i == 12) x->x2_elem12[x->pts] = argv[i].a_w.w_float;
      else if (i == 13) x->x2_elem13[x->pts] = argv[i].a_w.w_float;
      else if (i == 14) x->x2_elem14[x->pts] = argv[i].a_w.w_float;
      else if (i == 15) x->x2_elem15[x->pts] = argv[i].a_w.w_float;
      else if (i == 16) x->x2_elem16[x->pts] = argv[i].a_w.w_float;
      x->x2_s[x->pts] = 1;
    }
  }
  //  post("pts = %d", x->pts);
  x->pts++; 
}


static void tracks_clear(t_tracks *x)
{
  int z, j, i;
  t_atom at[3];  
  z = 0;
  for (j = 0; j < x->prev_pts; j++)
  {// for every old point
    if (x->x1_s[j] == 1)
    {
      x->i_reg1[z] = -2;
      x->i_reg2[z] = x->x1_i[j];
      z++;
    }
    if (z == x->prev_pts) 
      break;
  }
  for (i = 0; i < z; i++)
  {
    SETFLOAT(at,  x->i_reg1[i]);
    SETFLOAT(at+1,  x->i_reg2[i]);
    outlet_list(x->out03, 0, 2, at);
  }
  x->prev_pts = x->pts = 0;
  for (i = 0; i < TRACKSMAX; i++)
  {
    x->x1_elem03[i]=0;
    x->x1_elem04[i]=0;
    x->x1_elem05[i]=0;
    x->x1_elem06[i]=0;
    x->x1_elem07[i]=0;
    x->x1_elem08[i]=0;
    x->x1_elem09[i]=0;
    x->x1_elem10[i]=0;
    x->x1_elem11[i]=0;
    x->x1_elem12[i]=0;
    x->x1_elem13[i]=0;
    x->x1_elem14[i]=0;
    x->x1_elem15[i]=0;
    x->x1_elem16[i]=0;
    x->x2_elem03[i]=0;
    x->x2_elem04[i]=0;
    x->x2_elem05[i]=0;
    x->x2_elem06[i]=0;
    x->x2_elem07[i]=0;
    x->x2_elem08[i]=0;
    x->x2_elem09[i]=0;
    x->x2_elem10[i]=0;
    x->x2_elem11[i]=0;
    x->x2_elem12[i]=0;
    x->x2_elem13[i]=0;
    x->x2_elem14[i]=0;
    x->x2_elem15[i]=0;
    x->x2_elem16[i]=0;
  }
}


static void tracks_dimen(t_tracks *x, t_floatarg f)
{
  int i = 0;
  x->dimen = (int) f;
  post ("[tracks] dimension = %d", x->dimen);
  x->w_sum = 0;
  for (i = 0; i < x->dimen; i++)
  {
    x->w_sum += x->w[i];
  }
}


static void tracks_max_distance(t_tracks *x, t_floatarg f)
{
  x->max_dist = f;
  post ("[tracks] maximum distance = %.2f", x->max_dist);
}


static void tracks_spit(t_tracks *x)
{
  int i, j, z, new_pts, j_tmp, i_tmp, assigned;
  int free_i[TRACKSMAX];
  t_atom at[18];
  double dist, dist_temp;
  j_tmp = i_tmp = assigned = z = 0;
  new_pts = x->pts - x->prev_pts;
  for (i = 0; i < TRACKSMAX; i++)
  {
    free_i[i] = 1;
  }
  if (new_pts >= 0)
  {
  // if there are more or equal new pts than previous pts //  post (">=");
    for (i = 0; i < x->prev_pts; i++)
    {
    // for each previous pt (if there are 0 previous all are new...)
      dist_temp = 10000000; // (just a big number...)
      j_tmp = -1;
      for (j = 0; j < x->pts; j++)
      {
        if (x->x2_s[j] == 1)
        {
          switch (x->dimen) {
          case 1:
            dist = fabs(x->x1_elem01[i] - x->x2_elem01[j]);
            break;
          case 2:
            dist = sqrt(( (x->x1_elem01[i] - x->x2_elem01[j]) * (x->x1_elem01[i] - x->x2_elem01[j]) ) * x->w[0] + ( (x->x1_elem02[i] - x->x2_elem02[j]) * (x->x1_elem02[i] - x->x2_elem02[j]) ) * x->w[1]);
            break;
          case 3:
            dist = sqrt(((x->x1_elem01[i] - x->x2_elem01[j])*(x->x1_elem01[i] - x->x2_elem01[j]))*x->w[0] + ((x->x1_elem02[i] - x->x2_elem02[j])*(x->x1_elem02[i] - x->x2_elem02[j]))*x->w[1] + ((x->x1_elem03[i] - x->x2_elem03[j])*(x->x1_elem03[i] - x->x2_elem03[j]))*x->w[2]);
            break;
          default:
            post("wrong dimension, only 1, 2 and 3");
          }
          if (dist < dist_temp && dist < x->max_dist)
          {
            dist_temp = dist;
            j_tmp = j; //post("dist = %f", dist);
          }
        }
        else continue;
      } // found the closest new one.
      if (j_tmp >= 0)
      {
      // that is, if we found an index != -1
        x->x2_s[j_tmp] = 0; // set the state of the new tip to 0
        x->x1_s[i] = 0; // set the state of the old tip to 0
        x->i_reg1[z] = x->x2_i[j_tmp];
        x->x2_i[j_tmp] = x->x1_i[i];// assign the index of previous tip
        x->i_reg2[z] = x->x2_i[j_tmp];
        free_i[x->x2_i[j_tmp]] = 0;
        assigned++;
        z++;// post("assigned %d", x->x1_i[i] );
      }
      if (assigned == x->pts)
        break;
    }
    if (assigned < x->pts)
    { // the old points left behind
      for (j = 0; j < x->prev_pts; j++)
      { // for every old point
        if (x->x1_s[j] == 1)
        {
          x->x1_s[j] = 0;
          x->i_reg1[z] = -2;
          x->i_reg2[z] = x->x1_i[j];
          free_i[x->x1_i[j]] = 0;
          z++;
        }
        if (assigned == x->prev_pts)
          break;
      }
    }
    while (assigned != x->pts)
    {
      for (i = 0; i < x->pts; i++)
      {
        if (x->x2_s[i] == 1)
        {
          for (j = 0; j < TRACKSMAX; j++)
          {
            if (free_i[j] == 1)
            {
              x->x2_s[i] = 0;
              free_i[j] = 0;
              x->x2_i[i] = j;
              x->i_reg1[z] = -1;
              x->i_reg2[z] = x->x2_i[i]; //post("assign extra, index = %d", j);
              assigned++;
              z++;
              break;
            }
          }
        }
      }
    }
  }
  else if (new_pts < 0)
  {
  // if there are less new pts than previous pts //  post ("<");
    for (i = 0; i < x->pts; i++)
    { // for each new pt
      dist_temp = 10000000;
      j_tmp = -1;
      for (j = 0; j < x->prev_pts; j++)
      {
        if (x->x1_s[j] == 1)
        {
          dist = sqrt(((x->x1_elem01[j] - x->x2_elem01[i]) * (x->x1_elem01[j] - x->x2_elem01[i])) + 
              ((x->x1_elem02[j] - x->x2_elem02[i])*(x->x1_elem02[j] - x->x2_elem02[i])));
          if (dist < dist_temp && dist < x->max_dist) {
            dist_temp = dist;
            j_tmp = j; // post("dist = %f", dist);
          }
        }
        else continue;
      }// found the closest old one.
      if (j_tmp >= 0)
      {
        x->x1_s[j_tmp] = 0; // set the state of the new tip to 0
        x->i_reg1[z] = x->x2_i[i];
        x->x2_i[i] = x->x1_i[j_tmp]; // assign the index of previous tip
        x->i_reg2[z] = x->x2_i[i];
        assigned++; 
        z++; // post("assigned %d", x->x1_i[j_tmp] );
      }
      if (assigned == x->pts)
        break;
    } 
    for (j = 0; j < x->prev_pts; j++)
    {
      // for every old point
      if (x->x1_s[j] == 1)
      {
        x->i_reg1[z] = -2;
        x->i_reg2[z] = x->x1_i[j];
        z++;
      }
      if (z == x->prev_pts)
        break;
    }
  }  
  for (i = 0; i < z; i++)
  {
    SETFLOAT(at,  x->i_reg1[i]);
    SETFLOAT(at+1,  x->i_reg2[i]);
    outlet_list(x->out02, 0, 2, at);
  }
  for (i = 0; i < x->pts; i++)
  {
    SETFLOAT(at, x->x2_i[i]);
    SETFLOAT(at+1, x->x2_elem01[i]);
    SETFLOAT(at+2, x->x2_elem02[i]);
    SETFLOAT(at+3, x->x2_elem03[i]);
    SETFLOAT(at+4, x->x2_elem04[i]);
    SETFLOAT(at+5, x->x2_elem05[i]);
    SETFLOAT(at+6, x->x2_elem06[i]);
    SETFLOAT(at+7, x->x2_elem07[i]);
    SETFLOAT(at+8, x->x2_elem08[i]);
    SETFLOAT(at+9, x->x2_elem09[i]);
    SETFLOAT(at+10, x->x2_elem10[i]);
    SETFLOAT(at+11, x->x2_elem11[i]);
    SETFLOAT(at+12, x->x2_elem12[i]);
    SETFLOAT(at+13, x->x2_elem13[i]);
    SETFLOAT(at+14, x->x2_elem14[i]);
    SETFLOAT(at+15, x->x2_elem15[i]);
    SETFLOAT(at+16, x->x2_elem16[i]);
    outlet_list(x->out01, 0, 17, at);
  }
  for (i = 0; i < x->pts; i++)
  {
    x->x1_i[i]  = x->x2_i[i]; 
    x->x1_elem01[i] = x->x2_elem01[i];
    x->x1_elem02[i] = x->x2_elem02[i];
    x->x1_elem03[i] = x->x2_elem03[i];
    x->x1_elem04[i] = x->x2_elem04[i];
    x->x1_elem05[i] = x->x2_elem05[i];
    x->x1_elem06[i] = x->x2_elem06[i];
    x->x1_elem07[i] = x->x2_elem07[i];
    x->x1_elem08[i] = x->x2_elem08[i];
    x->x1_elem09[i] = x->x2_elem09[i];
    x->x1_elem10[i] = x->x2_elem10[i];
    x->x1_elem11[i] = x->x2_elem11[i];
    x->x1_elem12[i] = x->x2_elem12[i];
    x->x1_elem13[i] = x->x2_elem13[i];
    x->x1_elem14[i] = x->x2_elem14[i];
    x->x1_elem15[i] = x->x2_elem15[i];
    x->x1_elem16[i] = x->x2_elem16[i];
    x->x1_s[i]  = 1;
  }
  x->prev_pts = x->pts;
  x->pts = 0;
  outlet_float(x->out03, 1);
}


static void tracks_print(t_tracks *x)
{
  post("max_dist = %.2f", x->max_dist);
  post("w_sum = %d", x->w_sum);
  post("prev_pts = %d", x->prev_pts);
  post("pts = %d", x->pts);
  post("report = %d", x->report);
  post("dimen = %d", x->dimen);
}


static void *tracks_new(t_floatarg f)
{
  int i = 0;
  t_tracks *x = (t_tracks *)pd_new(tracks_class);
  x->x_size=f>0?f:TRACKSMAX;
  x->prev_pts = x->pts = 0;
  x->max_dist = 30.;
  x->dimen = 1;

  for (i = 0; i < 10; i++)
  {
    x->w[i] = 1;
  }

  x->w_sum = 0;
  for (i = 0; i < x->dimen; i++)
  {
    x->w_sum += x->w[i];
  }

  x->out01 = outlet_new(&x->x_ob, &s_list);
  x->out02 = outlet_new(&x->x_ob, &s_list);
  x->out03 = outlet_new(&x->x_ob, &s_list);

  // post("[tracks]");
  // post("Jaime Oliver, 2010 - www.jaimeoliver.pe");
  // post("Fede Camara Halac, 2019 - fdch.github.io");
  // post("dimen = %d\nmax_dist = %.2f", x->dimen, x->max_dist);
  return (void *)x;
}

void tracks_setup(void)
{
  tracks_class = class_new(
    gensym("tracks"), 
    (t_newmethod)tracks_new,
    0,
    sizeof(t_tracks),
    0,
    A_DEFFLOAT,
    A_NULL
  );

  class_addbang(tracks_class, tracks_spit);
  class_addlist(tracks_class, tracks_input);

  class_addmethod(
    tracks_class,
    (t_method)tracks_input,
    gensym("input"),
    A_GIMME,
    A_NULL
  );
  class_addmethod(
    tracks_class,
    (t_method)tracks_dimen,
    gensym("dimen"),
    A_FLOAT,
    A_NULL
  );
  class_addmethod(
    tracks_class,
    (t_method)tracks_max_distance,
    gensym("max_distance"),
    A_FLOAT,
    A_NULL
  );
  class_addmethod(
    tracks_class,
    (t_method)tracks_weights,
    gensym("weights"),
    A_GIMME,
    A_NULL
  );
  class_addmethod(
    tracks_class,
    (t_method)tracks_spit,
    gensym("spit"),
    A_NULL
  );
  class_addmethod(
    tracks_class,
    (t_method)tracks_clear,
     gensym("clear"),
     A_NULL
  );
  class_addmethod(
    tracks_class,
    (t_method)tracks_print,
     gensym("print"),
     A_NULL
  );
}