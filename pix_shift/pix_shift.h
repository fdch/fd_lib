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

  bool calculated;
  long m_size;
  unsigned char *m_data;
  

 private:

  //////////
  // Static member functions

};
#endif
