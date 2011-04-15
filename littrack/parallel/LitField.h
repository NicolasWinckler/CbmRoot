/** LitField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#ifndef LITFIELD_H_
#define LITFIELD_H_

#define LIT_POLYNOM_DEGREE 7

#include "LitMath.h"

//FIXME: use parallel impl fo utils
#include "../std/utils/CbmLitUtils.h"

template<class T>
class LitFieldValue
{
public:
   // components of the magnetic field
   T Bx, By, Bz;
} _fvecalignment;



template<class T>
class LitFieldSlice
{
public:

#if LIT_POLYNOM_DEGREE==3
   static const unsigned int N = 10; // for the 3rd degree polynom
#else
#if LIT_POLYNOM_DEGREE==5
   static const unsigned int N = 21; // for the 5th degree polynom
#else
#if LIT_POLYNOM_DEGREE==7
   static const unsigned int N = 36; // for the 7th degree polynom
#else
#if LIT_POLYNOM_DEGREE==9
   static const unsigned int N = 55; // for the 7th degree polynom
#endif
#endif
#endif
#endif

public:
   LitFieldSlice() {
      for(unsigned int i=0; i<N; i++) { cx[i] = cy[i] = cz[i] = 0.; }
   }

   void GetFieldValue(
      const T& x,
      const T& y,
      LitFieldValue<T> &B) const {
#if LIT_POLYNOM_DEGREE==3
//    T x2 = x*x;
//    T y2 = y*y;
//    T yx2 = y*x2;
//    T xy2 = x*y2;
//    T x3 = x2*x;
//    T y3 = y2*y;
//    B.Bx = cx[0] + cx[1]*x + cx[2]*y + cx[3]*x*y + cx[4]*x2 + cx[5]*y2 + cx[6]*xy2 + cx[7]*yx2 + cx[8]*x3 + cx[9]*y3;
//    B.By = cy[0] + cy[1]*x + cy[2]*y + cy[3]*x*y + cy[4]*x2 + cy[5]*y2 + cy[6]*xy2 + cy[7]*yx2 + cy[8]*x3 + cy[9]*y3;
//    B.Bz = cz[0] + cz[1]*x + cz[2]*y + cz[3]*x*y + cz[4]*x2 + cz[5]*y2 + cz[6]*xy2 + cz[7]*yx2 + cz[8]*x3 + cz[9]*y3;

      T x2 = x*x;
      T y2 = y*y;
      T x2y = x2*y;
      T xy2 = x*y2;
      T x3 = x2*x;
      T y3 = y2*y;

      B.Bx = cx[0]
             + cx[1]*x + cx[2]*y
             + cx[3]*x2 + cx[4]*x*y + cx[5]*y2
             + cx[6]*x3 + cx[7]*x2y + cx[8]*xy2 + cx[9]*y3;

      B.By = cy[0]
             + cy[1]*x + cy[2]*y
             + cy[3]*x2 + cy[4]*x*y + cy[5]*y2
             + cy[6]*x3 + cy[7]*x2y + cy[8]*xy2 + cy[9]*y3;

      B.Bz = cz[0]
             + cz[1]*x + cz[2]*y
             + cz[3]*x2 + cz[4]*x*y + cz[5]*y2
             + cz[6]*x3 + cz[7]*x2y + cz[8]*xy2 + cz[9]*y3;
#else
#if LIT_POLYNOM_DEGREE==5
//    T x2 = x*x;
//    T y2 = y*y;
//    T yx2 = y*x2;
//    T xy2 = x*y2;
//    T x3 = x2*x;
//    T y3 = y2*y;
//    T x4 = x2*x2;
//    T y4 = y2*y2;
//    T x5 = x4*x;
//    T y5 = y4*y;
//    B.Bx = cx[0] + cx[1]*x + cx[2]*y + cx[3]*x*y + cx[4]*x2 + cx[5]*y2 + cx[6]*xy2 + cx[7]*yx2 + cx[8]*x3 + cx[9]*y3 +
//          cx[10]*x2*y2 + cx[11]*x*y3 + cx[12]*y*x3 + cx[13]*x4 + cx[14]*y4 + cx[15]*x2*y3 + cx[16]*y2*x3 +
//          cx[17]*x*y4 + cx[18]*y*x4 + cx[19]*x5 + cx[20]*y5;
//    B.By = cy[0] + cy[1]*x + cy[2]*y + cy[3]*x*y + cy[4]*x2 + cy[5]*y2 + cy[6]*xy2 + cy[7]*yx2 + cy[8]*x3 + cy[9]*y3 +
//          cy[10]*x2*y2 + cy[11]*x*y3 + cy[12]*y*x3 + cy[13]*x4 + cy[14]*y4 + cy[15]*x2*y3 + cy[16]*y2*x3 +
//          cy[17]*x*y4 + cy[18]*y*x4 + cy[19]*x5 + cy[20]*y5;
//    B.Bz = cz[0] + cz[1]*x + cz[2]*y + cz[3]*x*y + cz[4]*x2 + cz[5]*y2 + cz[6]*xy2 + cz[7]*yx2 + cz[8]*x3 + cz[9]*y3 +
//          cz[10]*x2*y2 + cz[11]*x*y3 + cz[12]*y*x3 + cz[13]*x4 + cz[14]*y4 + cz[15]*x2*y3 + cz[16]*y2*x3 +
//          cz[17]*x*y4 + cz[18]*y*x4 + cz[19]*x5 + cz[20]*y5;

      T x2 = x*x;
      T y2 = y*y;
      T xy = x*y;

      T x3 = x2*x;
      T y3 = y2*y;
      T xy2 = x*y2;
      T x2y = x2*y;

      T x4 = x3*x;
      T y4 = y3*y;
      T xy3 = x*y3;
      T x2y2 = x2*y2;
      T x3y = x3*y;

      T x5 = x4*x;
      T y5 = y4*y;
      T xy4 = x*y4;
      T x2y3 = x2*y3;
      T x3y2 = x3*y2;
      T x4y = x4*y;

      B.Bx = cx[0]
             +cx[1]*x +cx[2]*y
             +cx[3]*x2 +cx[4]*xy +cx[5]*y2
             +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
             +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
             +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5;

      B.By = cy[0]
             +cy[1]*x +cy[2]*y
             +cy[3]*x2 +cy[4]*xy +cy[5]*y2
             +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
             +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
             +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5;

      B.Bz = cz[0]
             +cz[1]*x +cz[2]*y
             +cz[3]*x2 +cz[4]*xy +cz[5]*y2
             +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
             +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
             +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5;

#else
#if LIT_POLYNOM_DEGREE==7
//    T x2 = x*x;
//    T y2 = y*y;
//    T yx2 = y*x2;
//    T xy2 = x*y2;
//    T x3 = x2*x;
//    T y3 = y2*y;
//    T x4 = x2*x2;
//    T y4 = y2*y2;
//    T x5 = x4*x;
//    T y5 = y4*y;
//    T x6 = x4*x2;
//    T y6 = y4*y2;
//    T x7 = x4*x3;
//    T y7 = y4*y3;
//    B.Bx = cx[0] + cx[1]*x + cx[2]*y + cx[3]*x*y2 + cx[4]*x*y + cx[5]*x2 + cx[6]*y2 + cx[7]*x3 + cx[8]*y3 + cx[9]*y*x2 +
//          cx[10]*x*y3 + cx[11]*y*x3 + cx[12]*x2*y2 + cx[13]*x4 + cx[14]*y4 + cx[15]*x6 + cx[16]*y6 + cx[17]*x2*y4 +
//          cx[18]*y2*x4 + cx[19]*x*y5 + cx[20]*y*x5 + cx[21]*x3*y3 + cx[22]*x7 + cx[23]*y7 + cx[24]*y3*x4 + cx[25]*x2*y5 +
//          cx[26]*y2*x5 + cx[27]*x*y6 + cx[28]*y*x6 + cx[29]*x2*y3 + cx[30]*y2*x3 + cx[31]*x*y4 + cx[32]*y*x4 +
//          cx[33]*x3*y4 + cx[34]*y5 + cx[35]*x5;
//    B.By = cy[0] + cy[1]*x + cy[2]*y + cy[3]*x*y2 + cy[4]*x*y + cy[5]*x2 + cy[6]*y2 + cy[7]*x3 + cy[8]*y3 + cy[9]*y*x2 +
//          cy[10]*x*y3 + cy[11]*y*x3 + cy[12]*x2*y2 + cy[13]*x4 + cy[14]*y4 + cy[15]*x6 + cy[16]*y6 + cy[17]*x2*y4 +
//          cy[18]*y2*x4 + cy[19]*x*y5 + cy[20]*y*x5 + cy[21]*x3*y3 + cy[22]*x7 + cy[23]*y7 + cy[24]*y3*x4 + cy[25]*x2*y5 +
//          cy[26]*y2*x5 + cy[27]*x*y6 + cy[28]*y*x6 + cy[29]*x2*y3 + cy[30]*y2*x3 + cy[31]*x*y4 + cy[32]*y*x4 +
//          cy[33]*x3*y4 + cy[34]*y5 + cy[35]*x5;
//    B.Bz = cz[0] + cz[1]*x + cz[2]*y + cz[3]*x*y2 + cz[4]*x*y + cz[5]*x2 + cz[6]*y2 + cz[7]*x3 + cz[8]*y3 + cz[9]*y*x2 +
//          cz[10]*x*y3 + cz[11]*y*x3 + cz[12]*x2*y2 + cz[13]*x4 + cz[14]*y4 + cz[15]*x6 + cz[16]*y6 + cz[17]*x2*y4 +
//          cz[18]*y2*x4 + cz[19]*x*y5 + cz[20]*y*x5 + cz[21]*x3*y3 + cz[22]*x7 + cz[23]*y7 + cz[24]*y3*x4 + cz[25]*x2*y5 +
//          cz[26]*y2*x5 + cz[27]*x*y6 + cz[28]*y*x6 + cz[29]*x2*y3 + cz[30]*y2*x3 + cz[31]*x*y4 + cz[32]*y*x4 +
//          cz[33]*x3*y4 + cz[34]*y5 + cz[35]*x5;

      T x2 = x*x;
      T y2 = y*y;
      T xy = x*y;

      T x3 = x2*x;
      T y3 = y2*y;
      T xy2 = x*y2;
      T x2y = x2*y;

      T x4 = x3*x;
      T y4 = y3*y;
      T xy3 = x*y3;
      T x2y2 = x2*y2;
      T x3y = x3*y;

      T x5 = x4*x;
      T y5 = y4*y;
      T xy4 = x*y4;
      T x2y3 = x2*y3;
      T x3y2 = x3*y2;
      T x4y = x4*y;

      T x6 = x5*x;
      T y6 = y5*y;
      T xy5 = x*y5;
      T x2y4 = x2*y4;
      T x3y3 = x3*y3;
      T x4y2 = x4*y2;
      T x5y = x5*y;

      T x7 = x6*x;
      T y7 = y6*y;
      T xy6 = x*y6;
      T x2y5 = x2*y5;
      T x3y4 = x3*y4;
      T x4y3 = x4*y3;
      T x5y2 = x5*y2;
      T x6y = x6*y;

      B.Bx = cx[0]
             +cx[1]*x +cx[2]*y
             +cx[3]*x2 +cx[4]*xy +cx[5]*y2
             +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
             +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
             +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5
             +cx[21]*x6 +cx[22]*x5y +cx[23]*x4y2 +cx[24]*x3y3 +cx[25]*x2y4 +cx[26]*xy5 +cx[27]*y6
             +cx[28]*x7 +cx[29]*x6y +cx[30]*x5y2 +cx[31]*x4y3 +cx[32]*x3y4 +cx[33]*x2y5 +cx[34]*xy6 +cx[35]*y7;

      B.By = cy[0]
             +cy[1]*x +cy[2]*y
             +cy[3]*x2 +cy[4]*xy +cy[5]*y2
             +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
             +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
             +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5
             +cy[21]*x6 +cy[22]*x5y +cy[23]*x4y2 +cy[24]*x3y3 +cy[25]*x2y4 +cy[26]*xy5 +cy[27]*y6
             +cy[28]*x7 +cy[29]*x6y +cy[30]*x5y2 +cy[31]*x4y3 +cy[32]*x3y4 +cy[33]*x2y5 +cy[34]*xy6 +cy[35]*y7;

      B.Bz = cz[0]
             +cz[1]*x +cz[2]*y
             +cz[3]*x2 +cz[4]*xy +cz[5]*y2
             +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
             +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
             +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5
             +cz[21]*x6 +cz[22]*x5y +cz[23]*x4y2 +cz[24]*x3y3 +cz[25]*x2y4 +cz[26]*xy5 +cz[27]*y6
             +cz[28]*x7 +cz[29]*x6y +cz[30]*x5y2 +cz[31]*x4y3 +cz[32]*x3y4 +cz[33]*x2y5 +cz[34]*xy6 +cz[35]*y7;
#else
#if LIT_POLYNOM_DEGREE==9
//    T x2 = x*x;
//    T y2 = y*y;
//    T yx2 = y*x2;
//    T xy2 = x*y2;
//    T x3 = x2*x;
//    T y3 = y2*y;
//    T x4 = x2*x2;
//    T y4 = y2*y2;
//    T x5 = x4*x;
//    T y5 = y4*y;
//    T x6 = x4*x2;
//    T y6 = y4*y2;
//    T x7 = x4*x3;
//    T y7 = y4*y3;
//    T x8 = x4*x4;
//    T y8 = y4*y4;
//    T x9 = x5*x4;
//    T y9 = y5*y4;
//
//    B.Bx = cx[0] + cx[1]*x + cx[2]*y + cx[3]*xy2 + cx[4]*x*y + cx[5]*x2 + cx[6]*y2 + cx[7]*x3 + cx[8]*y3 + cx[9]*yx2 + cx[10]*y*x7 +
//          cx[11]*y2*x6 + cx[12]*x*y7 + cx[13]*x2*y6 + cx[14]*x2*y7 + cx[15]*x9 + cx[16]*y9 + cx[17]*x*y3 + cx[18]*y*x3 + cx[19]*x2*y2 +
//          cx[20]*x4 + cx[21]*y4 + cx[22]*x6 + cx[23]*y6 + cx[24]*x4*y4 + cx[25]*x2*y4 + cx[26]*y2*x4 + cx[27]*x*y5 + cx[28]*y*x5 +
//          cx[29]*x3*y3 + cx[30]*x7 + cx[31]*y7 + cx[32]*x3*y5 + cx[33]*y3*x4 + cx[34]*x2*y5 + cx[35]*y2*x5 + cx[36]*x*y6 + cx[37]*y*x6 +
//          cx[38]*x2*y3 + cx[39]*y2*x3 + cx[40]*x*y4 + cx[41]*y*x4 + cx[42]*x3*y4 + cx[43]*y5 + cx[44]*x5 + cx[45]*y3*x5 + cx[46]*x*y8 +
//          cx[47]*y3*x6 + cx[48]*x4*y5 + cx[49]*y4*x5 + cx[50]*x3*y6 + cx[51]*y2*x7 + cx[52]*y*x8 + cx[53]*x8 + cx[54]*y8;
//    B.By = cy[0] + cy[1]*x + cy[2]*y + cy[3]*xy2 + cy[4]*x*y + cy[5]*x2 + cy[6]*y2 + cy[7]*x3 + cy[8]*y3 + cy[9]*yx2 + cy[10]*y*x7 +
//          cy[11]*y2*x6 + cy[12]*x*y7 + cy[13]*x2*y6 + cy[14]*x2*y7 + cy[15]*x9 + cy[16]*y9 + cy[17]*x*y3 + cy[18]*y*x3 + cy[19]*x2*y2 +
//          cy[20]*x4 + cy[21]*y4 + cy[22]*x6 + cy[23]*y6 + cy[24]*x4*y4 + cy[25]*x2*y4 + cy[26]*y2*x4 + cy[27]*x*y5 + cy[28]*y*x5 +
//          cy[29]*x3*y3 + cy[30]*x7 + cy[31]*y7 + cy[32]*x3*y5 + cy[33]*y3*x4 + cy[34]*x2*y5 + cy[35]*y2*x5 + cy[36]*x*y6 + cy[37]*y*x6 +
//          cy[38]*x2*y3 + cy[39]*y2*x3 + cy[40]*x*y4 + cy[41]*y*x4 + cy[42]*x3*y4 + cy[43]*y5 + cy[44]*x5 + cy[45]*y3*x5 + cy[46]*x*y8 +
//          cy[47]*y3*x6 + cy[48]*x4*y5 + cy[49]*y4*x5 + cy[50]*x3*y6 + cy[51]*y2*x7 + cy[52]*y*x8 + cy[53]*x8 + cy[54]*y8;
//    B.Bz = cz[0] + cz[1]*x + cz[2]*y + cz[3]*xy2 + cz[4]*x*y + cz[5]*x2 + cz[6]*y2 + cz[7]*x3 + cz[8]*y3 + cz[9]*yx2 + cz[10]*y*x7 +
//          cz[11]*y2*x6 + cz[12]*x*y7 + cz[13]*x2*y6 + cz[14]*x2*y7 + cz[15]*x9 + cz[16]*y9 + cz[17]*x*y3 + cz[18]*y*x3 + cz[19]*x2*y2 +
//          cz[20]*x4 + cz[21]*y4 + cz[22]*x6 + cz[23]*y6 + cz[24]*x4*y4 + cz[25]*x2*y4 + cz[26]*y2*x4 + cz[27]*x*y5 + cz[28]*y*x5 +
//          cz[29]*x3*y3 + cz[30]*x7 + cz[31]*y7 + cz[32]*x3*y5 + cz[33]*y3*x4 + cz[34]*x2*y5 + cz[35]*y2*x5 + cz[36]*x*y6 + cz[37]*y*x6 +
//          cz[38]*x2*y3 + cz[39]*y2*x3 + cz[40]*x*y4 + cz[41]*y*x4 + cz[42]*x3*y4 + cz[43]*y5 + cz[44]*x5 + cz[45]*y3*x5 + cz[46]*x*y8 +
//          cz[47]*y3*x6 + cz[48]*x4*y5 + cz[49]*y4*x5 + cz[50]*x3*y6 + cz[51]*y2*x7 + cz[52]*y*x8 + cz[53]*x8 + cz[54]*y8;

      T x2 = x*x;
      T y2 = y*y;
      T xy = x*y;

      T x3 = x2*x;
      T y3 = y2*y;
      T xy2 = x*y2;
      T x2y = x2*y;

      T x4 = x3*x;
      T y4 = y3*y;
      T xy3 = x*y3;
      T x2y2 = x2*y2;
      T x3y = x3*y;

      T x5 = x4*x;
      T y5 = y4*y;
      T xy4 = x*y4;
      T x2y3 = x2*y3;
      T x3y2 = x3*y2;
      T x4y = x4*y;

      T x6 = x5*x;
      T y6 = y5*y;
      T xy5 = x*y5;
      T x2y4 = x2*y4;
      T x3y3 = x3*y3;
      T x4y2 = x4*y2;
      T x5y = x5*y;

      T x7 = x6*x;
      T y7 = y6*y;
      T xy6 = x*y6;
      T x2y5 = x2*y5;
      T x3y4 = x3*y4;
      T x4y3 = x4*y3;
      T x5y2 = x5*y2;
      T x6y = x6*y;

      T x8 = x7*x;
      T y8 = y7*y;
      T xy7 = x*y7;
      T x2y6 = x2*y6;
      T x3y5 = x3*y5;
      T x4y4 = x4*y4;
      T x5y3 = x5*y3;
      T x6y2 = x6*y2;
      T x7y = x7*y;

      T x9 = x8*x;
      T y9 = y8*y;
      T xy8 = x*y8;
      T x2y7 = x2*y7;
      T x3y6 = x3*y6;
      T x4y5 = x4*y5;
      T x5y4 = x5*y4;
      T x6y3 = x6*y3;
      T x7y2 = x7*y2;
      T x8y = x8*y;

      B.Bx = cx[0]
             +cx[1]*x +cx[2]*y
             +cx[3]*x2 +cx[4]*xy +cx[5]*y2
             +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
             +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
             +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5
             +cx[21]*x6 +cx[22]*x5y +cx[23]*x4y2 +cx[24]*x3y3 +cx[25]*x2y4 +cx[26]*xy5 +cx[27]*y6
             +cx[28]*x7 +cx[29]*x6y +cx[30]*x5y2 +cx[31]*x4y3 +cx[32]*x3y4 +cx[33]*x2y5 +cx[34]*xy6 +cx[35]*y7;
      +cx[36]*x8 +cx[37]*x7y +cx[38]*x6y2 +cx[39]*x5y3 +cx[40]*x4y4 +cx[41]*x3y5 +cx[42]*x2y6 +cx[43]*xy7 +cx[44]*y8
      +cx[45]*x9 +cx[46]*x8y +cx[47]*x7y2 +cx[48]*x6y3 +cx[49]*x5y4 +cx[50]*x4y5 +cx[51]*x3y6 +cx[52]*x2y7 +cx[53]*xy8 +cx[54]*y9;

      B.By = cy[0]
             +cy[1]*x +cy[2]*y
             +cy[3]*x2 +cy[4]*xy +cy[5]*y2
             +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
             +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
             +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5
             +cy[21]*x6 +cy[22]*x5y +cy[23]*x4y2 +cy[24]*x3y3 +cy[25]*x2y4 +cy[26]*xy5 +cy[27]*y6
             +cy[28]*x7 +cy[29]*x6y +cy[30]*x5y2 +cy[31]*x4y3 +cy[32]*x3y4 +cy[33]*x2y5 +cy[34]*xy6 +cy[35]*y7;
      +cy[36]*x8 +cy[37]*x7y +cy[38]*x6y2 +cy[39]*x5y3 +cy[40]*x4y4 +cy[41]*x3y5 +cy[42]*x2y6 +cy[43]*xy7 +cy[44]*y8
      +cy[45]*x9 +cy[46]*x8y +cy[47]*x7y2 +cy[48]*x6y3 +cy[49]*x5y4 +cy[50]*x4y5 +cy[51]*x3y6 +cy[52]*x2y7 +cy[53]*xy8 +cy[54]*y9;

      B.Bz = cz[0]
             +cz[1]*x +cz[2]*y
             +cz[3]*x2 +cz[4]*xy +cz[5]*y2
             +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
             +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
             +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5
             +cz[21]*x6 +cz[22]*x5y +cz[23]*x4y2 +cz[24]*x3y3 +cz[25]*x2y4 +cz[26]*xy5 +cz[27]*y6
             +cz[28]*x7 +cz[29]*x6y +cz[30]*x5y2 +cz[31]*x4y3 +cz[32]*x3y4 +cz[33]*x2y5 +cz[34]*xy6 +cz[35]*y7;
      +cz[36]*x8 +cz[37]*x7y +cz[38]*x6y2 +cz[39]*x5y3 +cz[40]*x4y4 +cz[41]*x3y5 +cz[42]*x2y6 +cz[43]*xy7 +cz[44]*y8
      +cz[45]*x9 +cz[46]*x8y +cz[47]*x7y2 +cz[48]*x6y3 +cz[49]*x5y4 +cz[50]*x4y5 +cz[51]*x3y6 +cz[52]*x2y7 +cz[53]*xy8 +cz[54]*y9;
#endif
#endif
#endif
#endif
   }

   T cx[N], cy[N], cz[N]; // polinom coefficients
   T Z; // Z position of the slice

   friend std::ostream& operator<<(std::ostream& strm, const LitFieldSlice& slice) {
      strm << "LitFieldSlice: Z=" << slice.Z << ", cx=";
      for(unsigned int i = 0; i < N; i++) { strm << slice.cx[i] << " "; }
      strm << std::endl;
      strm << "LitFieldSlice: cy=";
      for(unsigned int i = 0; i < N; i++) { strm << slice.cy[i] << " "; }
      strm << std::endl;
      strm << "LitFieldSlice: cz=";
      for(unsigned int i = 0; i < N; i++) { strm << slice.cz[i] << " "; }
      strm << std::endl;
      return strm;
   }

   std::string ToStringShort() {
      std::string str = lit::ToString<T>(Z) + "\n";
      for(unsigned int i = 0; i < N; i++) { str += lit::ToString<T>(cx[i]) + " "; }
      str += "\n";
      for(unsigned int i = 0; i < N; i++) { str += lit::ToString<T>(cy[i]) + " "; }
      str += "\n";
      for(unsigned int i = 0; i < N; i++) { str += lit::ToString<T>(cz[i]) + " "; }
      str += "\n";
      return str;
   }

} _fvecalignment;



template<class T>
class LitFieldRegion
{
public:
   void Set(
      const LitFieldValue<T> &B0,
      const T B0z,
      const LitFieldValue<T> &B1,
      const T B1z,
      const LitFieldValue<T> &B2,
      const T B2z ) {
      z0 = B0z[0];
      T dz1 = B1z - B0z, dz2 = B2z - B0z;
      T det = rcp(T(dz1 * dz2 * (dz2 - dz1)));
      T w21 = -dz2 * det;
      T w22 = dz1 * det;
      T w11 = -dz2 * w21;
      T w12 = -dz1 * w22;

      T dB1 = B1.Bx - B0.Bx;
      T dB2 = B2.Bx - B0.Bx;
      cx0 = B0.Bx;
      cx1 = dB1 * w11 + dB2 * w12;
      cx2 = dB1 * w21 + dB2 * w22;

      dB1 = B1.By - B0.By;
      dB2 = B2.By - B0.By;
      cy0 = B0.By;
      cy1 = dB1 * w11 + dB2 * w12;
      cy2 = dB1 * w21 + dB2 * w22;

      dB1 = B1.Bz - B0.Bz;
      dB2 = B2.Bz - B0.Bz;
      cz0 = B0.Bz;
      cz1 = dB1 * w11 + dB2 * w12;
      cz2 = dB1 * w21 + dB2 * w22;
   }

   void Set(
      const LitFieldValue<T>& B0,
      const T B0z,
      const LitFieldValue<T>& B1,
      const T B1z) {
      z0 = B0z;
      T dzi = rcp(T(B1z - B0z));
      cx0 = B0.Bx;
      cy0 = B0.By;
      cz0 = B0.Bz;
      cx1 = (B1.Bx - B0.Bx) * dzi;
      cy1 = (B1.By - B0.By) * dzi;
      cz1 = (B1.Bz - B0.Bz) * dzi;
      cx2 = cy2 = cz2 = 0;
   }

   void Shift(
      T z) {
      T dz = z-z0;
      T cx2dz = cx2*dz;
      T cy2dz = cy2*dz;
      T cz2dz = cz2*dz;
      z0 = z[0];
      cx0+= ( cx1 + cx2dz )*dz;
      cy0+= ( cy1 + cy2dz )*dz;
      cz0+= ( cz1 + cz2dz )*dz;
      cx1+= cx2dz + cx2dz;
      cy1+= cy2dz + cy2dz;
      cz1+= cz2dz + cz2dz;
   }

   void GetFieldValue(
      const T& z,
      LitFieldValue<T> &B) const {
      T dz = z - z0;
      T dzdz = dz * dz;
      B.Bx = cx0 + cx1 * dz + cx2 * dzdz;
      B.By = cy0 + cy1 * dz + cy2 * dzdz;
      B.Bz = cz0 + cz1 * dz + cz2 * dzdz;
   }

   T cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
   T cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
   T cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
   T z0;
} _fvecalignment;


#endif /* LITFIELD_H_ */
