#include "CbmLitTofMerger.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrack.h"
#include "CbmLitComparators.h"

#include "FairRootManager.h"
#include "CbmGlobalTrack.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"

#include <algorithm>

CbmLitTofMerger::CbmLitTofMerger()
{
}

CbmLitTofMerger::~CbmLitTofMerger()
{

}

void CbmLitTofMerger::Init()
{
    FairRootManager *rootMgr = FairRootManager::Instance();
    if(rootMgr == NULL)
    	TObject::Fatal("CbmLitTofMerger::Init()", "ROOT manager is not instantiated");

    fTrdTracks = (TClonesArray*) rootMgr->GetObject("TRDTrack");
    if(fTrdTracks == NULL) TObject::Fatal("CbmLitTofMerger::Init()", "no TRD track array");
    fTrdHits = (TClonesArray*) rootMgr->GetObject("TRDHit");
    if(fTrdHits == NULL) TObject::Fatal("CbmLitTofMerger::Init()", "no TRD hit array");

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	SetPropagator(factory->CreateTrackPropagator("lit"));
	SetFilter(factory->CreateTrackUpdate("kalman"));
	SetPDG(11);

	fLayout = CbmLitEnvironment::Instance()->GetTofLayout();

	//gating
	fSigmaCoef = 3.5;
	fMaxCovSq = 400.;
	fChiSqStripHitCut = 10.;
	fChiSqPixelHitCut = 15.;
	fUseFastSearch = false;
	//
}

Int_t CbmLitTofMerger::DoMerge(
		TClonesArray* glbTracks,
		TClonesArray* tofHits)
{
//	CbmLitConverter::TrdTrackArrayToTrackVector(fTrdTracks, fLitTracks, fTrdHits);
	GlobalTrackArrayToTrackVector(fTrdTracks, fLitTracks, fTrdHits, glbTracks);
	CbmLitConverter::HitArrayToHitVector(tofHits, fLitHits);

	//for (int i = 0; i < fLitHits.size(); i++) std::cout << fLitHits[i]->ToString();

	fHitData.SetDetectorLayout(fLayout);
	ArrangeHits(fLitHits.begin(), fLitHits.end(), fHitData);

//	CbmLitNearestHitToTrackMerger::DoMerge(fHitData, fLitTracks, fLayout, 0, 0);

	AddTofHitToGlobalTrack(fLitTracks, glbTracks);

	fHitData.Clear();

	for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
	for_each(fLitHits.begin(), fLitHits.end(), DeleteObject());
//	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	fLitTracks.clear();
	fLitHits.clear();
//	trackSeeds.clear();

	return glbTracks->GetEntriesFast();
}

void CbmLitTofMerger::ArrangeHits(
		HitPtrIterator itBegin,
		HitPtrIterator itEnd,
		CbmLitHitData& hitData)
{
    for(HitPtrIterator hit = itBegin; hit != itEnd; hit++) {
    	//if (fUsedHitsSet.find((*hit)->GetRefId()) != fUsedHitsSet.end()) continue;
    	Int_t planeId = (*hit)->GetPlaneId();
     	hitData.AddHit(planeId, *hit);
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

void CbmLitTofMerger::GlobalTrackArrayToTrackVector(
		const TClonesArray* trdTracks,
		TrackPtrVector& litTracks,
		TClonesArray* trdHits,
		TClonesArray* globalTracks)
{
	Int_t nofTracks = globalTracks->GetEntriesFast();
	for(Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
	   CbmGlobalTrack* track = (CbmGlobalTrack*) globalTracks->At(iTrack);
	   if (track == NULL) continue;
	   if(track->GetStsTrackIndex() < 0 || track->GetTrdTrackIndex() < 0) continue;

	   CbmTrdTrack* trdTrack = (CbmTrdTrack*) trdTracks->At(track->GetTrdTrackIndex());
	   //if (track->GetParamLast()->GetQp() == 0) continue;
	   CbmLitTrack* litTrack = new CbmLitTrack;
	   CbmLitConverter::TrackToLitTrack(trdTrack, litTrack, trdHits, NULL);
	   //litTrack->SetPreviousTrackId(iTrack);
	   litTrack->SetRefId(iTrack);
	   litTracks.push_back(litTrack);
	}
}

void CbmLitTofMerger::AddTofHitToGlobalTrack(
		const TrackPtrVector& litTracks,
		TClonesArray* globalTracks)
{
	for (int i = 0; i < litTracks.size(); i++) {
		CbmLitTrack* track = litTracks[i];
		Int_t globalTrackId = track->GetRefId();
		CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) globalTracks->At(globalTrackId);
		const CbmLitHit* hit = track->GetHit(track->GetNofHits()-1);
		if (hit->GetZ() > 990.) globalTrack->SetTofHitIndex(hit->GetRefId());
	}
}

ClassImp(CbmLitTofMerger);
