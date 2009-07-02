/** CbmLitTGeoTrackPropagator.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 1.0
 **/

#include "CbmLitTGeoTrackPropagator.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitTGeoNavigator.h"
#include "CbmLitMaterialEffectsImp.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitTrackParam.h"
#include "CbmLitMath.h"
#include "CbmLitMatrixMath.h"

#include <vector>
#include <cmath>
#include <iostream>

CbmLitTGeoTrackPropagator::CbmLitTGeoTrackPropagator(
		TrackExtrapolatorPtr extrapolator):
   CbmLitTrackPropagator("CbmLitTGeoTrackPropagator"),
   fExtrapolator(extrapolator),
   fCalcTransportMatrix(true),
   fMaxStepSize(10.)
{
	fNavigator = GeoNavigatorPtr(new CbmLitTGeoNavigator());
	fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());

	fFm.resize(25);
}

CbmLitTGeoTrackPropagator::~CbmLitTGeoTrackPropagator()
{
}

LitStatus CbmLitTGeoTrackPropagator::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTGeoTrackPropagator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTGeoTrackPropagator::Propagate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        myf zOut,
        int pdg)
{
	*parOut = *parIn;
	return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitTGeoTrackPropagator::Propagate(
		CbmLitTrackParam *par,
        myf zOut,
        int pdg)

{
	if (!IsParCorrect(par)) return kLITERROR;

	fPDG = pdg;

	myf zIn = par->GetZ();
	myf dz = zOut - zIn;

	if(std::fabs(dz) < 1e-3) return kLITSUCCESS;

	//Check whether upstream or downstream
	//TODO check upstream/downstream
	fDownstream = dz > 0;

	if (fCalcTransportMatrix) {
		std::fill(fFm.begin(), fFm.end(), 0.);
		fFm[0] = 1.; fFm[6] = 1.; fFm[12] = 1.; fFm[18] = 1.; fFm[24] = 1.;
	}

	int nofSteps = int(std::abs(dz) / fMaxStepSize);
	myf stepSize;
	if (nofSteps == 0) stepSize = dz; else  stepSize = fMaxStepSize;
	myf z = zIn;

	//Loop over steps + additional step to propagate to virtual plane at zOut
	for (int iStep = 0; iStep < nofSteps + 1; iStep++) {
		if (!IsParCorrect(par)) {
			std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: incorrect track parameters" << std::endl;
			return kLITERROR;
		}

		// update current z position
		if (iStep != nofSteps) z += stepSize; else z = zOut;

		//Get intersections with the materials for this step
		std::vector<CbmLitMaterialInfo> inter;
		if (fNavigator->FindIntersections(par, z, inter) == kLITERROR) {
			std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: navigation failed" << std::endl;
			return kLITERROR;
		}

		//Loop over the materials
		for(unsigned int  iMat = 0; iMat < inter.size() ; iMat++) {
			CbmLitMaterialInfo mat = inter[iMat];

			// check if track parameters are correct
			if (!IsParCorrect(par)) {
				std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: incorrect track parameters" << std::endl;
				return kLITERROR;
			}

			// extrapolate to the next boundary
			if (fExtrapolator->Extrapolate(par, mat.GetZpos()) == kLITERROR) {
				std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: extrapolation failed" << std::endl;
				return kLITERROR;
			}

			// update transport matrix
			if (fCalcTransportMatrix) {
				std::vector<myf> Fnew(25);
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

void CbmLitTGeoTrackPropagator::TransportMatrix(
		   std::vector<myf>& F)
{
	F.assign(fFm.begin(), fFm.end());
}

void CbmLitTGeoTrackPropagator::UpdateF(
		std::vector<myf>& F,
		const std::vector<myf>& newF)
{
	std::vector<myf> A(25);
	Mult25(newF, F, A);
	F.assign(A.begin(), A.end());
//	F = newF * F;
}

bool CbmLitTGeoTrackPropagator::IsParCorrect(
		const CbmLitTrackParam* par)
{
	myf maxSlope = 5.;
	myf minSlope = 1e-6;
	myf maxQp = 1000.; // p = 10 MeV

	if (std::abs(par->GetTx()) > maxSlope ||
		std::abs(par->GetTy()) > maxSlope ||
		std::abs(par->GetTx()) < minSlope ||
		std::abs(par->GetTy()) < minSlope ||
		std::abs(par->GetQp()) > maxQp) {
		return false;
	}
	return true;
}
