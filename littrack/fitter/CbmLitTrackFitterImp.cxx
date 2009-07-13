#include "CbmLitTrackFitterImp.h"

#include "CbmLitTrackParam.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitMath.h"

#include <iostream>

CbmLitTrackFitterImp::CbmLitTrackFitterImp(
		TrackPropagatorPtr propagator,
		TrackUpdatePtr update):
	fDownstream(true)
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
	fDownstream = downstream;
	track->SortHits(fDownstream);
	track->SetChi2(0.0);
	int nofHits = track->GetNofHits();
	FitNodeVector nodes(nofHits);
	CbmLitTrackParam par;
	std::vector<myf> F(25);

	if (fDownstream) {
	    track->SetParamLast(track->GetParamFirst());
	    par = *track->GetParamLast();
	} else {
		track->SetParamFirst(track->GetParamLast());
		par = *track->GetParamFirst();
	}

	for (int i = 0; i < nofHits; i++) {
		const CbmLitHit* hit = track->GetHit(i);
	    myf Ze = hit->GetZ();
	    if (Ze > 500. && hit->GetDetectorId() != kLITTOF) {
	    	std::cout << std::endl << std::endl << std::endl;
	    	std::cout << "WWAAAAAAAAAAAAAAAARNINNNNNNNNNNNNNNNNNNGGGGGGGGG!!!!!!!!" << std::endl;
	    	std::cout << hit->ToString();
	    	std::cout << track->ToString();
	    }
	    if (fPropagator->Propagate(&par, Ze, track->GetPDG()) == kLITERROR) {
	    	//std::cout << "-E- CbmLitTrackFitterImp::Fit: propagation failed" << std::endl;
	    	track->SetQuality(kLITBAD);
	    	return kLITERROR;
	    }
	    nodes[i].SetPredictedParam(&par);
	    fPropagator->TransportMatrix(F);
	    nodes[i].SetF(F);
	    if (fUpdate->Update(&par, hit) == kLITERROR) {
	    	//std::cout << "-E- CbmLitTrackFitterImp::Fit: track update failed" << std::endl;
	    	track->SetQuality(kLITBAD);
	    	return kLITERROR;
	    }
	    nodes[i].SetUpdatedParam(&par);
	    myf chi2Hit = ChiSq(&par, hit);
	    nodes[i].SetChiSqFiltered(chi2Hit);
	    track->SetChi2(track->GetChi2() + chi2Hit);
	}
	if (fDownstream) track->SetParamLast(&par);
	else track->SetParamFirst(&par);

	track->SetFitNodes(nodes);
	track->SetNDF(NDF(track));

	return kLITSUCCESS;
}
