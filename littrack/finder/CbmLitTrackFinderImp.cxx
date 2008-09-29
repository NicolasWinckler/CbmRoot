#include "CbmLitTrackFinderImp.h"

#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <iostream>
#include <cmath> 
#include <algorithm>

CbmLitTrackFinderImp::CbmLitTrackFinderImp()
{
	//TODO set default settings for the tracking
	fEventNo = 0;
	fMaxErrSq = 20. * 20.;
}

CbmLitTrackFinderImp::~CbmLitTrackFinderImp()
{
	if (fPropagator) delete fPropagator;
	if (fFilter) delete fFilter;
	if (fFitter) delete fFitter;
	if (fSeedSelection) delete fSeedSelection;
	if (fStationSelection) delete fStationSelection;
	if (fFinalSelection) delete fFinalSelection;
	if (fFinalPreSelection) delete fFinalPreSelection;
}

void CbmLitTrackFinderImp::ArrangeHits()
{
	std::fill(fMaxErrX.begin(), fMaxErrX.end(), 0.);
	std::fill(fMaxErrY.begin(), fMaxErrY.end(), 0.);

    for(HitIterator iHit = fHitArray.begin(); iHit != fHitArray.end(); iHit++) {
    	if (fUsedHitsSet.find((*iHit)->GetRefId()) != fUsedHitsSet.end()) continue;

    	Int_t planeId = (*iHit)->GetPlaneId();
        if(planeId < 0 || planeId > fLayout.GetNofLayers()) {
    		std::cout << "-W- CbmLitTrackFinderImp::ArrangeHits: "
    				  << "wrong planeId number." << std::endl;
    		continue;
    	}
      
    	if (fMaxErrX[planeId] < (*iHit)->GetDx()) fMaxErrX[planeId] = (*iHit)->GetDx();
    	if (fMaxErrY[planeId] < (*iHit)->GetDy()) fMaxErrY[planeId] = (*iHit)->GetDy();

    	fHits[planeId].push_back(*iHit);
    }
    
    if (fVerbose > 1) {
    	std::cout << "-I- CbmLitTrackFinderImp::ArrangeHits: " << std::endl;
	    for (Int_t i = 0; i < fLayout.GetNofLayers(); i++) {
	    	std::cout << "  Station " << i << " has " 
	                  << fHits[i].size() << " hits" << std::endl;
	    }
	}
	   
    if (fVerbose > 1)
    	std::cout << "-I- CbmLitTrackFinderImp::ArrangeHits: " << std::endl;

    // sort hits on each layer by x or y coordinate
    for (Int_t i = 0; i < fLayout.GetNofLayers(); i++) {
	    if (fMaxErrX[i] > fMaxErrY[i]) {
	    	std::sort(fHits[i].begin(), fHits[i].end(), CompareHitPtrYLess());
	    	fMaxErrX[i] = 0.;
	        if (fVerbose > 1) std::cout << "-y-";
	    } else {
	        std::sort(fHits[i].begin(), fHits[i].end(), CompareHitPtrXLess());
	        fMaxErrY[i] = 0.;
	        if (fVerbose > 1) std::cout << "-x-";
	    }
    }
    if (fVerbose > 1) std::cout << std::endl;

    if (fVerbose > 1) {
    	std::cout << "-I- CbmLitTrackFinderImp::ArrangeHits: "
                  << " max hit error: " << std::endl;
    	for (Int_t i = 0; i < fLayout.GetNofLayers(); i++)
    		std::cout << "   fMaxErrX[" << i << "]=" << fMaxErrX[i]
                      << "   fMaxErrY[" << i << "]=" << fMaxErrY[i] << std::endl;
    }
}

void CbmLitTrackFinderImp::InitTrackSeeds()
{
	//TODO if more than one iteration, restore the state of the seeds
	fSeedSelection->DoSelect(fTrackSeedArray);
	
	for (TrackIterator iTrack = fTrackSeedArray.begin(); iTrack != fTrackSeedArray.end(); iTrack++) {
		if ((*iTrack)->GetQuality() == kLITBAD) continue;
		if (fSeedsIdSet.find((*iTrack)->GetPreviousTrackId()) 
				!= fSeedsIdSet.end()) continue;
		fTracks.push_back(new CbmLitTrack(*(*iTrack)));
	}
	
	// extrapolate to the begin layer for the tracking
	Int_t beginLayer = 0;
	if (fBeginStation > 0) {    
		for(Int_t i = 0; i < fBeginStation; i++) 
			beginLayer += fLayout.GetNofLayers(i);   
		
		Double_t Ze = fLayout.GetLayerZ(beginLayer);
		for (TrackIterator iTrack = fTracks.begin();iTrack != fTracks.end(); iTrack++) {
			CbmLitTrackParam par(*(*iTrack)->GetParamLast());
			fPropagator->Propagate(&par, Ze, fPDG);	
			(*iTrack)->SetParamLast(&par);			
		} 
	}
}

Bool_t CbmLitTrackFinderImp::IsIn(
		const CbmLitTrackParam* par, 
		const CbmLitHit *hit) const 
{
    Double_t Cov00 = par->GetCovariance(0);
    Double_t Cov11 = par->GetCovariance(5);
    
	if(Cov00 > fMaxErrSq || Cov11 > fMaxErrSq) return kFALSE;  
	if(Cov00 < 0. || Cov11 < 0.) return kFALSE;  
   
   	Double_t x1 = par->GetX();
   	Double_t x2 = hit->GetX();
    Double_t dx2 = hit->GetDx();
   	Double_t y1 = par->GetY();
   	Double_t y2 = hit->GetY();
   	Double_t dy2 = hit->GetDy();   
//    Double_t devX = fSigmaCoef * std::sqrt(Cov00 + dx2 * dx2);      
//    Double_t devY = fSigmaCoef * std::sqrt(Cov11 + dy2 * dy2);
    Double_t devX = fSigmaCoef * (std::sqrt(Cov00) + dx2);      
    Double_t devY = fSigmaCoef * (std::sqrt(Cov11) + dy2);
   	
    return ( ( (x1 + devX) >= x2 ) &&
             ( (x1 - devX) <= x2 ) &&
             ( (y1 + devY) >= y2 ) &&
	         ( (y1 - devY) <= y2 ) );
}

HitIteratorPair CbmLitTrackFinderImp::MinMaxIndex(
		const CbmLitTrackParam* par,
		Int_t layer)
{
	HitIteratorPair bounds;
	CbmLitHit hit;
   
   if (fMaxErrY[layer] != 0) {
      Double_t devY = CalcDevY(par, layer);
      if (devY == 0.) return bounds;
      hit.SetY(par->GetY() - devY);
      bounds.first = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                               &hit, CompareHitPtrYLess());
      hit.SetY(par->GetY() + devY);
      bounds.second = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                               &hit, CompareHitPtrYLess());
   } else {
      Double_t devX = CalcDevX(par, layer);
      if (devX == 0.) return bounds;
      hit.SetX(par->GetX() - devX);
      bounds.first = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                              &hit, CompareHitPtrXLess());
      hit.SetX(par->GetX() + devX);
      bounds.second = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                              &hit, CompareHitPtrXLess());
   }   
   return bounds;
}

Double_t CbmLitTrackFinderImp::CalcDevX(
		const CbmLitTrackParam* par,
		Int_t layer) const
{
	Double_t C0 = par->GetCovariance(0);
	if(C0 > fMaxErrSq || C0 < 0.) return 0.;
//	return fSigmaCoef * std::sqrt(C0 + fMaxErrX[layer] * fMaxErrX[layer]);
	return fSigmaCoef * (std::sqrt(C0) + fMaxErrX[layer]);
}

Double_t CbmLitTrackFinderImp::CalcDevY(
		const CbmLitTrackParam* par,
		Int_t layer) const
{
	Double_t C5 = par->GetCovariance(5);
	if(C5 > fMaxErrSq || C5 < 0.) return 0.;
//  return fSigmaCoef * std::sqrt(C5 + fMaxErrY[layer] * fMaxErrY[layer]);
	return fSigmaCoef * (std::sqrt(C5) + fMaxErrY[layer]);
}

void CbmLitTrackFinderImp::RemoveHits(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
   for(TrackIterator it = itBegin; it != itEnd; it++) {
      if((*it)->GetQuality() == kLITBAD) continue;
      Int_t nofHits = (*it)->GetNofHits();
      for (Int_t iHit = 0; iHit < nofHits; iHit++) {
         Int_t HitId = (*it)->GetHit(iHit)->GetRefId();
         fUsedHitsSet.insert(HitId);               
      }
   }
   
   if (fVerbose > 1) 
      std::cout << "-I- CbmLitTrackFinder::RemoveHits(): "
                << "fUsedHitsSet.size() = " << fUsedHitsSet.size() << std::endl;
}

void CbmLitTrackFinderImp::CopyToOutput(
		TrackIterator itBegin,
		TrackIterator itEnd,
		TrackVector& tracks)
{
	for(TrackIterator it = itBegin; it != itEnd; it++) {
		if( (*it)->GetQuality() == kLITBAD) continue;
		tracks.push_back(new CbmLitTrack(*(*it)));
	    fSeedsIdSet.insert((*it)->GetPreviousTrackId());
	}
}

ClassImp(CbmLitTrackFinderImp)
