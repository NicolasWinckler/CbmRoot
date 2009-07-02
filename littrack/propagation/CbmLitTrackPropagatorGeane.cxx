#include "CbmLitTrackPropagatorGeane.h"

#include "CbmLitTrackParam.h"
#include "CbmLitConverter.h"

#include "CbmPropagatorGeane.h"
#include "FairTrackParam.h"
#include "FairTrackParP.h"

#include "TVector3.h"

#include <cmath>

CbmLitTrackPropagatorGeane::CbmLitTrackPropagatorGeane():
   CbmLitTrackPropagator("CbmLitTrackPropagatorGeane")
{
   fPropagator = new CbmPropagatorGeane();
}

CbmLitTrackPropagatorGeane::~CbmLitTrackPropagatorGeane()
{
	if (fPropagator) delete fPropagator;
}

LitStatus CbmLitTrackPropagatorGeane::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackPropagatorGeane::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackPropagatorGeane::Propagate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        myf zOut,
        int pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitTrackPropagatorGeane::Propagate(
		CbmLitTrackParam *par,
        myf zOut,
        int pdg)
{
	FairTrackParam param;
	StatusCode result;

	CbmLitConverter::LitTrackParamToTrackParam(par, &param);

	if (pdg == 13) pdg = (par->GetQp() > 0) ? 13 : -13;

	result = fPropagator->Propagate(&param, zOut, pdg);

	if (result == kCBMSUCCESS) {
		CbmLitConverter::TrackParamToLitTrackParam(&param, par);
		return kLITSUCCESS;
	} else {
		return kLITERROR;
	}
}
