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

#include <iostream>
#include <algorithm>

CbmLitTrackFinderBranch::CbmLitTrackFinderBranch()
{
	fMaxNofBranches = 32;
	fIsAlwaysCreateMissingHit = true;
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
		const HitPtrVector& hits,
		const TrackPtrVector& trackSeeds,
		TrackPtrVector& tracks)
{
	//TODO copy links
	fHitArray = hits;
	fTrackSeedArray = trackSeeds;

	fTracks.clear();
	fFoundTracks.clear();
	fSeedsIdSet.clear();
	fUsedHitsSet.clear();

	fHitData.SetDetectorLayout(fLayout);

	for (Int_t iIter = 0; iIter < fNofIter; iIter++) {

		SetIterationParameters(iIter);

		ArrangeHits(fHitArray.begin(), fHitArray.end());

		InitTrackSeeds(fTrackSeedArray.begin(), fTrackSeedArray.end());

		FollowTracks();

//		fFinalPreSelection->DoSelect(fTracksCopy.begin(), fTracksCopy.end());

//		RefitTracks(fTracksCopy.begin(), fTracksCopy.end());

		fFinalSelection->DoSelect(fTracksCopy.begin(), fTracksCopy.end());

		RemoveHits(fTracksCopy.begin(), fTracksCopy.end());

		CopyToOutput(fTracksCopy.begin(), fTracksCopy.end(), tracks);

		for_each(fTracksCopy.begin(), fTracksCopy.end(), DeleteObject());
		fTracksCopy.clear();

		fHitData.Clear();
	}

	std::cout << "-I- CbmLitTrackFinderBranch: " << fEventNo++
		<< " events processed" << std::endl;

	return kLITSUCCESS;
}


void CbmLitTrackFinderBranch::FollowTracks()
{
	for (Int_t stationGroup = fBeginStationGroup;
		stationGroup < fEndStationGroup + 1; stationGroup++) {

		for (TrackPtrIterator it = fTracks.begin(); it != fTracks.end(); it++)
			ProcessStationGroup(*it, stationGroup);

		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderBranch:"
				<< fFoundTracks.size() << " followed in the "
				<< stationGroup << " station group" << std::endl;

		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();

		fStationGroupSelection->DoSelect(fFoundTracks.begin(), fFoundTracks.end());
		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderBranch: track selection in station group finished" << std::endl;

		CopyToOutputArray();
		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderBranch: copy to output array finished" << std::endl;

	} //loop over station groups

	for_each(fTracks.begin(), fTracks.end(), DeleteObject());
	fTracks.clear();
}

void CbmLitTrackFinderBranch::ProcessStationGroup(
		const CbmLitTrack *track,
		Int_t stationGroup)
{
   Int_t nofStations = fLayout.GetNofStations(stationGroup);
   std::vector<TrackPtrVector> tracks(nofStations);

   fNofBranches = 0;

   if (ProcessStation(track, stationGroup, 0, tracks[0])){ //0
	   for (TrackPtrIterator trk0 = tracks[0].begin(); trk0 != tracks[0].end(); trk0++) { //1
		   if (!ProcessStation(*trk0, stationGroup, 1, tracks[1])) continue;

	       if (nofStations < 3) {
	    	   AddTrackCandidate(tracks[1], stationGroup);
	    	   continue;
	       }

	       for (TrackPtrIterator trk1 = tracks[1].begin(); trk1 != tracks[1].end(); trk1++) { //2
	    	   if (!ProcessStation(*trk1, stationGroup, 2, tracks[2])) continue;

	    	   if (nofStations < 4) {
	    		  AddTrackCandidate(tracks[2], stationGroup);
	    		  continue;
	    	   }

	    	   for (TrackPtrIterator trk2 = tracks[2].begin(); trk2 != tracks[2].end(); trk2++) { //3
	    		   if (!ProcessStation(*trk2, stationGroup, 3, tracks[3])) continue;
	    		   AddTrackCandidate(tracks[3], stationGroup);
	    	   } //3
	       } //2
	   }//1
   }//0

   for (Int_t i = 0; i < nofStations; i++) {
	   for_each(tracks[i].begin(), tracks[i].end(), DeleteObject());
	   tracks[i].clear();
   }
   tracks.clear();
}

Bool_t CbmLitTrackFinderBranch::ProcessStation(
		const CbmLitTrack *track,
		Int_t stationGroup,
		Int_t station,
		TrackPtrVector& tracksOut)
{
	for_each(tracksOut.begin(), tracksOut.end(), DeleteObject());
	tracksOut.clear();

	Bool_t isBranchCreated = ProcessStation1(track, stationGroup, station, tracksOut);

	Bool_t isMissingHitOk = false;
	if (fIsAlwaysCreateMissingHit || !isBranchCreated) {
		Int_t nofMissingHits = track->GetNofMissingHits() + 1;
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

Bool_t CbmLitTrackFinderBranch::ProcessStation1(
		const CbmLitTrack *track,
		Int_t stationGroup,
		Int_t station,
		TrackPtrVector& tracksOut)
{
	Bool_t result = false;

	Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);

	//TrackBranchPtrVector branches(nofSubstations);

	std::vector<CbmLitTrackParam> par(nofSubstations);
	par[0] = *track->GetParamLast();
	std::vector<CbmLitTrackParam> uPar(nofSubstations);
	HitPtrVector hits(nofSubstations);
	std::vector<HitPtrIteratorPair> bounds(nofSubstations);
	std::vector<Double_t> chiSq(nofSubstations, 0.);
	std::vector<Int_t> extraLoop(nofSubstations, 1);

	Double_t z = fLayout.GetSubstation(stationGroup, station, 0).GetZ();
	if (fPropagator->Propagate(&par[0], z, fPDG) == kLITERROR) return false;
	bounds[0] = MinMaxIndex(&par[0], stationGroup, station, 0);
	for (HitPtrIterator hit0 = bounds[0].first; hit0 < bounds[0].second + extraLoop[0]; hit0++) { //0
		if (!ProcessSubstation(0, hit0, bounds[0], &par[0], &uPar[0], hits, chiSq)) continue;
		else { if (!fIsAlwaysCreateMissingHit) extraLoop[0] = 0; }

		if (nofSubstations < 2) {
			if (AddTrackBranch(track, hits, chiSq, &uPar[0], tracksOut)) result = true;
//			if (fNofBranches > fMaxNofBranches) return result;
			continue;
		}

		Double_t z = fLayout.GetSubstation(stationGroup, station, 1).GetZ();
		if (fPropagator->Propagate(&uPar[0], &par[1], z, fPDG) == kLITERROR) continue;
		bounds[1] = MinMaxIndex(&par[1], stationGroup, station, 1);
		for (HitPtrIterator hit1 = bounds[1].first; hit1 < bounds[1].second + extraLoop[1]; hit1++) { //1
			if (!ProcessSubstation(1, hit1, bounds[1], &par[1], &uPar[1], hits, chiSq)) continue;
			else { if (!fIsAlwaysCreateMissingHit) extraLoop[1] = 0;}
			if (AddTrackBranch(track, hits, chiSq, &uPar[1], tracksOut)) result = true;
//			if (fNofBranches > fMaxNofBranches) return result;
		} //1
	} //0

	return result;
}

Bool_t CbmLitTrackFinderBranch::ProcessSubstation(
		Int_t substation,
		HitPtrIterator hitIt,
		HitPtrIteratorPair bounds,
		const CbmLitTrackParam* par,
		CbmLitTrackParam* uPar,
		HitPtrVector& hits,
		std::vector<Double_t>& chiSq)
{
   Bool_t result = true;
   *uPar = *par;

   if (hitIt < bounds.second) {
      if (!IsIn(par, *hitIt)) result = false;
      if (result) {
    	  fFilter->Update(uPar,  *hitIt);
    	  chiSq[substation] = ChiSq(uPar, *hitIt);
    	  hits[substation] = *hitIt;
      } else {
    	  chiSq[substation] = 0.;
    	  hits[substation] = NULL;
      }
   } else {
      hits[substation] = NULL;
      chiSq[substation] = 0.;
   }

   return result;
}

Bool_t CbmLitTrackFinderBranch::AddTrackBranch(
		const CbmLitTrack* track,
		const HitPtrVector& hits,
		const std::vector<Double_t>& chiSq,
		const CbmLitTrackParam* par,
		TrackPtrVector& tracksOut)
{
   CbmLitTrack* newTrack = new CbmLitTrack(*track);
   Double_t chi2sum = newTrack->GetChi2();

   Bool_t hitAdded = false;
   for (Int_t i = 0; i < hits.size(); i++) {
	  if (hits[i] == NULL) continue;
      newTrack->AddHit(hits[i]);
      chi2sum += chiSq[i];
      hitAdded = true;
   }

   if (!hitAdded) {
	   delete newTrack;
	   return false;
   }

   newTrack->SortHits();
   newTrack->SetPDG(fPDG);
//   newTrack->SetLastPlaneId(stationGroup);
   newTrack->SetParamLast(par);
   newTrack->SetChi2(chi2sum);
//   newTrack->SetNDF(NDF(newTrack->GetNofHits()));

 //  fNofBranches++;

   tracksOut.push_back(newTrack);
   return true;
}

void CbmLitTrackFinderBranch::AddTrackCandidate(
		TrackPtrVector& tracks,
		Int_t stationGroup)
{
	for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
		CbmLitTrack* newTrack = new CbmLitTrack(**it);
		newTrack->SetLastPlaneId(stationGroup);
		newTrack->SetNDF(NDF(newTrack->GetNofHits()));
		fFoundTracks.push_back(newTrack);
	}
}


void CbmLitTrackFinderBranch::RefitTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
   for (TrackPtrIterator it = itBegin; it != itEnd; it++)
	   if ((*it)->GetQuality() == kLITGOOD) fFitter->Fit(*it);
}

void CbmLitTrackFinderBranch::CopyToOutputArray()
{
	for (TrackPtrIterator iTrack = fFoundTracks.begin();
		iTrack != fFoundTracks.end(); iTrack++) {
		if ((*iTrack)->GetQuality() == kLITBAD) {
			delete (*iTrack);
		} else {
			fTracks.push_back(*iTrack);
			fTracksCopy.push_back(new CbmLitTrack(*(*iTrack)));
		}
	}
	fFoundTracks.clear();
}

ClassImp(CbmLitTrackFinderBranch);
