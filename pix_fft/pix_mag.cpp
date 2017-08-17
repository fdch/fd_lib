////////////////////////////////////////////////////////
//
// pix_mag
//
// Calculates the magnitude of incoming real and imaginary parts
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
#include "pix_mag.h"
#include "Utils/Functions.h"//for CLAMP

#define FFTWPLANNERFLAG FFTW_ESTIMATE

CPPEXTERN_NEW_WITH_ONE_ARG(pix_mag, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_mag
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_mag :: pix_mag(t_floatarg n):
m_norm(n?n:4)
{}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_mag :: ~pix_mag()
{}

/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_mag :: processGray_Gray(imageStruct &image, imageStruct &right)
{

  unsigned char *leftPix = image.data;
  unsigned char *rightPix = right.data;
  
  float re, im, mag;
  long i,j, k=0,step;
//calculate magnitude and normalize
  for(i=0;i<image.ysize;i++)
    for(j=0;j<image.xsize;j++) {
      step=i*image.ysize+j;
      re = leftPix[step];
      im = rightPix[step];
      mag = sqrt(re*re+im*im);
      leftPix[step] = CLAMP(logf(1.+mag)*image.xsize/m_norm);
      //k++;
  }
}


/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_mag :: obj_setupCallback(t_class *classPtr) {

}