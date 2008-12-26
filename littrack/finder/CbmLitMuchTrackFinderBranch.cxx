#include "CbmLitMuchTrackFinderBranch.h"

#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"

#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <vector>
#include <iostream>
#include <algorithm>


CbmLitMuchTrackFinderBranch::CbmLitMuchTrackFinderBranch()
{	
}

CbmLitMuchTrackFinderBranch::~CbmLitMuchTrackFinderBranch()
{
	if (fPropagatorToDet) delete fPropagatorToDet;
}

void CbmLitMuchTrackFinderBranch::Init()
{
   CbmRootManager* rootMgr = CbmRootManager::Instance();
   if(NULL == rootMgr) 
      TObject::Fatal("CbmLitMuchTrackFinderBranch::Init","ROOT manager is not instantiated");

   fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("STSTrack");
   if(NULL == fTrackSeedsArray) 
      TObject::Fatal("CbmLitMuchTrackFinderBranch::Init","no STS track array");
       
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fPropagatorToDet = fPropagator = factory->CreateTrackPropagator("Much");
   fFilter = factory->CreateTrackUpdate("Much");
   fSeedSelection = factory->CreateTrackSelection("MomentumSeed");
   fStationGroupSelection = factory->CreateTrackSelection("Momentum");
   fFinalSelection = factory->CreateTrackSelection("MuchFinal");
   fFinalPreSelection = factory->CreateTrackSelection("Empty");
   fFitter = factory->CreateTrackFitter("Much");
 
   fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();
   fVerbose = 1;
   fNofIter = 1;
   fBeginStationGroup = 0;
   fEndStationGroup = fLayout.GetNofStationGroups() - 1;
   fMaxNofMissingHitsInStationGroup = 0;
   fMaxNofMissingHits = 0;
   fSigmaCoef = 3.;
   fPDG = 13;
}

Int_t CbmLitMuchTrackFinderBranch::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;
	
	CbmLitConverter::MuchHitArrayToHitVector(hitArray, hits);
	CreateTrackSeeds(fTrackSeedsArray, trackSeeds);
	
	CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);
	
	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);
	
	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();	
	
	return trackArray->GetEntriesFast();
}

void CbmLitMuchTrackFinderBranch::CreateTrackSeeds(
		TClonesArray* trackArray,
		TrackPtrVector& trackSeeds)
{
	CbmLitConverter::StsTrackArrayToTrackVector(trackArray, trackSeeds);
    
    if (fVerbose > 1) 
      std::cout << "-I- CbmMuchTrackFinderNew::CreateTrackSeeds: " << std::endl
                << trackArray->GetEntriesFast() << " tracks were loaded, "
                << trackSeeds.size() << " tracks were created" << std::endl;
    
    Double_t Ze = fLayout.GetSubstation(0, 0, 0).GetZ();
    for (TrackPtrIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {   	
    	CbmLitTrackParam par = *(*iTrack)->GetParamLast();
       fPropagatorToDet->Propagate(&par, Ze, fPDG);
       (*iTrack)->SetParamLast(&par);
       (*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
       (*iTrack)->SetChi2(0.);
    }

    if (fVerbose > 1)
       std::cout << "-I- CbmLitTrackFinderStsNew::CreateTrackSeeds: "
                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitMuchTrackFinderBranch);
