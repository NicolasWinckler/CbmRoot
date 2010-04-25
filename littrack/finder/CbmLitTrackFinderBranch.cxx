/** CbmLitTrackFinderBranch.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 **/

#include "CbmLitTrackFinderBranch.h"

#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitMath.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"
#include "CbmLitHit.h"
#include "CbmLitComparators.h"
#include "CbmLitHitChiSq.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderBranch::CbmLitTrackFinderBranch():
	fMaxNofHitsInValidationGate(3),
	fIsProcessSubstationsTogether(true),
	fMaxNofBranches(27)
{
}

CbmLitTrackFinderBranch::~CbmLitTrackFinderBranch()
{
}

LitStatus CbmLitTrackFinderBranch::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderBranch::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderBranch::DoFind(
		HitPtrVector& hits,
		TrackPtrVector& trackSeeds,
		TrackPtrVector& tracks)
{
	fTracks.clear();
	fFoundTracks.clear();
	fUsedSeedsSet.clear();
	fUsedHitsSet.clear();

	fHitData.SetDetectorLayout(fLayout);

	for (int iIter = 0; iIter < fNofIter; iIter++) {
		SetIterationParameters(iIter);
		ArrangeHits(hits.begin(), hits.end());
		InitTrackSeeds(trackSeeds.begin(), trackSeeds.end());
		FollowTracks();
		fFinalSelection->DoSelect(fTracksCopy.begin(), fTracksCopy.end());
		RemoveHits(fTracksCopy.begin(), fTracksCopy.end());
		CopyToOutput(fTracksCopy.begin(), fTracksCopy.end(), tracks);

		for_each(fTracksCopy.begin(), fTracksCopy.end(), DeleteObject());
		fTracksCopy.clear();
		fHitData.Clear();
	}
	std::cout << "-I- CbmLitTrackFinderBranch: " << fEventNo++	<< " events processed" << std::endl;
	return kLITSUCCESS;
}

void CbmLitTrackFinderBranch::InitTrackSeeds(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	//TODO if more than one iteration, restore the state of the seeds
	fSeedSelection->DoSelect(itBegin, itEnd);

	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		CbmLitTrack* track = *it;
		if (track->GetQuality() == kLITBAD) continue;
		if (fUsedSeedsSet.find(track->GetPreviousTrackId()) != fUsedSeedsSet.end()) continue;
		CbmLitTrack* newTrack = new CbmLitTrack(*track);
		newTrack->SetPDG(fPDG);
		newTrack->SetChi2(0.);
		fTracks.push_back(newTrack);
	}
}

void CbmLitTrackFinderBranch::FollowTracks()
{
	// Loop over the station groups
	for (int stationGroup = 0; stationGroup < fLayout.GetNofStationGroups(); stationGroup++) {

		// Loops over fTracks, processes the station group and
		// writes output tracks to fFoundTracks array
		for (TrackPtrIterator it = fTracks.begin(); it != fTracks.end(); it++)
			ProcessStationGroup(*it, stationGroup);

		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();

		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderBranch:"
				<< fFoundTracks.size() << " followed in the "
				<< stationGroup << " station group" << std::endl;

		fStationGroupSelection->DoSelect(fFoundTracks.begin(), fFoundTracks.end());
		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderBranch: track selection in station group finished" << std::endl;

		// Copy fFoundTracks to fTracksCopy array to store also short tracks
		CopyToOutputArray();
		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderBranch: copy to output array finished" << std::endl;

	} //loop over station groups

	for_each(fTracks.begin(), fTracks.end(), DeleteObject());
	fTracks.clear();
}

void CbmLitTrackFinderBranch::ProcessStationGroup(
		const CbmLitTrack* track,
		int stationGroup)
{
   int nofStations = fLayout.GetNofStations(stationGroup);
   std::vector<TrackPtrVector> tracks(nofStations);
   fNofBranches = 0;

   if (ProcessStation(track, stationGroup, 0, tracks[0])){ //0
	   for (TrackPtrIterator trk0 = tracks[0].begin(); trk0 != tracks[0].end(); trk0++) { //1
		   if (!ProcessStation(*trk0, stationGroup, 1, tracks[1])) continue;

	       if (nofStations < 3) {
	    	  if (!AddTrackCandidate(tracks[1], stationGroup)) return;
	    	  continue;
	       }

	       for (TrackPtrIterator trk1 = tracks[1].begin(); trk1 != tracks[1].end(); trk1++) { //2
	    	   if (!ProcessStation(*trk1, stationGroup, 2, tracks[2])) continue;

	    	   if (nofStations < 4) {
	    		  if (!AddTrackCandidate(tracks[2], stationGroup)) return;
	    		  continue;
	    	   }

	    	   for (TrackPtrIterator trk2 = tracks[2].begin(); trk2 != tracks[2].end(); trk2++) { //3
	    		   if (!ProcessStation(*trk2, stationGroup, 3, tracks[3])) continue;
	    		   if (!AddTrackCandidate(tracks[3], stationGroup)) return;
	    	   } //3
	       } //2
	   }//1
   }//0

   for (int i = 0; i < nofStations; i++) {
	   for_each(tracks[i].begin(), tracks[i].end(), DeleteObject());
	   tracks[i].clear();
   }
   tracks.clear();
}

bool CbmLitTrackFinderBranch::ProcessStation(
		const CbmLitTrack *track,
		int stationGroup,
		int station,
		TrackPtrVector& tracksOut)
{
	for_each(tracksOut.begin(), tracksOut.end(), DeleteObject());
	tracksOut.clear();

	bool isBranchCreated;
	if (fIsProcessSubstationsTogether)
		isBranchCreated = ProcessStation1(track, stationGroup, station, tracksOut);
	else
		isBranchCreated = ProcessStation2(track, stationGroup, station, tracksOut);

	bool isMissingHitOk = false;
	if (fIsAlwaysCreateMissingHit || !isBranchCreated) {
		int nofMissingHits = track->GetNofMissingHits() + 1;
		if (nofMissingHits <= fMaxNofMissingHits) {
			CbmLitTrack* trk = new CbmLitTrack(*track);
			trk->SetNofMissingHits(nofMissingHits);
			tracksOut.push_back(trk);
			isMissingHitOk = true;
		}
	}

	if (isBranchCreated) return true;
	else return isMissingHitOk;
}

bool CbmLitTrackFinderBranch::ProcessStation1(
			const CbmLitTrack *track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut)
{
	// if fIsProcessSubstationsTogether == true
	bool result = false;
	CbmLitTrackParam par(*track->GetParamLast());
	HitPtrIteratorPair bounds;
	std::vector<CbmLitHitChiSq> hits;
	int nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		myf z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		if (fPropagator->Propagate(&par, z, fPDG) == kLITERROR) return false;
		bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation),
				fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation));
		ProcessSubstation(&par, bounds, hits);
	}

	if (hits.empty()) return false;
	int nofHits = hits.size();
	// if too many hits in the validation gate,
	// sort hits in the validation gate by the chi-square
	if (nofHits > fMaxNofHitsInValidationGate){
		std::sort(hits.begin(), hits.end(), CompareHitChiSqLess());
	}

	//Select the best hits to be attached
	//Create track branches
	int N = (nofHits > fMaxNofHitsInValidationGate) ? fMaxNofHitsInValidationGate : nofHits;
	for (int iHit = 0; iHit < N; iHit++) {
		CbmLitTrack* newTrack = new CbmLitTrack(*track);
		newTrack->AddHit(hits[iHit].GetHit());
		newTrack->SetChi2(newTrack->GetChi2() + hits[iHit].GetChiSq());
		newTrack->SetPDG(fPDG);
		newTrack->SetParamLast(hits[iHit].GetParam());
		tracksOut.push_back(newTrack);
		result = true;
	}

	return result;
}

bool CbmLitTrackFinderBranch::ProcessStation2(
			const CbmLitTrack *track,
			int stationGroup,
			int station,
			TrackPtrVector& tracksOut)
{
	// if fIsProcessSubstationsTogether == true
	bool result = false;
	CbmLitTrackParam par(*track->GetParamLast());
	int nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	for (int iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		HitPtrIteratorPair bounds;
		std::vector<CbmLitHitChiSq> hits;

		myf z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		if (fPropagator->Propagate(&par, z, fPDG) == kLITERROR) return false;
		bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation),
				fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation));
		ProcessSubstation(&par, bounds, hits);

		if (hits.empty()) continue; //return false;
		int nofHits = hits.size();
		// if too many hits in the validation gate,
		// sort hits in the validation gate by the chi-square
		if (nofHits > fMaxNofHitsInValidationGate){
			std::sort(hits.begin(), hits.end(), CompareHitChiSqLess());
		}

		//Select the best hits to be attached
		//Create track branches
		int N = (nofHits > fMaxNofHitsInValidationGate) ? fMaxNofHitsInValidationGate : nofHits;
		for (int iHit = 0; iHit < N; iHit++) {
			CbmLitTrack* newTrack = new CbmLitTrack(*track);
			newTrack->AddHit(hits[iHit].GetHit());
			newTrack->SetChi2(newTrack->GetChi2() + hits[iHit].GetChiSq());
			newTrack->SetPDG(fPDG);
			newTrack->SetParamLast(hits[iHit].GetParam());
			tracksOut.push_back(newTrack);
			result = true;
		}
	}

	return result;
}

void CbmLitTrackFinderBranch::ProcessSubstation(
		const CbmLitTrackParam* par,
		HitPtrIteratorPair bounds,
		std::vector<CbmLitHitChiSq>& hits)
{
	CbmLitTrackParam uPar;
	for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
		CbmLitHit* hit = *iHit;
		fFilter->Update(par, &uPar, hit);
		if (IsHitInValidationGate(&uPar, hit)) {
			myf chi = ChiSq(&uPar, hit);
			CbmLitHitChiSq h;

			h.SetHit(hit);
			h.SetParam(&uPar);
			h.SetChiSq(chi);
			hits.push_back(h);
		}
	}
}

bool CbmLitTrackFinderBranch::AddTrackCandidate(
		TrackPtrVector& tracks,
		int stationGroup)
{
	for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
		fNofBranches++;
		if (fNofBranches > fMaxNofBranches) return false;
		CbmLitTrack* newTrack = new CbmLitTrack(**it);
		newTrack->SetLastPlaneId(stationGroup);
		newTrack->SetNDF(NDF(newTrack));
		fFoundTracks.push_back(newTrack);
	}
	return true;
}

void CbmLitTrackFinderBranch::CopyToOutputArray()
{
	for (TrackPtrIterator iTrack = fFoundTracks.begin(); iTrack != fFoundTracks.end(); iTrack++) {
		if ((*iTrack)->GetQuality() == kLITBAD) {
			delete (*iTrack);
		} else {
			fTracks.push_back(*iTrack);
			fTracksCopy.push_back(new CbmLitTrack(*(*iTrack)));
		}
	}
	fFoundTracks.clear();
}
