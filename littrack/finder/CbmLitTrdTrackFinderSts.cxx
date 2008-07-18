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
#include "CbmLitTrackSelection.h"
#include "CbmLitTrackSelectionTrd.h"
#include "CbmLitTrackSelectionEmpty.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"
#include "CbmLitTrackSelectionMuch.h"

#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <vector>

CbmLitTrdTrackFinderSts::CbmLitTrdTrackFinderSts()
{

}

CbmLitTrdTrackFinderSts::~CbmLitTrdTrackFinderSts()
{
	//delete fTrackFinder;
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
   fPropagatorToDet->Properties().SetProperty("fMass", 0.139);
   fPropagatorToDet->Properties().SetProperty("fApplyEnergyLoss", true);
   //fPropagatorToDet->Properties().SetProperty("fApplyElectronEnergyLoss", true);
   fPropagatorToDet->Properties().SetProperty("fEnergyLoss", 0.00354);
   fPropagatorToDet->Properties().SetProperty("fFms", 1.1);
   fPropagatorToDet->Initialize();
      
   fExtrapolator = new CbmLitLineTrackExtrapolator();
   fPropagator = new CbmLitTrackPropagatorImp(fExtrapolator);
   fPropagator->Properties().SetProperty("fMass", 0.139);
   fPropagator->Properties().SetProperty("fApplyEnergyLoss", true);
   //fPropagator->Properties().SetProperty("fApplyElectronEnergyLoss", true);
   fPropagator->Properties().SetProperty("fEnergyLoss", 0.00354);
   fPropagator->Properties().SetProperty("fFms", 1.1);
   fPropagator->Initialize();
   
   fFilter = new CbmLitKalmanFilter();
   fFilter->Initialize();
   
   fFitter = new CbmLitTrackFitterImp(fPropagator, fFilter);
   fFitter->Initialize();
   
   //CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   //fTrackSelectionStation = factory->CreateTrackSelection("Much");
 
   fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();
   
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fTrackSeedSelection = factory->CreateTrackSelection("Momentum");
   fTrackSeedSelection->Properties().SetProperty("fMinMomentum", 0.1);
   
   //fTrackSelectionStation = new CbmLitTrackSelectionEmpty();    
   fTrackSelectionStation = new CbmLitTrackSelectionTrd();
   fTrackSelectionStation->Properties().SetProperty("fNofSharedHits", 2);
   fTrackSelectionStation->Properties().SetProperty("fMinNofHits", 1);
   fTrackSelectionStation->Properties().SetProperty("fMinLastPlaneId", 0);
   fTrackSelectionStation->Initialize();
      
   fTrackSelectionFinal = new CbmLitTrackSelectionTrd();
   fTrackSelectionFinal->Properties().SetProperty("fNofSharedHits", 2);
   fTrackSelectionFinal->Properties().SetProperty("fMinNofHits", 1);
   fTrackSelectionFinal->Properties().SetProperty("fMinLastPlaneId", fLayout.GetNofStations()-1);
   fTrackSelectionFinal->Initialize();

   CbmLitTrackFinderImp::SetDetectorLayout(fLayout);
   CbmLitTrackFinderImp::SetPropagator(fPropagator);
   CbmLitTrackFinderImp::SetFilter(fFilter); 
   CbmLitTrackFinderImp::SetFitter(fFitter);    
   CbmLitTrackFinderImp::SetTrackSeedSelection(fTrackSeedSelection);
   CbmLitTrackFinderImp::SetTrackSelectionStation(fTrackSelectionStation);
   CbmLitTrackFinderImp::SetTrackSelectionFinal(fTrackSelectionFinal);
   CbmLitTrackFinderImp::SetVerbose(3);
   CbmLitTrackFinderImp::SetNofIter(1); 
   CbmLitTrackFinderImp::SetBeginStation(0); 
   CbmLitTrackFinderImp::SetEndStation(fLayout.GetNofStations() - 1);
   CbmLitTrackFinderImp::SetPrecalcSearchRegions(false); 
}

void CbmLitTrdTrackFinderSts::SetIterationParameters(Int_t iter)
{
	if (iter == 0) {
		CbmLitTrackFinderImp::SetMaxNofMissingHitsInStation(0);
		CbmLitTrackFinderImp::SetMaxNofMissingHits(0);
		CbmLitTrackFinderImp::SetSigmaCoef(5.); 
		CbmLitTrackFinderImp::SetApplyUpdateInLayer(true);
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
    	fPropagatorToDet->Propagate(&par, Ze);
    	(*iTrack)->SetParamLast(&par);
       (*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
       (*iTrack)->SetChi2( 0.0 );
    }

    if (fVerbose > 1) 
       std::cout << "-I- CbmLitTrackFinderStsNew::CreateTrackSeeds: "
                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitTrdTrackFinderSts);
