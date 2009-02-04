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


//void CbmLitTrackFinderBranch::ProcessStationGroup(
//		const CbmLitTrack *track,
//		Int_t stationGroup)
//{
//	Int_t nofStations = fLayout.GetNofStations(stationGroup);
//
//	std::vector<CbmLitTrackParam> par(nofStations);
//	par[0] = *track->GetParamLast();
//	std::vector<CbmLitTrackParam> uPar(nofStations);
//    HitPtrVector hits(nofStations);
//    std::vector<HitPtrIteratorPair> bounds(nofStations);
//    std::vector<Int_t> nofMissingHits(nofStations, 0);
//    std::vector<Double_t> chiSq(nofStations, 0.);
//    Int_t nofBranches = 0;
//
//   Int_t plane = 0;
//   for (Int_t i = 0; i < stationGroup; i++)
//	   plane += fLayout.GetNofStations(i);
//
//   Int_t extraLoop = 0;
//   if (fMaxNofMissingHitsInStationGroup > 0) extraLoop = 1;
//   if (std::abs(plane - track->GetNofHits()) >= fMaxNofMissingHits) extraLoop = 0;
//
//   Double_t z = fLayout.GetSubstation(stationGroup, 0, 0).GetZ();
//   if (fPropagator->Propagate(&par[0], z, fPDG) == kLITERROR) return;
//   bounds[0] = MinMaxIndex(&par[0], stationGroup, 0, 0);
//   for (HitPtrIterator iHit0 = bounds[0].first; iHit0 != bounds[0].second + extraLoop; iHit0++) { //1
//      if (!ProcessStation(0, 0, iHit0, bounds[0], nofMissingHits, &par[0], &uPar[0], hits, chiSq)) continue;
//
//      Double_t z = fLayout.GetSubstation(stationGroup, 1, 0).GetZ();
//      if (fPropagator->Propagate(&uPar[0], &par[1], z, fPDG) == kLITERROR) continue;
//      bounds[1] = MinMaxIndex(&par[1], stationGroup, 1, 0);
//      for (HitPtrIterator iHit1 = bounds[1].first; iHit1 != bounds[1].second + extraLoop; iHit1++) { //2
//    	 if (!ProcessStation(1, 0, iHit1, bounds[1], nofMissingHits, &par[1], &uPar[1], hits, chiSq)) continue;
//
//         if (nofStations < 3) {
//        	 if (nofBranches++ > fMaxNofBranches) return;
//        	 AddTrackCandidate(track, hits, chiSq, &uPar[1], stationGroup);
//             continue;
//         }
//
//         Double_t z = fLayout.GetSubstation(stationGroup, 2, 0).GetZ();
//         if (fPropagator->Propagate(&uPar[1], &par[2], z, fPDG) == kLITERROR) continue;
//         bounds[2] = MinMaxIndex(&par[2], stationGroup, 2, 0);
//         for (HitPtrIterator iHit2 = bounds[2].first; iHit2 != bounds[2].second + extraLoop; iHit2++) { //3
//            if (!ProcessStation(2, 0, iHit2, bounds[2], nofMissingHits, &par[2], &uPar[2], hits, chiSq)) continue;
//
//            if (nofStations < 4) {
//            	if (nofBranches++ > fMaxNofBranches) return;
//               AddTrackCandidate(track, hits, chiSq, &uPar[2], stationGroup);
//               continue;
//            }
//
//            Double_t z = fLayout.GetSubstation(stationGroup, 3, 0).GetZ();
//            if (fPropagator->Propagate(&uPar[2], &par[3], z, fPDG) == kLITERROR) continue;
//            bounds[3] = MinMaxIndex(&par[3], stationGroup, 3, 0);
//            for (HitPtrIterator iHit3 = bounds[3].first; iHit3 != bounds[3].second + extraLoop; iHit3++) { //4
//               if (!ProcessStation(3, 0, iHit3, bounds[3], nofMissingHits, &par[3],&uPar[3], hits, chiSq)) continue;
//
//               if (nofBranches++ > fMaxNofBranches) return;
//               AddTrackCandidate(track, hits, chiSq, &uPar[3], stationGroup);
//
//            } //4
//         } //3
//      } //2
//   } //1
//}
//
//Bool_t CbmLitTrackFinderBranch::ProcessStation(
//		Int_t station,
//		Int_t substation,
//		HitPtrIterator &hitIt,
//		HitPtrIteratorPair &bounds,
// 		std::vector<Int_t>& nofMissingHits,
//		CbmLitTrackParam* par,
//		CbmLitTrackParam* uPar,
//		HitPtrVector& hits,
//		std::vector<Double_t>& chiSq)
//{
//   Bool_t result = true;
//
//   if (station == 0) nofMissingHits[0] = 0;
//   else nofMissingHits[station] = nofMissingHits[station - 1];
//
//   if (hitIt < bounds.second) {
//      hits[station] = *hitIt;
//      if (!IsIn(par,  hits[station])) result = false;
//      *uPar = *par;
//      if (result) {
//    	  fFilter->Update(uPar,  hits[station]);
//    	  chiSq[station] = ChiSq(uPar, hits[station]);
//      } else {
//    	  chiSq[station] = 0.;
//      }
//   } else {
//      hits[station] = NULL;
//      nofMissingHits[station]++;
//      if (nofMissingHits[station] > fMaxNofMissingHitsInStationGroup) result = false;
//      chiSq[station] = 0.;
//   }
//
//   return result;
//}
//
//void CbmLitTrackFinderBranch::AddTrackCandidate(
//		const CbmLitTrack* track,
//		const HitPtrVector& hits,
//		const std::vector<Double_t>& chiSq,
//		const CbmLitTrackParam* par,
//		Int_t stationGroup)
//{
//   CbmLitTrack* newTrack = new CbmLitTrack(*track);
//   Double_t chi2sum = newTrack->GetChi2();
//
//   for (Int_t i = 0; i < hits.size(); i++) {
//	  if (hits[i] == NULL) continue;
//      newTrack->AddHit(hits[i]);
//      chi2sum += chiSq[i];
//   }
//
//   newTrack->SortHits();
//   newTrack->SetPDG(fPDG);
//   newTrack->SetLastPlaneId(stationGroup);
//   newTrack->SetParamLast(par);
//   newTrack->SetChi2(chi2sum);
//   newTrack->SetNDF(NDF(newTrack->GetNofHits()));
//
//   if (newTrack->GetNofHits() == 0) {
//      delete newTrack;
//      return;
//   }
//   fFoundTracks.push_back(newTrack);
//}


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
//	Int_t nofBranches = 0;

	Int_t extraLoop = 1;

	Double_t z = fLayout.GetSubstation(stationGroup, station, 0).GetZ();
	if (fPropagator->Propagate(&par[0], z, fPDG) == kLITERROR) return false;
	bounds[0] = MinMaxIndex(&par[0], stationGroup, station, 0);
	for (HitPtrIterator hit0 = bounds[0].first; hit0 != bounds[0].second + extraLoop; hit0++) { //0
		if (!ProcessSubstation(0, hit0, bounds[0], &par[0], &uPar[0], hits, chiSq)) continue;

		if (nofSubstations < 2) {
			if (AddTrackBranch(track, hits, chiSq, &uPar[0], tracksOut)) result = true;
		//	if (fNofBranches > fMaxNofBranches) return result;
			continue;
		}

		Double_t z = fLayout.GetSubstation(stationGroup, station, 1).GetZ();
		if (fPropagator->Propagate(&uPar[0], &par[1], z, fPDG) == kLITERROR) continue;
		bounds[1] = MinMaxIndex(&par[1], stationGroup, station, 1);
		for (HitPtrIterator hit1 = bounds[1].first; hit1 != bounds[1].second + extraLoop; hit1++) { //1
			if (!ProcessSubstation(1, hit1, bounds[1], &par[1], &uPar[1], hits, chiSq)) continue;
			if (AddTrackBranch(track, hits, chiSq, &uPar[1], tracksOut)) result = true;
		//	if (fNofBranches > fMaxNofBranches) return result;
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
