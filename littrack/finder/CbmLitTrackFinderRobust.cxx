#include "CbmLitTrackFinderRobust.h"

#include "CbmLitTrackSelection.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitWeightCalculatorSimple.h"
#include "CbmLitEffHitCalculatorImp.h"
#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <iostream>
#include <algorithm>

CbmLitTrackFinderRobust::CbmLitTrackFinderRobust()
{
}

CbmLitTrackFinderRobust::~CbmLitTrackFinderRobust()
{
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
		const HitPtrVector& hits,
		const TrackPtrVector& trackSeeds,
		TrackPtrVector& tracks)
{

	//TODO copy links
	fHitArray = hits;
	fTrackSeedArray = trackSeeds;

	fTracks.clear();
	fSeedsIdSet.clear();
	fUsedHitsSet.clear();
	fHitData.SetDetectorLayout(fLayout);
	
	for (Int_t iIter = 0; iIter < fNofIter; iIter++) {
	       
		SetIterationParameters(iIter);
		
		ArrangeHits(fHitArray.begin(), fHitArray.end());
		      
		InitTrackSeeds(fTrackSeedArray.begin(), fTrackSeedArray.end());
		
		FollowTracks(fTracks.begin(), fTracks.end());
		
		fFinalSelection->DoSelect(fTracks.begin(), fTracks.end());
		
		RemoveHits(fTracks.begin(), fTracks.end());
				
		CopyToOutput(fTracks.begin(), fTracks.end(), tracks);
		
		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();
		fHitData.Clear();
	}
//	fEventNo++;
//	std::cout << "-I- CbmLitTrackFinderRobust: " << fEventNo << " events processed" << std::endl;
	
	return kLITSUCCESS;
}

void CbmLitTrackFinderRobust::FollowTracks(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
		//std::cout << "id=" << (it - itBegin) << " ";
		FollowTrack(*it);
		//std::cout << std::endl;
	}
}

void CbmLitTrackFinderRobust::FollowTrack(
		CbmLitTrack *track) 
{
	Int_t nofLayers = fLayout.GetNofPlanes();
	Int_t nofMissingHits = 0;
	CbmLitTrackParam par = *track->GetParamLast();
	
	//TODO: rewrite loop station groups -> stations
	// there will be two loops
	
	for(Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
		//fPropagator->Propagate(&par, fLayout.GetLayerZ(iLayer), fPDG);
		//HitPtrIteratorPair bounds = MinMaxIndex(&par, iLayer);
		
//		Bool_t hitAdded = false;
//		for (HitPtrIterator iHit = bounds.first; iHit != bounds.second; iHit++) {
//			if (IsIn(&par, *iHit)) {
//				track->AddHit(*iHit);
//				//std::cout << (*iHit)->GetRefId() << " ";
//				hitAdded = true;
//			}
//		}
//		
//		if (!hitAdded) {
//			nofMissingHits++;
//			if (nofMissingHits > fMaxNofMissingHits) {
//				//track->SetQuality(kLITBAD);
//				return;
//			}
//			continue;
//		}
//		
//		track->SetPDG(fPDG);
//		track->SetLastPlaneId(iLayer);
//		track->SetParamLast(&par);
		//track->SetQuality(kLITGOOD);
	}
}

ClassImp(CbmLitTrackFinderRobust)
