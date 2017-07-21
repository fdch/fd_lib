/* This is [tracks2] from Jaime Oliver 

  Changes by Fede Camara Halac:
  
  Changed dimensions to a switch statement;
  Changed cbrt to sqrt in 3 dimension distance check;
  Changed elements to double;
  Elements 1 and 2 used to be int, they are now double;
  Added a MAX definition for maximum number of points/frames;
  Added a 'bang' method, to callback tracks2_spit;
  Added a 'list' method, to callback tracks2_input;
  max_dist and dimen methods are now A_FLOAT instead of A_GIMME;
  'clear' method now also clears all the elements;
  Improved some printing and added a 'print' method for debugging;
  Removed some comments and reindented;
  
  TODO: elements should really be structures, with proper memory management;
*/

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#define MAX 2048
#define INS 128

t_class *tracks2_class;

typedef struct tracks2 {
  t_object x_ob;
  int x1_i[MAX], x1_s[MAX], x2_i[MAX], x2_s[MAX], c[INS];
  double x1_elem[INS][MAX], x2_elem[INS][MAX], w[INS], max_dist;
  float i_reg1[MAX], i_reg2[MAX];
  int w_sum, prev_pts, pts, dimen, elnum, debug;
  t_outlet *out01, *out02, *out03;
} t_tracks2;

static double tracks2_getdist(t_tracks2 *x, int i, int j) {
  int d = 0;
  double sum = 0.0;
  for (d = 0; d < x->dimen; d++)
    sum += (x->x1_elem[x->c[d]][i] - x->x2_elem[x->c[d]][j]) *
           (x->x1_elem[x->c[d]][i] - x->x2_elem[x->c[d]][j]) *
           x->w[x->c[d]];
  return sqrt(sum);
}

static void tracks2_choose_dimen(t_tracks2 *x, t_symbol *s, int argc, t_atom *argv) {
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
    t_atom at[INS];
    startpost("Choosing Dimensions:");
    for(i = 0; i < dim; i++) SETFLOAT(at+i,x->c[i]);
    postatom(i,at);
    endpost();
  }
}

void tracks2_weights(t_tracks2 *x, t_symbol *s, int argc, t_atom *argv) {
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
    t_atom at[INS];
    startpost("Weights:");
    for(i = 0; i < argc; i++) SETFLOAT(at+i,x->w[i]);
    postatom(i,at);
    endpost();
  }
}

static void tracks2_dimen(t_tracks2 *x, t_floatarg f) {
  if (f > INS) x->dimen = INS;
  else x->dimen = (int) f;
  if (x->debug) post ("Dimensions: %d", x->dimen);
}

static void tracks2_max_distance(t_tracks2 *x, t_floatarg f) {
  x->max_dist = f;
  if (x->debug) post ("Maximum Distance: %.2f", x->max_dist);
}

static void tracks2_input(t_tracks2 *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  float val;
  if(argc-1 > INS) {
    if (x->debug) {
      startpost("Ignoring extra elements:");
      postatom(argc-1-INS, argv+1+INS);
      endpost();
    }
    x->elnum = INS;
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

static void tracks2_spit(t_tracks2 *x) {
  int i, j, z, new_pts, j_tmp, i_tmp, assigned;
  int free_i[MAX];
  t_atom at[INS];
  double dist, dist_temp;
  j_tmp = i_tmp = assigned = z = 0;
  new_pts = x->pts - x->prev_pts;
  
  for (i = 0; i < MAX; i++)
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
          if ((dist=tracks2_getdist(x,i,j)) < dist_temp && dist < x->max_dist) {
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
          for (j = 0; j < MAX; j++)
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
          if ((dist=tracks2_getdist(x,j,i)) < dist_temp && dist < x->max_dist) {
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

static void tracks2_clear(t_tracks2 *x) {
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
  for (i = 0; i < INS; i++)
    for (j = 0; j < MAX; j++)
      x->x1_elem[i][j]=0;
  if (x->debug) post("All instances cleared");
}

static void tracks2_print(t_tracks2 *x) {
  int i,j;
  t_atom at[INS];
  if (x->debug) {
    post("Debug Mode ON\nMax Elements: %d\n\
Max Frames: %d\nMax Distance: %.2f\nDimensions: %d\nPrevious Frames: %d\
", INS, MAX, x->max_dist, x->dimen, x->prev_pts);
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

static void tracks2_debug(t_tracks2 *x, t_floatarg *f) {
  x->debug = (int) f;
  post("Debug Mode %s",x->debug?"ON":"OFF");
}

void *tracks2_new(void) {
  int i = 0;
  t_tracks2 *x = (t_tracks2 *)pd_new(tracks2_class);
  x->prev_pts = x->pts = 0;
  x->max_dist = 30.;
  x->dimen = 2;
  x->debug = 0;
  for (i = 0; i < INS; i++) {
    x->w[i] = 1;
    x->c[i] = i;
  }
  x->w_sum = 0;
  for (i = 0; i < x->dimen; i++)
    x->w_sum += x->w[i];
  x->out01 = outlet_new(&x->x_ob, &s_list);
  x->out02 = outlet_new(&x->x_ob, &s_list);
  x->out03 = outlet_new(&x->x_ob, &s_list);
  post("[tracks2] version by Fede Camara Halac of Jaime Oliver's [tracks]");
  return (void *)x;
}

void tracks2_setup(void) {
  tracks2_class = class_new(gensym("tracks2"), (t_newmethod)tracks2_new, 0, sizeof(t_tracks2), 0, A_GIMME, 0);
  class_addbang(tracks2_class, tracks2_spit);
  class_addlist(tracks2_class, tracks2_input);
  class_addmethod(tracks2_class, (t_method)tracks2_input, gensym("input"), A_GIMME, 0);
  class_addmethod(tracks2_class, (t_method)tracks2_dimen, gensym("dimen"), A_FLOAT, 0);
  class_addmethod(tracks2_class, (t_method)tracks2_debug, gensym("debug"), A_FLOAT, 0);
  class_addmethod(tracks2_class, (t_method)tracks2_max_distance, gensym("max_distance"), A_FLOAT, 0);
  class_addmethod(tracks2_class, (t_method)tracks2_weights, gensym("weights"), A_GIMME, 0);
  class_addmethod(tracks2_class, (t_method)tracks2_choose_dimen, gensym("choose_dimen"), A_GIMME, 0);
  class_addmethod(tracks2_class, (t_method)tracks2_spit, gensym("spit"), 0);
  class_addmethod(tracks2_class, (t_method)tracks2_clear, gensym("clear"), 0);
  class_addmethod(tracks2_class, (t_method)tracks2_print, gensym("print"), 0);
}