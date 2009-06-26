#include "CbmLitTrackFinderWeight.h"

#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderWeight::CbmLitTrackFinderWeight()
{
}

CbmLitTrackFinderWeight::~CbmLitTrackFinderWeight()
{
}

LitStatus CbmLitTrackFinderWeight::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderWeight::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderWeight::DoFind(
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

	for (int iIter = 0; iIter < fNofIter; iIter++) {
		SetIterationParameters(iIter);
		ArrangeHits(fHitArray.begin(), fHitArray.end());
		InitTrackSeeds(fTrackSeedArray.begin(), fTrackSeedArray.end());
		FollowTracks(fTracks.begin(), fTracks.end());
		FitTracks(fTracks.begin(), fTracks.end());
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

void CbmLitTrackFinderWeight::FollowTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		FollowTrack(*it);
	}
}

void CbmLitTrackFinderWeight::FollowTrack(
		CbmLitTrack *track)
{
	int nofStationGroups = fLayout.GetNofStationGroups();
	for(int iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
		if (!ProcessStationGroup(track, iStationGroup)) return;
	}
}

bool CbmLitTrackFinderWeight::ProcessStationGroup(
		CbmLitTrack *track,
		int stationGroup)
{
	int nofStations = fLayout.GetNofStations(stationGroup);
	for(int iStation = 0; iStation < nofStations; iStation++){
		if (!ProcessStation(track, stationGroup, iStation)) {
			track->SetNofMissingHits(track->GetNofMissingHits() + 1);
			if (track->GetNofMissingHits() > fMaxNofMissingHits) return false;
		}
	}
	track->SetPDG(fPDG);
	track->SetLastPlaneId(stationGroup);
	return true;
}

bool CbmLitTrackFinderWeight::ProcessStation(
		CbmLitTrack *track,
		int stationGroup,
		int station)
{
	bool hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast());
	int nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		double z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		fPropagator->Propagate(&par, z, fPDG);
		track->SetParamLast(&par);
		HitPtrIteratorPair bounds = MinMaxIndex(&par, stationGroup, station, iSubstation);
		if (AddHits(track, bounds)) hitAdded = true;
	}
	return hitAdded;
}

bool CbmLitTrackFinderWeight::AddHits(
		CbmLitTrack* track,
		HitPtrIteratorPair bounds)
{
	bool hitAdded = false;
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

void CbmLitTrackFinderWeight::FitTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->GetNofHits() < 11) (*it)->SetQuality(kLITBAD);
		if ((*it)->GetLastPlaneId() < 5) {(*it)->SetQuality(kLITBAD); continue;}
		if (fFitter->Fit(*it) == kLITERROR) (*it)->SetQuality(kLITBAD);
		if ((*it)->GetNofHits() < 12) (*it)->SetQuality(kLITBAD);
//		if ((*it)->GetLastPlaneId() < 40) (*it)->SetQuality(kLITBAD);
//		if ((*it)->GetChi2() / (*it)->GetNDF() > 200) (*it)->SetQuality(kLITBAD);
	}
}
