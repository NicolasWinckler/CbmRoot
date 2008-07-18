 
// -------------------------------------------------------------------------
// -----                  CbmTrackPropagatorGeane source file               -----
// -----                  Created 22/11/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 


#include "CbmTrackPropagatorGeane.h"

#include "CbmLitTrackParam.h"

#include "CbmTrackParP.h"
#include "TVector3.h"

#include <vector>
#include <cmath>

//constructors
CbmTrackPropagatorGeane::CbmTrackPropagatorGeane():
   CbmLitTrackPropagator("CbmTrackPropagatorGeane")
{
   fPropagator = new CbmGeanePro();
}

//Destructor
CbmTrackPropagatorGeane::~CbmTrackPropagatorGeane() 
{
   //
	if (fPropagator) delete fPropagator;
}

// Initialization
LitStatus CbmTrackPropagatorGeane::Initialize()
{
	return kLITSUCCESS;
}

// Finalization
LitStatus CbmTrackPropagatorGeane::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmTrackPropagatorGeane::Propagate(
		const CbmLitTrackParam *pParamIn,
        CbmLitTrackParam *pParamOut,
        Double_t zOut)
{
   *pParamOut = *pParamIn;
   return Propagate(pParamOut, zOut);
}

LitStatus CbmTrackPropagatorGeane::Propagate(
		CbmLitTrackParam *pParam,
        Double_t zOut)
{
	if (std::fabs(zOut - pParam->GetZ()) < 0.01) return kLITSUCCESS;
	
	if (!IsInParCorrect(pParam)) return kLITERROR;
	
	std::cout << "in:";
	pParam->Print();
	
   // covariance matrix for GEANE
   std::vector<Double_t> gCov(15);
   // fill GEANE covariant matrix
   ToGeaneCovMatrix(pParam->GetCovMatrix(), gCov);
 
   TVector3 v1(1, 0, 0);
   TVector3 v2(0, 1, 0);    
   Bool_t result1 = fPropagator->PropagateFromPlane(v1, v2);
   
   // input parameter for the tracking
   // the input parameter is defined in the xy plane 
   // which is perpendicular to z axes with origin at Z.
   // In this case X=V, Y=W
   CbmTrackParP parStart(pParam->GetX(), pParam->GetY(),
                         pParam->GetTx(), pParam->GetTy(), 
                         pParam->GetQp(), &gCov[0], 
                         TVector3(0, 0, pParam->GetZ()), 
                         TVector3(1, 0, 0), TVector3(0, 1, 0), 1);
     
   // output track parameter
   CbmTrackParP parEnd; 
  
   // plane to which propagate
   // the plane is perpendicular to z axes
   TVector3 v0(0, 0, zOut);
   Bool_t result2 = fPropagator->PropagateToPlane(v0, v1, v2);
         
   // pdg code of the particle,
   // +/- 13 muon code 
   Int_t pdg = 13;
   if (pParam->GetQp() > 0) pdg = -13;

   // CbmGeanePro is used to propagate the track parameters
   Bool_t propResult;
   propResult = fPropagator->Propagate(&parStart, &parEnd, pdg);
   
   // if propagation fails
   if (!propResult) {
	   //std::cout << "PROPAGATION FAILED!!!!" <<std::endl;
	   return kLITERROR;
   }
     
   // fill the CBM track parameter representation 
   pParam->SetX(parEnd.GetV());
   pParam->SetY(parEnd.GetW());
   pParam->SetZ(zOut);//parEnd.GetZ());
   pParam->SetTx(parEnd.GetTV());
   pParam->SetTy(parEnd.GetTW());
   pParam->SetQp(parEnd.GetQp());
   
   // fill and reorder covariant matrix
   std::vector<Double_t> covEnd(15);
   std::vector<Double_t> gCovEnd(15);
   parEnd.GetCov(&gCovEnd[0]);
   FromGeaneCovMatrix(gCovEnd, covEnd);
   pParam->SetCovMatrix(covEnd);

   //std::cout << "out:";
   //pParam->Print();
   
   return kLITSUCCESS;
}

void CbmTrackPropagatorGeane::ToGeaneCovMatrix(
		const std::vector<Double_t>& cov,
		std::vector<Double_t>& gCov)
{
	// reorder covariant matrix elements CBM->GEANE
	// for CBM diagonal elements in the following order: 
	// [0]=DX, [5]=DY, [9]=DTx, [12]=DTy, [14]=DQp
	// and for the GEANE:
	// [0]=DQp, [5]=DTv, [9]=DTw, [12]=DV, [14]=DW
	gCov[0]  = cov[14];
	gCov[1]  = cov[11];
	gCov[2]  = cov[13];
	gCov[3]  = cov[4];
	gCov[4]  = cov[8];
	gCov[5]  = cov[9];
	gCov[6]  = cov[10];
	gCov[7]  = cov[2];
	gCov[8]  = cov[6];
	gCov[9]  = cov[12];
	gCov[10] = cov[3];
	gCov[11] = cov[7];
	gCov[12] = cov[0];
	gCov[13] = cov[1];
	gCov[14] = cov[5];
}

void CbmTrackPropagatorGeane::FromGeaneCovMatrix(
		const std::vector<Double_t>& gCov,
		std::vector<Double_t>& cov)
{
	// reorder covariant matrix elements GEANE->CBM
	cov[0]  = gCov[12];
	cov[1]  = gCov[13];
	cov[2]  = gCov[7];
	cov[3]  = gCov[10];
	cov[4]  = gCov[3];
	cov[5]  = gCov[14];
	cov[6]  = gCov[8];
	cov[7]  = gCov[11];
	cov[8]  = gCov[4];
	cov[9]  = gCov[5];
	cov[10] = gCov[6];
	cov[11] = gCov[1];
	cov[12] = gCov[9];
	cov[13] = gCov[2];
	cov[14] = gCov[0];
}

Bool_t CbmTrackPropagatorGeane::IsInParCorrect(
		const CbmLitTrackParam* par)
{
	Double_t maxSlope = 4.;
	Double_t minP = 0.1;
	Double_t maxCoordErr = 10. * 10;
	Double_t maxSlopeErr = 1. * 1.;
	
	if (std::abs(par->GetTx()) > maxSlope ||
		std::abs(par->GetTy()) > maxSlope) {
		std::cout << "slopes are incorrect" << std::endl;
		par->Print();
		return false;
	}
	
	if (std::abs(1. / par->GetQp()) < minP) {
		std::cout << "momentum too small" << std::endl;
		par->Print();
		return false;
	}
	
	if (std::abs(par->GetCovariance(0)) > maxCoordErr ||
		std::abs(par->GetCovariance(5)) > maxCoordErr ||
		std::abs(par->GetCovariance(9)) > maxSlopeErr ||
		std::abs(par->GetCovariance(12)) > maxSlopeErr) {
		std::cout << "errors too big" << std::endl;
		par->Print();
		return false;
	}
	
	return true;
}

ClassImp(CbmTrackPropagatorGeane)
