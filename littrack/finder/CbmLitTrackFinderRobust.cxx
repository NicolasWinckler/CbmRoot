#include "CbmLitTrackFinderRobust.h"

#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitWeightCalculatorSimple.h"
#include "CbmLitEffHitCalculatorImp.h"
#include "CbmLitComparators.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderRobust::CbmLitTrackFinderRobust()
{
//	fWeightCalc = new CbmLitWeightCalculatorSimple;
//	fEffHitCalc = new CbmLitEffHitCalculatorImp;
}

CbmLitTrackFinderRobust::~CbmLitTrackFinderRobust()
{
//	delete fWeightCalc;
//	delete fEffHitCalc;
}

LitStatus CbmLitTrackFinderRobust::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderRobust::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderRobust::DoFind(
		const HitVector& hits,
		const TrackVector& trackSeeds,
		TrackVector& tracks)
{

	//TODO copy links
	fHitsArray = hits;
	fTrackSeeds = trackSeeds;

	fTracks.clear();
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
		
		FollowTracks(fTracks.begin(), fTracks.end());
		
		fTrackSelectionFinal->DoSelect(fTracks.begin(), fTracks.end());
		
		RemoveHits(fTracks.begin(), fTracks.end());
				
		CopyToOutput(fTracks.begin(), fTracks.end(), tracks);
		
		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();
		for (Int_t i = 0; i < fLayout.GetNofLayers(); i++) fHits[i].clear();
	}
	fEventNo++;
	std::cout << "-I- CbmLitTrackFinderRobust: " << fEventNo << " events processed" << std::endl;
	
	return kLITSUCCESS;
}

void CbmLitTrackFinderRobust::FollowTracks(
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	for (TrackIterator it = itBegin; it != itEnd; it++) {
		FollowTrack(*it);
	}
}

void CbmLitTrackFinderRobust::FollowTrack(
		CbmLitTrack *track) 
{
	Int_t nofLayers = fLayout.GetNofLayers();
	Int_t nofMissingHits = 0;
	CbmLitTrackParam par = *track->GetParamLast();
	
	for(Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
		fPropagator->Propagate(&par, fLayout.GetLayerZ(iLayer));
		HitIteratorPair bounds = MinMaxIndex(&par, iLayer);
		
		Bool_t hitAdded = false;
		for (HitIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
			if (IsIn(&par, *iHit)) {
				track->AddHit(*iHit);
				hitAdded = true;
			}
		}
		
		if (!hitAdded) {
			nofMissingHits++;
			if (nofMissingHits > fMaxNofMissingHits) return;
			continue;
		}
				
		track->SetLastPlaneId(iLayer);
		track->SetParamLast(&par);
		track->SetFlag(0);
	}
}

ClassImp(CbmLitTrackFinderRobust)
