#include "CbmLitTrackFinderBase.h"

#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrack.h"
#include "CbmLitComparators.h"
#include "CbmLitMath.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"

#include <iostream>
#include <cmath>
#include <algorithm>

CbmLitTrackFinderBase::CbmLitTrackFinderBase()
{
	fMaxCovSq = 20. * 20.;
	fVerbose = 1;
	fEventNo = 0;
}

CbmLitTrackFinderBase::~CbmLitTrackFinderBase()
{
}

void CbmLitTrackFinderBase::ArrangeHits(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd)
{
    for(HitPtrIterator hit = itBegin; hit != itEnd; hit++) {
    	if (fUsedHitsSet.find((*hit)->GetRefId()) != fUsedHitsSet.end()) continue;
    	int planeId = (*hit)->GetPlaneId();
     	fHitData.AddHit(planeId, *hit);
    }

    if (fVerbose > 1) std::cout << fHitData.ToString();

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
    		}
    	}
    }
}

void CbmLitTrackFinderBase::InitTrackSeeds(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	//TODO if more than one iteration, restore the state of the seeds
	fSeedSelection->DoSelect(itBegin, itEnd);

	for (TrackPtrIterator track = itBegin; track != itEnd; track++) {
		if ((*track)->GetQuality() == kLITBAD) continue;
		if (fSeedsIdSet.find((*track)->GetPreviousTrackId())
				!= fSeedsIdSet.end()) continue;
		(*track)->SetPDG(fPDG);
		fTracks.push_back(new CbmLitTrack(*(*track)));
	}

	// extrapolate to the begin station group for the tracking
	if (fBeginStationGroup > 0) {
		double Ze = fLayout.GetSubstation(fBeginStationGroup, 0, 0).GetZ();
		for (TrackPtrIterator track = fTracks.begin(); track != fTracks.end(); track++) {
			CbmLitTrackParam par(*(*track)->GetParamLast());
			fPropagator->Propagate(&par, Ze, fPDG);
			(*track)->SetParamLast(&par);
		}
	}
}

bool CbmLitTrackFinderBase::IsHitInValidationWindow(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit) const
{
	double chiSq = ChiSq(par, hit);
	if (hit->GetType() == kLITSTRIPHIT) return chiSq < fChiSqStripHitCut;
	if (hit->GetType() == kLITPIXELHIT) return chiSq < fChiSqPixelHitCut;
	return false;
}

HitPtrIteratorPair CbmLitTrackFinderBase::MinMaxIndex(
		const CbmLitTrackParam* par,
		int stationGroup,
		int station,
		int substation)
{
	HitPtrIteratorPair bounds;
	CbmLitStation st = fLayout.GetStationGroup(stationGroup).GetStation(station);
	if (st.GetType() == kLITSTRIPHIT || st.GetType() == kLITMIXHIT || !fUseFastSearch) {
		bounds = fHitData.GetHits(stationGroup, station, substation);
	} else if (st.GetType() == kLITPIXELHIT){
		CbmLitPixelHit hit;
		HitPtrIteratorPair hits = fHitData.GetHits(stationGroup, station, substation);
		double devX = CalcDevX(par, stationGroup, station, substation);
		if (devX == 0.) return bounds;
	    hit.SetX(par->GetX() - devX);
	    bounds.first = std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	    hit.SetX(par->GetX() + devX);
	    bounds.second =	std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	}

    return bounds;
}

double CbmLitTrackFinderBase::CalcDevX(
		const CbmLitTrackParam* par,
		int stationGroup,
		int station,
		int substation) const
{
	double C0 = par->GetCovariance(0);
	if(C0 > fMaxCovSq || C0 < 0.) return 0.;
//	return fSigmaCoef * (std::sqrt(C0) + fHitData.GetMaxErr(stationGroup, station, substation).first);
	return fSigmaCoef * (std::sqrt(C0) + fHitData.GetMaxErr(stationGroup, station, substation).first);
}

void CbmLitTrackFinderBase::RemoveHits(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
   for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
      if((*it)->GetQuality() == kLITBAD) continue;
      for (int hit = 0; hit < (*it)->GetNofHits(); hit++)
    	  fUsedHitsSet.insert((*it)->GetHit(hit)->GetRefId());
   }
}

void CbmLitTrackFinderBase::CopyToOutput(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd,
		TrackPtrVector& tracks)
{
	for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
		if( (*it)->GetQuality() == kLITBAD) continue;
		if (!(*it)->CheckParams()) continue;
		tracks.push_back(new CbmLitTrack(*(*it)));
	    fSeedsIdSet.insert((*it)->GetPreviousTrackId());
	}
}
