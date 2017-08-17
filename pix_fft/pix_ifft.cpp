////////////////////////////////////////////////////////
//
// pix_ifft
//
// Calculates the Inverse Forward Fourier Transform using FFTW
// from incoming real and imaginary parts
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
#include "pix_ifft.h"
#include "Utils/Functions.h"//for CLAMP
#include <cmath>
#define FFTWPLANNERFLAG FFTW_ESTIMATE

CPPEXTERN_NEW_WITH_ONE_ARG(pix_ifft, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_ifft
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_ifft :: pix_ifft(t_floatarg n):
  m_xsize(n),m_ysize(n), m_insize(n*n), m_size(n*(n/2+1)),
  m_enable(false)
{
  reallocAll(n,n);
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_ifft :: ~pix_ifft()
{
  deallocAll();
}
/////////////////////////////////////////////////////////
// Utility functions
//
/////////////////////////////////////////////////////////
void pix_ifft :: deallocAll()
{
  if(!m_size||!fftwPlan)return;
  else{
    delete [] fftwOut;
    fftwf_free(fftwIn);
    fftwf_destroy_plan(fftwPlan);
  }
}
void pix_ifft :: reallocAll(int n, int m)
{
  m_enable=false;
// Destroy previous arrays
  deallocAll();
// Get new sizes
  m_xsize = n;
  m_ysize = m;
  m_insize = n*m; //actual size of image
  m_size = n*(m/2+1); //FFTW output size
// Allocate arrays
  fftwOut = new float [m_insize];
  fftwIn = (fftwf_complex *)fftwf_alloc_complex(m_size);
  fftwPlan = fftwf_plan_dft_c2r_2d(n, m, fftwIn, fftwOut, FFTWPLANNERFLAG);
// Notify and enable computing
  post("m_insize=%d, m_size=%d", m_insize,m_size);
  m_enable=true;
}
/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_ifft :: processGray_Gray(imageStruct &image, imageStruct &right)
{

  unsigned char *leftPix = image.data;
  unsigned char *rightPix = right.data;

  int rows = image.ysize;
  int cols = image.xsize;
  long i,j, k=0,step;
  
  if(!m_enable)return;
// Check if sizes match and reallocate.
  if(m_insize!=rows*cols) reallocAll(cols, rows);
  else {

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize/2+1;j++) {
        step=i*m_ysize+j;
        fftwIn[k][0] = (float)leftPix[step];
        fftwIn[k][1] = (float)rightPix[step];
        k++;
      }

    fftwf_execute(fftwPlan);

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize/2+1;j++) {
        step=i*m_ysize+j;
        leftPix[step] = fftwOut[k++];
      }

    for(i=0;i<m_ysize;i++)
      for(j=m_xsize/2+1;j<m_xsize;j++) {
        step=i*m_ysize+j;
        leftPix[step] = leftPix[m_insize-step];
      }
  }

}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_ifft :: obj_setupCallback(t_class *classPtr) {}