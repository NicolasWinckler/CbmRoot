#include "CbmLitTrdTrackFinderSts.h"

#include "CbmLitTrackFinderImp.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"

#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <vector>

CbmLitTrdTrackFinderSts::CbmLitTrdTrackFinderSts()
{

}

CbmLitTrdTrackFinderSts::~CbmLitTrdTrackFinderSts()
{
	delete fExtrapolatorToDet;
	delete fPropagatorToDet;
	delete fPropagator;
	delete fExtrapolator;
	delete fFilter;
	delete fFitter;
	delete fTrackSelectionStation;
	delete fTrackSeedSelection;
	delete fTrackSelectionFinal;
}

void CbmLitTrdTrackFinderSts::Init()
{
   CbmRootManager* rootMgr = CbmRootManager::Instance();
   if(NULL == rootMgr) 
      TObject::Fatal("CbmLitTrdTrackFinderSts::Init", "ROOT manager is not instantiated"); 

   fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("STSTrack");
   if(NULL == fTrackSeedsArray)
      TObject::Fatal("CbmLitTrdTrackFinderSts::Init", "no STS track array");
   
   fExtrapolatorToDet = new CbmLitRK4TrackExtrapolator();
   fPropagatorToDet = new CbmLitTrackPropagatorImp(fExtrapolatorToDet);   
   fPropagatorToDet->Initialize();
      
   fExtrapolator = new CbmLitLineTrackExtrapolator();
   fPropagator = new CbmLitTrackPropagatorImp(fExtrapolator);
   fPropagator->Initialize();
   
   fFilter = new CbmLitKalmanFilter();
   fFilter->Initialize();
   
   fFitter = new CbmLitTrackFitterImp(fPropagator, fFilter);
   fFitter->Initialize();
      
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fTrackSeedSelection = factory->CreateTrackSelection("Momentum");
   fTrackSelectionStation = factory->CreateTrackSelection("TrdStation");
   fTrackSelectionFinal = factory->CreateTrackSelection("TrdFinal");
   
   fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();
   
   SetDetectorLayout(fLayout);
   SetPropagator(fPropagator);
   SetFilter(fFilter); 
   SetFitter(fFitter);    
   SetTrackSeedSelection(fTrackSeedSelection);
   SetTrackSelectionStation(fTrackSelectionStation);
   SetTrackSelectionFinal(fTrackSelectionFinal);
   SetVerbose(1);
   SetNofIter(1); 
   SetBeginStation(0); 
   SetEndStation(fLayout.GetNofStations() - 1);
//   SetPrecalcSearchRegions(false); 
   SetPDG(211);
}

void CbmLitTrdTrackFinderSts::SetIterationParameters(Int_t iter)
{
	if (iter == 0) {
		SetMaxNofMissingHitsInStation(1);
		SetMaxNofMissingHits(1);
		SetSigmaCoef(5.); 
//      SetApplyUpdateInLayer(true);
	} 
}

Int_t CbmLitTrdTrackFinderSts::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitVector hits;
	TrackVector trackSeeds;
	TrackVector foundTracks;
	
	CbmLitConverter::TrkHitArrayToHitVector(hitArray, hits);
	CreateTrackSeeds(fTrackSeedsArray, trackSeeds);
	
	CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);
	
	CbmLitConverter::TrackVectorToTrdTrackArray(foundTracks, trackArray);
	
	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();
	
	return trackArray->GetEntriesFast();
}

void CbmLitTrdTrackFinderSts::CreateTrackSeeds(
		TClonesArray* trackArray,
		TrackVector& trackSeeds)
{
	CbmLitConverter::StsTrackArrayToTrackVector(trackArray, trackSeeds);

    if (fVerbose > 1) 
      std::cout << "-I- CbmLitTrdTrackFinderSts::CreateTrackSeeds: " << std::endl
                << trackArray->GetEntriesFast() << " tracks were loaded, "
                << trackSeeds.size() << " tracks were created" << std::endl;
    
    Double_t Ze = fLayout.GetLayerZ(0);
    for (TrackIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {
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

ClassImp(CbmLitTrdTrackFinderSts);
