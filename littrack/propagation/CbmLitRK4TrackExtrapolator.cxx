#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitEnvironment.h"

#include <cmath>

CbmLitRK4TrackExtrapolator::CbmLitRK4TrackExtrapolator():
   CbmLitTrackExtrapolator("CbmLitRK4TrackExtrapolator")
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fMagneticField = env->GetField();
   fF.resize(25);
}

CbmLitRK4TrackExtrapolator::~CbmLitRK4TrackExtrapolator()
{
}

LitStatus CbmLitRK4TrackExtrapolator::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitRK4TrackExtrapolator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitRK4TrackExtrapolator::Extrapolate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        myf zOut)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut);
}

LitStatus CbmLitRK4TrackExtrapolator::Extrapolate(
		CbmLitTrackParam *par,
        myf zOut)
{
   myf zIn = par->GetZ();

   std::vector<myf> xIn = par->GetStateVector();
   std::vector<myf> xOut(5, 0.);
   fF.assign(25, 0.);

   RK4Order(xIn, zIn, xOut, zOut, fF);

   std::vector<myf> cIn = par->GetCovMatrix();
   std::vector<myf> cOut(15);
   TransportC(cIn, fF, cOut);

   par->SetStateVector(xOut);
   par->SetCovMatrix(cOut);
   par->SetZ(zOut);

   return kLITSUCCESS;
}

void CbmLitRK4TrackExtrapolator::TransportMatrix(
		   std::vector<myf>& F)
{
	F.assign(fF.begin(), fF.end());
}

void CbmLitRK4TrackExtrapolator::RK4Order(
		const std::vector<myf>& xIn,
		myf zIn,
		std::vector<myf>& xOut,
        myf zOut,
        std::vector<myf>& derivs) const
{
   const myf fC = 0.000299792458;

   myf coef[4] = {0.0, 0.5, 0.5, 1.0};

   myf Ax[4], Ay[4];
   myf dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
   myf k[4][4];

   myf h = zOut - zIn;
   myf hC   = h * fC;
   myf hCqp = h * fC * xIn[4];
   myf x0[4];

   myf x[4] = {xIn[0], xIn[1], xIn[2], xIn[3]};

   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 1
      if (iStep > 0) {
    	 for(unsigned int i = 0; i < 4; i++) {
            x[i] = xIn[i] + coef[iStep] * k[i][iStep - 1];
         }
      }

      //TODO:: change doouble to myf here
      double pos[3] = {x[0], x[1], zIn + coef[iStep] * h};
      double B[3];
      fMagneticField->GetFieldValue(pos, B);

      myf tx = x[2];
      myf ty = x[3];
      myf txtx = tx * tx;
      myf tyty = ty * ty;
      myf txty = tx * ty;
      myf txtxtyty1= 1.0 + txtx + tyty;
      myf t1 = std::sqrt(txtxtyty1);
      myf t2 = 1.0 / txtxtyty1;

      Ax[iStep] = ( txty * B[0] + ty * B[2] - ( 1.0 + txtx ) * B[1] ) * t1;
      Ay[iStep] = (-txty * B[1] - tx * B[2] + ( 1.0 + tyty ) * B[0] ) * t1;

      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * B[0] - 2.0 * tx * B[1] ) * t1;
      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * B[0] + B[2] ) * t1;
      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * B[1] - B[2] ) * t1;
      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * B[1] + 2.0 * ty * B[0] ) * t1;

      k[0][iStep] = tx * h;
      k[1][iStep] = ty * h;
      k[2][iStep] = Ax[iStep] * hCqp;
      k[3][iStep] = Ay[iStep] * hCqp;

   } // 1

   for (unsigned int i = 0; i < 4; i++) xOut[i] = CalcOut(xIn[i], k[i]);
   xOut[4] = xIn[4];

   // Calculation of the derivatives

   // derivatives dx/dx and dx/dy
   // dx/dx
   derivs[0] = 1.;
   derivs[5] = 0.;
   derivs[10] = 0.;
   derivs[15] = 0.;
   derivs[20] = 0.;
   // dx/dy
   derivs[1] = 0.;
   derivs[6] = 1.;
   derivs[11] = 0.;
   derivs[16] = 0.;
   derivs[21] = 0.;
   // end of derivatives dx/dx and dx/dy

   // Derivatives dx/tx
   x[0] = x0[0] = 0.0;
   x[1] = x0[1] = 0.0;
   x[2] = x0[2] = 1.0;
   x[3] = x0[3] = 0.0;
   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 2
      if (iStep > 0) {
         for (unsigned int i = 0; i < 4; i++) {
            if (i != 2) {
               x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
            }
         }
      }

      k[0][iStep] = x[2] * h;
      k[1][iStep] = x[3] * h;
      //k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   } // 2

   derivs[2] = CalcOut(x0[0], k[0]);
   derivs[7] = CalcOut(x0[1], k[1]);
   derivs[12] = 1.0;
   derivs[17] = CalcOut(x0[3], k[3]);
   derivs[22] = 0.0;
   // end of derivatives dx/dtx

   // Derivatives    dx/ty
   x[0] = x0[0] = 0.0;
   x[1] = x0[1] = 0.0;
   x[2] = x0[2] = 0.0;
   x[3] = x0[3] = 1.0;
   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 4
      if (iStep > 0) {
         for(unsigned int i = 0; i < 4; i++) {
            if(i!=3) {
               x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
            }
         }
      }

      k[0][iStep] = x[2] * h;
      k[1][iStep] = x[3] * h;
      k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      //k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   }  // 4

   derivs[3] = CalcOut(x0[0], k[0]);
   derivs[8] = CalcOut(x0[1], k[1]);
   derivs[13] = CalcOut(x0[2], k[2]);
   derivs[18] = 1.;
   derivs[23] = 0.;
   // end of derivatives dx/dty

   // Derivatives dx/dqp
   x[0] = x0[0] = 0.0;
   x[1] = x0[1] = 0.0;
   x[2] = x0[2] = 0.0;
   x[3] = x0[3] = 0.0;
   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 4
      if (iStep > 0) {
         for(unsigned int i = 0; i < 4; i++) {
            x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
         }
      }

      k[0][iStep] = x[2] * h;
      k[1][iStep] = x[3] * h;
      k[2][iStep] = Ax[iStep] * hC +
                     hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
      k[3][iStep] = Ay[iStep] * hC +
                     hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
   }  // 4

   derivs[4] = CalcOut(x0[0], k[0]);
   derivs[9] = CalcOut(x0[1], k[1]);
   derivs[14] = CalcOut(x0[2], k[2]);
   derivs[19] = CalcOut(x0[3], k[3]);
   derivs[24] = 1.;
   // end of derivatives dx/dqp

   // end calculation of the derivatives
}

myf CbmLitRK4TrackExtrapolator::CalcOut(
		myf in,
		const myf k[4]) const
{
	return in + k[0]/6. + k[1]/3. + k[2]/3. + k[3]/6.;
}

void CbmLitRK4TrackExtrapolator::TransportC(
		const std::vector<myf>& cIn,
		const std::vector<myf>& F,
		std::vector<myf>& cOut) const
{
	// F*C*Ft
	myf A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
	myf B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
	myf C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

	myf D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
	myf E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
	myf G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

	myf H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
	myf I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
	myf J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

	myf K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

	cOut[0] = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
	cOut[1] = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
	cOut[2] = A + B * F[13] + C * F[14];
	cOut[3] = B + A * F[17] + C * F[19];
	cOut[4] = C;

	cOut[5] = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
	cOut[6] = D + E * F[13] + G * F[14];
	cOut[7] = E + D * F[17] + G * F[19];
	cOut[8] = G;

	cOut[9] = H + I * F[13] + J * F[14];
	cOut[10] = I + H * F[17] + J * F[19];
	cOut[11] = J;

	cOut[12] = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
	cOut[13] = K;

	cOut[14] = cIn[14];
}
