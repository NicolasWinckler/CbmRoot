#include "fitter/CbmLitKalmanFilter.h"

#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrackParam.h"
#include "utils/CbmLitMatrixMath.h"

#include <iostream>
#include <cmath>

CbmLitKalmanFilter::CbmLitKalmanFilter():
   CbmLitTrackUpdate("CbmLitKalmanFilter")
{
}

CbmLitKalmanFilter::~CbmLitKalmanFilter()
{
}

LitStatus CbmLitKalmanFilter::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitKalmanFilter::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitKalmanFilter::Update(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        const CbmLitHit *hit)
{
   *parOut = *parIn;
   return Update(parOut, hit);
}

LitStatus CbmLitKalmanFilter::Update(
		CbmLitTrackParam *par,
        const CbmLitHit *hit)
{
	LitStatus result = kLITSUCCESS;
	if (hit->GetType() == kLITSTRIPHIT)
		result = Update(par, static_cast<const CbmLitStripHit*>(hit));
	else if (hit->GetType() == kLITPIXELHIT)
		result = Update(par, static_cast<const CbmLitPixelHit*>(hit));
	return result;
}

LitStatus CbmLitKalmanFilter::Update(
		CbmLitTrackParam *par,
        const CbmLitPixelHit *hit)
{
    std::vector<myf> cIn = par->GetCovMatrix();

	static const myf ONE = 1., TWO = 2.;

	myf dxx = hit->GetDx() * hit->GetDx();
	myf dxy = hit->GetDxy();
	myf dyy = hit->GetDy() * hit->GetDy();

	// calculate residuals
	myf dx = hit->GetX() - par->GetX();
	myf dy = hit->GetY() - par->GetY();

	// Calculate and inverse residual covariance matrix
	myf t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
					dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
	myf R00 = (dyy + cIn[5]) * t;
	myf R01 = -(dxy + cIn[1]) * t;
	myf R11 = (dxx + cIn[0]) * t;

	 // Calculate Kalman gain matrix
	myf K00 = cIn[0] * R00 + cIn[1] * R01;
	myf K01 = cIn[0] * R01 + cIn[1] * R11;
	myf K10 = cIn[1] * R00 + cIn[5] * R01;
	myf K11 = cIn[1] * R01 + cIn[5] * R11;
	myf K20 = cIn[2] * R00 + cIn[6] * R01;
	myf K21 = cIn[2] * R01 + cIn[6] * R11;
	myf K30 = cIn[3] * R00 + cIn[7] * R01;
	myf K31 = cIn[3] * R01 + cIn[7] * R11;
	myf K40 = cIn[4] * R00 + cIn[8] * R01;
	myf K41 = cIn[4] * R01 + cIn[8] * R11;

	// Calculate filtered state vector
	myf xOut[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };
	xOut[0] += K00 * dx + K01 * dy;
	xOut[1] += K10 * dx + K11 * dy;
	xOut[2] += K20 * dx + K21 * dy;
	xOut[3] += K30 * dx + K31 * dy;
	xOut[4] += K40 * dx + K41 * dy;

	// Calculate filtered covariance matrix
//    myf cIn[15] = {par.C0,  par.C1,  par.C2,  par.C3,  par.C4,
//		           par.C5,  par.C6,  par.C7,  par.C8,  par.C9,
//		           par.C10, par.C11, par.C12, par.C13, par.C14};
	std::vector<myf> cOut = cIn;

	cOut[0]  += -K00 * cIn[0] - K01 * cIn[1];
	cOut[1]  += -K00 * cIn[1] - K01 * cIn[5];
	cOut[2]  += -K00 * cIn[2] - K01 * cIn[6];
	cOut[3]  += -K00 * cIn[3] - K01 * cIn[7];
	cOut[4]  += -K00 * cIn[4] - K01 * cIn[8];

	cOut[5]  += -K11 * cIn[5] - K10 * cIn[1];
	cOut[6]  += -K11 * cIn[6] - K10 * cIn[2];
	cOut[7]  += -K11 * cIn[7] - K10 * cIn[3];
	cOut[8]  += -K11 * cIn[8] - K10 * cIn[4];

	cOut[9]  += -K20 * cIn[2] - K21 * cIn[6];
	cOut[10] += -K20 * cIn[3] - K21 * cIn[7];
	cOut[11] += -K20 * cIn[4] - K21 * cIn[8];

	cOut[12] += -K30 * cIn[3] - K31 * cIn[7];
	cOut[13] += -K30 * cIn[4] - K31 * cIn[8];

	cOut[14] += -K40 * cIn[4] - K41 * cIn[8];

	// Copy filtered state to output
	par->SetX(xOut[0]);
	par->SetY(xOut[1]);
	par->SetTx(xOut[2]);
	par->SetTy(xOut[3]);
	par->SetQp(xOut[4]);
	par->SetCovMatrix(cOut);
	return kLITSUCCESS;

}


LitStatus CbmLitKalmanFilter::UpdateWMF(
		CbmLitTrackParam *par,
        const CbmLitPixelHit *hit)
{
    std::vector<myf> cIn = par->GetCovMatrix();
    std::vector<myf> cInInv = par->GetCovMatrix();

	myf dxx = hit->GetDx() * hit->GetDx();
	myf dxy = hit->GetDxy();
	myf dyy = hit->GetDy() * hit->GetDy();

	// Inverse predicted cov matrix
	InvSym15(cInInv);
	// Calculate C1
	std::vector<myf> C1 = cInInv;
	myf det = dxx * dyy - dxy * dxy;
	C1[0] += dyy / det;
	C1[1] += -dxy / det;
	C1[5] += dxx / det;
	// Inverse C1 -> output updated covariance matrix
	InvSym15(C1);

	std::vector<myf> t(5);
	t[0] = cInInv[0]*par->GetX() + cInInv[1]*par->GetY() + cInInv[2]*par->GetTx()
			+ cInInv[3]*par->GetTy() + cInInv[4]*par->GetQp()
			+ dyy * hit->GetX() / det - dxy * hit->GetY() / det;
	t[1] = cInInv[1]*par->GetX() + cInInv[5]*par->GetY() + cInInv[6]*par->GetTx()
			+ cInInv[7]*par->GetTy() + cInInv[8]*par->GetQp()
			- dxy * hit->GetX() / det + dxx * hit->GetY() / det;
	t[2] = cInInv[2]*par->GetX() + cInInv[6]*par->GetY() + cInInv[9]*par->GetTx()
			+ cInInv[10]*par->GetTy() + cInInv[11]*par->GetQp();
	t[3] = cInInv[3]*par->GetX() + cInInv[7]*par->GetY() + cInInv[10]*par->GetTx()
			+ cInInv[12]*par->GetTy() + cInInv[13]*par->GetQp();
	t[4] = cInInv[4]*par->GetX() + cInInv[8]*par->GetY() + cInInv[11]*par->GetTx()
			+ cInInv[13]*par->GetTy() + cInInv[14]*par->GetQp();

	std::vector<myf> xOut(5);
	Mult15On5(C1, t, xOut);

	// Copy filtered state to output
	par->SetX(xOut[0]);
	par->SetY(xOut[1]);
	par->SetTx(xOut[2]);
	par->SetTy(xOut[3]);
	par->SetQp(xOut[4]);
	par->SetCovMatrix(C1);
	return kLITSUCCESS;
}

//LitStatus CbmLitKalmanFilter::Update(
//		CbmLitTrackParam *par,
//        const CbmLitStripHit *hit)
//{
//	 myf xIn[5] = { par->GetX(), par->GetY(),
//			 			par->GetTx(), par->GetTy(),
//			 			par->GetQp() };
//	 std::vector<myf> cIn = par->GetCovMatrix();
//
//	 myf u = hit->GetU();
//	 myf duu = hit->GetDu() * hit->GetDu();
//	 myf phiCos = hit->GetCosPhi();
//	 myf phiSin = hit->GetSinPhi();
//	 myf phiCosSq = phiCos * phiCos;
//	 myf phiSinSq = phiSin * phiSin;
//	 myf phi2SinCos = 2 * phiCos * phiSin;
//
//	 // residual
//	 myf r = u - xIn[0] * phiCos - xIn[1] * phiSin;
//	 myf norm = duu + cIn[0] * phiCosSq + phi2SinCos * cIn[1] + cIn[5] * phiSinSq;
////	 myf norm = duu + cIn[0] * phiCos + cIn[5] * phiSin;
//	 myf R = 1./norm;
//
//	 //std::cout << "r=" << r << " R=" << 1./R
//	 //	<< " ucos=" << u * phiCos << " usin=" << u * phiSin << std::endl;
//
//	 // Calculate Kalman gain matrix
//	myf K0 = cIn[0] * phiCos + cIn[1] * phiSin;
//	myf K1 = cIn[1] * phiCos + cIn[5] * phiSin;
//	myf K2 = cIn[2] * phiCos + cIn[6] * phiSin;
//	myf K3 = cIn[3] * phiCos + cIn[7] * phiSin;
//	myf K4 = cIn[4] * phiCos + cIn[8] * phiSin;
//
//	myf KR0 = K0 * R;
//	myf KR1 = K1 * R;
//	myf KR2 = K2 * R;
//	myf KR3 = K3 * R;
//	myf KR4 = K4 * R;
//
//	// Calculate filtered state vector
//	std::vector<myf> xOut(5);
//	xOut[0] = xIn[0] + KR0 * r;
//	xOut[1] = xIn[1] + KR1 * r;
//	xOut[2] = xIn[2] + KR2 * r;
//	xOut[3] = xIn[3] + KR3 * r;
//	xOut[4] = xIn[4] + KR4 * r;
//
//	// Calculate filtered covariance matrix
//	std::vector<myf> cOut(15);
//	cOut[0] = cIn[0] - KR0 * K0;
//	cOut[1] = cIn[1] - KR0 * K1;
//	cOut[2] = cIn[2] - KR0 * K2;
//	cOut[3] = cIn[3] - KR0 * K3;
//	cOut[4] = cIn[4] - KR0 * K4;
//
//	cOut[5] = cIn[5] - KR1 * K1;
//	cOut[6] = cIn[6] - KR1 * K2;
//	cOut[7] = cIn[7] - KR1 * K3;
//	cOut[8] = cIn[8] - KR1 * K4;
//
//	cOut[9] = cIn[9] - KR2 * K2;
//	cOut[10] = cIn[10] - KR2 * K3;
//	cOut[11] = cIn[11] - KR2 * K4;
//
//	cOut[12] = cIn[12] - KR3 * K3;
//	cOut[13] = cIn[13] - KR3 * K4;
//
//	cOut[14] = cIn[14] - KR4 * K4;
//
//	// Copy filtered state to output
//	par->SetX(xOut[0]);
//	par->SetY(xOut[1]);
//	par->SetTx(xOut[2]);
//	par->SetTy(xOut[3]);
//	par->SetQp(xOut[4]);
//	par->SetCovMatrix(cOut);
//
//	return kLITSUCCESS;
//}

LitStatus CbmLitKalmanFilter::Update(
		CbmLitTrackParam *par,
        const CbmLitStripHit *hit)
{
	 myf xIn[5] = { par->GetX(), par->GetY(),
			 			par->GetTx(), par->GetTy(),
			 			par->GetQp() };
	 std::vector<myf> cIn = par->GetCovMatrix();

	 myf u = hit->GetU();
	 myf duu = hit->GetDu() * hit->GetDu();
	 myf phiCos = hit->GetCosPhi();
	 myf phiSin = hit->GetSinPhi();
//	 myf phiCosSq = phiCos * phiCos;
//	 myf phiSinSq = phiSin * phiSin;
//	 myf phi2SinCos = 2 * phiCos * phiSin;

//	  const myf ZERO = 0.0, ONE = 1.;

	  myf wi, zeta, zetawi, HCH;
	  myf F0, F1, F2, F3, F4;
	  myf K1, K2, K3, K4;

	  zeta = phiCos*xIn[0] + phiSin*xIn[1] - u;

	  // F = CH'
	  F0 = phiCos*cIn[0] + phiSin*cIn[1];
	  F1 = phiCos*cIn[1] + phiSin*cIn[5];

	  HCH = ( F0*phiCos + F1*phiSin );

	  F2 = phiCos*cIn[2] + phiSin*cIn[6];
	  F3 = phiCos*cIn[3] + phiSin*cIn[7];
	  F4 = phiCos*cIn[4] + phiSin*cIn[8];

	  wi = 1./(duu +HCH);
	  zetawi = zeta *wi;
//	  T.chi2 +=  zeta * zetawi ;
//	  T.NDF  += ONE;

	  K1 = F1*wi;
	  K2 = F2*wi;
	  K3 = F3*wi;
	  K4 = F4*wi;

//	  std::vector<myf> xOut(5);
	  xIn[0] -= F0*zetawi;
	  xIn[1] -= F1*zetawi;
	  xIn[2] -= F2*zetawi;
	  xIn[3] -= F3*zetawi;
	  xIn[4] -= F4*zetawi;

//	  std::vector<myf> cOut(15);
	  cIn[0]-= F0*F0*wi;
	  cIn[1]-= K1*F0;
	  cIn[5]-= K1*F1;
	  cIn[2]-= K2*F0;
	  cIn[6]-= K2*F1;
	  cIn[9]-= K2*F2;
	  cIn[3]-= K3*F0;
	  cIn[7]-= K3*F1;
	  cIn[10]-= K3*F2;
	  cIn[12]-= K3*F3;
	  cIn[4]-= K4*F0;
	  cIn[8]-= K4*F1;
	  cIn[11]-= K4*F2;
	  cIn[13]-= K4*F3;
	  cIn[14]-= K4*F4;

		// Copy filtered state to output
		par->SetX(xIn[0]);
		par->SetY(xIn[1]);
		par->SetTx(xIn[2]);
		par->SetTy(xIn[3]);
		par->SetQp(xIn[4]);
		par->SetCovMatrix(cIn);

		return kLITSUCCESS;
}

LitStatus CbmLitKalmanFilter::UpdateWMF(
		CbmLitTrackParam *par,
        const CbmLitStripHit *hit)
{
	 std::vector<myf> cIn = par->GetCovMatrix();
	 std::vector<myf> cInInv = par->GetCovMatrix();

	 myf duu = hit->GetDu() * hit->GetDu();
	 myf phiCos = hit->GetCosPhi();
	 myf phiSin = hit->GetSinPhi();

	// Inverse predicted cov matrix
	InvSym15(cInInv);
	// Calculate C1
	std::vector<myf> C1 = cInInv;
	C1[0] += phiCos*phiCos / duu;
	C1[1] += phiCos*phiSin / duu;
	C1[5] += phiSin*phiSin / duu;
	// Inverse C1 -> output updated covariance matrix
	InvSym15(C1);

	std::vector<myf> t(5);
	t[0] = cInInv[0]*par->GetX() + cInInv[1]*par->GetY() + cInInv[2]*par->GetTx()
			+ cInInv[3]*par->GetTy() + cInInv[4]*par->GetQp()
			+ phiCos*hit->GetU()/duu;
	t[1] = cInInv[1]*par->GetX() + cInInv[5]*par->GetY() + cInInv[6]*par->GetTx()
			+ cInInv[7]*par->GetTy() + cInInv[8]*par->GetQp()
			+ phiSin*hit->GetU()/duu;
	t[2] = cInInv[2]*par->GetX() + cInInv[6]*par->GetY() + cInInv[9]*par->GetTx()
			+ cInInv[10]*par->GetTy() + cInInv[11]*par->GetQp();
	t[3] = cInInv[3]*par->GetX() + cInInv[7]*par->GetY() + cInInv[10]*par->GetTx()
			+ cInInv[12]*par->GetTy() + cInInv[13]*par->GetQp();
	t[4] = cInInv[4]*par->GetX() + cInInv[8]*par->GetY() + cInInv[11]*par->GetTx()
			+ cInInv[13]*par->GetTy() + cInInv[14]*par->GetQp();

	std::vector<myf> xOut(5);
	Mult15On5(C1, t, xOut);

	// Copy filtered state to output
	par->SetX(xOut[0]);
	par->SetY(xOut[1]);
	par->SetTx(xOut[2]);
	par->SetTy(xOut[3]);
	par->SetQp(xOut[4]);
	par->SetCovMatrix(C1);
	return kLITSUCCESS;
}
