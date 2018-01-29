#ifndef _INCLUDE__GEM_PIXES_pix_phase2_H_
#define _INCLUDE__GEM_PIXES_pix_phase2_H_

#include "Base/GemPixObj.h"
#include "fftw3.h"


class GEM_EXTERN pix_phase2 : public GemPixObj
{
  CPPEXTERN_HEADER(pix_phase2, GemPixObj);

  public:

  //////////
  // Constructor
  pix_phase2(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_phase2(void);

  virtual void  processGrayImage(imageStruct &image);

  float  *fftwIn;
  unsigned char  *q1,*q2,*q3,*q4;
  
  fftwf_complex  *fftwOut;
  fftwf_plan  fftwPlan;
  int m_size, m_xsize, m_ysize, m_insize;
  bool m_enable;
  
  void  reallocAll(int n, int m);
  void  copyRect(unsigned char*s,unsigned char*t, bool dir, bool Yoff, bool Xoff);
  void  phaseSpec(unsigned char*data, fftwf_complex *fft);
  void  shiftFFT(unsigned char*data);

};
#endif
