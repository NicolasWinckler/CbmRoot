/** CbmLitMyTrackPropagator.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2009
 **
 **/

#include "CbmLitMyTrackPropagator.h"
#include "CbmLitMyGeoNavigator.h"
#include "CbmLitMaterialEffectsImp.h"
#include "CbmLitMath.h"
#include "CbmLitMatrixMath.h"
#include "CbmLitDefaultSettings.h"

#include <cmath>
#include <iostream>

CbmLitMyTrackPropagator::CbmLitMyTrackPropagator(
		TrackExtrapolatorPtr extrapolator):
			fExtrapolator(extrapolator)
{
	fNavigator = GeoNavigatorPtr(new CbmLitMyGeoNavigator());
	fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());
}

CbmLitMyTrackPropagator::~CbmLitMyTrackPropagator()
{
}

LitStatus CbmLitMyTrackPropagator::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitMyTrackPropagator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitMyTrackPropagator::Propagate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        myf zOut,
        int pdg,
        std::vector<myf>* F)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg, F);
}

LitStatus CbmLitMyTrackPropagator::Propagate(
		CbmLitTrackParam *par,
        myf zOut,
        int pdg,
        std::vector<myf>* F)
{
	myf zIn = par->GetZ();
	myf dz = zOut - zIn;
	if(std::fabs(dz) < lit::MINIMUM_PROPAGATION_DISTANCE) return kLITSUCCESS;

	//Check whether upstream or downstream
	bool downstream = dz > 0;

	if (F != NULL) {
		F->assign(25, 0.);
		(*F)[0] = 1.; (*F)[6] = 1.; (*F)[12] = 1.; (*F)[18] = 1.; (*F)[24] = 1.;
	}

	std::vector<CbmLitMaterialInfo> inter;
	if (fNavigator->FindIntersections(par, zOut, inter) == kLITERROR) {
		std::cout << "-E- CbmLitMyTrackPropagator::Propagate: navigation failed" << std::endl;
		return kLITERROR;
	}

	for(unsigned int iMat = 0; iMat < inter.size(); iMat++) {
		CbmLitMaterialInfo mat = inter[iMat];

		std::vector<myf>* Fnew = NULL;
		if (F != NULL) Fnew = new std::vector<myf>(25, 0.);
		if (fExtrapolator->Extrapolate(par, mat.GetZpos(), Fnew) == kLITERROR) {
			std::cout << "-E- CbmLitMyTrackPropagator::Propagate extrapolation failed" << std::endl;
			return kLITERROR;
		}

		// update transport matrix
		if (F != NULL) UpdateF(*F, *Fnew);
		delete Fnew;

		//scale material length
		myf norm = std::sqrt(1. + par->GetTx() * par->GetTx() + par->GetTy() * par->GetTy());
		mat.SetLength(mat.GetLength() * norm);
		// add material effects
		fMaterial->Update(par, &mat, pdg, downstream);
	}
	std::vector<myf>* Fnew = NULL;
	if (F != NULL) Fnew = new std::vector<myf>(25, 0.);
	fExtrapolator->Extrapolate(par, zOut, Fnew);
	if (F != NULL) UpdateF(*F, *Fnew);
	delete Fnew;

	return kLITSUCCESS;
}

void CbmLitMyTrackPropagator::UpdateF(
		std::vector<myf>& F,
		const std::vector<myf>& newF)
{
	std::vector<myf> A(25);
	Mult25(newF, F, A);
	F.assign(A.begin(), A.end());
}
