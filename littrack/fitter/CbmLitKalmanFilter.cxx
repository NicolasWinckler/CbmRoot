#include "CbmLitKalmanFilter.h"

#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrackParam.h"

#include <iostream>

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
	if (hit->GetType() == kLITSTRIPHIT)
		Update(par, static_cast<const CbmLitStripHit*>(hit));
	else if (hit->GetType() == kLITPIXELHIT)
		Update(par, static_cast<const CbmLitPixelHit*>(hit));
}

LitStatus CbmLitKalmanFilter::Update(
		CbmLitTrackParam *par,
        const CbmLitPixelHit *hit)
{
	 double xIn[5] = { par->GetX(), par->GetY(),
			 			par->GetTx(), par->GetTy(),
			 			par->GetQp() };
	 std::vector<double> cIn = par->GetCovMatrix();

	 double x = hit->GetX();
	 double y = hit->GetY();
	 double dxx = hit->GetDx() * hit->GetDx();
	 double dxy = hit->GetDxy();
	 double dyy = hit->GetDy() * hit->GetDy();

	 // calculate residuals
	 double dx = x - xIn[0];
	 double dy = y - xIn[1];

	 // Calculate and inverse residual covariance matrix
	 double norm = dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
	 					dxy * dxy - 2 * dxy * cIn[1] - cIn[1] * cIn[1];
	 double R00 = (dyy + cIn[5]) / norm;
	 double R01 = -(dxy + cIn[1]) / norm;
	 double R11 = (dxx + cIn[0]) / norm;

	 // Calculate Kalman gain matrix
	double K00 = cIn[0] * R00 + cIn[1] * R01;
	double K01 = cIn[0] * R01 + cIn[1] * R11;
	double K10 = cIn[1] * R00 + cIn[5] * R01;
	double K11 = cIn[1] * R01 + cIn[5] * R11;
	double K20 = cIn[2] * R00 + cIn[6] * R01;
	double K21 = cIn[2] * R01 + cIn[6] * R11;
	double K30 = cIn[3] * R00 + cIn[7] * R01;
	double K31 = cIn[3] * R01 + cIn[7] * R11;
	double K40 = cIn[4] * R00 + cIn[8] * R01;
	double K41 = cIn[4] * R01 + cIn[8] * R11;

	// Calculate filtered state vector
	std::vector<double> xOut(5);
	xOut[0] = xIn[0] + K00 * dx + K01 * dy;
	xOut[1] = xIn[1] + K10 * dx + K11 * dy;
	xOut[2] = xIn[2] + K20 * dx + K21 * dy;
	xOut[3] = xIn[3] + K30 * dx + K31 * dy;
	xOut[4] = xIn[4] + K40 * dx + K41 * dy;

	// Calculate filtered covariance matrix
	std::vector<double> cOut(15);
	cOut[0] = (-K00 + 1) * cIn[0] - K01 * cIn[1];
	cOut[1] = (-K00 + 1) * cIn[1] - K01 * cIn[5];
	cOut[2] = (-K00 + 1) * cIn[2] - K01 * cIn[6];
	cOut[3] = (-K00 + 1) * cIn[3] - K01 * cIn[7];
	cOut[4] = (-K00 + 1) * cIn[4] - K01 * cIn[8];

	cOut[5] = (-K11 + 1) * cIn[5] - K10 * cIn[1];
	cOut[6] = (-K11 + 1) * cIn[6] - K10 * cIn[2];
	cOut[7] = (-K11 + 1) * cIn[7] - K10 * cIn[3];
	cOut[8] = (-K11 + 1) * cIn[8] - K10 * cIn[4];

	cOut[9] =  -K20 * cIn[2] - K21 * cIn[6] + cIn[9];
	cOut[10] = -K20 * cIn[3] - K21 * cIn[7] + cIn[10];
	cOut[11] = -K20 * cIn[4] - K21 * cIn[8] + cIn[11];

	cOut[12] = -K30 * cIn[3] - K31 * cIn[7] + cIn[12];
	cOut[13] = -K30 * cIn[4] - K31 * cIn[8] + cIn[13];

	cOut[14] = -K40 * cIn[4] - K41 * cIn[8] + cIn[14];

	// Copy filtered state to output
	par->SetX(xOut[0]);
	par->SetY(xOut[1]);
	par->SetTx(xOut[2]);
	par->SetTy(xOut[3]);
	par->SetQp(xOut[4]);
	par->SetCovMatrix(cOut);

	return kLITSUCCESS;
}

LitStatus CbmLitKalmanFilter::Update(
		CbmLitTrackParam *par,
        const CbmLitStripHit *hit)
{
	 double xIn[5] = { par->GetX(), par->GetY(),
			 			par->GetTx(), par->GetTy(),
			 			par->GetQp() };
	 std::vector<double> cIn = par->GetCovMatrix();

	 double u = hit->GetU();
	 double duu = hit->GetDu() * hit->GetDu();
	 double phiCos = hit->GetCosPhi();
	 double phiSin = hit->GetSinPhi();
	 double phiCosSq = phiCos * phiCos;
	 double phiSinSq = phiSin * phiSin;
	 double phi2SinCos = 2 * phiCos * phiSin;

	 // residual
	 double r = u - xIn[0] * phiCos - xIn[1] * phiSin;
	 double norm = duu + cIn[0] * phiCosSq + phi2SinCos * cIn[1] + cIn[5] * phiSinSq;
	 double R = 1./norm;

	 //std::cout << "r=" << r << " R=" << 1./R
	 //	<< " ucos=" << u * phiCos << " usin=" << u * phiSin << std::endl;

	 // Calculate Kalman gain matrix
	double K0 = cIn[0] * phiCos + cIn[1] * phiSin;
	double K1 = cIn[1] * phiCos + cIn[5] * phiSin;
	double K2 = cIn[2] * phiCos + cIn[6] * phiSin;
	double K3 = cIn[3] * phiCos + cIn[7] * phiSin;
	double K4 = cIn[4] * phiCos + cIn[8] * phiSin;

	double KR0 = K0 * R;
	double KR1 = K1 * R;
	double KR2 = K2 * R;
	double KR3 = K3 * R;
	double KR4 = K4 * R;

	// Calculate filtered state vector
	std::vector<double> xOut(5);
	xOut[0] = xIn[0] + KR0 * r;
	xOut[1] = xIn[1] + KR1 * r;
	xOut[2] = xIn[2] + KR2 * r;
	xOut[3] = xIn[3] + KR3 * r;
	xOut[4] = xIn[4] + KR4 * r;

	// Calculate filtered covariance matrix
	std::vector<double> cOut(15);
	cOut[0] = cIn[0] - KR0 * K0;
	cOut[1] = cIn[1] - KR0 * K1;
	cOut[2] = cIn[2] - KR0 * K2;
	cOut[3] = cIn[3] - KR0 * K3;
	cOut[4] = cIn[4] - KR0 * K4;

	cOut[5] = cIn[5] - KR1 * K1;
	cOut[6] = cIn[6] - KR1 * K2;
	cOut[7] = cIn[7] - KR1 * K3;
	cOut[8] = cIn[8] - KR1 * K4;

	cOut[9] = cIn[9] - KR2 * K2;
	cOut[10] = cIn[10] - KR2 * K3;
	cOut[11] = cIn[11] - KR2 * K4;

	cOut[12] = cIn[12] - KR3 * K3;
	cOut[13] = cIn[13] - KR3 * K4;

	cOut[14] = cIn[14] - KR4 * K4;

	// Copy filtered state to output
	par->SetX(xOut[0]);
	par->SetY(xOut[1]);
	par->SetTx(xOut[2]);
	par->SetTy(xOut[3]);
	par->SetQp(xOut[4]);
	par->SetCovMatrix(cOut);

	return kLITSUCCESS;
}
