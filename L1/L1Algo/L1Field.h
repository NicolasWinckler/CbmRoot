#ifndef L1Field_h
#define L1Field_h 1

#include "L1Types.h"

class L1FieldValue{

 public:

  fvec x, y, z;

  void Combine( L1FieldValue &B, fvec w )
    {
      x+= w*( B.x - x );
      y+= w*( B.y - y );
      z+= w*( B.z - z );
    }
} _fvecalignment;



class L1FieldSlice{

 public:

  fvec cx[10], cy[10], cz[10]; // polinom coeff.

  L1FieldSlice()
    { 
      for( int i=0; i<10; i++ ) cx[i] = cy[i] = cz[i] = 0; 
    }

  void GetFieldValue( const fvec &x, const fvec &y, L1FieldValue &B )
    {
      fvec x2 = x*x;
      fvec y2 = y*y;
      fvec xy = x*y;
      fvec x3 = x2*x;
      fvec y3 = y2*y;
      fvec xy2 = x*y2;
      fvec x2y = x2*y;
      
      B.x = cx[0] +cx[1]*x +cx[2]*y +cx[3]*x2 +cx[4]*xy +cx[5]*y2 +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3;
      B.y = cy[0] +cy[1]*x +cy[2]*y +cy[3]*x2 +cy[4]*xy +cy[5]*y2 +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3;
      B.z = cz[0] +cz[1]*x +cz[2]*y +cz[3]*x2 +cz[4]*xy +cz[5]*y2 +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3;
    }
} _fvecalignment;



class L1FieldRegion{

 public:

  fvec cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
  fvec cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
  fvec cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
  fvec z0;

  void Set( const L1FieldValue &B0, const fvec B0z, 
	    const L1FieldValue &B1, const fvec B1z, 
	    const L1FieldValue &B2, const fvec B2z )
    {
      z0 = B0z[0];
      fvec dz1 = B1z-B0z, dz2 = B2z-B0z;
      fvec det = rcp(fvec(dz1*dz2*(dz2-dz1)));
      fvec w21 = -dz2*det;
      fvec w22 = dz1*det;
      fvec w11 = -dz2*w21;
      fvec w12 = -dz1*w22;
      
      fvec dB1 = B1.x - B0.x;
      fvec dB2 = B2.x - B0.x;
      cx0 = B0.x;
      cx1 = dB1*w11 + dB2*w12 ;
      cx2 = dB1*w21 + dB2*w22 ;
      
      dB1 = B1.y - B0.y;
      dB2 = B2.y - B0.y;
      cy0 = B0.y;
      cy1 = dB1*w11 + dB2*w12 ;
      cy2 = dB1*w21 + dB2*w22  ;

      dB1 = B1.z - B0.z;
      dB2 = B2.z - B0.z;
      cz0 = B0.z;
      cz1 = dB1*w11 + dB2*w12 ;
      cz2 = dB1*w21 + dB2*w22   ;      
    }
  
  void Set( const L1FieldValue &B0, const fvec B0z, 
	    const L1FieldValue &B1, const fvec B1z )
    {
      z0 = B0z[0];
      fvec dzi = rcp(fvec( B1z - B0z));
      cx0 = B0.x;
      cy0 = B0.y;
      cz0 = B0.z;
      cx1 = ( B1.x - B0.x )*dzi;
      cy1 = ( B1.y - B0.y )*dzi;
      cz1 = ( B1.z - B0.z )*dzi;
      cx2 = cy2 = cz2 = 0;
    }

  void Shift( fvec z)
    {
      fvec dz = z-z0;
      fvec cx2dz = cx2*dz;
      fvec cy2dz = cy2*dz;
      fvec cz2dz = cz2*dz;
      z0 = z[0];
      cx0+= ( cx1 + cx2dz )*dz;
      cy0+= ( cy1 + cy2dz )*dz;
      cz0+= ( cz1 + cz2dz )*dz;
      cx1+= cx2dz + cx2dz; 
      cy1+= cy2dz + cy2dz; 
      cz1+= cz2dz + cz2dz; 
    }
  
}  _fvecalignment;


#endif
