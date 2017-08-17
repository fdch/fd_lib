////////////////////////////////////////////////////////
//
// pix_phase
//
// Calculates the phase of incoming real and imaginary parts
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
#include "pix_phase.h"
#include "Utils/Functions.h"//for CLAMP

#define FFTWPLANNERFLAG FFTW_ESTIMATE

CPPEXTERN_NEW(pix_phase);
/////////////////////////////////////////////////////////
//
// pix_phase
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_phase :: pix_phase()
{}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_phase :: ~pix_phase()
{}

/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_phase :: processGray_Gray(imageStruct &image, imageStruct &right)
{

  unsigned char *leftPix = image.data;
  unsigned char *rightPix = right.data;

  long i,j,step;
//calculate magnitude and normalize
  for(i=0;i<image.ysize;i++)
    for(j=0;j<image.xsize;j++) {
      step=i*image.ysize+j;
      leftPix[step] = CLAMP((float)atan2(leftPix[step],rightPix[step])*128+128);
  }
}


/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_phase :: obj_setupCallback(t_class *classPtr) {}