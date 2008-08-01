 
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

CbmTrackPropagatorGeane::CbmTrackPropagatorGeane():
   CbmLitTrackPropagator("CbmTrackPropagatorGeane")
{
   fPropagator = new CbmGeanePro();
}

CbmTrackPropagatorGeane::~CbmTrackPropagatorGeane() 
{
	if (fPropagator) delete fPropagator;
}

LitStatus CbmTrackPropagatorGeane::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmTrackPropagatorGeane::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmTrackPropagatorGeane::Propagate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        Double_t zOut,
        Int_t pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmTrackPropagatorGeane::Propagate(
		CbmLitTrackParam *par,
        Double_t zOut,
        Int_t pdg)
{
	if (std::fabs(zOut - par->GetZ()) < 0.01) return kLITSUCCESS;
	
	if (!IsInParCorrect(par)) return kLITERROR;
	
//	std::cout << "in:";
//	pParam->Print();
	
   // covariance matrix for GEANE
   std::vector<Double_t> gCov(15);
   // fill GEANE covariant matrix
   ToGeaneCovMatrix(par->GetCovMatrix(), gCov);
 
   TVector3 v1(1, 0, 0);
   TVector3 v2(0, 1, 0);    
   Bool_t result1 = fPropagator->PropagateFromPlane(v1, v2);
   
   // input parameter for the tracking
   // the input parameter is defined in the xy plane 
   // which is perpendicular to z axes with origin at Z.
   // In this case X=V, Y=W
   CbmTrackParP parStart(par->GetX(), par->GetY(),
                         par->GetTx(), par->GetTy(), 
                         par->GetQp(), &gCov[0], 
                         TVector3(0, 0, par->GetZ()), 
                         TVector3(1, 0, 0), TVector3(0, 1, 0), 1);
     
   // output track parameter
   CbmTrackParP parEnd; 
  
   // plane to which propagate
   // the plane is perpendicular to z axes
   TVector3 v0(0, 0, zOut);
   Bool_t result2 = fPropagator->PropagateToPlane(v0, v1, v2);
         
   // pdg code of the particle,
   // +/- 13 muon code 
   pdg = 13;
   if (par->GetQp() > 0) pdg = -13;

   // CbmGeanePro is used to propagate the track parameters
   Bool_t propResult;
   propResult = fPropagator->Propagate(&parStart, &parEnd, pdg);
   
   // if propagation fails
   if (!propResult) {
	   //std::cout << "PROPAGATION FAILED!!!!" <<std::endl;
	   return kLITERROR;
   }
     
   // fill the CBM track parameter representation 
   par->SetX(parEnd.GetV());
   par->SetY(parEnd.GetW());
   par->SetZ(zOut);//parEnd.GetZ());
   par->SetTx(parEnd.GetTV());
   par->SetTy(parEnd.GetTW());
   par->SetQp(parEnd.GetQp());
   
   // fill and reorder covariant matrix
   std::vector<Double_t> covEnd(15);
   std::vector<Double_t> gCovEnd(15);
   parEnd.GetCov(&gCovEnd[0]);
   FromGeaneCovMatrix(gCovEnd, covEnd);
   par->SetCovMatrix(covEnd);

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
	Double_t maxSlope = 5.;
	Double_t minSlope = 1e-4;
	
	if (std::abs(par->GetTx()) > maxSlope ||
		std::abs(par->GetTy()) > maxSlope ||
		std::abs(par->GetTx()) < minSlope ||
		std::abs(par->GetTy()) < minSlope) {
		return false;
	}
	return true;
}

ClassImp(CbmTrackPropagatorGeane)
