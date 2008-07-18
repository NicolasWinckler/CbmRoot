 
// -------------------------------------------------------------------------
// -----                  CbmLitTrdTrackFinderMuch source file               -----
// -----                  Created 29/08/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 
#include "CbmLitTrdTrackFinderMuch.h"

#include "base/CbmTrackPropagator.h"
#include "base/CbmTrackUpdate.h"
#include "base/CbmTrackExtrapolator.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitParabolicTrackExtrapolator.h"
#include "CbmLitEnvironment.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmBaseParSet.h"
#include "CbmRuntimeDb.h"
#include "CbmDetector.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmMuchTrack.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"

#include <iostream>
#include <set>

// ----------------------- Default constructor ---------------------------
CbmLitTrdTrackFinderMuch::CbmLitTrdTrackFinderMuch()
{
   //fIsStandalone = false;
}
// -----------------------------------------------------------------------


// --------------------------- Destructor --------------------------------
CbmLitTrdTrackFinderMuch::~CbmLitTrdTrackFinderMuch()
{

   delete fPropagator;
   delete fExtrapolator;
   delete fPropagatorToDet;
   delete fExtrapolatorToDet;
   delete fFilter;
}
// -----------------------------------------------------------------------



// ------------------------- Initialisation ------------------------------
void CbmLitTrdTrackFinderMuch::Init()
{
   // Activate data branches

   CbmRootManager* rootMgr = CbmRootManager::Instance();
   if(NULL == rootMgr) 
      TObject::Fatal("CbmLitTrdTrackFinderMuch::Init", "ROOT manager is not instantiated");
   
   // Get pointer to the Much track array
   fSeedTracksArray = (TClonesArray*) rootMgr->GetObject("MuchTrack");
   if(NULL == fSeedTracksArray) 
      TObject::Fatal("CbmLitTrdTrackFinderMuch::Init", "no Much track array");
   
   fExtrapolatorToDet = new CbmLitRK4TrackExtrapolator();
   fPropagatorToDet = new CbmLitTrackPropagator(fExtrapolatorToDet);
   if(NULL == fPropagatorToDet) 
      TObject::Fatal("CbmLitTrdTrackFinderMuch::Init", "propagator is not initialized");
      
   fPropagatorToDet->Properties().SetProperty("fMass",0.105);
   fPropagatorToDet->Properties().SetProperty("fApplyEnergyLoss", false);
   fPropagatorToDet->Properties().SetProperty("fEnergyLoss", 0.003);
   fPropagatorToDet->Properties().SetProperty("fFms", 1.1);
   fPropagatorToDet->Initialize();
   
   fExtrapolator = new CbmLitLineTrackExtrapolator();
   fPropagator = new CbmLitTrackPropagator(fExtrapolator);
   if(NULL == fPropagator) 
       TObject::Fatal("CbmLitTrdTrackFinderMuch::Init", " propagator is not initialized");
   
   fPropagator->Properties().SetProperty("fMass",0.105);
   fPropagator->Properties().SetProperty("fApplyEnergyLoss", false);
   fPropagator->Properties().SetProperty("fEnergyLoss", 0.003);
   fPropagator->Properties().SetProperty("fFms", 1.1);
   fPropagator->Initialize();

   
   fFilter = new CbmLitKalmanFilter();
   if(NULL == fFilter) 
      TObject::Fatal("CbmLitTrdTrackFinderMuch::Init", "filter is not initialized"); 
   fFilter->Initialize();
   
   // Determine the TRD geometry
   ReadDetectorGeometry();

   fHits.clear();
   fHits.resize(fNofLayers);
}
// -----------------------------------------------------------------------
// ------------------- Determines the TRD geometry ------------------------
void CbmLitTrdTrackFinderMuch::ReadDetectorGeometry()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   
   env->GetTrdLayout(fNofStations, fNofLayers, fNofLayersPerStation, fLayerZPos);

   fMaxErrX.resize(fNofLayers);
   fMaxErrY.resize(fNofLayers);
}
// -----------------------------------------------------------------------


// -------------------- Algorithm implementation -------------------------
Int_t CbmLitTrdTrackFinderMuch::DoFind(TClonesArray* hitArray,
                                      TClonesArray* trackArray)
{
  // return 0;
   
   std::cout << "==============================================================================" << std::endl;
   std::cout << "-I- CbmLitTrdTrackFinderMuch::DoFind: ACTIVE" << std::endl;
   
   
   fHitArray = hitArray;

   fTracks.clear();
   fFoundTracks.clear();
   fSeedsIdSet.clear();

   fApplyUpdateInLayer = false;
   fPrecalcSearchRegions = false;
   fNofSharedHits = 0;
   
   fUsedHitsSet.clear();

   fHits.clear();
   fHits.resize(fNofLayers);
   
   ArrangeHits();

   CreateTracksFromSeeds();

   ExtrapolateToDetector();
 
   
   fNofIter = 4;
   
   for (Int_t iIter = 0; iIter < fNofIter; iIter++) {
       
      if (fVerbose > 1) {
         std::cout << "-I- CbmLitTrdTrackFinderMuch::DoFind: "
              << "iteration " << iIter << std::endl;
      }
   
      //fHits.resize(fNofTrdLayers);
      //fHitIdMap.clear();
      
      if (iIter == 0) SetIterPar(3, 0, fNofStations - 1, 0);
      if (iIter == 1) SetIterPar(5, 0, fNofStations - 2, 0);
      if (iIter == 2) SetIterPar(5, 1, fNofStations - 1, 0);
      if (iIter == 3) SetIterPar(5, 0, fNofStations - 1, 1);
    
            
      if (iIter != 0) ArrangeHits();
      
      InitTracks();

      TrackFollowing();
     
      RemoveHits();

      CopyToOutput(trackArray);
   
    //  for(UInt_t i = 0; i < fTrdTracks.size(); i++)
    //       delete fTrdTracks[i];
    //  fTrdTracks.clear();

      for (Int_t i = 0; i < fNofLayers; i++) 
      fHits[i].clear();
      //fHits.clear();
   }

   fUsedHitsSet.clear();
   fHits.clear();
   
   for(std::vector<CbmLitTrack*>::iterator iTrack = fTracksS.begin();
      iTrack != fTracksS.end(); iTrack++) {
      delete (*iTrack);
   }
   fTracksS.clear();

   std::cout << "-I- CbmLitTrdTrackFinderMuch::DoFind : "
        << trackArray->GetEntriesFast() << " TRD tracks found." << std::endl;

   fEvents++;

   std::cout << "-I- CbmLitTrdTrackFinderMuch : "
        << fEvents << " events processed" << std::endl;
        
   std::cout << "--------------------------------------------------------" << std::endl;

   return trackArray->GetEntriesFast();
}
// -----------------------------------------------------------------------

// ------------Set iteration parameters-----------------------------------
void CbmLitTrdTrackFinderMuch::SetIterPar(
                               Double_t SigmaCoef, 
                               Int_t beginStation, 
                               Int_t endStation,
                               Int_t maxNofMissingHitsInStation) 
{
   fSigmaCoef = SigmaCoef;
   fBeginStation = beginStation;
   fEndStation= endStation;
   fMaxNofMissingHitsInStation = maxNofMissingHitsInStation;
   
}
// -----------------------------------------------------------------------

// --------------Creates Trd tracks from Sts tracks-----------------------
void CbmLitTrdTrackFinderMuch::CreateTracksFromSeeds()
{

    Int_t nofMuchTracks = fSeedTracksArray->GetEntriesFast();

    for(Int_t iMuchTrack = 0; iMuchTrack < nofMuchTracks; iMuchTrack++) {
       
       CbmMuchTrack* muchTrack = (CbmMuchTrack*) fSeedTracksArray->At(iMuchTrack);
   
       if(muchTrack == NULL) continue;
       
       CbmLitTrack* track = new CbmLitTrack();
       //track->MapFromSts(stsTrack);
       track->SetPreviousTrackId(iMuchTrack);
       track->SetParamLast(muchTrack->GetMuchTrack());
       track->SetChi2(muchTrack->GetChi2());
       track->SetNDF(1);
       fTracksS.push_back(track);
       track = NULL; ///?!?!?
    }

    if (fVerbose > 1) {
      std::cout << "-I- CbmLitTrdTrackFinderMuch::CreateTrdFromMuch : " << std::endl
            << nofMuchTracks << " MUCH tracks were loaded, "
            << fTracksS.size() << " TRD tracks were created" << std::endl;
    }
}
// -----------------------------------------------------------------------

// --------------------Copy to output TClonesArray------------------------
void CbmLitTrdTrackFinderMuch::CopyToOutput(TClonesArray* trackArray)
{
   Int_t nofTracks = trackArray->GetEntriesFast();
   for(std::vector<CbmLitTrack*>::iterator iTrack = fFoundTracks.begin();
         iTrack != fFoundTracks.end(); iTrack++) {
      if( (*iTrack)->GetFlag() == 0) {
         //if (fSeedsIdSet.find((*iTrack)->GetPreviousTrackId()) == fSeedsIdSet.end()) {
                 
            new ((*trackArray)[nofTracks]) CbmTrdTrack((*iTrack)->MapToTrd());
                                    
            fSeedsIdSet.insert((*iTrack)->GetPreviousTrackId());
            
            nofTracks++;
         //}
      }
      delete (*iTrack);
   }
   
   //clear array
   fFoundTracks.clear();
}
// -----------------------------------------------------------------------

ClassImp(CbmLitTrdTrackFinderMuch);

