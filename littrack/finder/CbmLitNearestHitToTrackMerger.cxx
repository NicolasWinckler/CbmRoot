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
		bool hitAdded = false;
		for (int substation = 0; substation < fStation.GetNofSubstations(); substation++) {
			//propagate each track to plane where to merge
			fPropagator->Propagate(&par, fStation.GetSubstation(substation).GetZ(), fPDG);
			//fast search of hits (optional)
			//HitPtrIteratorPair bounds = MinMaxIndex(&par, hitData, layout, stationGroup, station, substation);
			HitPtrIteratorPair bounds = HitPtrIteratorPair(hits.begin(), hits.end());
			//TODO: set parameters only if hit was added
			track->SetParamLast(&par);
			hitAdded = AddNearestHit(track, bounds);
			//update track parameters
			//calculate chi square
			//attach nearest hit to track
		}
	}
	return kLITSUCCESS;
}

bool CbmLitNearestHitToTrackMerger::AddNearestHit(
		CbmLitTrack* track,
		HitPtrIteratorPair bounds) const
{
	bool hitAdded = false;
	const CbmLitTrackParam* par = track->GetParamLast();
	CbmLitTrackParam uPar, param;
	HitPtrIterator hit(bounds.second);
	myf chiSq = 1e10;
	for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
		fFilter->Update(par, &uPar, *iHit);
		if (IsHitInValidationGate(&uPar, *iHit)) {
			myf chi = ChiSq(&uPar, *iHit);
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
	}
	return hitAdded;
}
