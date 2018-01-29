////////////////////////////////////////////////////////
//
// pix_fftfilt
//
// Calculates the Forward Fourier Transform using FFTW
// The output is not shifted
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
#include "pix_fftfilt.h"
#include "Utils/Functions.h"//for CLAMP
#include <cmath>
#define FFTWPLANNERFLAG FFTW_ESTIMATE

CPPEXTERN_NEW_WITH_ONE_ARG(pix_fftfilt, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_fftfilt
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_fftfilt :: pix_fftfilt(t_floatarg n):
  m_xsize(n),m_ysize(n), m_insize(n*n), m_size(n*(n/2+1)),
  m_enable(false)
{
  reallocAll(n,n);
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_fftfilt :: ~pix_fftfilt()
{
  if(!m_size)return;
  else{
    delete [] fftwInR;
    delete [] fftwOutR;
    fftwf_free(fftwOutOrig);
    fftwf_free(fftwOutFilt);
    fftwf_destroy_plan(fftwOrig);
    fftwf_destroy_plan(fftwFilt);
    fftwf_destroy_plan(ifftwPlan);
  }
}
void pix_fftfilt :: reallocAll(int n, int m)
{
  m_enable=false;
// Get new sizes
  m_xsize = n;
  m_ysize = m;
  m_insize = n*m; //actual size of image
  m_size = n*(m/2+1); //FFTW output size
// Allocate arrays

  q1 = new unsigned char [m_insize/4];q2 = new unsigned char [m_insize/4];
  q4 = new unsigned char [m_insize/4];q3 = new unsigned char [m_insize/4];

  //ifft arrays
  fftwOutR = new float [m_insize];

  //fft arrays
  fftwInR = new float [m_insize]; //USed by left and right images
  fftwOutOrig = (fftwf_complex *)fftwf_alloc_complex(m_size);//original fft
  fftwOutFilt = (fftwf_complex *)fftwf_alloc_complex(m_size);//filter image fft

//Planners 2: fft plans, one for each input
  fftwOrig = fftwf_plan_dft_r2c_2d(n, m, fftwInR, fftwOutOrig, FFTWPLANNERFLAG);
  fftwFilt = fftwf_plan_dft_r2c_2d(n, m, fftwInR, fftwOutFilt, FFTWPLANNERFLAG);
//One plan for the output
  ifftwPlan = fftwf_plan_dft_c2r_2d(n, m, fftwOutOrig, fftwOutR, FFTWPLANNERFLAG);

// Notify and enable computing
  post("m_insize=%d, m_size=%d", m_insize,m_size);
  m_enable=true;
}

void pix_fftfilt :: copyRect(unsigned char*s,unsigned char *t,bool dir,bool Yoff, bool Xoff)
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
void pix_fftfilt :: shiftFFT(unsigned char *data)
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

/////////////////////////////////////////////////////////
// Process image (grey space only)
//
/////////////////////////////////////////////////////////
void pix_fftfilt :: processGray_Gray(imageStruct &image,imageStruct &right)
{
// Pointer to the pixels (unsigned char 0-255)
  unsigned char *pixels = image.data;
  unsigned char *pixRight = right.data;
  
  int rows = image.ysize;
  int cols = image.xsize;
  long i,j, k=0,step;
  float re, im, mag, norm, phase;
  if(!m_enable)return;

// Check if sizes match and reallocate.
  if(m_insize!=rows*cols) reallocAll(cols, rows);
  else {

////////////////////// 
    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++) {
        step=i*m_ysize+j;
        fftwInR[step] = pixels[step];
      }
    
    fftwf_execute(fftwOrig);//////the original image

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++)
        fftwInR[step] = pixRight[i*m_ysize+j];
        
        
    fftwf_execute(fftwFilt);//////the filter image
    

  //calculate magnitude 
    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize/2+1;j++) {
        step=i*m_ysize+j;
        re = fftwOutFilt[step][0];
        im = fftwOutFilt[step][1];
        mag = sqrt(re*re+im*im);
        phase = atan2(re, im);
        //norm = CLAMP(logf(1.+mag)*255);
        //multiply original complex by filter magnitude
        fftwOutOrig[step][0] = mag*exp(phase);
        fftwOutOrig[step][1] = mag*exp(phase);
      }
   
    fftwf_execute(ifftwPlan);//////////////the inverse fft

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++) {
        step=i*m_ysize+j;
       // pixels[step] = fftwOutR[step];
        pixels[step] = CLAMP(logf(1.+fftwOutR[step])*8);
      }

      //copy the non-computed symmetry back to the data
    for(i=0;i<m_ysize;i++)
      for(j=m_xsize/2+1;j<m_xsize;j++) {
        step=i*m_ysize+j;
        pixels[step] = pixels[m_insize-step];
      }
      
          //shift zero-th frequency to center
    //shiftFFT(pixels);







/*
    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++) {
        step=i*m_ysize+j;
        fftwInR[step] = pixels[i*m_ysize+j]/255.;
      }
    
    fftwf_execute(fftwFilt);//////the original image

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize/2+1;j++) {
        step=i*m_ysize+j;
        re = fftwOutFilt[step][0];
        im = fftwOutFilt[step][1];
        mag = sqrt(re*re+im*im);
        norm = CLAMP(mag*m_xsize/255);
        fftwOutOrig[step][0] = norm*pixRight[step];
        fftwOutOrig[step][1] = norm*pixRight[step];
      }
      
      fftwf_execute(ifftwPlan);//////////////the inverse fft

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++) {
        step=i*m_ysize+j;
       // pixels[step] = fftwOutR[step];
       pixels[step] = CLAMP(logf(1.+fftwOutR[step])*m_xsize/4);
      }

      //copy the non-computed symmetry back to the data
    for(i=0;i<m_ysize;i++)
      for(j=m_xsize/2+1;j<m_xsize;j++) {
        step=i*m_ysize+j;
        pixels[step] = pixels[m_insize-step];
      }
          //shift zero-th frequency to center
    shiftFFT(pixels);
    
*/ 
/*
  //pd style image filter... not working as it should...

////////////////////// 
    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++) {
        step=i*m_ysize+j;
        fftwInR[step] = pixels[step];
      }
    
    fftwf_execute(fftwOrig);//////the original image

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++)
        fftwInR[step] = pixRight[i*m_ysize+j];
        
        
    fftwf_execute(fftwFilt);//////the filter image
    

  //calculate magnitude 
    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize/2+1;j++) {
        step=i*m_ysize+j;
        re = fftwOutFilt[step][0];
        im = fftwOutFilt[step][1];
        mag = sqrt(re*re+im*im);
        norm = CLAMP(logf(1.+mag)*255);
        //multiply original complex by filter magnitude
        fftwOutOrig[step][0] *= norm;
        fftwOutOrig[step][1] *= norm;
      }
   
    fftwf_execute(ifftwPlan);//////////////the inverse fft

    for(i=0;i<m_ysize;i++)
      for(j=0;j<m_xsize;j++) {
        step=i*m_ysize+j;
       // pixels[step] = fftwOutR[step];
        pixels[step] = CLAMP(logf(1.+fftwOutR[step])*8);
      }

      //copy the non-computed symmetry back to the data
    for(i=0;i<m_ysize;i++)
      for(j=m_xsize/2+1;j<m_xsize;j++) {
        step=i*m_ysize+j;
        pixels[step] = pixels[m_insize-step];
      }
      
          //shift zero-th frequency to center
    shiftFFT(pixels);
*/
  }
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_fftfilt :: obj_setupCallback(t_class *classPtr) {}