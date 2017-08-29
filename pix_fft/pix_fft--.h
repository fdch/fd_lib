#include "fftw3.h"
#include "Base/GemPixObj.h"
//#include "Base/GemBase.h"
//#include "Gem/Image.h"
//#include "stdlib.h"
//#include "string.h"
//#include "math.h"


//class GEM_EXTERN pix_fft : public GemBase
class GEM_EXTERN pix_fft : public GemPixObj


{
//  CPPEXTERN_HEADER(pix_fft, GemBase);
  CPPEXTERN_HEADER(pix_fft, GemPixObj);

  public:

  //////////
  // Constructor
  pix_fft(t_floatarg n, t_floatarg t);

  protected:

  //////////
  // Destructor
  virtual ~pix_fft(void);

  /*
  //////////
  // If the derived class needs the image resent.
  //  	This sets the dirty bit on the pixBlock.
  void	    	setPixModified();
  
  
  virtual void render(GemState *state);
  virtual void postrender(GemState *state);
  void startRendering(void) {
    post("start rendering");
    setPixModified();
  }
  */
  
  
  void sendOutlet2(GemState*state);
  void processGrayImage(imageStruct &image);


  fftwf_complex  *fftwOut;
  fftwf_plan  fftwPlan;


  float  *fftwIn;
  int m_size, m_insize;
  bool m_enable, m_type;
  
  void  reallocAll(int n, int m);
  void  deallocAll(void);
  void  processFFT(void);
  


  //////////
  // the pixBlock-cache
  //pixBlock    cachedPixBlock;
  //pixBlock    *orgPixBlock;
  
      	//////////
      	GemCache    	*m_cacheRight;

    	//////////
    	pixBlock    	*m_pixRight;

        int		m_pixRightValid;
        int		org_pixRightValid;
 
  
  //t_outlet    	*m_out2;

};