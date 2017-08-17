#ifndef _INCLUDE__GEM_PIXES_pix_fft_H_
#define _INCLUDE__GEM_PIXES_pix_fft_H_

#include "Base/GemPixObj.h"
#include "fftw3.h"


class GEM_EXTERN pix_fft : public GemPixObj
{
  CPPEXTERN_HEADER(pix_fft, GemPixObj);

  public:

  //////////
  // Constructor
  pix_fft(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_fft(void);

  virtual void  processGrayImage(imageStruct &image);

  unsigned char  *pixels;
  float  *fftwIn;
  t_atom *imagOut;
  
  fftwf_complex  *fftwOut;
  fftwf_plan  fftwPlan;
  int m_size, m_insize;
  bool m_enable;
  
  void  reallocAll(int n, int m);
  void  deallocAll(void);
  
  t_outlet    	*m_imag;

};
#endif
