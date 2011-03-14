/** CbmLitTrackFitterImp.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

#include "fitter/CbmLitTrackFitterImp.h"

#include "data/CbmLitTrackParam.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMath.h"

#include <iostream>

CbmLitTrackFitterImp::CbmLitTrackFitterImp(
		TrackPropagatorPtr propagator,
		TrackUpdatePtr update)
{
	fPropagator = propagator;
	fUpdate = update;
}

CbmLitTrackFitterImp::~CbmLitTrackFitterImp()
{
}

LitStatus CbmLitTrackFitterImp::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterImp::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterImp::Fit(
		CbmLitTrack *track,
		bool downstream)
{
	track->SortHits(downstream);
	track->SetChi2(0.0);
	int nofHits = track->GetNofHits();
	FitNodeVector nodes(nofHits);
	CbmLitTrackParam par;
	std::vector<myf> F(25);

	if (downstream) {
	    track->SetParamLast(track->GetParamFirst());
	    par = *track->GetParamLast();
	} else {
		track->SetParamFirst(track->GetParamLast());
		par = *track->GetParamFirst();
	}

	for (int i = 0; i < nofHits; i++) {
		const CbmLitHit* hit = track->GetHit(i);
	    myf Ze = hit->GetZ();
	    if (fPropagator->Propagate(&par, Ze, track->GetPDG(), &F) == kLITERROR) {
	    	//std::cout << "-E- CbmLitTrackFitterImp::Fit: propagation failed" << std::endl;
	    	track->SetQuality(kLITBAD);
	    	return kLITERROR;
	    }
	    nodes[i].SetPredictedParam(&par);
	    nodes[i].SetF(F);
	    myf chi2Hit = 0.;
	    if (fUpdate->Update(&par, hit, chi2Hit) == kLITERROR) {
	    	//std::cout << "-E- CbmLitTrackFitterImp::Fit: track update failed" << std::endl;
	    	track->SetQuality(kLITBAD);
	    	return kLITERROR;
	    }
	    nodes[i].SetUpdatedParam(&par);
	    nodes[i].SetChiSqFiltered(chi2Hit);
	    track->SetChi2(track->GetChi2() + chi2Hit);
	}
	if (downstream) track->SetParamLast(&par);
	else track->SetParamFirst(&par);

	track->SetFitNodes(nodes);
	track->SetNDF(lit::NDF(track));

	return kLITSUCCESS;
}
