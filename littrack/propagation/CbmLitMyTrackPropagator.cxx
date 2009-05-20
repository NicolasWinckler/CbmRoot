#include "CbmLitMyTrackPropagator.h"

CbmLitMyTrackPropagator::CbmLitMyTrackPropagator(
		TrackExtrapolatorPtr extrapolator)
{

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
        Double_t zOut,
        Int_t pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitMyTrackPropagator::Propagate(
		CbmLitTrackParam *par,
        Double_t zOut,
        Int_t pdg)

{

}

void CbmLitMyTrackPropagator::TransportMatrix(
		   std::vector<Double_t>& F)
{

}

void CbmLitMyTrackPropagator::TransportMatrix(
		   TMatrixD& F)
{

}
ClassImp(CbmLitMyTrackPropagator);
