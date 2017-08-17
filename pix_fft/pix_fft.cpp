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
  m_insize(n*n), m_size(n*(n/2+1)),m_enable(false)
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
// Process image (grey space only)
//
/////////////////////////////////////////////////////////
void pix_fft :: processGrayImage(imageStruct &image)
{
// Pointer to the pixels (unsigned char 0-255)
  pixels = image.data;
  int rows = image.ysize;
  int cols = image.xsize;
  long i;
  if(!m_enable)return;
// Check if sizes match and reallocate.
  if(m_insize!=rows*cols) reallocAll(cols, rows);
  else {
    //perform FFT
    for (i=0; i<m_insize; i++)
      fftwIn[i] = pixels[i];
    fftwf_execute(fftwPlan);
    for (i=0; i<m_size; i++) {
      pixels[i] = fftwOut[i][0];
      SETFLOAT(imagOut+i, fftwOut[i][1]);
    }
  outlet_list(m_imag, gensym("list"), m_size, imagOut);
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_fft :: obj_setupCallback(t_class *classPtr) {}