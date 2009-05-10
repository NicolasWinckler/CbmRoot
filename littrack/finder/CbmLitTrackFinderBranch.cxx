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

#include <iostream>
#include <algorithm>

//TODO: track branches manager. Limit the number of branches in the
// validation gate. Create only a certain number of the best branches,
// selecting them by the chi-square. Similar to the NN, but several
// branches can be created.
//TODO: propagate track to the first detector station internally
// here in the CbmLitTrackFinderBase class.

CbmLitTrackFinderBranch::CbmLitTrackFinderBranch()
{
//	fMaxNofBranches = 32;
	fMaxNofHitsInValidationGate = 4;
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
	std::cout << "-I- CbmLitTrackFinderBranch: " << fEventNo++	<< " events processed" << std::endl;
	return kLITSUCCESS;
}

void CbmLitTrackFinderBranch::FollowTracks()
{
	// Loop over the station groups
	for (Int_t stationGroup = fBeginStationGroup;
		stationGroup < fEndStationGroup + 1; stationGroup++) {

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
		const CbmLitTrack *track,
		Int_t stationGroup)
{
   Int_t nofStations = fLayout.GetNofStations(stationGroup);
   std::vector<TrackPtrVector> tracks(nofStations);
//   fNofBranches = 0;

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
	CbmLitTrackParam par(*track->GetParamLast());
	HitPtrIteratorPair bounds;
	std::vector<CbmLitHitChiSq> hits;
	Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
	for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		Double_t z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		if (fPropagator->Propagate(&par, z, fPDG) == kLITERROR) return false;
		bounds = MinMaxIndex(&par, stationGroup, station, iSubstation);
		ProcessSubstation(&par, bounds, hits);
	}

	if (hits.empty()) return false;
	Int_t nofHits = hits.size();
	// if too many hits in the validation gate,
	// sort hits in the validation gate by the chi-square
	if (nofHits > fMaxNofHitsInValidationGate){
		std::sort(hits.begin(), hits.end(), CompareHitChiSqLess());
	} else {

	}

	//Select the best hits to be attached

	//Create track branches
	Int_t N = (nofHits > fMaxNofHitsInValidationGate) ? fMaxNofHitsInValidationGate : nofHits;
	for (Int_t iHit = 0; iHit < N; iHit++) {
		CbmLitTrack* newTrack = new CbmLitTrack(*track);
		newTrack->AddHit(hits[iHit].GetHit());
		newTrack->SetChi2(newTrack->GetChi2() + hits[iHit].GetChiSq());
//		newTrack->SortHits();
		newTrack->SetPDG(fPDG);
//		newTrack->SetLastPlaneId(stationGroup);
		newTrack->SetParamLast(hits[iHit].GetParam());
		tracksOut.push_back(newTrack);
		result = true;
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
		fFilter->Update(par, &uPar, *iHit);
		if (IsHitInValidationWindow(&uPar, *iHit)) {
			Double_t chi = ChiSq(&uPar, *iHit);
			CbmLitHitChiSq h;
			h.SetHit(*iHit);
			h.SetParam(&uPar);
			h.SetChiSq(chi);
			hits.push_back(h);
		}
	}
}

Bool_t CbmLitTrackFinderBranch::AddTrackCandidate(
		TrackPtrVector& tracks,
		Int_t stationGroup)
{
	for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
//		if (fNofBranches++ > fMaxNofBranches) return false;
		CbmLitTrack* newTrack = new CbmLitTrack(**it);
		newTrack->SetLastPlaneId(stationGroup);
		newTrack->SetNDF(NDF(newTrack));
		fFoundTracks.push_back(newTrack);
	}
	return true;
}

//void CbmLitTrackFinderBranch::RefitTracks(
//		TrackPtrIterator itBegin,
//		TrackPtrIterator itEnd)
//{
//   for (TrackPtrIterator it = itBegin; it != itEnd; it++)
//	   if ((*it)->GetQuality() == kLITGOOD) fFitter->Fit(*it);
//}

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



//Bool_t CbmLitTrackFinderBranch::ProcessStation1(
//		const CbmLitTrack *track,
//		Int_t stationGroup,
//		Int_t station,
//		TrackPtrVector& tracksOut)
//{
//	Bool_t result = false;
//	Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
//	std::vector<CbmLitTrackParam> par(nofSubstations);
//	par[0] = *track->GetParamLast();
//	std::vector<CbmLitTrackParam> uPar(nofSubstations);
//	HitPtrVector hits(nofSubstations);
//	std::vector<HitPtrIteratorPair> bounds(nofSubstations);
//	std::vector<Double_t> chiSq(nofSubstations, 0.);
//	std::vector<Int_t> extraLoop(nofSubstations, 1);
//	if (nofSubstations < 2) std::fill(extraLoop.begin(), extraLoop.end(), 0);
//
//	Double_t z = fLayout.GetSubstation(stationGroup, station, 0).GetZ();
//	if (fPropagator->Propagate(&par[0], z, fPDG) == kLITERROR) return false;
//	bounds[0] = MinMaxIndex(&par[0], stationGroup, station, 0);
//	for (HitPtrIterator hit0 = bounds[0].first; hit0 < bounds[0].second + extraLoop[0]; hit0++) { //0
//		if (!ProcessSubstation(0, hit0, bounds[0], &par[0], &uPar[0], hits, chiSq)) continue;
////		else { if (!fIsAlwaysCreateMissingHit) extraLoop[0] = 0; }
//
//		if (nofSubstations < 2) {
//			if (AddTrackBranch(track, hits, chiSq, &uPar[0], tracksOut)) result = true;
//			continue;
//		}
//
//		Double_t z = fLayout.GetSubstation(stationGroup, station, 1).GetZ();
//		if (fPropagator->Propagate(&uPar[0], &par[1], z, fPDG) == kLITERROR) continue;
//		bounds[1] = MinMaxIndex(&par[1], stationGroup, station, 1);
//		for (HitPtrIterator hit1 = bounds[1].first; hit1 < bounds[1].second + extraLoop[1]; hit1++) { //1
//			if (!ProcessSubstation(1, hit1, bounds[1], &par[1], &uPar[1], hits, chiSq)) continue;
////			else { if (!fIsAlwaysCreateMissingHit) extraLoop[1] = 0;}
//			if (AddTrackBranch(track, hits, chiSq, &uPar[1], tracksOut)) result = true;
//		} //1
//	} //0
//
//	return result;
//}
//Bool_t CbmLitTrackFinderBranch::ProcessSubstation(
//		Int_t substation,
//		HitPtrIterator hitIt,
//		HitPtrIteratorPair bounds,
//		const CbmLitTrackParam* par,
//		CbmLitTrackParam* uPar,
//		HitPtrVector& hits,
//		std::vector<Double_t>& chiSq)
//{
//   Bool_t result = true;
//   *uPar = *par;
//
//   if (hitIt < bounds.second) {
//	  fFilter->Update(uPar,  *hitIt);
//      if (!IsHitInValidationWindow(uPar, *hitIt)) result = false;
//      if (result) {
////    	  fFilter->Update(uPar,  *hitIt);
//    	  chiSq[substation] = ChiSq(uPar, *hitIt);
//    	  hits[substation] = *hitIt;
//      } else {
//    	  chiSq[substation] = 0.;
//    	  hits[substation] = NULL;
//      }
//   } else {
//      hits[substation] = NULL;
//      chiSq[substation] = 0.;
//   }
//
//   return result;
//}

//Bool_t CbmLitTrackFinderBranch::AddTrackBranch(
//		const CbmLitTrack* track,
//		const HitPtrVector& hits,
//		const std::vector<Double_t>& chiSq,
//		const CbmLitTrackParam* par,
//		TrackPtrVector& tracksOut)
//{
//   CbmLitTrack* newTrack = new CbmLitTrack(*track);
//   Double_t chi2sum = newTrack->GetChi2();
//
//   Bool_t hitAdded = false;
//   for (Int_t i = 0; i < hits.size(); i++) {
//	  if (hits[i] == NULL) continue;
//      newTrack->AddHit(hits[i]);
//      chi2sum += chiSq[i];
//      hitAdded = true;
//   }
//
//   if (!hitAdded) {
//	   delete newTrack;
//	   return false;
//   }
//
//   newTrack->SortHits();
//   newTrack->SetPDG(fPDG);
////   newTrack->SetLastPlaneId(stationGroup);
//   newTrack->SetParamLast(par);
//   newTrack->SetChi2(chi2sum);
////   newTrack->SetNDF(NDF(newTrack->GetNofHits()));
//
//   tracksOut.push_back(newTrack);
//   return true;
//}

ClassImp(CbmLitTrackFinderBranch);
