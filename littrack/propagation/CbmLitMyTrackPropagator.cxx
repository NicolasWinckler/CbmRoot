#include "CbmLitMyTrackPropagator.h"
#include "CbmLitMyGeoNavigator.h"
#include "CbmLitMaterialEffectsImp.h"

#include <cmath>
#include <iostream>

CbmLitMyTrackPropagator::CbmLitMyTrackPropagator(
		TrackExtrapolatorPtr extrapolator):
			fExtrapolator(extrapolator),
			fCalcTransportMatrix(true)
{
	fNavigator = GeoNavigatorPtr(new CbmLitMyGeoNavigator());
	fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());
	fFm.ResizeTo(5,5);
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
        double zOut,
        int pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitMyTrackPropagator::Propagate(
		CbmLitTrackParam *par,
        double zOut,
        int pdg)

{
	fPDG = pdg;
	double zIn = par->GetZ();
	double dz = zOut - zIn;
	if(std::fabs(dz) < 1e-3) return kLITSUCCESS;

	//Check whether upstream or downstream
	fDownstream = dz > 0;

	if (fCalcTransportMatrix) fFm.UnitMatrix();

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
			TMatrixD Fnew(5,5);
			fExtrapolator->TransportMatrix(Fnew);
			UpdateF(fFm, Fnew);
		}

		//scale material length
		double norm = std::sqrt(1. + par->GetTx() * par->GetTx() + par->GetTy() * par->GetTy());
		mat.SetLength(mat.GetLength() * norm);
		// add material effects
		fMaterial->Update(par, &mat, fPDG, fDownstream);
	}
	fExtrapolator->Extrapolate(par, zOut);
	return kLITSUCCESS;
}

void CbmLitMyTrackPropagator::TransportMatrix(
		   std::vector<double>& F)
{
	F.assign(fFm.GetMatrixArray(), fFm.GetMatrixArray() + fFm.GetNoElements());
}

void CbmLitMyTrackPropagator::TransportMatrix(
		   TMatrixD& F)
{
	F = fFm;
}

void CbmLitMyTrackPropagator::UpdateF(
		TMatrixD& F,
		const TMatrixD& newF)
{
	F = newF * F;
}

