////////////////////////////////////////////////////////
//
// pix_morph
//
// morphing between two grey images
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
#include "pix_morph.h"
#include "Utils/Functions.h"//for CLAMP
#include <cmath>

CPPEXTERN_NEW_WITH_ONE_ARG(pix_morph, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_morph
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_morph :: pix_morph(t_floatarg n):
  m_size(0)
{
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_morph :: ~pix_morph()
{
}
/////////////////////////////////////////////////////////
// Process image (grey space only)
//
/////////////////////////////////////////////////////////
void pix_morph :: processGray_Gray(imageStruct &image,imageStruct &right)
{
  unsigned char *pixels = image.data;
  unsigned char *pixRight = right.data;
  int rows = image.ysize;
  int cols = image.xsize;
  int datasize = rows*cols;
  long i,j,step;
  unsigned char *min,*max;
  if (m_size!=datasize) {
    min = new unsigned char [datasize];
    max = new unsigned char [datasize];
  }
  for(i=0;i<datasize;i++) {
    min[i]=MIN(pixels[i],pixRight[i]);
    max[i]=MAX(pixels[i],pixRight[i]);
  }
  for(i=0;i<datasize;i++) {
    INT_MULT(pixels[i], min[i]);
    INT_MULT(pixRight[i], max[i]);
  }

  datasize>>=5;
  int restsize = image.xsize * image.ysize * image.csize - datasize;

  while(datasize--) {
    ADD8(pixels,pixRight);
    pixels+=8;pixRight+=8;
  }
  while(restsize--){
    *pixels = CLAMP_HIGH(static_cast<int>(*pixels + *pixRight));
    pixels++; pixRight++;
  }
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_morph :: obj_setupCallback(t_class *classPtr) {}