////////////////////////////////////////////////////////
//
// pix_fft
//
// Calculates the Forward Fourier Transform using FFTW
// The output is not shifted, and is sent as an image 
// via outlet 0. The other outlet outputs an imaginary list
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
#include "Utils/Functions.h"//for CLAMP
#include "Gem/Cache.h"
//#include "Gem/Manager.h"
#include <cmath>
#define FFTWPLANNERFLAG FFTW_ESTIMATE

CPPEXTERN_NEW_WITH_ONE_ARG(pix_fft, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_fft
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_fft :: pix_fft(t_floatarg n):
  m_insize(n*n), m_size(n*(n/2+1)),m_enable(false),
  m_cacheImag(new GemCache(NULL))
{
  reallocAll(n,n);
  m_imag = outlet_new(this->x_obj, 0);
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_fft :: ~pix_fft()
{
  deallocAll();
  outlet_free(m_imag);
  if(m_cacheImag)delete m_cacheImag; m_cacheImag=NULL;
}
/////////////////////////////////////////////////////////
// Utility functions
//
/////////////////////////////////////////////////////////
void pix_fft :: deallocAll()
{
  if(!m_size||!fftwPlan)return;
  else{
    delete [] fftwIn;
    delete [] imagOut;
    fftwf_free(fftwOut);
    fftwf_destroy_plan(fftwPlan);
  }
}
void pix_fft :: reallocAll(int n, int m)
{
  m_enable=false;
// Destroy previous arrays
  deallocAll();
// Get new sizes
  m_insize = n*m; //actual size of image
  m_size = n*(m/2+1); //FFTW output size
// Allocate arrays
  fftwIn = new float [m_insize];
  imagOut = new t_atom [m_size];
  fftwOut = (fftwf_complex *)fftwf_alloc_complex(m_size);
  fftwPlan = fftwf_plan_dft_r2c_2d(n, m, fftwIn, fftwOut, FFTWPLANNERFLAG);
// Notify and enable computing
  post("m_insize=%d, m_size=%d", m_insize,m_size);
  m_enable=true;
}
/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void pix_fft :: render(GemState *state)
{
  
  state->set(GemState::_PIX,&m_pixBlockReal);
  m_stateImag->set(GemState::_PIX,&m_pixBlockImag);
  processFFT();
}
/////////////////////////////////////////////////////////
// startRendering
//
/////////////////////////////////////////////////////////
void pix_fft :: startRendering(void)
{
  m_pixBlockReal->newimage = true;
  m_pixBlockImag->newimage = true;
}
/////////////////////////////////////////////////////////
// postrender
//
/////////////////////////////////////////////////////////
void pix_fft :: postrender(GemState *state)
{
  m_pixBlockReal->newimage = false;
  m_pixBlockImag->newimage = false;
  //state->image = NULL;
}
/////////////////////////////////////////////////////////
// Process image (grey space only)
//
/////////////////////////////////////////////////////////
void pix_fft :: processFFT(void)
{
  unsigned char *pixReal = m_pixBlockReal->image.data;
  unsigned char *pixImag = m_pixBlockImag->image.data;

  int rows = m_pixBlockReal->image.ysize;
  int cols = m_pixBlockReal->image.xsize;
  long i;
  t_atom ap[2];
  
  if(!m_enable)return;
// Check if sizes match and reallocate.
  if(m_insize!=rows*cols) reallocAll(cols, rows);
  else {
    //perform FFT
    for (i=0; i<m_insize; i++)
      fftwIn[i] = pixReal[i];
    fftwf_execute(fftwPlan);
    for (i=0; i<m_size; i++) {
      pixReal[i] = fftwOut[i][0];
      pixImag[i] = fftwOut[i][1];
      //SETFLOAT(imagOut+i, fftwOut[i][1]);
    }
  //outlet_list(m_imag, gensym("list"), m_size, imagOut);


    m_stateImag->set(GemState::_DIRTY, m_cacheImag->dirty);

    ap->a_type=A_POINTER;
    ap->a_w.w_gpointer=reinterpret_cast<t_gpointer*>(m_cacheImag);
    (ap+1)->a_type=A_POINTER;
    (ap+1)->a_w.w_gpointer=reinterpret_cast<t_gpointer*>(m_stateImag);
    outlet_anything(m_imag, gensym("gem_state"), 2, ap);
    
    m_cacheImag->dirty = false;

  }
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_fft :: obj_setupCallback(t_class *classPtr) {}