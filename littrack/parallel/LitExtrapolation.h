/** LitExtrapolation.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **
 ** Functions for track parameters extrapolation for parallel version of the tracking.
 **/

#ifndef LITEXTRAPOLATION_H_
#define LITEXTRAPOLATION_H_

#include "LitTrackParam.h"
#include "LitField.h"

//#include "CbmLitField.h"

namespace lit {
namespace parallel {

/* Line track extrapolation for parallel version of the tracking.
 *@param par Input/Output track parameters.
 *@param zOut Z position to extrapolate to.
 */
template<class T>
inline void LitLineExtrapolation(
   LitTrackParam<T>& par,
   T zOut)
{
   T dz = zOut - par.Z;

   // transport state vector F*X*F.T()
   par.X += dz * par.Tx;
   par.Y += dz * par.Ty;

   // transport covariance matrix F*C*F.T()
   T t3 = par.C2 + dz * par.C9;
   T t7 = dz * par.C10;
   T t8 = par.C3 + t7;
   T t19 = par.C7 + dz * par.C12;
   par.C0 += dz * par.C2 + t3 * dz;
   par.C1 += dz * par.C6 + t8 * dz;
   par.C2 = t3;
   par.C3 = t8;
   par.C4 += dz * par.C11;
   par.C5 += dz * par.C7 + t19 * dz;
   par.C6 += t7;
   par.C7 = t19;
   par.C8 += dz * par.C13;

   par.Z = zOut;
}

/* Runge-Kutta track extrapolation for parallel version of the tracking.
 *@param par Input/Output track parameters.
 *@param zOut Z position to extrapolate to.
 *@param field Reference to the field region.
 */
template<class T>
inline void LitRK4Extrapolation(
   LitTrackParam<T>& par,
   T zOut,
   const LitFieldRegion<T>& field)
{
   static const T fC = 0.000299792458;
   static const T ZERO = 0., ONE = 1., TWO = 2., C1_3 = 1./3., C1_6 = 1./6.;

   T coef[4] = {0.0, 0.5, 0.5, 1.0};

   T Ax[4], Ay[4];
   T dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
   T kx[4];
   T ky[4];
   T ktx[4];
   T kty[4];

   T zIn = par.Z;
   T h = zOut - zIn;
   T hC   = h * fC;
   T hCqp = h * fC * par.Qp;
   T x0[4];

   T x[4] = {par.X, par.Y, par.Tx, par.Ty};

   T F[25]; // derivatives, transport matrix

   // Get field values
   LitFieldValue<T> B[4];
   field.GetFieldValue(zIn + coef[0] * h, B[0]);
   field.GetFieldValue(zIn + coef[1] * h, B[1]);
   B[2] = B[1];
   field.GetFieldValue(zIn + coef[3] * h, B[3]);

   // Calculation for zero step
   {
      T Bx = B[0].Bx;
      T By = B[0].By;
      T Bz = B[0].Bz;

      T tx = x[2];
      T ty = x[3];
      T txtx = tx * tx;
      T tyty = ty * ty;
      T txty = tx * ty;
      T txtxtyty1 = ONE + txtx + tyty;
      T t1 = sqrt(txtxtyty1);
      T t2 = rcp(txtxtyty1); //1.0 / txtxtyty1;

      Ax[0] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
      Ay[0] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;

      dAx_dtx[0] = Ax[0] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
      dAx_dty[0] = Ax[0] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[0] = Ay[0] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[0] = Ay[0] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;

      kx[0] = tx * h;
      ky[0] = ty * h;
      ktx[0] = Ax[0] * hCqp;
      kty[0] = Ay[0] * hCqp;
   }
   // end calculation for zero step

   // Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 1
      x[0] = par.X  + coef[iStep] * kx[iStep - 1];
      x[1] = par.Y  + coef[iStep] * ky[iStep - 1];
      x[2] = par.Tx + coef[iStep] * ktx[iStep - 1];
      x[3] = par.Ty + coef[iStep] * kty[iStep - 1];

      T Bx = B[iStep].Bx;
      T By = B[iStep].By;
      T Bz = B[iStep].Bz;

      T tx = x[2];
      T ty = x[3];
      T txtx = tx * tx;
      T tyty = ty * ty;
      T txty = tx * ty;
      T txtxtyty1 = ONE + txtx + tyty;
      T t1 = sqrt(txtxtyty1);
      T t2 = rcp(txtxtyty1); //1.0 / txtxtyty1;

      Ax[iStep] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
      Ay[iStep] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;

      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;

      kx[iStep] = tx * h;
      ky[iStep] = ty * h;
      ktx[iStep] = Ax[iStep] * hCqp;
      kty[iStep] = Ay[iStep] * hCqp;

   } // 1

   // Calculate output state vector
//   for (unsigned int i = 0; i < 4; i++) xOut[i] = CalcOut(xIn[i], k[i]);
   par.X  += kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   par.Y  += ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   par.Tx += ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   par.Ty += kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
//   xOut[4] = xIn[4];


   // Calculation of the derivatives

   // derivatives dx/dx and dx/dy
   // dx/dx
   F[0] = ONE;
   F[5] = ZERO;
   F[10] = ZERO;
   F[15] = ZERO;
   F[20] = ZERO;
   // dx/dy
   F[1] = ZERO;
   F[6] = ONE;
   F[11] = ZERO;
   F[16] = ZERO;
   F[21] = ZERO;
   // end of derivatives dx/dx and dx/dy

   // Derivatives dx/tx
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ONE;
   x[3] = x0[3] = ZERO;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   //ktx[0] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
   kty[0] = (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]) * hCqp;
   // Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 2
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
//         x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      //ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   } // 2

   F[2]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[7]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[12] = ONE;
   F[17] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
   F[22] = ZERO;
   // end of derivatives dx/dtx

   // Derivatives    dx/ty
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ZERO;
   x[3] = x0[3] = ONE;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   ktx[0] = (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]) * hCqp;
   //kty[0] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   //Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
      x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
//         x[3] = x0[0] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      //kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   }  // 4

   F[3]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;// CalcOut(x0[0], k[0]);
   F[8]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;//CalcOut(x0[1], k[1]);
   F[13] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;//CalcOut(x0[2], k[2]);
   F[18] = ONE;
   F[23] = ZERO;
   // end of derivatives dx/dty

   // Derivatives dx/dqp
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ZERO;
   x[3] = x0[3] = ZERO;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   ktx[0] = Ax[0] * hC + hCqp * (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]);
   kty[0] = Ay[0] * hC + hCqp * (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]);
   //Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
      x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      ktx[iStep] = Ax[iStep] * hC + hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
      kty[iStep] = Ay[iStep] * hC + hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
   }  // 4

   F[4]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[9]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[14] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   F[19] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
   F[24] = 1.;
   // end of derivatives dx/dqp

   // end calculation of the derivatives


   // Transport C matrix
   {
      T cIn[15] = {par.C0,  par.C1,  par.C2,  par.C3,  par.C4,
                   par.C5,  par.C6,  par.C7,  par.C8,  par.C9,
                   par.C10, par.C11, par.C12, par.C13, par.C14
                  };
      // F*C*Ft
      T A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
      T B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
      T C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

      T D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
      T E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
      T G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

      T H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
      T I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
      T J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

      T K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

      par.C0 = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
      par.C1 = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
      par.C2 = A + B * F[13] + C * F[14];
      par.C3 = B + A * F[17] + C * F[19];
      par.C4 = C;

      par.C5 = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
      par.C6 = D + E * F[13] + G * F[14];
      par.C7 = E + D * F[17] + G * F[19];
      par.C8 = G;

      par.C9 = H + I * F[13] + J * F[14];
      par.C10 = I + H * F[17] + J * F[19];
      par.C11 = J;

      par.C12 = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
      par.C13 = K;

      par.C14 = cIn[14];
   }
   //end transport C matrix

   par.Z = zOut;
}





template<class T>
inline void LitRK4ExtrapolationElectron(
   LitTrackParam<T>& par,
   T zOut,
//    const CbmLitField* fField)
//    const LitFieldValue<T>& value1,
//    const LitFieldValue<T>& value2,
//    const LitFieldValue<T>& value3)
   const LitFieldSlice<T>& slice1,
   const LitFieldSlice<T>& slice2,
   const LitFieldSlice<T>& slice3)
{
   static const T fC = 0.000299792458;
   static const T ZERO = 0., ONE = 1., TWO = 2., C1_3 = 1./3., C1_6 = 1./6.;

   T coef[4] = {0.0, 0.5, 0.5, 1.0};

   T Ax[4], Ay[4];
   T dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
   T kx[4];
   T ky[4];
   T ktx[4];
   T kty[4];

   T zIn = par.Z;
   T h = zOut - zIn;
   T hC   = h * fC;
   T hCqp = h * fC * par.Qp;
   T x0[4];

   T x[4] = {par.X, par.Y, par.Tx, par.Ty};

   T F[25]; // derivatives, transport matrix

   // Get field values
//   LitFieldValue<T> B[4];
//   field.GetFieldValue(zIn + coef[0] * h, B[0]);
//   field.GetFieldValue(zIn + coef[1] * h, B[1]);
//   B[2] = B[1];
//   field.GetFieldValue(zIn + coef[3] * h, B[3]);
//   B[0] = value1;
//   B[1] = value2;
//   B[2] = value2;
//   B[3] = value3;

   LitFieldSlice<T> slices[4] = {slice1, slice2, slice2, slice3};

   // Calculation for zero step
   {
//   T Bx = B[0].Bx;
//   T By = B[0].By;
//   T Bz = B[0].Bz;

//   myf tbx, tby, tbz, ttx = x[0], tty = x[1], ttz = zIn + coef[0] * h;
//   fField->GetFieldValue(ttx, tty, ttz, tbx, tby, tbz);
//   T Bx = tbx, By = tby, Bz = tbz;

      LitFieldValue<T> v1;
      slices[0].GetFieldValue(x[0], x[1], v1);
      T Bx = v1.Bx;
      T By = v1.By;
      T Bz = v1.Bz;

      T tx = x[2];
      T ty = x[3];
      T txtx = tx * tx;
      T tyty = ty * ty;
      T txty = tx * ty;
      T txtxtyty1 = ONE + txtx + tyty;
      T t1 = sqrt(txtxtyty1);
      T t2 = rcp(txtxtyty1); //1.0 / txtxtyty1;

      Ax[0] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
      Ay[0] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;

      dAx_dtx[0] = Ax[0] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
      dAx_dty[0] = Ax[0] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[0] = Ay[0] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[0] = Ay[0] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;

      kx[0] = tx * h;
      ky[0] = ty * h;
      ktx[0] = Ax[0] * hCqp;
      kty[0] = Ay[0] * hCqp;
   }
   // end calculation for zero step

   // Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 1
      x[0] = par.X  + coef[iStep] * kx[iStep - 1];
      x[1] = par.Y  + coef[iStep] * ky[iStep - 1];
      x[2] = par.Tx + coef[iStep] * ktx[iStep - 1];
      x[3] = par.Ty + coef[iStep] * kty[iStep - 1];

//      T Bx = B[iStep].Bx;
//      T By = B[iStep].By;
//      T Bz = B[iStep].Bz;
//      myf Bx, By, Bz;
//      fField->GetFieldValue(x[0], x[1], zIn + coef[iStep] * h, Bx, By, Bz);

//      myf tbx, tby, tbz, ttx = x[0], tty = x[1], ttz = zIn + coef[iStep] * h;
//      fField->GetFieldValue(ttx, tty, ttz, tbx, tby, tbz);
//      T Bx = tbx, By = tby, Bz = tbz;

      LitFieldValue<T> v1;
      slices[iStep].GetFieldValue(x[0], x[1], v1);
      T Bx = v1.Bx;
      T By = v1.By;
      T Bz = v1.Bz;

      T tx = x[2];
      T ty = x[3];
      T txtx = tx * tx;
      T tyty = ty * ty;
      T txty = tx * ty;
      T txtxtyty1 = ONE + txtx + tyty;
      T t1 = sqrt(txtxtyty1);
      T t2 = rcp(txtxtyty1); //1.0 / txtxtyty1;

      Ax[iStep] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
      Ay[iStep] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;

      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;

      kx[iStep] = tx * h;
      ky[iStep] = ty * h;
      ktx[iStep] = Ax[iStep] * hCqp;
      kty[iStep] = Ay[iStep] * hCqp;

   } // 1

   // Calculate output state vector
//   for (unsigned int i = 0; i < 4; i++) xOut[i] = CalcOut(xIn[i], k[i]);
   par.X  += kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   par.Y  += ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   par.Tx += ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   par.Ty += kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
//   xOut[4] = xIn[4];


   // Calculation of the derivatives

   // derivatives dx/dx and dx/dy
   // dx/dx
   F[0] = ONE;
   F[5] = ZERO;
   F[10] = ZERO;
   F[15] = ZERO;
   F[20] = ZERO;
   // dx/dy
   F[1] = ZERO;
   F[6] = ONE;
   F[11] = ZERO;
   F[16] = ZERO;
   F[21] = ZERO;
   // end of derivatives dx/dx and dx/dy

   // Derivatives dx/tx
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ONE;
   x[3] = x0[3] = ZERO;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   //ktx[0] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
   kty[0] = (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]) * hCqp;
   // Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 2
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
//         x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      //ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   } // 2

   F[2]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[7]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[12] = ONE;
   F[17] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
   F[22] = ZERO;
   // end of derivatives dx/dtx

   // Derivatives    dx/ty
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ZERO;
   x[3] = x0[3] = ONE;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   ktx[0] = (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]) * hCqp;
   //kty[0] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   //Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
      x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
//         x[3] = x0[0] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      //kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   }  // 4

   F[3]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;// CalcOut(x0[0], k[0]);
   F[8]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;//CalcOut(x0[1], k[1]);
   F[13] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;//CalcOut(x0[2], k[2]);
   F[18] = ONE;
   F[23] = ZERO;
   // end of derivatives dx/dty

   // Derivatives dx/dqp
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ZERO;
   x[3] = x0[3] = ZERO;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   ktx[0] = Ax[0] * hC + hCqp * (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]);
   kty[0] = Ay[0] * hC + hCqp * (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]);
   //Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
      x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      ktx[iStep] = Ax[iStep] * hC + hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
      kty[iStep] = Ay[iStep] * hC + hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
   }  // 4

   F[4]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[9]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[14] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   F[19] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
   F[24] = 1.;
   // end of derivatives dx/dqp

   // end calculation of the derivatives


   // Transport C matrix
   {
      T cIn[15] = {par.C0,  par.C1,  par.C2,  par.C3,  par.C4,
                   par.C5,  par.C6,  par.C7,  par.C8,  par.C9,
                   par.C10, par.C11, par.C12, par.C13, par.C14
                  };
      // F*C*Ft
      T A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
      T B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
      T C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

      T D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
      T E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
      T G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

      T H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
      T I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
      T J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

      T K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

      par.C0 = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
      par.C1 = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
      par.C2 = A + B * F[13] + C * F[14];
      par.C3 = B + A * F[17] + C * F[19];
      par.C4 = C;

      par.C5 = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
      par.C6 = D + E * F[13] + G * F[14];
      par.C7 = E + D * F[17] + G * F[19];
      par.C8 = G;

      par.C9 = H + I * F[13] + J * F[14];
      par.C10 = I + H * F[17] + J * F[19];
      par.C11 = J;

      par.C12 = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
      par.C13 = K;

      par.C14 = cIn[14];
   }
   //end transport C matrix

   par.Z = zOut;
}


//
//template<class T>
//inline void LitRK4ExtrapolationElectron1(
//    LitTrackParam<T>& par,
//    T zOut,
//    const CbmLitField* fField)
////     const LitFieldValue<T>& value1,
////     const LitFieldValue<T>& value2,
////     const LitFieldValue<T>& value3)
////     const LitFieldSlice<T>& slice1,
////     const LitFieldSlice<T>& slice2,
////     const LitFieldSlice<T>& slice3)
//{
//   static const T fC = 0.000299792458;
//   static const T ZERO = 0., ONE = 1., TWO = 2., C1_3 = 1./3., C1_6 = 1./6.;
//
//   T coef[4] = {0.0, 0.5, 0.5, 1.0};
//
//   T Ax[4], Ay[4];
//   T dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
//   T kx[4];
//   T ky[4];
//   T ktx[4];
//   T kty[4];
//
//   T zIn = par.Z;
//   T h = zOut - zIn;
//   T hC   = h * fC;
//   T hCqp = h * fC * par.Qp;
//   T x0[4];
//
//   T x[4] = {par.X, par.Y, par.Tx, par.Ty};
//
//   T F[25]; // derivatives, transport matrix
//
//   // Get field values
////   LitFieldValue<T> B[4];
////   field.GetFieldValue(zIn + coef[0] * h, B[0]);
////   field.GetFieldValue(zIn + coef[1] * h, B[1]);
////   B[2] = B[1];
////   field.GetFieldValue(zIn + coef[3] * h, B[3]);
////   B[0] = value1;
////   B[1] = value2;
////   B[2] = value2;
////   B[3] = value3;
//
////   LitFieldSlice<T> slices[4] = {slice1, slice2, slice2, slice3};
//
//   // Calculation for zero step
//   {
////   T Bx = B[0].Bx;
////   T By = B[0].By;
////   T Bz = B[0].Bz;
//
//   myf tbx, tby, tbz, ttx = x[0], tty = x[1], ttz = zIn + coef[0] * h;
//   fField->GetFieldValue(ttx, tty, ttz, tbx, tby, tbz);
//   T Bx = tbx, By = tby, Bz = tbz;
//
////     LitFieldValue<T> v1;
////     slices[0].GetFieldValue(x[0], x[1], v1);
////     T Bx = v1.Bx;
////     T By = v1.By;
////     T Bz = v1.Bz;
//
//   T tx = x[2];
//   T ty = x[3];
//   T txtx = tx * tx;
//   T tyty = ty * ty;
//   T txty = tx * ty;
//   T txtxtyty1 = ONE + txtx + tyty;
//   T t1 = sqrt(txtxtyty1);
//   T t2 = rcp(txtxtyty1); //1.0 / txtxtyty1;
//
//   Ax[0] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
//   Ay[0] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;
//
//   dAx_dtx[0] = Ax[0] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
//   dAx_dty[0] = Ax[0] * ty * t2 + ( tx * Bx + Bz ) * t1;
//   dAy_dtx[0] = Ay[0] * tx * t2 + (-ty * By - Bz ) * t1;
//   dAy_dty[0] = Ay[0] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;
//
//   kx[0] = tx * h;
//   ky[0] = ty * h;
//   ktx[0] = Ax[0] * hCqp;
//   kty[0] = Ay[0] * hCqp;
//   }
//   // end calculation for zero step
//
//   // Calculate for steps starting from 1
//   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 1
//      x[0] = par.X  + coef[iStep] * kx[iStep - 1];
//      x[1] = par.Y  + coef[iStep] * ky[iStep - 1];
//      x[2] = par.Tx + coef[iStep] * ktx[iStep - 1];
//      x[3] = par.Ty + coef[iStep] * kty[iStep - 1];
//
////      T Bx = B[iStep].Bx;
////      T By = B[iStep].By;
////      T Bz = B[iStep].Bz;
////      myf Bx, By, Bz;
////      fField->GetFieldValue(x[0], x[1], zIn + coef[iStep] * h, Bx, By, Bz);
//
//      myf tbx, tby, tbz, ttx = x[0], tty = x[1], ttz = zIn + coef[iStep] * h;
//      fField->GetFieldValue(ttx, tty, ttz, tbx, tby, tbz);
//      T Bx = tbx, By = tby, Bz = tbz;
//
////     LitFieldValue<T> v1;
////     slices[iStep].GetFieldValue(x[0], x[1], v1);
////     T Bx = v1.Bx;
////     T By = v1.By;
////     T Bz = v1.Bz;
//
//      T tx = x[2];
//      T ty = x[3];
//      T txtx = tx * tx;
//      T tyty = ty * ty;
//      T txty = tx * ty;
//      T txtxtyty1 = ONE + txtx + tyty;
//      T t1 = sqrt(txtxtyty1);
//      T t2 = rcp(txtxtyty1); //1.0 / txtxtyty1;
//
//      Ax[iStep] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
//      Ay[iStep] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;
//
//      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
//      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * Bx + Bz ) * t1;
//      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz ) * t1;
//      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;
//
//      kx[iStep] = tx * h;
//      ky[iStep] = ty * h;
//      ktx[iStep] = Ax[iStep] * hCqp;
//      kty[iStep] = Ay[iStep] * hCqp;
//
//   } // 1
//
//   // Calculate output state vector
////   for (unsigned int i = 0; i < 4; i++) xOut[i] = CalcOut(xIn[i], k[i]);
//   par.X  += kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
//   par.Y  += ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
//   par.Tx += ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
//   par.Ty += kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
////   xOut[4] = xIn[4];
//
//
//   // Calculation of the derivatives
//
//   // derivatives dx/dx and dx/dy
//   // dx/dx
//   F[0] = ONE;
//   F[5] = ZERO;
//   F[10] = ZERO;
//   F[15] = ZERO;
//   F[20] = ZERO;
//   // dx/dy
//   F[1] = ZERO;
//   F[6] = ONE;
//   F[11] = ZERO;
//   F[16] = ZERO;
//   F[21] = ZERO;
//   // end of derivatives dx/dx and dx/dy
//
//   // Derivatives dx/tx
//   x[0] = x0[0] = ZERO;
//   x[1] = x0[1] = ZERO;
//   x[2] = x0[2] = ONE;
//   x[3] = x0[3] = ZERO;
//
//   //Calculate for zero step
//   kx[0] = x[2] * h;
//   ky[0] = x[3] * h;
//   //ktx[0] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
//   kty[0] = (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]) * hCqp;
//   // Calculate for steps starting from 1
//   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 2
//      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
//      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
////       x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
//      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];
//
//      kx[iStep] = x[2] * h;
//      ky[iStep] = x[3] * h;
//      //ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
//      kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
//   } // 2
//
//   F[2]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
//   F[7]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
//   F[12] = ONE;
//   F[17] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
//   F[22] = ZERO;
//   // end of derivatives dx/dtx
//
//   // Derivatives    dx/ty
//   x[0] = x0[0] = ZERO;
//   x[1] = x0[1] = ZERO;
//   x[2] = x0[2] = ZERO;
//   x[3] = x0[3] = ONE;
//
//   //Calculate for zero step
//   kx[0] = x[2] * h;
//   ky[0] = x[3] * h;
//   ktx[0] = (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]) * hCqp;
//   //kty[0] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
//   //Calculate for steps starting from 1
//   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
//    x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
//    x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
//    x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
////       x[3] = x0[0] + coef[iStep] * kty[iStep - 1];
//
//    kx[iStep] = x[2] * h;
//    ky[iStep] = x[3] * h;
//    ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
//    //kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
//   }  // 4
//
//   F[3]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;// CalcOut(x0[0], k[0]);
//   F[8]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;//CalcOut(x0[1], k[1]);
//   F[13] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;//CalcOut(x0[2], k[2]);
//   F[18] = ONE;
//   F[23] = ZERO;
//   // end of derivatives dx/dty
//
//   // Derivatives dx/dqp
//   x[0] = x0[0] = ZERO;
//   x[1] = x0[1] = ZERO;
//   x[2] = x0[2] = ZERO;
//   x[3] = x0[3] = ZERO;
//
//   //Calculate for zero step
//   kx[0] = x[2] * h;
//   ky[0] = x[3] * h;
//   ktx[0] = Ax[0] * hC + hCqp * (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]);
//   kty[0] = Ay[0] * hC + hCqp * (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]);
//   //Calculate for steps starting from 1
//   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
//    x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
//    x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
//    x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
//    x[3] = x0[3] + coef[iStep] * kty[iStep - 1];
//
//    kx[iStep] = x[2] * h;
//    ky[iStep] = x[3] * h;
//    ktx[iStep] = Ax[iStep] * hC + hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
//    kty[iStep] = Ay[iStep] * hC + hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
//   }  // 4
//
//   F[4]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
//   F[9]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
//   F[14] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
//   F[19] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
//   F[24] = 1.;
//   // end of derivatives dx/dqp
//
//   // end calculation of the derivatives
//
//
//   // Transport C matrix
//   {
//   T cIn[15] = {par.C0,  par.C1,  par.C2,  par.C3,  par.C4,
//               par.C5,  par.C6,  par.C7,  par.C8,  par.C9,
//               par.C10, par.C11, par.C12, par.C13, par.C14};
// // F*C*Ft
// T A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
// T B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
// T C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];
//
// T D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
// T E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
// T G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];
//
// T H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
// T I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
// T J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];
//
// T K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];
//
// par.C0 = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
// par.C1 = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
// par.C2 = A + B * F[13] + C * F[14];
// par.C3 = B + A * F[17] + C * F[19];
// par.C4 = C;
//
// par.C5 = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
// par.C6 = D + E * F[13] + G * F[14];
// par.C7 = E + D * F[17] + G * F[19];
// par.C8 = G;
//
// par.C9 = H + I * F[13] + J * F[14];
// par.C10 = I + H * F[17] + J * F[19];
// par.C11 = J;
//
// par.C12 = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
// par.C13 = K;
//
// par.C14 = cIn[14];
//   }
// //end transport C matrix
//
// par.Z = zOut;
//}

} // namespace parallel
} // namespace lit
#endif /* LITEXTRAPOLATION_H_ */
