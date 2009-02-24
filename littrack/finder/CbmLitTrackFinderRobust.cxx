#include "CbmLitTrackFinderRobust.h"

#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderRobust::CbmLitTrackFinderRobust()
{
}

CbmLitTrackFinderRobust::~CbmLitTrackFinderRobust()
{
}

LitStatus CbmLitTrackFinderRobust::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderRobust::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderRobust::DoFind(
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
//		FitTracks(fTracks.begin(), fTracks.end());
		fFinalSelection->DoSelect(fTracks.begin(), fTracks.end());
		RemoveHits(fTracks.begin(), fTracks.end());
		CopyToOutput(fTracks.begin(), fTracks.end(), tracks);

		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();
		fHitData.Clear();
	}

	std::cout << "-I- CbmLitTrackFinderRobust: " << fEventNo++ << " events processed" << std::endl;

	return kLITSUCCESS;
}

void CbmLitTrackFinderRobust::FollowTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		FollowTrack(*it);
	}
}

void CbmLitTrackFinderRobust::FollowTrack(
		CbmLitTrack *track)
{
	Int_t nofStationGroups = fLayout.GetNofStationGroups();
	for(Int_t iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
		if (!ProcessStationGroup(track, iStationGroup)) return;
	}
}

Bool_t CbmLitTrackFinderRobust::ProcessStationGroup(
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

Bool_t CbmLitTrackFinderRobust::ProcessStation(
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
		if (AddHits(track, bounds)) hitAdded = true;
	}
	return hitAdded;
}

Bool_t CbmLitTrackFinderRobust::AddHits(
		CbmLitTrack* track,
		HitPtrIteratorPair bounds)
{
	Bool_t hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast()), uPar;
	for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
		fFilter->Update(&par, &uPar, *iHit);
		if (IsHitInValidationWindow(&uPar, *iHit)) {
			track->AddHit(*iHit);
			hitAdded = true;
		}
	}
	return hitAdded;
}

void CbmLitTrackFinderRobust::FitTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		if (fFitter->Fit(*it) == kLITERROR) (*it)->SetQuality(kLITBAD);
//		if ((*it)->GetLastPlaneId() < 20) (*it)->SetQuality(kLITBAD);
//		if ((*it)->GetChi2() / (*it)->GetNDF() > 9) (*it)->SetQuality(kLITBAD);
	}
}
ClassImp(CbmLitTrackFinderRobust)
