#include "CbmLitMuchTrackFinderRobust.h"

#include "CbmLitTrack.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitTrackSelectionD.h"

#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <vector>
#include <iostream>

CbmLitMuchTrackFinderRobust::CbmLitMuchTrackFinderRobust()
{
	
}

CbmLitMuchTrackFinderRobust::~CbmLitMuchTrackFinderRobust()
{
	if (fPropagatorToDet) delete fPropagatorToDet;
}

void CbmLitMuchTrackFinderRobust::Init()
{
   CbmRootManager* rootMgr = CbmRootManager::Instance();
   if(NULL == rootMgr) 
      TObject::Fatal("CbmLitMuchTrackFinderRobust::Init","ROOT manager is not instantiated");

   fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("STSTrack");
   if(NULL == fTrackSeedsArray) 
      TObject::Fatal("CbmLitMuchTrackFinderRobust::Init","no STS track array");
     
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fPropagatorToDet = fPropagator = factory->CreateTrackPropagator("Much");
   
   fSeedSelection = factory->CreateTrackSelection("MomentumSeed");
   fFinalSelection = factory->CreateTrackSelection("MuchRobust");
	
   fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();
   
   fVerbose = 1;
   fNofIter = 1; 
   fMaxNofMissingHits = 1;
   fSigmaCoef = 3.5; 
   fPDG = 13;
}

Int_t CbmLitMuchTrackFinderRobust::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;
	
	CbmLitConverter::TrkHitArrayToPixelHitVector(hitArray, hits);
	CreateTrackSeeds(fTrackSeedsArray, trackSeeds);
	
	CbmLitTrackFinderRobust::DoFind(hits, trackSeeds, foundTracks);
	
	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);
	
	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();	
	
	return trackArray->GetEntriesFast();
}

void CbmLitMuchTrackFinderRobust::CreateTrackSeeds(
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
       (*iTrack)->SetChi2( 0.0 );
    }

    if (fVerbose > 1)
       std::cout << "-I- CbmLitTrackFinderStsNew::CreateTrackSeeds: "
                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitMuchTrackFinderRobust);
