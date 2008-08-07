// -------------------------------------------------------------------------
// -----                  CbmLitKalmanFilter source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 
#include "CbmLitKalmanFilter.h"

#include "CbmLitHit.h"
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
	 Double_t xIn[5] = { par->GetX(), par->GetY(),
			 			par->GetTx(), par->GetTy(),
			 			par->GetQp() };
	 std::vector<Double_t> cIn = par->GetCovMatrix();
	 
	 Double_t x = hit->GetX();
	 Double_t y = hit->GetY();
	 Double_t dxx = hit->GetDx() * hit->GetDx();
	 Double_t dxy = hit->GetDxy();
	 Double_t dyy = hit->GetDy() * hit->GetDy();
	
	 // calculate residuals
	 Double_t dx = x - xIn[0];
	 Double_t dy = y - xIn[1];
	 
	 // Calculate and inverse residual covariance matrix
	 Double_t norm = dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
	 					dxy * dxy - 2 * dxy * cIn[1] - cIn[1] * cIn[1];
	 Double_t R00 = (dyy + cIn[5]) / norm;
	 Double_t R01 = -(dxy + cIn[1]) / norm;
	 Double_t R11 = (dxx + cIn[0]) / norm;
	 
	 // Calculate Kalman gain matrix
	Double_t K00 = cIn[0] * R00 + cIn[1] * R01;
	Double_t K01 = cIn[0] * R01 + cIn[1] * R11;
	Double_t K10 = cIn[1] * R00 + cIn[5] * R01;
	Double_t K11 = cIn[1] * R01 + cIn[5] * R11;
	Double_t K20 = cIn[2] * R00 + cIn[6] * R01;
	Double_t K21 = cIn[2] * R01 + cIn[6] * R11;
	Double_t K30 = cIn[3] * R00 + cIn[7] * R01;
	Double_t K31 = cIn[3] * R01 + cIn[7] * R11;
	Double_t K40 = cIn[4] * R00 + cIn[8] * R01;
	Double_t K41 = cIn[4] * R01 + cIn[8] * R11;
	
	// Calculate filtered state vector
	std::vector<Double_t> xOut(5);
	xOut[0] = xIn[0] + K00 * dx + K01 * dy;
	xOut[1] = xIn[1] + K10 * dx + K11 * dy;
	xOut[2] = xIn[2] + K20 * dx + K21 * dy;
	xOut[3] = xIn[3] + K30 * dx + K31 * dy;
	xOut[4] = xIn[4] + K40 * dx + K41 * dy; 
	
	// Calculate filtered covariance matrix
	std::vector<Double_t> cOut(15);
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

ClassImp(CbmLitKalmanFilter)

