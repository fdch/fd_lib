#include "fftw3.h"
#include "Base/GemPixObj.h"

class GEM_EXTERN pix_fft : public GemPixObj

{
  CPPEXTERN_HEADER(pix_fft, GemPixObj);

  public:

  //////////
  // Constructor
  pix_fft(t_floatarg n, t_floatarg t);

  protected:

  //////////
  // Destructor
  virtual ~pix_fft(void);

  void processGrayImage(imageStruct &image);


  fftwf_complex  *fftwOut;
  fftwf_plan  fftwPlan;

  float  *fftwIn;
  int m_size, m_insize;
  bool m_enable, m_type;
  
  void  reallocAll(int n, int m);
  void  processFFT(void);

};