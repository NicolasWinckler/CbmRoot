#include "finder/CbmLitFindMuchTracksEpoch.h"

#include "base/CbmLitToolFactory.h"
#include "base/CbmLitEnvironment.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmStsHit.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>

CbmLitFindMuchTracksEpoch::CbmLitFindMuchTracksEpoch():
   fStsTracks(NULL),
   fStsHits(NULL),
   fMuchPixelHits(NULL),
   fMuchTracks(NULL),
   fGlobalTracks(NULL),
   fLitStsTracks(),
   fLitHits(),
   fLitOutputTracks(),
   fFinder(),
   fMerger(),
   fFitter(),
   fTrackingType("branch"),
   fMergerType("nearest_hit"),
   fFitterType("lit_kalman"),
   fEventNo(0)
{}

CbmLitFindMuchTracksEpoch::~CbmLitFindMuchTracksEpoch(){}

InitStatus CbmLitFindMuchTracksEpoch::Init(){
   ReadAndCreateDataBranches();
   InitTrackReconstruction();
   return kSUCCESS;
}

void CbmLitFindMuchTracksEpoch::Exec(Option_t* opt){
  printf("MuchTracks\n");
  fMuchTracks->Clear();
  printf("GlobalTracks\n");
  fGlobalTracks->Clear();
  printf("Converter1\n");
  CbmLitConverter::StsTrackArrayToTrackVector(fStsTracks, fLitStsTracks);
  printf("Converter2\n");
  CbmLitConverter::HitArrayToHitVector(fMuchPixelHits, fLitHits);
  printf("Reco\n");
  RunTrackReconstruction();
  printf("Converter3\n");
  CbmLitConverter::LitTrackVectorToGlobalTrackArray(fLitOutputTracks, fGlobalTracks, fStsTracks,NULL, fMuchTracks);
  ClearArrays();
  printf("Event: %i\n",fEventNo++);
}

void CbmLitFindMuchTracksEpoch::SetParContainers(){
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("FairBaseParSet");
   rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitFindMuchTracksEpoch::Finish(){}

void CbmLitFindMuchTracksEpoch::ReadAndCreateDataBranches(){
  FairRootManager* ioman = FairRootManager::Instance();
  fStsTracks     = (TClonesArray*) ioman->GetObject("StsTrack");
  fStsHits       = (TClonesArray*) ioman->GetObject("StsHit");
  fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
  fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
  fMuchTracks = new TClonesArray("CbmMuchTrack", 100);
  ioman->Register("MuchTrack", "Much", fMuchTracks, kTRUE);
  ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);
}

void CbmLitFindMuchTracksEpoch::InitTrackReconstruction(){
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "weight" || fTrackingType == "nn_parallel") {
     std::string st("mu_");
     st += fTrackingType;
     fFinder = factory->CreateTrackFinder(st);
   } else {
     TObject::Fatal("CbmLitFindMuchTracksEpoch","Tracking type not found");
   }

   if (fFitterType == "lit_kalman") fFitter = factory->CreateTrackFitter("lit_kalman");
   else  TObject::Fatal("CbmLitFindMuchTracksEpoch","Fitter type not found");
}

void CbmLitFindMuchTracksEpoch::ClearArrays(){
   // Free memory
   for_each(fLitStsTracks.begin(), fLitStsTracks.end(), DeleteObject());
   for_each(fLitOutputTracks.begin(), fLitOutputTracks.end(), DeleteObject());
   for_each(fLitHits.begin(), fLitHits.end(), DeleteObject());
   fLitStsTracks.clear();
   fLitOutputTracks.clear();
   fLitHits.clear();
}

void CbmLitFindMuchTracksEpoch::RunTrackReconstruction(){
  fFinder->DoFind(fLitHits, fLitStsTracks, fLitOutputTracks);
   // Refit found tracks
   for(TrackPtrIterator it = fLitOutputTracks.begin(); it != fLitOutputTracks.end(); it++) {
      CbmLitTrack* track = *it;
      fFitter->Fit(track);
   }
}

ClassImp(CbmLitFindMuchTracksEpoch);
