#ifndef _INCLUDE__GEM_PIXES_pix_dump2_H_
#define _INCLUDE__GEM_PIXES_pix_dump2_H_

#include "Base/GemPixObj.h"
// #include "fftw3.h"


class GEM_EXTERN pix_dump2 : public GemPixObj
{
  CPPEXTERN_HEADER(pix_dump2, GemPixObj);

  public:

  //////////
  // Constructor
  pix_dump2(t_floatarg fx, t_floatarg fy);

  protected:

  //////////
  // Destructor
  virtual ~pix_dump2(void);


  virtual void  processImage(imageStruct &image); 
  // virtual void  processGrayImage(imageStruct &image);


  //////////
  void      trigger();

  //////////
  // The color outlet
  t_outlet      *m_dataOut;

  //////////
  // the buffer
  int           xsize, ysize;      // proposed x/y-sizes
  int           m_xsize,  m_ysize;
  int           m_csize;
  unsigned int  m_bufsize;
  t_atom       *m_buffer;
        // whether we output byte values (0..255) or normalized values (0..1.f)

  int           oldimagex;
  int           oldimagey;

  //////////
  // navigation
  float         m_xstep;
  float         m_ystep;

  /////////
  // pointer to the image data
  unsigned char *m_data;

};
#endif
