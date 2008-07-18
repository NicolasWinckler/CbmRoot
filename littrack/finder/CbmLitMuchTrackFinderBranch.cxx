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


CbmLitMuchTrackFinderBranch::CbmLitMuchTrackFinderBranch()
{
	
}

CbmLitMuchTrackFinderBranch::~CbmLitMuchTrackFinderBranch()
{
	if (fPropagatorToDet) delete fPropagatorToDet;
	if (fPropagator) delete fPropagator;
	if (fFilter) delete fFilter;
	if (fFitter) delete fFitter;
	if (fTrackSeedSelection) delete fTrackSeedSelection;
	if (fTrackSelectionStation) delete fTrackSelectionStation;
	if (fTrackSelectionFinal) delete fTrackSelectionFinal;
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
   fTrackSeedSelection = factory->CreateTrackSelection("Momentum");
   fTrackSeedSelection->Properties().SetProperty("fMinMomentum", 0.1);
   fTrackSelectionStation = factory->CreateTrackSelection("Momentum");
   fTrackSelectionStation->Properties().SetProperty("fMinMomentum", 0.1);
   fTrackSelectionFinal = factory->CreateTrackSelection("Much");
   fFitter = factory->CreateTrackFitter("Much");
 
   fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();
   SetDetectorLayout(fLayout);
   SetPropagator(fPropagator);
   SetTrackSeedSelection(fTrackSeedSelection);
   SetTrackSelectionStation(fTrackSelectionStation);
   SetTrackSelectionFinal(fTrackSelectionFinal);
   SetFilter(fFilter); 
   SetFitter(fFitter); 
   SetVerbose(1);
   SetNofIter(1); 
   SetBeginStation(0); 
   SetEndStation(fLayout.GetNofStations() - 1);
   SetMaxNofMissingHitsInStation(0);
   SetMaxNofMissingHits(0);
   SetSigmaCoef(3.5); 
   SetPrecalcSearchRegions(false); 
   SetApplyUpdateInLayer(true);
   //SetSigmaX(sigmaX);
   //SetSigmaY(sigmaY);
}

Int_t CbmLitMuchTrackFinderBranch::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitVector hits;
	TrackVector trackSeeds;
	TrackVector foundTracks;
	
	CbmLitConverter::TrkHitArrayToHitVector(hitArray, hits);
	CreateTrackSeeds(fTrackSeedsArray, trackSeeds);
	
	CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);
	
	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);
	
	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();	
}

void CbmLitMuchTrackFinderBranch::CreateTrackSeeds(
		TClonesArray* trackArray,
		TrackVector& trackSeeds)
{
	CbmLitConverter::StsTrackArrayToTrackVector(trackArray, trackSeeds);
    
    if (fVerbose > 1) 
      std::cout << "-I- CbmMuchTrackFinderNew::CreateTrackSeeds: " << std::endl
                << trackArray->GetEntriesFast() << " tracks were loaded, "
                << trackSeeds.size() << " tracks were created" << std::endl;
    
    Double_t Ze = fLayout.GetLayerZ(0);
    for (TrackIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {   	
    	CbmLitTrackParam par = *(*iTrack)->GetParamLast();
       fPropagatorToDet->Propagate(&par, Ze);
       (*iTrack)->SetParamLast(&par);
       (*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
       (*iTrack)->SetChi2(0.);
    }

    if (fVerbose > 1)
       std::cout << "-I- CbmLitTrackFinderStsNew::CreateTrackSeeds: "
                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitMuchTrackFinderBranch);
