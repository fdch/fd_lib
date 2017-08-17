#include "pix_shift.h"
#include "Utils/Functions.h"
#define DEFSIZE 64*64

CPPEXTERN_NEW_WITH_ONE_ARG(pix_shift, t_floatarg, A_DEFFLOAT);

pix_shift :: pix_shift(t_floatarg n):
  m_size(n>0?n*n:DEFSIZE)
{
  m_data = new unsigned char [m_size];
  post("m_size = %d", m_size);
}

pix_shift :: ~pix_shift()
{ 
  if(m_data) delete [] m_data;
}

/* Could be useful
void pix_shift :: cvCopy(float*to, float*from, int sizeArr) {
  int i, sourcepts;
  sourcepts = m_size/4+sizeArr;
  for(i=0; i<sourcepts; i++)
    to[i] = from[i];
}
*/
void pix_shift :: processGrayImage(imageStruct &image)
{
  //if (calculated) return;
  //else {
    
    int x=image.xsize/2;
    int y=image.ysize/2;
    int xsize=image.xsize;
    int ysize=image.ysize;
    int nsize=xsize*ysize;
    if(m_size!=nsize) {
      m_size = nsize;
      if(m_data) delete [] m_data;
      m_data = new unsigned char [m_size];
    }
    
    
    int i=0;
    int offX=0;
    int offY=0;
    while(i<y){
      int oldrow=(offY+i)%ysize;
      unsigned char *newdata = m_data+(x*i);
      unsigned char *olddata = image.data+(offX+xsize*oldrow);
      int j=x;
      while(j--)
        *newdata++=*olddata++;
      i++;
    }
    image.data = m_data;
    image.xsize = x;//new size
    image.ysize = y;//cropping
  //  calculated = true;
  //}
}
void pix_shift :: obj_setupCallback(t_class *classPtr) {}