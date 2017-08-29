#include "Base/GemPixObj.h"

class GEM_EXTERN pix_turb : public GemPixObj
{
  CPPEXTERN_HEADER(pix_turb, GemPixObj);

  public:

  //////////
  // Constructor
  pix_turb(t_floatarg n);

  protected:

  //////////
  // Destructor
  virtual ~pix_turb(void);

  virtual void  processRGBAImage(imageStruct &image);

  float  *u, *v, *u_prev, *v_prev, *dens, *dens_prev;
  int m_size;
  bool m_enable;
  float m_step, m_visc, m_diff;
  
  void  set_bnd(int N,int b,float *x);
  void  project(int N,float *u,float *v,float *p,float *div);
  void  vel_step(int N,float *u,float *v,float *u0,float *v0,float visc,float dt);
  void  dens_step(int N,float *x,float *x0,float *u,float *v,float diff,float dt);
  void  advect(int N,int b,float *d,float *d0,float *u,float *v,float dt);
  void  diffuse(int N,int b,float *x,float *x0,float diff,float dt);
  void  add_source(int N,float *x,float *s,float dt);
};