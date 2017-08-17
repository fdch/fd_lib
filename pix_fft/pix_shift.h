#ifndef _INCLUDE__GEM_PIXES_pix_shift_H_
#define _INCLUDE__GEM_PIXES_pix_shift_H_

#include "Base/GemPixObj.h"


class GEM_EXTERN pix_shift : public GemPixObj
{
  CPPEXTERN_HEADER(pix_shift, GemPixObj);

  public:

  //////////
  // Constructor
  pix_shift(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_shift(void);

  virtual void  processGrayImage(imageStruct &image);

  unsigned char  *pixels;
  unsigned char  *q1,*q2,*q3,*q4;

  int m_size, m_xsize, m_ysize, m_insize;
  bool m_enable;
  

  void  reallocAll(int n, int m);
  void  deallocAll(void);
  void  copyRect(unsigned char*s,unsigned char*t, bool dir, bool Yoff, bool Xoff);

};
#endif
