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
			fExtrapolator(extrapolator),
			fCalcTransportMatrix(true)
{
	fNavigator = GeoNavigatorPtr(new CbmLitMyGeoNavigator());
	fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());
	fFm.resize(25);
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
        int pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitMyTrackPropagator::Propagate(
		CbmLitTrackParam *par,
        myf zOut,
        int pdg)

{
	fPDG = pdg;
	myf zIn = par->GetZ();
	myf dz = zOut - zIn;
	if(std::fabs(dz) < lit::MINIMUM_PROPAGATION_DISTANCE) return kLITSUCCESS;

	//Check whether upstream or downstream
	fDownstream = dz > 0;

	if (fCalcTransportMatrix) {
		std::fill(fFm.begin(), fFm.end(), 0.);
		fFm[0] = 1.; fFm[6] = 1.; fFm[12] = 1.; fFm[18] = 1.; fFm[24] = 1.;
	}

	std::vector<CbmLitMaterialInfo> inter;
	if (fNavigator->FindIntersections(par, zOut, inter) == kLITERROR) {
		std::cout << "-E- CbmLitMyTrackPropagator::Propagate: navigation failed" << std::endl;
		return kLITERROR;
	}

	for(unsigned int iMat = 0; iMat < inter.size(); iMat++) {
		CbmLitMaterialInfo mat = inter[iMat];

		if (fExtrapolator->Extrapolate(par, mat.GetZpos()) == kLITERROR) {
			std::cout << "-E- CbmLitMyTrackPropagator::Propagate extrapolation failed" << std::endl;
			return kLITERROR;
		}

		// update transport matrix
		if (fCalcTransportMatrix) {
			std::vector<myf> Fnew(25);
			fExtrapolator->TransportMatrix(Fnew);
			UpdateF(fFm, Fnew);
		}

		//scale material length
		myf norm = std::sqrt(1. + par->GetTx() * par->GetTx() + par->GetTy() * par->GetTy());
		mat.SetLength(mat.GetLength() * norm);
		// add material effects
		fMaterial->Update(par, &mat, fPDG, fDownstream);
	}
	fExtrapolator->Extrapolate(par, zOut);
	return kLITSUCCESS;
}

void CbmLitMyTrackPropagator::TransportMatrix(
		   std::vector<myf>& F)
{
	F.assign(fFm.begin(), fFm.end());
}

void CbmLitMyTrackPropagator::UpdateF(
		std::vector<myf>& F,
		const std::vector<myf>& newF)
{
	std::vector<myf> A(25);
	Mult25(newF, F, A);
	F.assign(A.begin(), A.end());
}
