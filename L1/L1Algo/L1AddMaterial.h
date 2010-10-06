#ifndef L1AddMaterial_h
#define L1AddMaterial_h

#include "CbmL1Def.h"
#include "L1MaterialInfo.h"
#include "L1TrackPar.h"

#define cnst static const fvec 

inline void L1AddMaterial( L1TrackPar &T, L1MaterialInfo &info, fvec qp0 )
{
  cnst ZERO = 0.0f, ONE = 1.;
  cnst mass2 = 0.1395679f*0.1395679f;

  fvec tx = T.tx;
  fvec ty = T.ty;
  fvec txtx = tx*tx;
  fvec tyty = ty*ty;
  fvec txtx1 = txtx + ONE;
  fvec h = txtx + tyty;
  fvec t = sqrt(txtx1 + tyty);
  fvec h2 = h*h;
  fvec qp0t = qp0*t;
  
  cnst c1=0.0136f, c2=c1*0.038f, c3=c2*0.5f, c4=-c3/2.0f, c5=c3/3.0f, c6=-c3/4.0f;
    
  fvec s0 = (c1+c2*info.logRadThick + c3*h + h2*(c4 + c5*h +c6*h2) )*qp0t;    
  fvec a = ( (ONE+mass2*qp0*qp0t)*info.RadThick*s0*s0 );
// std::cout <<" a " << a << std::endl;
//  a=0.000005;
  T.C22 += txtx1*a;
  T.C32 += tx*ty*a; T.C33 += (ONE+tyty)*a; 
}

inline void L1AddHalfMaterial( L1TrackPar &T, L1MaterialInfo &info, fvec qp0 )
{
  cnst ZERO = 0.0f, ONE = 1.;
  cnst mass2 = 0.1395679f*0.1395679f;

  fvec tx = T.tx;
  fvec ty = T.ty;
  fvec txtx = tx*tx;
  fvec tyty = ty*ty;
  fvec txtx1 = txtx + ONE;
  fvec h = txtx + tyty;
  fvec t = sqrt(txtx1 + tyty);
  fvec h2 = h*h;
  fvec qp0t = qp0*t;
  
  cnst c1=0.0136f, c2=c1*0.038f, c3=c2*0.5f, c4=-c3/2.0f, c5=c3/3.0f, c6=-c3/4.0f;
    
  fvec s0 = (c1+c2*info.logRadThick + c3*h + h2*(c4 + c5*h +c6*h2) )*qp0t;    
  fvec a = ( (ONE+mass2*qp0*qp0t)*info.RadThick*0.5*s0*s0 );
// std::cout <<" a " << a << std::endl;
//  a=0.000005;
  T.C22 += txtx1*a;
  T.C32 += tx*ty*a; T.C33 += (ONE+tyty)*a; 
}

inline void L1AddPipeMaterial( L1TrackPar &T, fvec qp0 )
{
  cnst ZERO = 0.0f, ONE = 1.f;
  cnst mass2 = 0.1395679f*0.1395679f;

  static const fscal RadThick=0.0009f;//0.5/18.76;
  static const fscal logRadThick=log(RadThick);
  fvec tx = T.tx;
  fvec ty = T.ty;
  fvec txtx = tx*tx;
  fvec tyty = ty*ty;
  fvec txtx1 = txtx + ONE;
  fvec h = txtx + tyty;
  fvec t = sqrt(txtx1 + tyty);
  fvec h2 = h*h;
  fvec qp0t = qp0*t;

  cnst c1=0.0136f, c2=c1*0.038f, c3=c2*0.5f, c4=-c3/2.0f, c5=c3/3.0f, c6=-c3/4.0f;
  fvec s0 = (c1+c2*(logRadThick) + c3*h + h2*(c4 + c5*h +c6*h2) )*qp0t;
  fvec a = ( (ONE+mass2*qp0*qp0t)*RadThick*s0*s0 );
  T.C22 += txtx1*a;
  T.C32 += tx*ty*a; T.C33 += (ONE+tyty)*a;

}




#undef cnst

#endif
