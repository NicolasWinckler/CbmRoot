/** CbmLitTrackFinderNN.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/


#include "finder/CbmLitTrackFinderNN.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "interface/CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"

#include <algorithm>
#include <iostream>
#include <limits>

CbmLitTrackFinderNN::CbmLitTrackFinderNN():
	fIsProcessSubstationsTogether(true)
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

		for (int i = 0; i < tracks.size(); i++) {
			std::cout << "TRACK: " << tracks[i]->ToString();
			for (int j = 0; j < tracks[i]->GetNofHits(); j++){
				std::cout << " " << tracks[i]->GetHit(j)->GetRefId();
			}
			std::cout << std::endl;
		}

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
		(*track)->SetChi2(0.);
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
	int nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	CbmLitTrackParam par[nofSubstations];
	par[0] = *track->GetParamLast();
	HitPtrIteratorPair hits[nofSubstations];
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		myf z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		fPropagator->Propagate(&par[iSubstation], z, fPDG);
		if (iSubstation < nofSubstations - 1) par[iSubstation + 1] = par[iSubstation];
		HitPtrIteratorPair bounds = MinMaxIndex(&par[iSubstation], fHitData.GetHits(stationGroup, station, iSubstation),
				fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation));
		hits[iSubstation] = bounds;
	}

//	if (AddNearestHit1(track, hits, par, nofSubstations)) hitAdded = true;
	if (fIsProcessSubstationsTogether) hitAdded = AddNearestHit1(track, hits, par, nofSubstations);
	else hitAdded = AddNearestHit2(track, hits, par, nofSubstations);

	return hitAdded;
}

bool CbmLitTrackFinderNN::AddNearestHit1(
		CbmLitTrack* track,
		HitPtrIteratorPair hits[],
		const CbmLitTrackParam par[],
		int nofSubstations)
{
	//fIsProcessSubstationsTogether == true
	bool hitAdded = false;
	CbmLitTrackParam uPar, param;
	HitPtrIterator hit(hits[0].second);
	myf chiSq = std::numeric_limits<myf>::max();
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		for (HitPtrIterator iHit = hits[iSubstation].first; iHit != hits[iSubstation].second; iHit++) {
			//First update track parameters with KF, than check whether the hit is in the validation gate.
			myf chi = 0.;
			fFilter->Update(&par[iSubstation], &uPar, *iHit, chi);
			if (IsHitInValidationGate(*iHit, chi, &par[iSubstation])) {
				// Check if current hit is closer by statistical distance than the previous ones
				if (chi < chiSq) {
					chiSq = chi;
					hit = iHit;
					param = uPar;
				}
			}
		}
	}
	// if hit was attached than change track information
	if (hit != hits[0].second) {
		track->AddHit(*hit);
		track->SetParamLast(&param);
		track->SetChi2(track->GetChi2() + chiSq);
		track->SetNDF(lit::NDF(track));
		hitAdded = true;
	}
	return hitAdded;
}

bool CbmLitTrackFinderNN::AddNearestHit2(
		CbmLitTrack* track,
		HitPtrIteratorPair hits[],
		const CbmLitTrackParam par[],
		int nofSubstations)
{
	//fIsProcessSubstationsTogether == false
	bool hitAdded = false;
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		CbmLitTrackParam uPar, param;
		HitPtrIterator hit(hits[iSubstation].second);
		myf chiSq = std::numeric_limits<myf>::max();
		for (HitPtrIterator iHit = hits[iSubstation].first; iHit != hits[iSubstation].second; iHit++) {
			//First update track parameters with KF, than check whether the hit is in the validation gate.
			myf chi = 0.;
			fFilter->Update(&par[iSubstation], &uPar, *iHit, chi);
			if (IsHitInValidationGate(*iHit, chi, &par[iSubstation])) {
				// Check if current hit is closer by statistical distance than the previous ones
				if (chi < chiSq) {
					chiSq = chi;
					hit = iHit;
					param = uPar;
				}
			}
		}
		// if hit was attached than change track information
		if (hit != hits[iSubstation].second) {
			track->AddHit(*hit);
			track->SetParamLast(&param);
			track->SetChi2(track->GetChi2() + chiSq);
			track->SetNDF(lit::NDF(track));
			hitAdded = true;
		}
	}

	return hitAdded;
}
