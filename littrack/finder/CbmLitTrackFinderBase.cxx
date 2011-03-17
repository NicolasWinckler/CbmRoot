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
	fVerbose(1),
	fEventNo(0)
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
    	if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) continue;
     	fHitData.AddHit(hit->GetPlaneId(), hit);
    }

//    if (fVerbose > 1) std::cout << fHitData.ToString();

    for (int i = 0; i < fLayout.GetNofStationGroups(); i++){
    	for (int j = 0; j < fLayout.GetNofStations(i); j++){
    		CbmLitStation station = fLayout.GetStation(i, j);
    		if (station.GetType() == kLITPIXELHIT) {
    			for (int k = 0; k < fLayout.GetNofSubstations(i, j); k++){
    				HitPtrIteratorPair hits = fHitData.GetHits(i, j, k);
    				std::sort(hits.first, hits.second, CompareHitPtrXULess());
//    				std::cout << "station group " << i << " station " << j
//    					<< " substation " << k << std::endl;
//    				for(HitPtrIterator it = hits.first; it != hits.second; it++)
//    					std::cout << (*it)->ToString();
    			}
    		} else {
    			for (int k = 0; k < fLayout.GetNofSubstations(i, j); k++){
					HitPtrIteratorPair hits = fHitData.GetHits(i, j, k);
					std::sort(hits.first, hits.second, CompareHitPtrXULess());
//					std::cout << "station group " << i << " station " << j
//						<< " substation " << k << std::endl;
//					for(HitPtrIterator it = hits.first; it != hits.second; it++)
//						std::cout << (*it)->ToString();
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
		if(track->GetQuality() == kLITBAD) continue;
		for (int hit = 0; hit < track->GetNofHits(); hit++)
			fUsedHitsSet.insert(track->GetHit(hit)->GetRefId());
	}
}

void CbmLitTrackFinderBase::CopyToOutput(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd,
		TrackPtrVector& tracks)
{
	for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
		CbmLitTrack* track = *it;
		if(track->GetQuality() == kLITBAD) continue;
		if (!track->CheckParams()) continue;
		fUsedSeedsSet.insert(track->GetPreviousTrackId());
		tracks.push_back(new CbmLitTrack(*track));
	}
}
