#ifndef _INCLUDE__GEM_PIXES_pix_morph_H_
#define _INCLUDE__GEM_PIXES_pix_morph_H_

#include "Base/GemPixDualObj.h"
#include "fftw3.h"


class GEM_EXTERN pix_morph : public GemPixDualObj
{
  CPPEXTERN_HEADER(pix_morph, GemPixDualObj);

  public:

  //////////
  // Constructor
  pix_morph(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_morph(void);

  virtual void  processGray_Gray(imageStruct &image,imageStruct &right);

  int m_size;

};
#endif
