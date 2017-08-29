#ifndef _INCLUDE__GEM_PIXES_pix_fftfilt_H_
#define _INCLUDE__GEM_PIXES_pix_fftfilt_H_

#include "Base/GemPixDualObj.h"
#include "fftw3.h"


class GEM_EXTERN pix_fftfilt : public GemPixDualObj
{
  CPPEXTERN_HEADER(pix_fftfilt, GemPixDualObj);

  public:

  //////////
  // Constructor
  pix_fftfilt(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_fftfilt(void);

  virtual void  processGray_Gray(imageStruct &image,imageStruct &right);

  float  *fftwOutR, *fftwInR;
  unsigned char  *q1,*q2,*q3,*q4;
  
  fftwf_complex  *fftwOutOrig, *fftwOutFilt;
  fftwf_plan fftwOrig, fftwFilt, ifftwPlan;
  int m_size, m_xsize, m_ysize, m_insize;
  bool m_enable;
  
  void  reallocAll(int n, int m);
  void  copyRect(unsigned char*s,unsigned char*t, bool dir, bool Yoff, bool Xoff);
  void  shiftFFT(unsigned char*data);

};
#endif
