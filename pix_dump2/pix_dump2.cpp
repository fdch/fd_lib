////////////////////////////////////////////////////////
//
// pix_dump2
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
#include "pix_dump2.h"
#include "Utils/Functions.h"//for CLAMP
#include <cmath>

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_dump2, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT);
/////////////////////////////////////////////////////////
//
// pix_dump2
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_dump2 :: pix_dump2(t_floatarg fx, t_floatarg fy) :
    m_dataOut(0),
    xsize(0), ysize(0),
    m_xsize(0), m_ysize(0), m_csize(3),
    m_buffer(0),
    m_bufsize(0),
    oldimagex(0), oldimagey(0)
{
  xsize = static_cast<int>(fx);
  ysize = static_cast<int>(fy);

  if (xsize < 0) xsize = 0;
  if (ysize < 0) ysize = 0;

  m_xsize = xsize;
  m_ysize = ysize;

  oldimagex = xsize;
  oldimagey = ysize;

  m_bufsize = m_xsize * m_ysize * m_csize;

  m_buffer = new t_atom[m_bufsize];

  m_dataOut = outlet_new(this->x_obj, &s_list);
}
/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_dump2 :: ~pix_dump2()
{
  outlet_free(m_dataOut);
  delete[]m_buffer;
}



void pix_dump2 :: processImage(imageStruct &image)
{
  int x = m_xsize, y = m_ysize, c = m_csize;

  if (image.xsize != oldimagex) {
    oldimagex = image.xsize;
    m_xsize = ((!xsize) || (xsize > oldimagex))?oldimagex:xsize;
  }
  if (image.ysize != oldimagey) {
    oldimagey = image.ysize;
    m_ysize = ((!ysize) || (ysize > oldimagey))?oldimagey:ysize;
  }

  if (image.csize != m_csize) m_csize = image.csize;

  if ( (m_xsize != x) || (m_ysize != y) || (m_csize != c) ) {
    // resize the image buffer
    if(m_buffer)delete [] m_buffer;
    m_bufsize = m_xsize * m_ysize * m_csize;
    m_buffer = new t_atom[m_bufsize];
    // post("buffer size:%d",m_bufsize);


    m_xstep = m_csize * (static_cast<float>(image.xsize)/static_cast<float>(m_xsize));
    m_ystep = m_csize * (static_cast<float>(image.ysize)/static_cast<float>(m_ysize)) * image.xsize;
  }

  m_data = image.data;
} 

/////////////////////////////////////////////////////////
// output
//
/////////////////////////////////////////////////////////
void pix_dump2 :: trigger()
{
  if (!m_data) return;
  
  int n = 0, m = 0;
  int i = 0, j=0;

  int roi_x1=0;
  int roi_x2=m_xsize;
  int roi_y1=0;
  int roi_y2=m_ysize;

  unsigned char *buffer = m_data;

  int datasize=m_bufsize;

  unsigned char *data, *line;

  data = line = buffer;
  
  while (datasize--) {
    float v;
    v = static_cast<float>(data[0]) / 255.f;    SETFLOAT(&m_buffer[i++], v);
    
  }

  outlet_list(m_dataOut, gensym("list"), i, m_buffer);

}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_dump2 :: obj_setupCallback(t_class *classPtr) {
    CPPEXTERN_MSG0(classPtr, "bang", trigger);
}