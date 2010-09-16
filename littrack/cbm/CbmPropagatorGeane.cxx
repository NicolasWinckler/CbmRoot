#include "CbmPropagatorGeane.h"

#include "FairTrackParam.h"
#include "FairTrackParP.h"

#include "TVector3.h"

#include <vector>
#include <cmath>

CbmPropagatorGeane::CbmPropagatorGeane():
	fMinSlope(1e-4),
	fMaxSlope(5.)
{
   fPropagator = new FairGeanePro();
}

CbmPropagatorGeane::~CbmPropagatorGeane()
{
	if (fPropagator) delete fPropagator;
}

StatusCode CbmPropagatorGeane::Propagate(
		const FairTrackParam *parIn,
        FairTrackParam *parOut,
        Double_t zOut,
        Int_t pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

StatusCode CbmPropagatorGeane::Propagate(
		FairTrackParam *par,
        Double_t zOut,
        Int_t pdg)
{
	if (std::fabs(zOut - par->GetZ()) < 0.01) return kCBMSUCCESS;
	if (!IsInParCorrect(par)) return kCBMERROR;

   // covariance matrix for GEANE
   std::vector<Double_t> cov(15);
   std::vector<Double_t> gCov(15);
   // fill GEANE covariant matrix
   par->CovMatrix(&cov[0]);
   ToGeaneCovMatrix(cov, gCov);

   TVector3 v1(1, 0, 0);
   TVector3 v2(0, 1, 0);
   Bool_t result1 = fPropagator->PropagateFromPlane(v1, v2);
   if (!result1) return kCBMERROR;

   // input parameter for the tracking
   // the input parameter is defined in the xy plane
   // which is perpendicular to z axes with origin at Z.
   // In this case X=V, Y=W
   FairTrackParP parStart(par->GetX(), par->GetY(),
                         par->GetTx(), par->GetTy(),
                         par->GetQp(), &gCov[0],
                         TVector3(0, 0, par->GetZ()),
                         TVector3(1, 0, 0), TVector3(0, 1, 0), 1);

   // output track parameter
   FairTrackParP parEnd;

   // plane to which propagate
   // the plane is perpendicular to z axes
   TVector3 v0(0, 0, zOut);
   Bool_t result2 = fPropagator->PropagateToPlane(v0, v1, v2);
   if (!result2) return kCBMERROR;

//   pdg code of the particle,
//   +/- 13 muon code
//   pdg = 13;
//   if (par->GetQp() > 0) pdg = -13;

   // CbmGeanePro is used to propagate the track parameters
   Bool_t propResult;
   propResult = fPropagator->Propagate(&parStart, &parEnd, pdg);

   // if propagation fails
   if (!propResult) {
	   //std::cout << "PROPAGATION FAILED!!!!" <<std::endl;
	   return kCBMERROR;
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
   par->SetCovMatrix(&covEnd[0]);

   //std::cout << "out:";
   //pParam->Print();

   return kCBMSUCCESS;
}

void CbmPropagatorGeane::TransportMatrix(
		   std::vector<Double_t>& F)
{
	std::cout << "-WARNING- CbmPropagatorGeane::TransportMatrix not implemented yet" << std::endl;
}

void CbmPropagatorGeane::TransportMatrix(
		   TMatrixD& F)
{
	std::cout << "-WARNING- CbmPropagatorGeane::TransportMatrix not implemented yet" << std::endl;
}

void CbmPropagatorGeane::ToGeaneCovMatrix(
		const std::vector<Double_t>& cov,
		std::vector<Double_t>& gCov) const
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

void CbmPropagatorGeane::FromGeaneCovMatrix(
		const std::vector<Double_t>& gCov,
		std::vector<Double_t>& cov) const
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

Bool_t CbmPropagatorGeane::IsInParCorrect(
		const FairTrackParam* par) const
{
	if (std::abs(par->GetTx()) > fMaxSlope ||
		std::abs(par->GetTy()) > fMaxSlope ||
		std::abs(par->GetTx()) < fMinSlope ||
		std::abs(par->GetTy()) < fMinSlope) {
		return false;
	}
	return true;
}
