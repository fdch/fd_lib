#ifndef _INCLUDE__GEM_PIXES_pix_mag_H_
#define _INCLUDE__GEM_PIXES_pix_mag_H_

#include "Base/GemPixDualObj.h"


class GEM_EXTERN pix_mag :  public GemPixDualObj
{
  CPPEXTERN_HEADER(pix_mag, GemPixDualObj);

  public:

  //////////
  // Constructor
  pix_mag(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_mag(void);

  virtual void 	processGray_Gray(imageStruct &image, imageStruct &right);


  int m_norm;


};
#endif
