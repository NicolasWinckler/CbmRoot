/** CbmLitTrackFinderNN.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/


#include "finder/CbmLitTrackFinderNN.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitPixelHit.h"
#include "interface/CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"

#include <algorithm>
#include <iostream>
#include <limits>

#include "TGeoManager.h"
#include "TGeoNode.h"

CbmLitTrackFinderNN::CbmLitTrackFinderNN():
   fIsProcessSubstationsTogether(true)
{
}

CbmLitTrackFinderNN::~CbmLitTrackFinderNN()
{
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

   fNofIter = fSettings.GetNofIter();
   for (Int_t iIter = 0; iIter < fNofIter; iIter++) {
      SetIterationParameters(iIter);
      ArrangeHits(hits.begin(), hits.end());
      InitTrackSeeds(trackSeeds.begin(), trackSeeds.end());
      FollowTracks(fTracks.begin(), fTracks.end());
      fFinalSelection->DoSelect(fTracks.begin(), fTracks.end());
      RemoveHits(fTracks.begin(), fTracks.end());
      CopyToOutput(fTracks.begin(), fTracks.end(), tracks);

//    for (Int_t i = 0; i < tracks.size(); i++) {
//       std::cout << "TRACK: " << tracks[i]->ToString();
//       for (Int_t j = 0; j < tracks[i]->GetNofHits(); j++){
//          std::cout << " " << tracks[i]->GetHit(j)->GetRefId();
//       }
//       std::cout << std::endl;
//    }

      for_each(fTracks.begin(), fTracks.end(), DeleteObject());
      fTracks.clear();
      fHitData.Clear();
   }
   static Int_t eventNo = 0;
   std::cout << "CbmLitTrackFinderNN::DoFind: " << eventNo++ << " events processed" << std::endl;
   return kLITSUCCESS;
}

void CbmLitTrackFinderNN::SetIterationParameters(
   Int_t iter)
{
   SetPropagator(fSettings.GetPropagator(iter));
   SetSeedSelection(fSettings.GetSeedSelection(iter));
   SetFinalSelection(fSettings.GetFinalSelection(iter));
   SetFilter(fSettings.GetFilter(iter));
   IsUseFastSearch(fSettings.GetIsUseFastSearch(iter));
   SetPDG(fSettings.GetPDG(iter));
   IsProcessSubstationsTogether(fSettings.GetIsProcessSubstationsTogether(iter));
   SetMaxNofMissingHits(fSettings.GetMaxNofMissingHits(iter));
   SetSigmaCoef(fSettings.GetSigmaCoef(iter));
   SetChiSqPixelHitCut(fSettings.GetChiSqPixelHitCut(iter));
   SetChiSqStripHitCut(fSettings.GetChiSqStripHitCut(iter));
}

void CbmLitTrackFinderNN::InitTrackSeeds(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
      (*it)->SetQuality(kLITGOOD);
   }

   fSeedSelection->DoSelect(itBegin, itEnd);

   for (TrackPtrIterator track = itBegin; track != itEnd; track++) {
      if ((*track)->GetQuality() == kLITBAD) { continue; }
      if (fUsedSeedsSet.find((*track)->GetPreviousTrackId()) != fUsedSeedsSet.end()) { continue; }
      (*track)->SetPDG(fPDG);
      (*track)->SetChi2(0.);
      fTracks.push_back(new CbmLitTrack(*(*track)));
   }

   if (fZPropagationForTrackSeeds > 0.) {
		for (TrackPtrIterator track = fTracks.begin(); track != fTracks.end(); track++) {
			CbmLitTrackParam par = *((*track)->GetParamLast());
			fPropagator->Propagate(&par, fZPropagationForTrackSeeds, fPDG);
			(*track)->SetParamLast(&par);
		}
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
   CbmLitTrack* track)
{
   Int_t nofStationGroups = fLayout.GetNofStationGroups();
   for(Int_t iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
      if (!ProcessStationGroup(track, iStationGroup)) { return; }
   }
}

bool CbmLitTrackFinderNN::ProcessStationGroup(
   CbmLitTrack* track,
   Int_t stationGroup)
{
   Int_t nofStations = fLayout.GetNofStations(stationGroup);
   for(Int_t iStation = 0; iStation < nofStations; iStation++) {
      if (!ProcessStation(track, stationGroup, iStation)) {
         track->SetNofMissingHits(track->GetNofMissingHits() + 1);
         if (track->GetNofMissingHits() > fMaxNofMissingHits) { return false; }
      }
   }
   track->SetPDG(fPDG);
   track->SetLastPlaneId(stationGroup);
   return true;
}

bool CbmLitTrackFinderNN::ProcessStation(
   CbmLitTrack* track,
   Int_t stationGroup,
   Int_t station)
{
   bool hitAdded = false;
   Int_t nofSubstations = fLayout.GetNofSubstations(stationGroup, station);
   std::vector<CbmLitTrackParam> par(nofSubstations);
   par[0] = *track->GetParamLast();
   std::vector<HitPtrIteratorPair> hits(nofSubstations);
   for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
	  litfloat z = 0.;
	  Int_t moduleRotation = 0;
	  if (!fUseTGeo) {
		  z = fLayout.GetSubstation(stationGroup, station, iSubstation).GetZ();
	  } else {
		  Int_t moduleId = 0;
		  LitStatus result = fLayout.FindBoundary(&par[iSubstation], stationGroup, station, iSubstation, true, z, moduleId);
		  if (result == kLITERROR) return false;
		  moduleRotation = fLayout.GetSubstation(stationGroup, station, iSubstation).GetModuleRotationId(moduleId);
	  }
      LitStatus propResult = fPropagator->Propagate(&par[iSubstation], z, fPDG);
      if (propResult == kLITERROR) return false;

      if (iSubstation < nofSubstations - 1) { par[iSubstation + 1] = par[iSubstation]; }
      HitPtrIteratorPair bounds = MinMaxIndex(&par[iSubstation], fHitData.GetHits(stationGroup, station, iSubstation, moduleRotation),
                                              fLayout.GetStation(stationGroup, station), fHitData.GetMaxErr(stationGroup, station, iSubstation, moduleRotation));
      hits[iSubstation] = bounds;
   }

   if (fIsProcessSubstationsTogether) { hitAdded = AddNearestHit1(track, hits, par, nofSubstations); }
   else { hitAdded = AddNearestHit2(track, hits, par, nofSubstations); }

   return hitAdded;
}

bool CbmLitTrackFinderNN::AddNearestHit1(
   CbmLitTrack* track,
   std::vector<HitPtrIteratorPair>& hits,
   const std::vector<CbmLitTrackParam>& par,
   Int_t nofSubstations)
{
   // fIsProcessSubstationsTogether == true
   bool hitAdded = false;
   CbmLitTrackParam uPar, param;
   HitPtrIterator hit(hits[0].second);
   litfloat chiSq = std::numeric_limits<litfloat>::max();
   for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
      for (HitPtrIterator iHit = hits[iSubstation].first; iHit != hits[iSubstation].second; iHit++) {
         //First update track parameters with KF, than check whether the hit is in the validation gate.
         litfloat chi = 0.;
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
   std::vector<HitPtrIteratorPair>& hits,
   const std::vector<CbmLitTrackParam>& par,
   Int_t nofSubstations)
{
   // fIsProcessSubstationsTogether == false
   bool hitAdded = false;
   for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
      CbmLitTrackParam uPar, param;
      HitPtrIterator hit(hits[iSubstation].second);
      litfloat chiSq = std::numeric_limits<litfloat>::max();
      for (HitPtrIterator iHit = hits[iSubstation].first; iHit != hits[iSubstation].second; iHit++) {
         //First update track parameters with KF, than check whether the hit is in the validation gate.
         litfloat chi = 0.;
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
