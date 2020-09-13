/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

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
  Added a TMAX definition for maximum number of points/frames;
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

typedef struct tracks {
  t_object x_ob;
  int x1_i[TMAX], x1_s[TMAX], x2_i[TMAX], x2_s[TMAX], c[TINS];
  double x1_elem[TINS][TMAX], x2_elem[TINS][TMAX], w[TINS], max_dist;
  float i_reg1[TMAX], i_reg2[TMAX];
  int w_sum, prev_pts, pts, dimen, elnum, debug;
  t_outlet *out01, *out02, *out03;
} t_tracks;

static double tracks_getdist(t_tracks *x, int i, int j) {
  int d = 0;
  double sum = 0.0;
  for (d = 0; d < x->dimen; d++)
    sum += (x->x1_elem[x->c[d]][i] - x->x2_elem[x->c[d]][j]) *
           (x->x1_elem[x->c[d]][i] - x->x2_elem[x->c[d]][j]) *
           x->w[x->c[d]];
  return sqrt(sum);
}

static void tracks_choose_dimen(t_tracks *x, t_symbol *s, int argc, t_atom *argv) {
  int i, d=x->dimen,dim;
  if(argc > d) {
    if (x->debug) {
      startpost("Ignoring extra dimensions:");
      postatom(argc-d, argv+d);
      endpost();
    }
    dim = d;
  } else
      dim = argc;
  for (i = 0; i < dim; i++) {
    if (argv[i].a_type == A_FLOAT) {
      x->c[i] = argv[i].a_w.w_float;
    }
  }
  if (x->debug) {
    t_atom at[TINS];
    startpost("Choosing Dimensions:");
    for(i = 0; i < dim; i++) SETFLOAT(at+i,x->c[i]);
    postatom(i,at);
    endpost();
  }
}

static void tracks_weights(t_tracks *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  x->w_sum = 0;
  if (x->dimen != argc) //adjust to new dimensions
    x->dimen = argc;
  for (i = 0; i < argc; i++)
    if (argv[i].a_type == A_FLOAT) {
      x->w[i] = atom_getfloatarg(i,argc,argv);
      x->w_sum += x->w[i];
    }
  for (i = 0; i < argc; i++) //normalize weights
    x->w[i] = x->w[i]/x->w_sum;
  if (x->debug) {
    t_atom at[TINS];
    startpost("Weights:");
    for(i = 0; i < argc; i++) SETFLOAT(at+i,x->w[i]);
    postatom(i,at);
    endpost();
  }
}

static void tracks_dimen(t_tracks *x, t_floatarg f) {
  if (f > TINS) x->dimen = TINS;
  else x->dimen = (int) f;
  if (x->debug) post ("Dimensions: %d", x->dimen);
}

static void tracks_max_distance(t_tracks *x, t_floatarg f) {
  x->max_dist = f;
  if (x->debug) post ("Maximum Distance: %.2f", x->max_dist);
}

static void tracks_input(t_tracks *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  float val;
  if(argc-1 > TINS) {
    if (x->debug) {
      startpost("Ignoring extra elements:");
      postatom(argc-1-TINS, argv+1+TINS);
      endpost();
    }
    x->elnum = TINS;
  } else 
    x->elnum = argc-1;
  for (i = 0; i < x->elnum+1; i++) {
    if (argv[i].a_type == A_FLOAT) {
      val = argv[i].a_w.w_float;
      if (i == 0)
        x->x2_i[x->pts] = (int) val;
      if (i >= 1)
        x->x2_elem[i-1][x->pts] = val;
      x->x2_s[x->pts] = 1;
    }
  }
  x->pts++;
}

static void tracks_spit(t_tracks *x) {
  int i, j, z, new_pts, j_tmp, i_tmp, assigned;
  int free_i[TMAX];
  t_atom at[TINS];
  double dist, dist_temp;
  j_tmp = i_tmp = assigned = z = 0;
  new_pts = x->pts - x->prev_pts;
  
  for (i = 0; i < TMAX; i++)
    free_i[i] = 1;

  if (new_pts >= 0) {
  // if there are more or equal new pts than previous pts
  if (x->debug) post (">=");
    for (i = 0; i < x->prev_pts; i++) {
    // for each previous pt (if there are 0 previous all are new...)
      dist_temp = 10000000; // (just a big number...)
      j_tmp = -1;
      for (j = 0; j < x->pts; j++) {
        if (x->x2_s[j] == 1) {
          if ((dist=tracks_getdist(x,i,j)) < dist_temp && dist < x->max_dist) {
            dist_temp = dist;
            j_tmp = j;
            if (x->debug) post("dist = %f", dist);
          }
        }
        else continue;
      } // found the closest new one.
      if (j_tmp >= 0) {
      // that is, if we found an index != -1
        x->x2_s[j_tmp] = 0; // set the state of the new tip to 0
        x->x1_s[i] = 0; // set the state of the old tip to 0
        x->i_reg1[z] = x->x2_i[j_tmp];
        x->x2_i[j_tmp] = x->x1_i[i];// assign the index of previous tip
        x->i_reg2[z] = x->x2_i[j_tmp];
        free_i[x->x2_i[j_tmp]] = 0;
        assigned++;
        z++;
        if (x->debug) post("assigned %d", x->x1_i[i] );
      }
      if (assigned == x->pts) break;
    }
    if (assigned < x->pts) { // the old points left behind
      for (j = 0; j < x->prev_pts; j++) { // for every old point
        if (x->x1_s[j] == 1) {
          x->x1_s[j] = 0;
          x->i_reg1[z] = -2;
          x->i_reg2[z] = x->x1_i[j];
          free_i[x->x1_i[j]] = 0;
          z++;
        }
        if (assigned == x->prev_pts) break;
      }
    }
    while (assigned != x->pts)
      for (i = 0; i < x->pts; i++)
        if (x->x2_s[i] == 1)
          for (j = 0; j < TMAX; j++)
            if (free_i[j] == 1) {
              x->x2_s[i] = 0;
              free_i[j] = 0;
              x->x2_i[i] = j;
              x->i_reg1[z] = -1;
              x->i_reg2[z] = x->x2_i[i];
              if (x->debug) post("assign extra, index = %d", j);
              assigned++;
              z++;
              break;
            }
  }
  else if (new_pts < 0) {
  // if there are less new pts than previous pts 
  if (x->debug) post ("<");
    for (i = 0; i < x->pts; i++) { // for each new pt
      dist_temp = 10000000;
      j_tmp = -1;
      for (j = 0; j < x->prev_pts; j++) {
        if (x->x1_s[j] == 1) {
          if ((dist=tracks_getdist(x,j,i)) < dist_temp && dist < x->max_dist) {
            dist_temp  = dist;
            j_tmp = j;
            if (x->debug) post("dist = %f", dist);
          }
        }
        else continue;
      }// found the closest old one.
      if (j_tmp >= 0) {
        x->x1_s[j_tmp] = 0; // set the state of the new tip to 0
        x->i_reg1[z] = x->x2_i[i];
        x->x2_i[i] = x->x1_i[j_tmp]; // assign the index of previous tip
        x->i_reg2[z] = x->x2_i[i];
        assigned++; 
        z++;
        if (x->debug) post("assigned %d", x->x1_i[j_tmp] );
      }
      if (assigned == x->pts) break;
    } 
    for (j = 0; j < x->prev_pts; j++) {
      // for every old point
      if (x->x1_s[j] == 1) {
        x->i_reg1[z] = -2;
        x->i_reg2[z] = x->x1_i[j];
        z++;
      }
      if (z == x->prev_pts) break;
    }
  }  
  for (i = 0; i < z; i++) {
    SETFLOAT(at,  x->i_reg1[i]);
    SETFLOAT(at+1,  x->i_reg2[i]);
    outlet_list(x->out02, 0, 2, at);
  }
  for (i = 0; i < x->pts; i++) {
    SETFLOAT(at, x->x2_i[i]);
    for (j = 0; j < x->elnum; j++)
      SETFLOAT(at+j+1, x->x2_elem[j][i]);
    outlet_list(x->out01, 0, x->elnum+1, at);
  }
  for (i = 0; i < x->pts; i++) {
    x->x1_i[i]  = x->x2_i[i];
    for (j = 0; j < x->elnum; j++)
      x->x1_elem[j][i] = x->x2_elem[j][i];
    x->x1_s[i] = 1; 
  }
  x->prev_pts = x->pts;
  x->pts = 0;
  outlet_float(x->out03, 1);
}

static void tracks_clear(t_tracks *x) {
  int z, j, i;
  t_atom at[3];  
  z = 0;
  for (j = 0; j < x->prev_pts; j++) {
  // for every old point
    if (x->x1_s[j] == 1) {
      x->i_reg1[z] = -2;
      x->i_reg2[z] = x->x1_i[j];
      z++;
    }
    if (z == x->prev_pts) break;
  }
  for (i = 0; i < z; i++) {
    SETFLOAT(at,  x->i_reg1[i]);
    SETFLOAT(at+1,  x->i_reg2[i]);
    outlet_list(x->out03, 0, 2, at);
  }
  x->prev_pts = x->pts = 0;
  for (i = 0; i < TINS; i++)
    for (j = 0; j < TMAX; j++)
      x->x1_elem[i][j]=0;
  if (x->debug) post("All instances cleared");
}

static void tracks_print(t_tracks *x) {
  int i,j;
  t_atom at[TINS];
  if (x->debug) {
    post("Max Elements: %d\n\
Max Frames: %d\nMax Distance: %.2f\nDimensions: %d\nPrevious Frames: %d\
", TINS, TMAX, x->max_dist, x->dimen, x->prev_pts);
    startpost("Weights:");
    for(i = 0; i < x->dimen; i++)
      SETFLOAT(at+i,x->w[i]);
    postatom(i,at);
    endpost();
    startpost("Choosing Dimensions:");
    for(i = 0; i < x->dimen; i++)
      SETFLOAT(at+i,x->c[i]);
    postatom(i,at);
    endpost();
  } else {
    x->debug = 1;
    tracks_print(x);
    x->debug = 0;
  }
  if (x->pts || x->elnum) {
    for(i = 0; i < x->pts; i++ ) {
      for (j = 0; j < x->elnum; j++)
        SETFLOAT(at+j , (t_float)x->x2_elem[j][i]);
      startpost("STORED %d ELEMENTS IN FRAME %d:\n-->", j, i);
      postatom(j, at);
      endpost();
    }
  }
}

static void tracks_debug(t_tracks *x, t_floatarg f) {
  x->debug = (int) f;
  post("Debug Mode %s",x->debug==1?"ON":"OFF");
}

static void *tracks_new(void) {
  int i = 0;
  t_tracks *x = (t_tracks *)pd_new(tracks_class);
  x->prev_pts = x->pts = 0;
  x->max_dist = 30.;
  x->dimen = 2;
  x->debug = 0;
  for (i = 0; i < TINS; i++) {
    x->w[i] = 1;
    x->c[i] = i;
  }
  x->w_sum = 0;
  for (i = 0; i < x->dimen; i++)
    x->w_sum += x->w[i];
  x->out01 = outlet_new(&x->x_ob, &s_list);
  x->out02 = outlet_new(&x->x_ob, &s_list);
  x->out03 = outlet_new(&x->x_ob, &s_list);
  post("[tracks] version by Fede Camara Halac of Jaime Oliver's [tracks]");
  return (void *)x;
}

void tracks_setup(void) {
  tracks_class = class_new(gensym("tracks"), (t_newmethod)tracks_new, 0, sizeof(t_tracks), 0, A_GIMME, 0);
  class_addbang(tracks_class, tracks_spit);
  class_addlist(tracks_class, tracks_input);
  class_addmethod(tracks_class, (t_method)tracks_input, gensym("input"), A_GIMME, 0);
  class_addmethod(tracks_class, (t_method)tracks_dimen, gensym("dimen"), A_FLOAT, 0);
  class_addmethod(tracks_class, (t_method)tracks_debug, gensym("debug"), A_FLOAT, 0);
  class_addmethod(tracks_class, (t_method)tracks_max_distance, gensym("max_distance"), A_FLOAT, 0);
  class_addmethod(tracks_class, (t_method)tracks_weights, gensym("weights"), A_GIMME, 0);
  class_addmethod(tracks_class, (t_method)tracks_choose_dimen, gensym("choose_dimen"), A_GIMME, 0);
  class_addmethod(tracks_class, (t_method)tracks_spit, gensym("spit"), 0);
  class_addmethod(tracks_class, (t_method)tracks_clear, gensym("clear"), 0);
  class_addmethod(tracks_class, (t_method)tracks_print, gensym("print"), 0);
}