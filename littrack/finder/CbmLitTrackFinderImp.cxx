#include "CbmLitTrackFinderImp.h"

#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <iostream>
#include <cmath> 

CbmLitTrackFinderImp::CbmLitTrackFinderImp()
{
	//TODO set default settings for the tracking
	fEventNo = 0;
}

CbmLitTrackFinderImp::~CbmLitTrackFinderImp()
{

}

void CbmLitTrackFinderImp::ArrangeHits()
{
	Int_t nofHits = fHitsArray.size();

	for (Int_t i = 0; i < fLayout.GetNofLayers(); i++){
		fMaxErrY[i] = 0.;
		fMaxErrX[i] = 0.;
    }

    for(Int_t iHit = 0; iHit < nofHits; iHit++) {
    	if (fUsedHitsSet.find(iHit) != fUsedHitsSet.end()) continue;
      
    	CbmLitHit* hit = fHitsArray[iHit];
    	Int_t planeId = hit->GetPlaneId();
                 
    	if(planeId < 0 || planeId > fLayout.GetNofLayers()) {
    		std::cout << "-W- CbmLitTrackFinderImp::ArrangeHits: "
    				  << "wrong planeId number." << std::endl;
    		continue;
    	}
      
    	if (fMaxErrX[planeId] < hit->GetDx()) fMaxErrX[planeId] = hit->GetDx();
    	if (fMaxErrY[planeId] < hit->GetDy()) fMaxErrY[planeId] = hit->GetDy();

    	fHits[planeId].push_back(hit);
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
	    	fMaxErrX[i] = 0;
	        if (fVerbose > 1) std::cout << "-y-";
	    } else {
	        std::sort(fHits[i].begin(), fHits[i].end(), CompareHitPtrXLess());
	        fMaxErrY[i] = 0;
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
	fTrackSeedSelection->DoSelect(fTrackSeeds.begin(), fTrackSeeds.end());
	
	for (TrackIterator iTrack = fTrackSeeds.begin(); iTrack != fTrackSeeds.end(); iTrack++) {
		if ((*iTrack)->GetFlag() == 1) continue;
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
		const CbmLitHit *hit) 
{
 
   Double_t x1 = par->GetX();
   Double_t x2 = hit->GetX();
         
   Double_t y1 = par->GetY();
   Double_t y2 = hit->GetY();
   
   Double_t devX;
   Double_t devY;
   
   if (!fPrecalcSearchRegions) {
      Double_t Cov00 = TMath::Abs(par->GetCovariance(0));
      Double_t Cov11 = TMath::Abs(par->GetCovariance(5));
	      
      if( Cov00 > 100. ) return kFALSE;
      if( Cov11 > 100. ) return kFALSE;  
	   
      //Double_t x1 = par->GetX();
      //Double_t x2 = pHit->GetX();
      Double_t dx2 = hit->GetDx();
     
      //Double_t y1 = par->GetY();
      //Double_t y2 = pHit->GetY();
      Double_t dy2 = hit->GetDy();      
	      
      devX = fSigmaCoef * std::sqrt(Cov00 + dx2 * dx2);      
      devY = fSigmaCoef * std::sqrt(Cov11 + dy2 * dy2);
	   // devX = fSigmaCoef * (sqrt(Cov00) + dx2);      
	   // devY = fSigmaCoef * (sqrt(Cov11) + dy2);
	      
   } else {
	  Int_t layer = hit->GetPlaneId();
	  devX = fSigmaCoef * fSigmaX[layer];
	  devY = fSigmaCoef * fSigmaY[layer];
   }  
   
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
      Double_t devY= CalcDevY(par, layer);
      hit.SetY(par->GetY() - devY);
      bounds.first = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                               &hit, CompareHitPtrYLess());
      hit.SetY(par->GetY() + devY);
      bounds.second = lower_bound(fHits[layer].begin(), fHits[layer].end(),
                               &hit, CompareHitPtrYLess());
   } else {
      Double_t devX = CalcDevX(par, layer);
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
		Int_t layer)
{
	Double_t devX;
	if (!fPrecalcSearchRegions) {
		Double_t Cov00 = std::abs(par->GetCovariance(0));
	    if (std::abs(Cov00) > 100.) devX = 0;
	    else devX = fSigmaCoef * std::sqrt(Cov00 + fMaxErrX[layer] * fMaxErrX[layer]);
	} else {
	    devX = fSigmaCoef * fSigmaY[layer];
	} 
	return devX;
}

Double_t CbmLitTrackFinderImp::CalcDevY(
		const CbmLitTrackParam* par,
		Int_t layer)
{
	Double_t devY;
    if (!fPrecalcSearchRegions) {
       Double_t Cov11 = std::abs(par->GetCovariance(5));
       if (std::abs(Cov11) > 100.) devY = 0;
       else devY = fSigmaCoef * std::sqrt(Cov11 + fMaxErrY[layer] * fMaxErrY[layer]);
    } else {
       devY = fSigmaCoef * fSigmaY[layer];
    }
	return devY;
}


void CbmLitTrackFinderImp::RemoveHits(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
   for(TrackIterator it = itBegin; it != itEnd; it++) {
      if((*it)->GetFlag() == 1) continue;
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
		if( (*it)->GetFlag() == 1) continue;
		tracks.push_back(new CbmLitTrack(*(*it)));
	    fSeedsIdSet.insert((*it)->GetPreviousTrackId());
	}
}

ClassImp(CbmLitTrackFinderImp)


