/** CbmLitTrackFinderBranch.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 **/

#include "finder/CbmLitTrackFinderBranch.h"

#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitHitChiSq.h"
#include "interface/CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "interface/CbmLitTrackFitter.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderBranch::CbmLitTrackFinderBranch():
   fMaxNofHitsInValidationGate(3),
   fMaxNofBranchesStationGroup(27),
   fIsProcessSubstationsTogether(true),
   fMaxNofBranches(2048),
   fNofBranches()
{
}

CbmLitTrackFinderBranch::~CbmLitTrackFinderBranch()
{
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

   fNofIter = fSettings.GetNofIter();
   for (Int_t iIter = 0; iIter < fNofIter; iIter++) {
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
      fNofBranches.clear();
   }
   std::cout << "-I- CbmLitTrackFinderBranch: " << fEventNo++  << " events processed" << std::endl;
   return kLITSUCCESS;
}

void CbmLitTrackFinderBranch::SetIterationParameters(
   Int_t iter)
{
   SetPropagator(fSettings.GetPropagator(iter));
   SetSeedSelection(fSettings.GetSeedSelection(iter));
   SetStationGroupSelection(fSettings.GetStationGroupSelection(iter));
   SetFinalSelection(fSettings.GetFinalSelection(iter));
   SetFilter(fSettings.GetFilter(iter));
   IsUseFastSearch(fSettings.GetIsUseFastSearch(iter));
   SetPDG(fSettings.GetPDG(iter));
   IsProcessSubstationsTogether(fSettings.GetIsProcessSubstationsTogether(iter));
   SetMaxNofMissingHits(fSettings.GetMaxNofMissingHits(iter));
   IsAlwaysCreateMissingHit(fSettings.GetIsAlwaysCreateMissingHit(iter));
   SetSigmaCoef(fSettings.GetSigmaCoef(iter));
   SetChiSqPixelHitCut(fSettings.GetChiSqPixelHitCut(iter));
   SetChiSqStripHitCut(fSettings.GetChiSqStripHitCut(iter));
}

void CbmLitTrackFinderBranch::InitTrackSeeds(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
      (*it)->SetQuality(kLITGOOD);
   }

   fSeedSelection->DoSelect(itBegin, itEnd);

   for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitTrack* track = *it;
      if (track->GetQuality() == kLITBAD) { continue; }
      if (fUsedSeedsSet.find(track->GetPreviousTrackId()) != fUsedSeedsSet.end()) { continue; }
      CbmLitTrack* newTrack = new CbmLitTrack(*track);
      newTrack->SetPDG(fPDG);
      newTrack->SetChi2(0.);
      fTracks.push_back(newTrack);
   }
}

void CbmLitTrackFinderBranch::FollowTracks()
{
   // Loop over the station groups
   for (Int_t stationGroup = 0; stationGroup < fLayout.GetNofStationGroups(); stationGroup++) {

      // Loops over fTracks, processes the station group and
      // writes output tracks to fFoundTracks array
      for (TrackPtrIterator it = fTracks.begin(); it != fTracks.end(); it++) {
         ProcessStationGroup(*it, stationGroup);
      }

      for_each(fTracks.begin(), fTracks.end(), DeleteObject());
      fTracks.clear();

//      std::cout << "-I- CbmLitTrackFinderBranch:"   << fFoundTracks.size() << " followed in the " << stationGroup << " station group" << std::endl;

      fStationGroupSelection->DoSelect(fFoundTracks.begin(), fFoundTracks.end());
//         std::cout << "-I- CbmLitTrackFinderBranch: track selection in station group finished" << std::endl;

      // Copy fFoundTracks to fTracksCopy array to store also short tracks
      CopyToOutputArray();
//         std::cout << "-I- CbmLitTrackFinderBranch: copy to output array finished" << std::endl;
   } //loop over station groups

   for_each(fTracks.begin(), fTracks.end(), DeleteObject());
   fTracks.clear();
}

void CbmLitTrackFinderBranch::ProcessStationGroup(
   const CbmLitTrack* track,
   Int_t stationGroup)
{
   // Check if number of branches for this seed is acceptable
   if (fNofBranches[track->GetPreviousTrackId()] > fMaxNofBranches) { return; }
   // TODO: If number of branches is too big than start nearest
   // neighbor tracking for that seed. Not just return.

   Int_t nofStations = fLayout.GetNofStations(stationGroup);
   std::vector<TrackPtrVector> tracks(nofStations);
   fNofBranchesStationGroup = 0;

   if (ProcessStation(track, stationGroup, 0, tracks[0])) { //0
      for (TrackPtrIterator trk0 = tracks[0].begin(); trk0 != tracks[0].end(); trk0++) { //1
         if (!ProcessStation(*trk0, stationGroup, 1, tracks[1])) { continue; }

         if (nofStations < 3) {
            if (!AddTrackCandidate(tracks[1], stationGroup)) { return; }
            continue;
         }

         for (TrackPtrIterator trk1 = tracks[1].begin(); trk1 != tracks[1].end(); trk1++) { //2
            if (!ProcessStation(*trk1, stationGroup, 2, tracks[2])) { continue; }

            if (nofStations < 4) {
               if (!AddTrackCandidate(tracks[2], stationGroup)) { return; }
               continue;
            }

            for (TrackPtrIterator trk2 = tracks[2].begin(); trk2 != tracks[2].end(); trk2++) { //3
               if (!ProcessStation(*trk2, stationGroup, 3, tracks[3])) { continue; }
               if (!AddTrackCandidate(tracks[3], stationGroup)) { return; }
            } //3
         } //2
      }//1
   }//0

   for (Int_t i = 0; i < nofStations; i++) {
      for_each(tracks[i].begin(), tracks[i].end(), DeleteObject());
      tracks[i].clear();
   }
   tracks.clear();

   fNofBranches[track->GetPreviousTrackId()] += fNofBranchesStationGroup;
}

bool CbmLitTrackFinderBranch::ProcessStation(
   const CbmLitTrack* track,
   Int_t stationGroup,
   Int_t station,
   TrackPtrVector& tracksOut)
{
   for_each(tracksOut.begin(), tracksOut.end(), DeleteObject());
   tracksOut.clear();

   bool isBranchCreated;
   if (fIsProcessSubstationsTogether) {
      isBranchCreated = ProcessStation1(track, stationGroup, station, tracksOut);
   } else {
      isBranchCreated = ProcessStation2(track, stationGroup, station, tracksOut);
   }

   bool isMissingHitOk = false;
   if (fIsAlwaysCreateMissingHit || !isBranchCreated) {
      Int_t nofMissingHits = track->GetNofMissingHits() + 1;
      if (nofMissingHits <= fMaxNofMissingHits) {
         CbmLitTrack* trk = new CbmLitTrack(*track);
         trk->SetNofMissingHits(nofMissingHits);
         tracksOut.push_back(trk);
         isMissingHitOk = true;
      }
   }

   if (isBranchCreated) { return true; }
   else { return isMissingHitOk; }
}

bool CbmLitTrackFinderBranch::ProcessStation1(
   const CbmLitTrack* track,
   Int_t stationGroup,
   Int_t station,
   TrackPtrVector& tracksOut)
{
   // if fIsProcessSubstationsTogether == true
   bool result = false;
   CbmLitTrackParam par(*track->GetParamLast());
   HitPtrIteratorPair bounds;
   std::vector<CbmLitHitChiSq> hits;
   Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
   for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
	   // new TGeo
	  litfloat z = 0.;
	  Int_t moduleRotation = 0;
	  if (!fUseTGeo) {
		  z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
	  } else {
		  Int_t moduleId = 0;
		  LitStatus result = fLayout.FindBoundary(&par, stationGroup, station, iSubstation, true, z, moduleId);
		  if (result == kLITERROR) return false;
		  moduleRotation = fLayout.GetSubstation(stationGroup, station, iSubstation).GetModuleRotationId(moduleId);
	  }
      LitStatus propResult = fPropagator->Propagate(&par, z, fPDG);
      if (propResult == kLITERROR) return false;
      bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation, moduleRotation),
                           fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation, moduleRotation));
      ProcessSubstation(&par, bounds, hits);
      // end TGeo

//      litfloat z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
//      if (fPropagator->Propagate(&par, z, fPDG) == kLITERROR) { return false; }
//      bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation, 0),
//                           fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation, 0));
//      ProcessSubstation(&par, bounds, hits);
   }

   if (hits.empty()) { return false; }
   Int_t nofHits = hits.size();
   // If too many hits in the validation gate than
   // sort hits in the validation gate by chi-square
   if (nofHits > fMaxNofHitsInValidationGate) {
      std::sort(hits.begin(), hits.end(), CompareHitChiSqLess());
   }

   // Select the best hits (lowest chi-square) to be attached to track.
   // Create track branches for each of such hits.
   Int_t N = (nofHits > fMaxNofHitsInValidationGate) ? fMaxNofHitsInValidationGate : nofHits;
   for (Int_t iHit = 0; iHit < N; iHit++) {
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
   const CbmLitTrack* track,
   Int_t stationGroup,
   Int_t station,
   TrackPtrVector& tracksOut)
{
   // if fIsProcessSubstationsTogether == false
   bool result = false;
   CbmLitTrackParam par(*track->GetParamLast());
   Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
   for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
      HitPtrIteratorPair bounds;
      std::vector<CbmLitHitChiSq> hits;

		// new TGeo
		litfloat z = 0.;
		Int_t moduleRotation = 0;
		if (!fUseTGeo) {
		  z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
		} else {
		  Int_t moduleId = 0;
		  LitStatus result = fLayout.FindBoundary(&par, stationGroup, station, iSubstation, true, z, moduleId);
		  if (result == kLITERROR) return false;
		  moduleRotation = fLayout.GetSubstation(stationGroup, station, iSubstation).GetModuleRotationId(moduleId);
		}
		LitStatus propResult = fPropagator->Propagate(&par, z, fPDG);
		if (propResult == kLITERROR) return false;
		bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation, moduleRotation),
						  fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation, moduleRotation));
		ProcessSubstation(&par, bounds, hits);
		// end TGeo

//      litfloat z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
//      if (fPropagator->Propagate(&par, z, fPDG) == kLITERROR) { return false; }
//      bounds = MinMaxIndex(&par, fHitData.GetHits(stationGroup, station, iSubstation, 0),
//                           fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation, 0));
//      ProcessSubstation(&par, bounds, hits);

      if (hits.empty()) { continue; } //return false;
      Int_t nofHits = hits.size();
      // If too many hits in the validation gate than
      // sort hits in the validation gate by chi-square
      if (nofHits > fMaxNofHitsInValidationGate) {
         std::sort(hits.begin(), hits.end(), CompareHitChiSqLess());
      }

      // Select the best hits (lowest chi-square) to be attached to track.
      // Create track branches for each of such hits.
      Int_t N = (nofHits > fMaxNofHitsInValidationGate) ? fMaxNofHitsInValidationGate : nofHits;
      for (Int_t iHit = 0; iHit < N; iHit++) {
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
   // Calculate validation gate for input range of hits.
   // Store hits which are in validation gate.

   CbmLitTrackParam uPar;
   for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
      CbmLitHit* hit = *iHit;
      litfloat chi = 0.;
      fFilter->Update(par, &uPar, hit, chi);
      if (IsHitInValidationGate(hit, chi, par)) {
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
   Int_t stationGroup)
{
   // Add track candidates to the global track candidate array.
   // If number of created branches is too big than do not add tracks.

   for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
      fNofBranchesStationGroup++;
      if (fNofBranchesStationGroup > fMaxNofBranchesStationGroup) { return false; }
      CbmLitTrack* newTrack = new CbmLitTrack(**it);
      newTrack->SetLastPlaneId(stationGroup);
      newTrack->SetNDF(lit::NDF(newTrack));
      fFoundTracks.push_back(newTrack);
   }
   return true;
}

void CbmLitTrackFinderBranch::CopyToOutputArray()
{
   // Copy tracks to local output array for later final track selection

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
