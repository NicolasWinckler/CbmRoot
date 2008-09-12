// -------------------------------------------------------------------------
// -----                  CbmLitTrackPropagatorGeane source file               -----
// -----                  Created 22/11/07  by A. Lebedev               -----
// -------------------------------------------------------------------------

#include "CbmLitTrackPropagatorGeane.h"

#include "CbmLitTrackParam.h"
#include "CbmLitConverter.h"

#include "CbmPropagatorGeane.h"
#include "CbmTrackParam.h"
#include "CbmTrackParP.h"

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
        Double_t zOut,
        Int_t pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

LitStatus CbmLitTrackPropagatorGeane::Propagate(
		CbmLitTrackParam *par,
        Double_t zOut,
        Int_t pdg)
{
	CbmTrackParam param;
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

ClassImp(CbmLitTrackPropagatorGeane)
