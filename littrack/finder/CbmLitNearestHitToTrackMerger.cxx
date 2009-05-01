#include "CbmLitNearestHitToTrackMerger.h"
#include "CbmLitHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"
#include "CbmLitStation.h"
#include "CbmLitSubstation.h"
#include "CbmLitDetectorLayout.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTypes.h"
#include "CbmLitMath.h"

#include <iostream>


CbmLitNearestHitToTrackMerger::CbmLitNearestHitToTrackMerger()
{
}

CbmLitNearestHitToTrackMerger::~CbmLitNearestHitToTrackMerger()
{
}

LitStatus CbmLitNearestHitToTrackMerger::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitNearestHitToTrackMerger::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitNearestHitToTrackMerger::DoMerge(
		HitPtrVector& hits,
		TrackPtrVector& tracks)
{
	//loop over tracks
	for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
		CbmLitTrack* track = *it;
		CbmLitTrackParam par(*track->GetParamLast());
		Bool_t hitAdded = false;
		for (Int_t substation = 0; substation < fStation.GetNofSubstations(); substation++) {
			//propagate each track to plane where to merge
			fPropagator->Propagate(&par, fStation.GetSubstation(substation).GetZ(), fPDG);
			//fast search of hits (optional)
			//HitPtrIteratorPair bounds = MinMaxIndex(&par, hitData, layout, stationGroup, station, substation);
			HitPtrIteratorPair bounds = HitPtrIteratorPair(hits.begin(), hits.end());
			//TODO: set parameters only if hit was added
			track->SetParamLast(&par);
			if (AddNearestHit(track, bounds)) hitAdded = true;
			//update track parameters
			//calculate chi square
			//attach nearest hit to track
		}
	}
}

Bool_t CbmLitNearestHitToTrackMerger::AddNearestHit(
		CbmLitTrack* track,
		HitPtrIteratorPair bounds) const
{
	Bool_t hitAdded = false;
	const CbmLitTrackParam* par = track->GetParamLast();
	CbmLitTrackParam uPar, param;
	HitPtrIterator hit(bounds.second);
	Double_t chiSq = 1e10;
	for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
		fFilter->Update(par, &uPar, *iHit);
		if (IsHitInValidationGate(&uPar, *iHit)) {
			Double_t chi = ChiSq(&uPar, *iHit);
			if (chi < chiSq) {
				chiSq = chi;
				hit = iHit;
				param = uPar;
			}
		}
	}
	if (hit != bounds.second) {
		track->AddHit(*hit);
		track->SetParamLast(&param);
		track->SetChi2(track->GetChi2() + chiSq);
		track->SetNDF(NDF(track));
		hitAdded = true;
//		std::cout << "hit added:" << (*hit)->ToString();
//		std::cout << "chiSq=" << chiSq << std::endl;
	}
	return hitAdded;
}

ClassImp(CbmLitNearestHitToTrackMerger);
