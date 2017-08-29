////////////////////////////////////////////////////////
//
// pix_fft2
//
// Calculates the Forward Fourier Transform using FFTW
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
#include "pix_fft2.h"
#include "Utils/Functions.h"//for CLAMP
#include <cmath>
#define FFTWPLANNERFLAG FFTW_ESTIMATE
#define DEF 64

CPPEXTERN_NEW_WITH_ONE_ARG(pix_fft2, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_fft2
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_fft2 :: pix_fft2(t_floatarg n):
  m_xsize(0),m_ysize(0), m_insize(0), m_size(0),m_enable(false)
{
  n=n<=0?DEF:n;
  m_xsize=n;
  m_ysize=n;
  m_insize=n*n;
  m_size=n*(n/2+1);
  reallocAll(n,n);
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_fft2 :: ~pix_fft2()
{
  if(!m_size)return;
  else{
    delete [] fftwIn;
    delete [] q1;
    delete [] q2;
    delete [] q3;
    delete [] q4;
    fftwf_free(fftwOut);
    fftwf_destroy_plan(fftwPlan);
  }
}
/////////////////////////////////////////////////////////
// Utility functions
//
/////////////////////////////////////////////////////////
void pix_fft2 :: reallocAll(int n, int m)
{
  m_enable=false;
// Get new sizes
  m_xsize = n;
  m_ysize = m;
  m_insize = n*m; //actual size of image
  m_size = n*(m/2+1); //FFTW output size
// Allocate arrays
  fftwIn = new float [m_insize];
  q1 = new unsigned char [m_insize/4];
  q2 = new unsigned char [m_insize/4];
  q3 = new unsigned char [m_insize/4];
  q4 = new unsigned char [m_insize/4];
  fftwOut = (fftwf_complex *)fftwf_alloc_complex(m_size);
  fftwPlan = fftwf_plan_dft_r2c_2d(n, m, fftwIn, fftwOut, FFTWPLANNERFLAG);
// Notify and enable computing
  post("m_insize=%d, m_size=%d", m_insize,m_size);
  m_enable=true;
}

void pix_fft2 :: copyRect(unsigned char*s,unsigned char *t,bool dir,bool Yoff, bool Xoff)
{
// copyRect: 
// *s  *t  direction(1=s->t, 0=t->s) Y  X (offsets)
  unsigned char *src = s;//safe local copies
  unsigned char *tar = t;
  int cols = m_xsize/2;//n
  int rows = m_ysize/2;//m
  int Xoffset = Xoff?cols:0;//add n/2 to start at mid column
  int Yoffset = Yoff?m_insize/2:0;//n*m/2 to start at mid row
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++) {
      int step=i*rows*2+j+Xoffset+Yoffset;
      if (dir) tar[step] = *src++;
      else *src++ = tar[step];
    }
}
void pix_fft2 :: shiftFFT(unsigned char*data)
{
  //original
  copyRect(q1,data, 0, 0, 0);
  copyRect(q2,data, 0, 0, 1);
  copyRect(q3,data, 0, 1, 1);
  copyRect(q4,data, 0, 1, 0);
  //swapped
  copyRect(q1,data, 1, 1, 1);
  copyRect(q2,data, 1, 1, 0);
  copyRect(q3,data, 1, 0, 0);
  copyRect(q4,data, 1, 0, 1);
}

void pix_fft2 :: magSpec(unsigned char *data, fftwf_complex *fft)
{
  float re, im, mag;
  long i,j, k=0,step;
//calculate magnitude and normalize
  for(i=0;i<m_ysize;i++)
    for(j=0;j<m_xsize/2+1;j++) {
      step=i*m_ysize+j;
      re = std::abs(fft[k][0]);
      im = std::abs(fft[k][1]);
      mag = sqrt(re*re+im*im);
      data[step] = CLAMP(logf(1.+mag)*m_xsize/4);
      k++;
  }
//copy the non-computed symmetry back to the data
  for(i=0;i<m_ysize;i++)
    for(j=m_xsize/2+1;j<m_xsize;j++) {
      step=i*m_ysize+j;
      data[step] = data[m_insize-step];
    }
}

/////////////////////////////////////////////////////////
// Process image (grey space only)
//
/////////////////////////////////////////////////////////
void pix_fft2 :: processGrayImage(imageStruct &image)
{
// Pointer to the pixels (unsigned char 0-255)
  unsigned char *pixels = image.data;
  int rows = image.ysize;
  int cols = image.xsize;
  long i,j,step;
  if(!m_enable)return;
// Check if sizes match and reallocate.
  if(m_insize!=rows*cols) {
    reallocAll(cols, rows);
  } else {
    //input to FFTW
    for (i=0; i<m_insize; i++)
      fftwIn[i] = (float)pixels[i]/255.;

    fftwf_execute(fftwPlan);

    //calculate magnitude
    magSpec(pixels, fftwOut);
    //shift zero-th frequency to center
    shiftFFT(pixels);

  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_fft2 :: obj_setupCallback(t_class *classPtr) {}