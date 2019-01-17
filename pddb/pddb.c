/*
code for the "pddb" pd class. 
[pddb] stores incoming lists into a database file
developed by Fede Camara Halac (fch226@nyu.edu)
@ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
Released under the GNU General Public License. 
*/
#include "m_pd.h"
#include "stdio.h"
// #include "stdlib.h"
// #include "math.h"

static t_class *pddb_class;
typedef struct pddb {
  t_object  x_ob;
  t_canvas *x_canvas;
  t_int     x_max;      // size of bins
  t_int    *x_bins;    // array to store all values
  // t_outlet  *x_outlet0;
} t_pddb;

static void pddb_dealloc(t_pddb *x) {
  t_freebytes(x->x_bins, (x->x_max)*sizeof(t_int));
}

static void pddb_alloc(t_pddb *x, int n) {
  pddb_dealloc(x);
  if (0>=n) { 
    pd_error(x,"Bad size!");
    return;
  } else {
    x->x_max=n;
    x->x_bins = (t_int *)t_getbytes(n*sizeof(t_int));
    post("Resized to %d",x->x_max);
  }
}

static void pddb_write(t_pddb *x, t_symbol *s) {
  FILE *filePtr;
  char *fileName, fileNameBuf[MAXPDSTRING];
  fileName = s->s_name;
  canvas_makefilename(x->x_canvas, fileName, fileNameBuf, MAXPDSTRING);
  filePtr = fopen(fileNameBuf, "wb");
  
  if(!filePtr) {
    pd_error(x, "Failed to create %s.", fileNameBuf);
    return;
  }

  for(int i=0; i<=x->x_max-1; i++) {    
    fwrite(&x->x_bins[i], sizeof(t_int), x->x_bins[i], filePtr);
  }
  
  post("Wrote %i values to %s.", x->x_max, fileNameBuf);

  fclose(filePtr);
}
static void pddb_read(t_pddb *x, t_symbol *s) {
  FILE *filePtr;
  char *fileName, fileNameBuf[MAXPDSTRING];
  fileName = s->s_name;
  canvas_makefilename(x->x_canvas, fileName, fileNameBuf, MAXPDSTRING);
  filePtr = fopen(fileNameBuf, "rb");

  if (!filePtr) {
    pd_error(x, "failed to open %s", fileNameBuf);
    return;
  }

  fseek(filePtr, 0L, SEEK_END);
  int fileSize = ftell(filePtr);

  if (0>=fileSize){
    pd_error(x,"%s has no size!", filePtr);
  } else {
    pddb_alloc(x,fileSize);

    for(int i=0; i<=x->x_max-1; i++) {
      unsigned char b;
      fread(&b, 1, 1, filePtr);
      x->x_bins[i]=(t_int)b;
    }

    post("Read %i values from %s", x->x_max, fileNameBuf);    
  }
  fclose(filePtr);
}
static void pddb_bang(t_pddb *x) {
  // pd_error(x,"... still no function for bang");
  if(x->x_max) 
    for (int i=0;i<=x->x_max-1;i++)
      post("%i",x->x_bins[i]);
}

static void pddb_vect(t_pddb *x, t_symbol *s, int argc, t_atom *argv) {
  int i,n = argc;
  t_symbol *sym=s;
  t_atom *vect=argv;
  
  post("list has %d elemets",n);
  
  pddb_alloc(x,n);

  for (i=0; i<n-1; i++) {
    x->x_bins[i] = (double) argv[i].a_w.w_float;
  }

}
static void pddb_free(t_pddb *x) {
  pddb_dealloc(x);
}
void *pddb_new(t_floatarg f) {
  t_pddb *x = (t_pddb *)pd_new(pddb_class);
  if (f) pddb_alloc(x,(int)f);
  // x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
  // x->x_outlet1 = outlet_new(&x->x_ob, &s_list);
  // x->x_outlet2 = outlet_new(&x->x_ob, &s_float);
  x->x_canvas = canvas_getcurrent();
  return (void *)x;
}
void pddb_setup(void) {
  pddb_class = class_new(gensym("pddb"),(t_newmethod)pddb_new,(t_method)pddb_free,sizeof(t_pddb),CLASS_DEFAULT,A_DEFFLOAT,0);
  class_addbang(pddb_class, pddb_bang);
  class_addlist(pddb_class, pddb_vect);
  class_addmethod(pddb_class,(t_method)pddb_write,gensym("write"),A_SYMBOL,0);
  class_addmethod(pddb_class,(t_method)pddb_read,gensym("read"),A_SYMBOL,0);
}