#include "CbmLitTrackFinderNN.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitMath.h"
#include "CbmLitTrackUpdate.h"

#include <algorithm>
#include <iostream>

CbmLitTrackFinderNN::CbmLitTrackFinderNN()
{
}

CbmLitTrackFinderNN::~CbmLitTrackFinderNN()
{
}

LitStatus CbmLitTrackFinderNN::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderNN::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderNN::DoFind(
		const HitPtrVector& hits,
		const TrackPtrVector& trackSeeds,
		TrackPtrVector& tracks)
{
	//TODO copy links
	fHitArray = hits;
	fTrackSeedArray = trackSeeds;

	fTracks.clear();
	fSeedsIdSet.clear();
	fUsedHitsSet.clear();
	fHitData.SetDetectorLayout(fLayout);

	for (Int_t iIter = 0; iIter < fNofIter; iIter++) {
		SetIterationParameters(iIter);
		ArrangeHits(fHitArray.begin(), fHitArray.end());
		InitTrackSeeds(fTrackSeedArray.begin(), fTrackSeedArray.end());
		FollowTracks(fTracks.begin(), fTracks.end());
		fFinalSelection->DoSelect(fTracks.begin(), fTracks.end());
		RemoveHits(fTracks.begin(), fTracks.end());
		CopyToOutput(fTracks.begin(), fTracks.end(), tracks);

		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();
		fHitData.Clear();
	}
	std::cout << "-I- CbmLitTrackFinderNN: " << fEventNo++ << " events processed" << std::endl;

	return kLITSUCCESS;
}

void CbmLitTrackFinderNN::FollowTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		FollowTrack(*it);
	}
}

void CbmLitTrackFinderNN::FollowTrack(
		CbmLitTrack *track)
{
	Int_t nofStationGroups = fLayout.GetNofStationGroups();
	for(Int_t iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
		if (!ProcessStationGroup(track, iStationGroup)) return;
	}
}

Bool_t CbmLitTrackFinderNN::ProcessStationGroup(
		CbmLitTrack *track,
		Int_t stationGroup)
{
	Int_t nofStations = fLayout.GetNofStations(stationGroup);
	for(Int_t iStation = 0; iStation < nofStations; iStation++){
		if (!ProcessStation(track, stationGroup, iStation)) {
			track->SetNofMissingHits(track->GetNofMissingHits() + 1);
			if (track->GetNofMissingHits() > fMaxNofMissingHits) return false;
		}
	}
	track->SetPDG(fPDG);
	track->SetLastPlaneId(stationGroup);
	return true;
}

Bool_t CbmLitTrackFinderNN::ProcessStation(
		CbmLitTrack *track,
		Int_t stationGroup,
		Int_t station)
{
	Bool_t hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast());
	Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		Double_t z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		fPropagator->Propagate(&par, z, fPDG);
		track->SetParamLast(&par);
		HitPtrIteratorPair bounds = MinMaxIndex(&par, stationGroup, station, iSubstation);
		if (AddNearestHit(track, bounds)) hitAdded = true;
	}
	return hitAdded;
}

Bool_t CbmLitTrackFinderNN::AddNearestHit(
		CbmLitTrack* track,
		HitPtrIteratorPair bounds)
{
	Bool_t hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast()), uPar, param;
	HitPtrIterator hit(bounds.second);
	Double_t chiSq = 1e10;
	for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
		if (IsIn(&par, *iHit)) {
			fFilter->Update(&par, &uPar, *iHit);
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
	}
	return hitAdded;
}

ClassImp(CbmLitTrackFinderNN);
