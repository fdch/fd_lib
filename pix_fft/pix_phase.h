#ifndef _INCLUDE__GEM_PIXES_pix_phase_H_
#define _INCLUDE__GEM_PIXES_pix_phase_H_

#include "Base/GemPixDualObj.h"


class GEM_EXTERN pix_phase :  public GemPixDualObj
{
  CPPEXTERN_HEADER(pix_phase, GemPixDualObj);

  public:

  //////////
  // Constructor
  pix_phase();

  protected:

  //////////
  // Destructor
  virtual ~pix_phase(void);

  virtual void 	processGray_Gray(imageStruct &image, imageStruct &right);

};
#endif
