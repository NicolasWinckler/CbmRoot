// -------------------------------------------------------------------------
// -----                  CbmLitTrackPropagatorImp source file         -----
// -----                  Created 16/07/07  by A. Lebedev              -----
// -------------------------------------------------------------------------

#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitEnvironment.h"
#include "CbmLitComparators.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitGeoNavigatorImp.h"
#include "CbmLitMaterialEffectsImp.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitTrackParam.h"

#include <vector>
#include <cmath>
#include <iostream>

CbmLitTrackPropagatorImp::CbmLitTrackPropagatorImp(CbmLitTrackExtrapolator *extrapolator):
   CbmLitTrackPropagator("CbmLitTrackPropagatorImp"),
   fExtrapolator(extrapolator),
   fCalcTransportMatrix(true)
{
   fNavigator = new CbmLitGeoNavigatorImp();
   fMaterial = new CbmLitMaterialEffectsImp();

   fFm.ResizeTo(5,5);
}

CbmLitTrackPropagatorImp::~CbmLitTrackPropagatorImp()
{
	delete fNavigator;
}

LitStatus CbmLitTrackPropagatorImp::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitTrackPropagatorImp::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackPropagatorImp::Propagate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        Double_t zOut,
        Int_t pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitTrackPropagatorImp::Propagate(
		CbmLitTrackParam *par,
        Double_t zOut,
        Int_t pdg)

{
	if (!IsParCorrect(par)) return kLITERROR;

	fPDG = pdg;

	Double_t zIn = par->GetZ();
	Double_t dz = zOut - zIn;

	if(std::fabs(dz) < 1e-3) return kLITSUCCESS;

	//Check whether upstream or downstream
	//TODO check upstream/downstream
	fDownstream = dz > 0;

	if (fCalcTransportMatrix) fFm.UnitMatrix();

	Double_t fMaxStepSize = 10.0; //cm
	Int_t nofSteps = Int_t(std::abs(dz) / fMaxStepSize);
	Double_t stepSize;
	if (nofSteps == 0) stepSize = dz;
	else  stepSize = fMaxStepSize;
	Double_t z = zIn;

	//Loop over steps + additional step to propagate to virtual plane at zOut
	for (Int_t iStep = 0; iStep < nofSteps + 1; iStep++) {
		if (!IsParCorrect(par)) {
			//std::cout << "ERROR CbmLitTrackPropagatorImp::Propagate parameters incorrect " << std::endl;
			//par->Print();
			return kLITERROR;
		}

		//std::cout << "step " << iStep << " z=" << z << std::endl;
		// update current z position
		if (iStep != nofSteps) z += stepSize;
		else z = zOut;
		//pParam->Print();
		//Get intersections with the materials for this step
		std::vector<CbmLitMaterialInfo> inter;
		if (fNavigator->FindIntersections(par, z, inter) == kLITERROR) {
			//std::cout << "ERROR CbmLitTrackPropagatorImp::Propagate navigation failed" << std::endl;
			return kLITERROR;
		}

		//Loop over the materials
		for(std::vector<CbmLitMaterialInfo>::iterator iMat = inter.begin(); iMat != inter.end() ; iMat++) {
			CbmLitMaterialInfo mat = *iMat;
			//mat.Print();
			//pParam->Print();
			//if ((mat.GetLength() / mat.GetRL()) < 1.e-4) continue;
			//mat.Print();
			// check if track parameters are correct
			if (!IsParCorrect(par)) return kLITERROR;

			// extrapolate to the next boundary
			if (fExtrapolator->Extrapolate(par, mat.GetZpos()) == kLITERROR) {
				//std::cout << "ERROR CbmLitTrackPropagatorImp::Propagate extrapolation failed" << std::endl;
				return kLITERROR;
			}

			// update transport matrix
			if (fCalcTransportMatrix) {
				TMatrixD Fnew(5,5);
				fExtrapolator->TransportMatrix(Fnew);
				UpdateF(fFm, Fnew);
			}

			// add material effects
			fMaterial->Update(par, &mat, fPDG, fDownstream);
		}
	}

	if (!IsParCorrect(par)) return kLITERROR;
	else return kLITSUCCESS;
}
void CbmLitTrackPropagatorImp::TransportMatrix(
		   std::vector<Double_t>& F)
{
	F.assign(fFm.GetMatrixArray(), fFm.GetMatrixArray() + fFm.GetNoElements());
}

void CbmLitTrackPropagatorImp::TransportMatrix(
		   TMatrixD& F)
{
	F = fFm;
}

void CbmLitTrackPropagatorImp::UpdateF(
		TMatrixD& F,
		const TMatrixD& newF)
{
	F = newF * F;
}

Bool_t CbmLitTrackPropagatorImp::IsParCorrect(
		const CbmLitTrackParam* par)
{
	Double_t maxSlope = 5.;
	Double_t minSlope = 1e-6;
	Double_t maxQp = 1000.; // p = 10 MeV

	if (std::abs(par->GetTx()) > maxSlope ||
		std::abs(par->GetTy()) > maxSlope ||
		std::abs(par->GetTx()) < minSlope ||
		std::abs(par->GetTy()) < minSlope ||
		std::abs(par->GetQp()) > maxQp) {
		return false;
	}
	return true;
}

ClassImp(CbmLitTrackPropagatorImp)
