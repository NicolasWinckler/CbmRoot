#include "CbmLitTrackFinderBranch.h"

#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitMath.h"

#include <iostream>
#include <cmath>

CbmLitTrackFinderBranch::CbmLitTrackFinderBranch()
{
	fMaxNofBranches = 64;
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
		const HitVector& hits,
		const TrackVector& trackSeeds,
		TrackVector& tracks)
{

	//TODO copy links
	fHitArray = hits;
	fTrackSeedArray = trackSeeds;

	fTracks.clear();
	fFoundTracks.clear();
	fSeedsIdSet.clear();
	fUsedHitsSet.clear();
	fHits.clear();
	fHits.resize(fLayout.GetNofLayers());
	fMaxErrX.resize(fLayout.GetNofLayers());
	fMaxErrY.resize(fLayout.GetNofLayers());
	
	for (Int_t iIter = 0; iIter < fNofIter; iIter++) {
	       
		SetIterationParameters(iIter);
		
		ArrangeHits();
		      
		InitTrackSeeds();
		
		TrackFollowing();
		
		fFinalPreSelection->DoSelect(fTracksCopy.begin(), fTracksCopy.end());
		
		RefitTracks(fTracksCopy.begin(), fTracksCopy.end());
		
		fFinalSelection->DoSelect(fTracksCopy.begin(), fTracksCopy.end());
		
		RemoveHits(fTracksCopy.begin(), fTracksCopy.end());
				
		CopyToOutput(fTracksCopy.begin(), fTracksCopy.end(), tracks);
		
		for_each(fTracksCopy.begin(), fTracksCopy.end(), DeleteObject());
		fTracksCopy.clear();		   
		for (Int_t i = 0; i < fLayout.GetNofLayers(); i++) fHits[i].clear();
	}
	   
	std::cout << "-I- CbmLitTrackFinderBranch: " << fEventNo++ << " events processed" << std::endl;
	
	return kLITSUCCESS;
}

void CbmLitTrackFinderBranch::TrackFollowing()
{  
	//loop over stations
	for (Int_t iStation = fBeginStation; iStation < fEndStation + 1; iStation++) { 

		for (TrackIterator it = fTracks.begin(); it != fTracks.end(); it++) 
			TrackFollowingStation(*it, iStation);
		
		if (fVerbose > 1)
			std::cout << "-I- CbmLitTrackFinderImp::TrackFollowing:" 
				<< fFoundTracks.size() << " followed in the "
				<< iStation << " station" << std::endl;
		
		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();

		fStationSelection->DoSelect(fFoundTracks.begin(), fFoundTracks.end());

		CopyToOutputArray();
	    
	} //loop over stations
	
	for_each(fTracks.begin(), fTracks.end(), DeleteObject());
	fTracks.clear();
}

void CbmLitTrackFinderBranch::TrackFollowingStation(
		const CbmLitTrack *track, 
		Int_t station) 
{
	Int_t nofLayers = fLayout.GetNofLayers(station);
	std::vector<CbmLitTrackParam> par(nofLayers);
	par[0] = *track->GetParamLast();
	std::vector<CbmLitTrackParam> uPar(nofLayers);
    HitVector hits(nofLayers);
    std::vector<HitIteratorPair> bounds(nofLayers);
    std::vector<Int_t> nofMissingHits(nofLayers, 0);
    std::vector<Double_t> chi2(nofLayers, 0);
    Int_t nofBranches = 0;
    
   Int_t layer = 0;
   for (Int_t i = 0; i < station; i++) layer += fLayout.GetNofLayers(i);
   
   Int_t extraLoop = 0;
   if (fMaxNofMissingHitsInStation > 0) extraLoop = 1;    
   if (std::abs(layer - track->GetNofHits()) >= fMaxNofMissingHits) extraLoop = 0;
   
   if (fPropagator->Propagate(&par[0], fLayout.GetLayerZ(layer + 0), fPDG) == kLITERROR) return;
   bounds[0] = MinMaxIndex(&par[0], layer + 0);
   for (HitIterator iHit0 = bounds[0].first; iHit0 != bounds[0].second + extraLoop; iHit0++) { //1
      if (!ProcessLayer(0, iHit0, bounds[0], nofMissingHits, &par[0], &uPar[0], hits, chi2)) continue;
       
      if (fPropagator->Propagate(&uPar[0], &par[1], fLayout.GetLayerZ(layer + 1), fPDG) == kLITERROR) continue;
      bounds[1] = MinMaxIndex(&par[1], layer + 1);
      for (HitIterator iHit1 = bounds[1].first; iHit1 != bounds[1].second + extraLoop; iHit1++) { //2
         if (!ProcessLayer(1, iHit1, bounds[1], nofMissingHits, &par[1], &uPar[1], hits, chi2)) continue;
         
         if (nofLayers < 3) {
        	 if (nofBranches++ > fMaxNofBranches) return;
        	 AddTrackCandidate(track, hits, chi2, &uPar[1], station);
             continue;
         }
         
         if (fPropagator->Propagate(&uPar[1], &par[2], fLayout.GetLayerZ(layer + 2), fPDG) == kLITERROR) continue;
         bounds[2] = MinMaxIndex(&par[2], layer + 2);
         for (HitIterator iHit2 = bounds[2].first; iHit2 != bounds[2].second + extraLoop; iHit2++) { //3
            if (!ProcessLayer(2, iHit2, bounds[2], nofMissingHits, &par[2], &uPar[2], hits, chi2)) continue;
  
            if (nofLayers < 4) {
            	if (nofBranches++ > fMaxNofBranches) return;
               AddTrackCandidate(track, hits, chi2, &uPar[2], station);
               continue;
            }
            
            if (fPropagator->Propagate(&uPar[2], &par[3], fLayout.GetLayerZ(layer + 3), fPDG) == kLITERROR) continue;
            bounds[3] = MinMaxIndex(&par[3], layer + 3);
            for (HitIterator iHit3 = bounds[3].first; iHit3 != bounds[3].second + extraLoop; iHit3++) { //4
               if (!ProcessLayer(3, iHit3, bounds[3], nofMissingHits, &par[3],&uPar[3], hits, chi2)) continue;
               
               if (nofBranches++ > fMaxNofBranches) return;
               AddTrackCandidate(track, hits, chi2, &uPar[3], station);
           
            } //4
         } //3
      } //2
   } //1
}

Bool_t CbmLitTrackFinderBranch::ProcessLayer(                      
		Int_t layerInStation,
		HitIterator &hitIt,
		HitIteratorPair &bounds,
 		std::vector<Int_t>& nofMissingHits,  
		CbmLitTrackParam* par,
		CbmLitTrackParam* uPar,
		HitVector& hits,
		std::vector<Double_t>& chi2)
{
   Bool_t result = true;
   
   if (layerInStation == 0) nofMissingHits[0] = 0; 
   else nofMissingHits[layerInStation] = nofMissingHits[layerInStation - 1];
             
   if (hitIt < bounds.second) {
      hits[layerInStation] = *hitIt;
      if (!IsIn(par,  hits[layerInStation])) result = false;
      *uPar = *par; 
      if (result) {
    	  fFilter->Update(uPar,  hits[layerInStation]);
    	  chi2[layerInStation] = ChiSq(uPar, hits[layerInStation]);
      } else {
    	  chi2[layerInStation] = 0;
      }
   } else {
      hits[layerInStation] = NULL;
      nofMissingHits[layerInStation]++;
      if (nofMissingHits[layerInStation] > fMaxNofMissingHitsInStation) result = false;
      chi2[layerInStation] = 0.;
   }
   
   return result;
}

void CbmLitTrackFinderBranch::AddTrackCandidate(
		const CbmLitTrack* track,
		const HitVector& hits, 
		const std::vector<Double_t>& chi2,
		const CbmLitTrackParam* par,
		Int_t station)
{
   CbmLitTrack* newTrack = new CbmLitTrack(*track);
   Double_t chi2sum = newTrack->GetChi2();
   
   for (Int_t i = 0; i < hits.size(); i++) {
	  if (hits[i] == NULL) continue;
      newTrack->AddHit(hits[i]);
      chi2sum += chi2[i];
   }
     
   newTrack->SortHits();
   newTrack->SetPDG(fPDG);
   newTrack->SetLastPlaneId(station);
   newTrack->SetParamLast(par);
   newTrack->SetChi2(chi2sum);
   newTrack->SetNDF(NDF(newTrack->GetNofHits())); 
   
   if (newTrack->GetNofHits() == 0) {
      delete newTrack;
      return;
   }   

   fFoundTracks.push_back(newTrack);
}

void CbmLitTrackFinderBranch::RefitTracks(
		TrackIterator itBegin,
		TrackIterator itEnd) 
{
   for (TrackIterator it = itBegin; it != itEnd; it++)
	   if ((*it)->GetQuality() == kLITGOOD) fFitter->Fit(*it);
}

void CbmLitTrackFinderBranch::CopyToOutputArray() 
{
	for (TrackIterator iTrack = fFoundTracks.begin();
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

ClassImp(CbmLitTrackFinderBranch)
