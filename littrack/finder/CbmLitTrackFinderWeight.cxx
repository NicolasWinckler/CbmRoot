#include "finder/CbmLitTrackFinderWeight.h"

#include "data/CbmLitHitChiSq.h"
#include "interface/CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackFitter.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderWeight::CbmLitTrackFinderWeight():
	fTracks(),
	fFitter(),
	fFilter(),
    fSeedSelection(),
    fFinalSelection(),
    fPropagator(),
	fMaxNofHitsInValidationGate(4)
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

void CbmLitTrackFinderWeight::InitTrackSeeds(
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
	std::vector<CbmLitHitChiSq> hits;
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		myf z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		fPropagator->Propagate(&par, z, fPDG);
		track->SetParamLast(&par);
		HitPtrIteratorPair bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation),
				fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation));
		AddHits(track, bounds, hits);
	}

	if (hits.empty()) return false;
	int nofHits = hits.size();
	// if too many hits in the validation gate,
	// sort hits in the validation gate by the chi-square
	if (nofHits > fMaxNofHitsInValidationGate){
		std::sort(hits.begin(), hits.end(), CompareHitChiSqLess());
	}

	//Select the best hits to be attached
	int N = (nofHits > fMaxNofHitsInValidationGate) ? fMaxNofHitsInValidationGate : nofHits;
	for (int iHit = 0; iHit < N; iHit++) {
	    track->AddHit(hits[iHit].GetHit());
		hitAdded = true;
	}

	return hitAdded;
}

bool CbmLitTrackFinderWeight::AddHits(
		CbmLitTrack* track,
		HitPtrIteratorPair bounds,
		std::vector<CbmLitHitChiSq>& hits)
{
	bool hitAdded = false;
	CbmLitTrackParam par(*track->GetParamLast()), uPar;
	for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
		myf chi = 0.;
		fFilter->Update(&par, &uPar, *iHit, chi);
		if (IsHitInValidationGate(*iHit, chi)) {
			CbmLitHitChiSq h;
			h.SetHit(*iHit);
			h.SetParam(&uPar);
			h.SetChiSq(chi);
			hits.push_back(h);
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
		if ((*it)->GetNofHits() < 11) (*it)->SetQuality(kLITBAD);
//		if ((*it)->GetLastPlaneId() < 40) (*it)->SetQuality(kLITBAD);
//		if ((*it)->GetChi2() / (*it)->GetNDF() > 200) (*it)->SetQuality(kLITBAD);
	}
}
