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
#include "CbmLitDefaultSettings.h"

#include <vector>
#include <cmath>
#include <iostream>

CbmLitTGeoTrackPropagator::CbmLitTGeoTrackPropagator(
		TrackExtrapolatorPtr extrapolator):
   CbmLitTrackPropagator("CbmLitTGeoTrackPropagator"),
   fExtrapolator(extrapolator)
{
	fNavigator = GeoNavigatorPtr(new CbmLitTGeoNavigator());
	fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());
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
        int pdg,
        std::vector<myf>* F)
{
	*parOut = *parIn;
	return Propagate(parOut, zOut, pdg, F);
}

LitStatus CbmLitTGeoTrackPropagator::Propagate(
		CbmLitTrackParam *par,
        myf zOut,
        int pdg,
        std::vector<myf>* F)

{
	if (!IsParCorrect(par)) return kLITERROR;

	myf zIn = par->GetZ();
	myf dz = zOut - zIn;

	if(std::fabs(dz) < lit::MINIMUM_PROPAGATION_DISTANCE) return kLITSUCCESS;

	//Check whether upstream or downstream
	//TODO check upstream/downstream
	bool downstream = dz > 0;

	if (F != NULL) {
		F->assign(25, 0.);
		(*F)[0] = 1.; (*F)[6] = 1.; (*F)[12] = 1.; (*F)[18] = 1.; (*F)[24] = 1.;
	}

	int nofSteps = int(std::abs(dz) / lit::MAXIMUM_PROPAGATION_STEP_SIZE);
	myf stepSize;
	if (nofSteps == 0) stepSize = dz; else  stepSize = lit::MAXIMUM_PROPAGATION_STEP_SIZE;
	myf z = zIn;

//	std::cout << "Propagation: zIn=" << zIn << " zOut=" << zOut << " stepSize=" << stepSize << " nofSteps=" << nofSteps << std::endl;
	//Loop over steps + additional step to propagate to virtual plane at zOut
	for (int iStep = 0; iStep < nofSteps + 1; iStep++) {
		if (!IsParCorrect(par)) {
//			std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: incorrect track parameters" << std::endl;
			return kLITERROR;
		}

		// update current z position
		if (iStep != nofSteps) z += stepSize; else z = zOut;

		//Get intersections with the materials for this step
		std::vector<CbmLitMaterialInfo> inter;
		if (fNavigator->FindIntersections(par, z, inter) == kLITERROR) {
//			std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: navigation failed" << std::endl;
			return kLITERROR;
		}
//		std::cout << iStep << " z0=" << par->GetZ() << " z1=" << z << " nofInt=" << inter.size() << std::endl;

//		std::cout << "  nof intersections=" << inter.size()  << " zIn=" << par->GetZ() << " zOut=" << z << std::endl;
		//Loop over the materials
		for(unsigned int  iMat = 0; iMat < inter.size() ; iMat++) {
			CbmLitMaterialInfo mat = inter[iMat];


			// check if track parameters are correct
			if (!IsParCorrect(par)) {
//				std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: incorrect track parameters" << std::endl;
				return kLITERROR;
			}

			std::vector<myf>* Fnew = NULL;
		    if (F != NULL) Fnew = new std::vector<myf>(25, 0.);
			// extrapolate to the next boundary
			if (fExtrapolator->Extrapolate(par, mat.GetZpos(), Fnew) == kLITERROR) {
//				std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: extrapolation failed" << std::endl;
				return kLITERROR;
			}

			// update transport matrix
			if (F != NULL) UpdateF(*F, *Fnew);
			delete Fnew;

			// add material effects
			fMaterial->Update(par, &mat, pdg, downstream);

//			std::cout << "    " << iStep << " " << iMat << " " << mat.ToString();
//			std::cout << "    " << iStep << " " << iMat << " " << par->ToString();
		}
	}
//	std::vector<myf>* Fnew = NULL;
//	if (F != NULL) Fnew = new std::vector<myf>(25, 0.);
//	fExtrapolator->Extrapolate(par, zOut, Fnew);
//	if (F != NULL) UpdateF(*F, *Fnew);
//	delete Fnew;
//	par->SetZ(zOut);

//	std::cout << "OUT " << par->ToString();

	if (!IsParCorrect(par)) return kLITERROR;
	else return kLITSUCCESS;
}

void CbmLitTGeoTrackPropagator::UpdateF(
		std::vector<myf>& F,
		const std::vector<myf>& newF)
{
	std::vector<myf> A(25);
	Mult25(newF, F, A);
	F.assign(A.begin(), A.end());
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
