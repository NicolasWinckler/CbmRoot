/** CbmLitTrackFinderBase.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 **/

#include "finder/CbmLitTrackFinderBase.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"
#include "interface//CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitMath.h"

#include <iostream>
#include <cmath>
#include <algorithm>

CbmLitTrackFinderBase::CbmLitTrackFinderBase():
   fLayout(),
   fHitData(),
   fUsedHitsSet(),
   fUsedSeedsSet(),
   fNofIter(0),
   fMaxNofMissingHits(0),
   fPDG(),
   fZPropagationForTrackSeeds(-1.),
   fUseTGeo(false)
{
}

CbmLitTrackFinderBase::~CbmLitTrackFinderBase()
{
}

void CbmLitTrackFinderBase::ArrangeHits(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd)
{
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitHit* hit = *it;
      if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) { continue; }
      Int_t stationGroup = hit->GetStationGroup();
      Int_t station = hit->GetStation();
      Int_t substation = hit->GetSubstation();
      Int_t moduleRotation = fLayout.GetSubstation(stationGroup, station, substation).GetModuleRotationId(hit->GetModule());
      fHitData.AddHit(stationGroup, station, substation, moduleRotation, hit);
   }

//   std::cout << "CbmLitTrackFinderBase::ArrangeHits:\n" << fHitData.ToString();
//   std::cout << "CbmLitTrackFinderBase::ArrangeHits:\n" << fLayout.ToString();

   for (Int_t i = 0; i < fLayout.GetNofStationGroups(); i++) {
      for (Int_t j = 0; j < fLayout.GetNofStations(i); j++) {
         const CbmLitStation& station = fLayout.GetStation(i, j);
         if (station.GetType() == kLITPIXELHIT) {
            for (Int_t k = 0; k < fLayout.GetNofSubstations(i, j); k++) {
            	for (Int_t l = 0; l < fLayout.GetNofModuleRotations(i, j, k); l++) {
            	   HitPtrIteratorPair hits = fHitData.GetHits(i, j, k, l);
            	   std::sort(hits.first, hits.second, CompareHitPtrXULess());
//	               std::cout << "station group " << i << " station " << j << " substation " << k << " moduleRotation " << l << std::endl;
//	               for(HitPtrIterator it = hits.first; it != hits.second; it++)
//	                   std::cout << (*it)->ToString();
				   }
            }
         } else {
            for (Int_t k = 0; k < fLayout.GetNofSubstations(i, j); k++) {
            	for (Int_t l = 0; l < fLayout.GetNofModuleRotations(i, j, k); l++) {
				   HitPtrIteratorPair hits = fHitData.GetHits(i, j, k, l);
				   std::sort(hits.first, hits.second, CompareHitPtrXULess());
	//             std::cout << "station group " << i << " station " << j
	//                << " substation " << k << std::endl;
	//             for(HitPtrIterator it = hits.first; it != hits.second; it++)
	//                std::cout << (*it)->ToString();
				}
            }
         }
      }
   }
}

void CbmLitTrackFinderBase::RemoveHits(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitTrack* track = *it;
      if(track->GetQuality() == kLITBAD) { continue; }
      for (Int_t hit = 0; hit < track->GetNofHits(); hit++) {
         fUsedHitsSet.insert(track->GetHit(hit)->GetRefId());
      }
   }
}

void CbmLitTrackFinderBase::CopyToOutput(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd,
   TrackPtrVector& tracks)
{
   for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitTrack* track = *it;
      if(track->GetQuality() == kLITBAD) { continue; }
      if (!track->CheckParams()) { continue; }
      fUsedSeedsSet.insert(track->GetPreviousTrackId());
      tracks.push_back(new CbmLitTrack(*track));
   }
}
