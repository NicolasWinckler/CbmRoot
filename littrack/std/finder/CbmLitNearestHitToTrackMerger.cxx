/** CbmLitNearestHitToTrackMerger.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "finder/CbmLitNearestHitToTrackMerger.h"

#include "base/CbmLitStation.h"
#include "base/CbmLitSubstation.h"
#include "base/CbmLitDetectorLayout.h"
#include "base/CbmLitTypes.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMath.h"

#include <iostream>

CbmLitNearestHitToTrackMerger::CbmLitNearestHitToTrackMerger()
{
}

CbmLitNearestHitToTrackMerger::~CbmLitNearestHitToTrackMerger()
{
}

LitStatus CbmLitNearestHitToTrackMerger::DoMerge(
   HitPtrVector& hits,
   TrackPtrVector& tracks)
{
   //loop over tracks
   for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
      CbmLitTrack* track = *it;
      if (track->GetQuality() != kLITGOODMERGE) { continue; }
      CbmLitTrackParam par(*track->GetParamLast());
      bool hitAdded = false;
      for (int substation = 0; substation < fStation.GetNofSubstations(); substation++) {
         //propagate each track to plane where to merge
         fPropagator->Propagate(&par, fStation.GetSubstation(substation).GetZ(), fPDG);
         //fast search of hits (optional)
         //HitPtrIteratorPair bounds = MinMaxIndex(&par, hitData, layout, stationGroup, station, substation);
         HitPtrIteratorPair bounds = HitPtrIteratorPair(hits.begin(), hits.end());
         //TODO: set parameters only if hit was added
         track->SetParamLast(&par);
         hitAdded = AddNearestHit(track, bounds);
         //update track parameters
         //calculate chi square
         //attach nearest hit to track
      }
   }
   return kLITSUCCESS;
}

bool CbmLitNearestHitToTrackMerger::AddNearestHit(
   CbmLitTrack* track,
   HitPtrIteratorPair bounds) const
{
   bool hitAdded = false;
   const CbmLitTrackParam* par = track->GetParamLast();
   CbmLitTrackParam uPar, param;
   HitPtrIterator hit(bounds.second);
   myf chiSq = 1e10;
   for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
      myf chi = 0.;
      fFilter->Update(par, &uPar, *iHit, chi);
      if (IsHitInValidationGate(*iHit, chi, par)) {
         if (chi < chiSq) {
            chiSq = chi;
            hit = iHit;
            param = uPar;
         }
      }
   }
   if (hit != bounds.second) {
      track->AddHit(*hit);
      track->SetParamLast(&param);
      track->SetChi2(track->GetChi2() + chiSq);
      track->SetNDF(lit::NDF(track));
      hitAdded = true;
   }
   return hitAdded;
}
