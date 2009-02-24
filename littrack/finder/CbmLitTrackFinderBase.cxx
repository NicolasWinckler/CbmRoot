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
    	Int_t planeId = (*hit)->GetPlaneId();
    	fHitData.AddHit(planeId, *hit);
    }

    if (fVerbose > 1) std::cout << fHitData.ToString();

    for (Int_t i = 0; i < fLayout.GetNofStationGroups(); i++){
    	for (Int_t j = 0; j < fLayout.GetNofStations(i); j++){
    		CbmLitStation station = fLayout.GetStation(i, j);
    		if (station.GetType() == kLITPIXELHIT) {
    			for (Int_t k = 0; k < fLayout.GetNofSubstations(i, j); k++){
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
		Double_t Ze = fLayout.GetSubstation(fBeginStationGroup, 0, 0).GetZ();
		for (TrackPtrIterator track = fTracks.begin(); track != fTracks.end(); track++) {
			CbmLitTrackParam par(*(*track)->GetParamLast());
			fPropagator->Propagate(&par, Ze, fPDG);
			(*track)->SetParamLast(&par);
		}
	}
}

Bool_t CbmLitTrackFinderBase::IsHitInValidationWindow(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit) const
{
	Double_t chiSq = ChiSq(par, hit);
	if (hit->GetType() == kLITSTRIPHIT) return chiSq < fChiSqStripHitCut;
	if (hit->GetType() == kLITPIXELHIT) return chiSq < fChiSqPixelHitCut;
	return false;

//    Double_t C0 = par->GetCovariance(0);
//    Double_t C5 = par->GetCovariance(5);
//
//	if(C0 > fMaxCovSq || C5 > fMaxCovSq) return kFALSE;
//	if(C0 < 0. || C5 < 0.) return kFALSE;
//
//	return ChiSq(par, hit) < fChiSqPixelHitCut;
//
//	if (hit->GetType() == kLITSTRIPHIT) {
//		const CbmLitStripHit* stripHit = static_cast<const CbmLitStripHit*>(hit);
//		return ChiSq(par, stripHit) < fChiSqStripHitCut;
//	} else if (hit->GetType() == kLITPIXELHIT) {
//		const CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
//		return ChiSq(par, pixelHit) < fChiSqPixelHitCut;
//	}

//	else if (hit->GetType() == kLITPIXELHIT) {
//		const CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
//		Double_t x1 = par->GetX();
//	   	Double_t x2 = pixelHit->GetX();
//	    Double_t dx2 = pixelHit->GetDx();
//	   	Double_t y1 = par->GetY();
//	   	Double_t y2 = pixelHit->GetY();
//	   	Double_t dy2 = pixelHit->GetDy();
////	   	Double_t devX = fSigmaCoef * std::sqrt(C0 + dx2 * dx2);
////		Double_t devY = fSigmaCoef * std::sqrt(C5 + dy2 * dy2);
//	    Double_t devX = fSigmaCoef * (std::sqrt(C0) + dx2);
//	    Double_t devY = fSigmaCoef * (std::sqrt(C5) + dy2);
//
//	    return ( ( (x1 + devX) >= x2 ) &&
//	             ( (x1 - devX) <= x2 ) &&
//	             ( (y1 + devY) >= y2 ) &&
//		         ( (y1 - devY) <= y2 ) );
//	}
}

HitPtrIteratorPair CbmLitTrackFinderBase::MinMaxIndex(
		const CbmLitTrackParam* par,
		Int_t stationGroup,
		Int_t station,
		Int_t substation)
{
	HitPtrIteratorPair bounds;
	CbmLitStation st = fLayout.GetStationGroup(stationGroup).GetStation(station);
	if (st.GetType() == kLITSTRIPHIT || st.GetType() == kLITMIXHIT || !fUseFastSearch) {
		bounds = fHitData.GetHits(stationGroup, station, substation);
	} else if (st.GetType() == kLITPIXELHIT){
		CbmLitPixelHit hit;
		HitPtrIteratorPair hits = fHitData.GetHits(stationGroup, station, substation);
		Double_t devX = CalcDevX(par, stationGroup, station, substation);
		if (devX == 0.) return bounds;
	    hit.SetX(par->GetX() - devX);
	    bounds.first = std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	    hit.SetX(par->GetX() + devX);
	    bounds.second =	std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	}

    return bounds;
}

Double_t CbmLitTrackFinderBase::CalcDevX(
		const CbmLitTrackParam* par,
		Int_t stationGroup,
		Int_t station,
		Int_t substation) const
{
	Double_t C0 = par->GetCovariance(0);
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
      for (Int_t hit = 0; hit < (*it)->GetNofHits(); hit++)
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

ClassImp(CbmLitTrackFinderBase);
