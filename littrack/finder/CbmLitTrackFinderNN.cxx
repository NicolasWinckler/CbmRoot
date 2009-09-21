/** CbmLitTrackFinderNN.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

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

CbmLitTrackFinderNN::CbmLitTrackFinderNN():
	fIsProcessSubstationsTogether(false)
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
		HitPtrVector& hits,
		TrackPtrVector& trackSeeds,
		TrackPtrVector& tracks)
{
	fTracks.clear();
	fUsedSeedsSet.clear();
	fUsedHitsSet.clear();
	fHitData.SetDetectorLayout(fLayout);

	for (int iIter = 0; iIter < fNofIter; iIter++) {
		SetIterationParameters(iIter);
		ArrangeHits(hits.begin(), hits.end());
		InitTrackSeeds(trackSeeds.begin(), trackSeeds.end());
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

void CbmLitTrackFinderNN::InitTrackSeeds(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	//TODO if more than one iteration, restore the state of the seeds
	fSeedSelection->DoSelect(itBegin, itEnd);

	for (TrackPtrIterator track = itBegin; track != itEnd; track++) {
		if ((*track)->GetQuality() == kLITBAD) continue;
		if (fUsedSeedsSet.find((*track)->GetPreviousTrackId()) != fUsedSeedsSet.end()) continue;
		(*track)->SetPDG(fPDG);
		fTracks.push_back(new CbmLitTrack(*(*track)));
	}
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
	int nofStationGroups = fLayout.GetNofStationGroups();
	for(int iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
		if (!ProcessStationGroup(track, iStationGroup)) return;
	}
}

bool CbmLitTrackFinderNN::ProcessStationGroup(
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

bool CbmLitTrackFinderNN::ProcessStation(
		CbmLitTrack *track,
		int stationGroup,
		int station)
{
	bool hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast());
	int nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	HitPtrVector hits;
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		myf z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		fPropagator->Propagate(&par, z, fPDG);
		track->SetParamLast(&par);
		HitPtrIteratorPair bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation),
				fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation));
		hits.insert(hits.end(), bounds.first, bounds.second);
		if (!fIsProcessSubstationsTogether) {
			if (AddNearestHit(track, hits)) hitAdded = true;
			hits.clear();
		}
	}
	if (fIsProcessSubstationsTogether && AddNearestHit(track, hits)) hitAdded = true;
	return hitAdded;
}

bool CbmLitTrackFinderNN::AddNearestHit(
		CbmLitTrack* track,
		HitPtrVector& hits)
{
	bool hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast()), uPar, param;
	HitPtrIterator hit(hits.end());
	myf chiSq = 1e10;
	for (HitPtrIterator iHit = hits.begin(); iHit != hits.end(); iHit++) {
		//First update track parameters with KF, than check whether the hit is in the validation gate.
		fFilter->Update(&par, &uPar, *iHit);
		if (IsHitInValidationGate(&uPar, *iHit)) {
			myf chi = ChiSq(&uPar, *iHit);
			// Check if current hit is closer by statistical distance than the previous ones
			if (chi < chiSq) {
				chiSq = chi;
				hit = iHit;
				param = uPar;
			}
		}
	}
	// if hit was attached than change track information
	if (hit != hits.end()) {
		track->AddHit(*hit);
		track->SetParamLast(&param);
		track->SetChi2(track->GetChi2() + chiSq);
		track->SetNDF(NDF(track));
		hitAdded = true;
	}
	return hitAdded;
}
