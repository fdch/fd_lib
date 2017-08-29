////////////////////////////////////////////////////////
//
// pix_fft
//
// Calculates the Forward Fourier Transform using FFTW
// and outputs real and imaginary values as images
// 
//
// fdch.github.io/tv
// camarafede@gmail.com
// Fede Camara Halac 2017
//
//
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1998 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
#include "pix_fft.h"
//#include "Gem/Cache.h"
//#include "Gem/State.h"
#include "Utils/Functions.h"
#include <cmath>
#define FFTWPLANNERFLAG FFTW_ESTIMATE
#define DEF 64

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_fft, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_fft
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_fft :: pix_fft(t_floatarg n, t_floatarg t):
 m_enable(false),
 m_type(t>0?1:0),
 m_insize(0),
 m_size(0),
 //orgPixBlock(NULL),
 //m_cacheRight(NULL),
 //m_pixRight(NULL),
 //m_pixRightValid(-1),
 //org_pixRightValid(-1)
{
  if(0>=n)n=DEF;
  m_insize=n*n;
  m_size=n*(n/2+1);
  //m_out2 = outlet_new(this->x_obj, 0);
  reallocAll(n,n);
  
 //cachedPixBlock.newimage=0;
 //cachedPixBlock.newfilm =0;
  //memset(&m_pixRight, 0, sizeof(m_pixRight));
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_fft :: ~pix_fft()
{
  if(!m_size)return;
  else {
    delete [] fftwIn;
    fftwf_free(fftwOut);
    fftwf_destroy_plan(fftwPlan);
    //post("deallocated");
  }
  
 // if (m_out2)  outlet_free(m_out2);
  //if(m_cacheImag)delete m_cacheImag; m_cacheImag=NULL;
}
/*
/////////////////////////////////////////////////////////
// setPixModified
//
/////////////////////////////////////////////////////////
void pix_fft :: setPixModified()
{
  if(m_cache && m_cache->m_magic!=GEMCACHE_MAGIC)m_cache=NULL;
  if(m_cache){
    m_cache->resendImage = 1;
    post("m_cache->resendImage= %d",m_cache->resendImage);
  }
}
/////////////////////////////////////////////////////////
// render adapted from GemPixObj
//
/////////////////////////////////////////////////////////
void pix_fft :: render(GemState *state){

  pixBlock*image=NULL;
  //if (!state || !state->get(GemState::_PIX, image)){post("returning");return;}
  if (!state->get(GemState::_PIX, image)){post("returning");return;}
  //state->get(GemState::_PIX, image);

  if(!image || !&image->image){post("2returning");return;}
  
  cachedPixBlock.newimage=image->newimage;
  
  if (!image->newimage) {
    //post("3returning");
    image = &cachedPixBlock;
  } else {
    //post("3returning");
    orgPixBlock = image;
    cachedPixBlock.newimage = image->newimage;
    cachedPixBlock.newfilm = image->newfilm; //added for newfilm copy from cache cgc 6-21-03
    image->image.copy2ImageStruct(&cachedPixBlock.image);
    image = &cachedPixBlock;
    if(image->image.format==GL_LUMINANCE)
      //processFFT(image->image);
      post("ok");
    else error("only Grey image allowed");
  }
  state->set(GemState::_PIX, image);
  //sendOutlet2(state);
}




//from GemBase continueRender(GemState*state)
void pix_fft :: sendOutlet2(GemState*state){
  t_atom ap[2];
  ap->a_type=A_POINTER;
  ap->a_w.w_gpointer=(t_gpointer *)m_cache;  // the cache ?
  (ap+1)->a_type=A_POINTER;
  (ap+1)->a_w.w_gpointer=(t_gpointer *)state;
  outlet_anything(this->m_out2, gensym("gem_state"), 2, ap);
}


//////////
// get the original state back
void pix_fft :: postrender(GemState *state){
  state->set(GemState::_PIX, orgPixBlock);
}
*/
void pix_fft :: reallocAll(int n, int m)
{
  if(m_enable)m_enable=false;
// Get new sizes
  m_insize = n*m; //actual size of image
  m_size = n*(m/2+1); //FFTW output size
// Allocate arrays
  fftwIn = new float [m_insize];
  fftwOut = (fftwf_complex *)fftwf_alloc_complex(m_size);
  fftwPlan = fftwf_plan_dft_r2c_2d(n, m, fftwIn, fftwOut, FFTWPLANNERFLAG);
// Notify
  post("m_insize=%d, m_size=%d", m_insize,m_size);
  m_enable=true;
}


void pix_fft :: processGrayImage(imageStruct &image) {
  unsigned char *pixels = image.data;
  
  int rows = image.ysize;
  int cols = image.xsize;
  
  long i;
  
  if(!m_enable)return;
// Check if sizes match and reallocate.
  if(m_insize!=rows*cols)
    reallocAll(cols, rows);
  else {
    //perform FFT
    for (i=0; i<m_insize; i++)
      fftwIn[i] = (float)pixels[i]/255.;
    
    fftwf_execute(fftwPlan);
    
    for (i=0; i<m_size; i++)
      if(!m_type)
        pixels[i] = fftwOut[i][0]/m_insize;
      else
        pixels[i] = fftwOut[i][1]/m_insize;

  }
  
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_fft :: obj_setupCallback(t_class *classPtr) {}