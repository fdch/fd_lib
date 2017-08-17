#ifndef _INCLUDE__GEM_PIXES_pix_ifft_H_
#define _INCLUDE__GEM_PIXES_pix_ifft_H_

#include "Base/GemPixDualObj.h"
#include "fftw3.h"


class GEM_EXTERN pix_ifft : public GemPixDualObj
{
  CPPEXTERN_HEADER(pix_ifft, GemPixDualObj);

  public:

  //////////
  // Constructor
  pix_ifft(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_ifft(void);

  virtual void 	processGray_Gray(imageStruct &image, imageStruct &right);

  float  *fftwOut;
  
  fftwf_complex  *fftwIn;
  fftwf_plan  fftwPlan;
  int m_size, m_xsize, m_ysize, m_insize;
  bool m_enable;
  
  void  reallocAll(int n, int m);
  void  deallocAll(void);

};
#endif
