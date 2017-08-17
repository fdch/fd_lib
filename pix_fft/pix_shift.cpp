////////////////////////////////////////////////////////
//
// pix_shift
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
#include "pix_shift.h"
#include "Utils/Functions.h"//for CLAMP
#define FFTWPLANNERFLAG FFTW_ESTIMATE

CPPEXTERN_NEW_WITH_ONE_ARG(pix_shift, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_shift
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_shift :: pix_shift(t_floatarg n):
  m_xsize(n),m_ysize(n), m_insize(n*n),
  m_enable(false)
{
  reallocAll(n,n);
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_shift :: ~pix_shift()
{
  deallocAll();
}
/////////////////////////////////////////////////////////
// Utility functions
//
/////////////////////////////////////////////////////////
void pix_shift :: deallocAll()
{
  if(!m_insize)return;
  else{
    delete [] q1;
    delete [] q2;
    delete [] q3;
    delete [] q4;
  }
}
void pix_shift :: reallocAll(int n, int m)
{
  m_enable=false;
// Destroy previous arrays
  deallocAll();
// Get new sizes
  m_xsize = n;
  m_ysize = m;
  m_insize = n*m; //actual size of image
// Allocate arrays
  q1 = new unsigned char [m_insize/4];
  q2 = new unsigned char [m_insize/4];
  q3 = new unsigned char [m_insize/4];
  q4 = new unsigned char [m_insize/4];
  m_enable=true;
}

void pix_shift :: copyRect(unsigned char*s,unsigned char *t,bool dir,bool Yoff, bool Xoff)
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

/////////////////////////////////////////////////////////
// Process image (grey space only)
//
/////////////////////////////////////////////////////////
void pix_shift :: processGrayImage(imageStruct &image)
{
// Pointer to the pixels (unsigned char 0-255)
  pixels = image.data;
  int rows = image.ysize;
  int cols = image.xsize;
  if(!m_enable)return;
// Check if sizes match and reallocate.
  if(m_insize!=rows*cols) reallocAll(cols, rows);
  else {
    //shift zero-th frequency to center
      //original
    copyRect(q1,pixels, 0, 0, 0);
    copyRect(q2,pixels, 0, 0, 1);
    copyRect(q3,pixels, 0, 1, 1);
    copyRect(q4,pixels, 0, 1, 0);

    //swapped
    copyRect(q1,pixels, 1, 1, 1);
    copyRect(q2,pixels, 1, 1, 0);
    copyRect(q3,pixels, 1, 0, 0);
    copyRect(q4,pixels, 1, 0, 1);
  }
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_shift :: obj_setupCallback(t_class *classPtr) {}