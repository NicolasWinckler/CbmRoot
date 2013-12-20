/**
 * \file CbmLitFindMvdTracks.cxx
 * \brief MVD tracking based on littrack package.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFindMvdTracks.h"
#include "base/CbmLitTrackingGeometryConstructor.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "finder/CbmLitTrackFinderNN.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"

#include "CbmStsTrack.h"
#include "FairHit.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>
#include <algorithm>

CbmLitFindMvdTracks::CbmLitFindMvdTracks():
   fDet(),
   fStsTracks(NULL),
   fMvdHits(NULL),
   fLitStsTracks(),
   fLitMvdHits(),
   fLitOutputTracks(),
   fFinder(),
   fEventNo(0)
{

}

CbmLitFindMvdTracks::~CbmLitFindMvdTracks()
{

}

InitStatus CbmLitFindMvdTracks::Init()
{
   fDet.DetermineSetup();
   std::cout << fDet.ToString();

   ReadAndCreateDataBranches();

   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fFinder = factory->CreateTrackFinder("mvd_nn");

   return kSUCCESS;
}

void CbmLitFindMvdTracks::Exec(
   Option_t* opt)
{
   ConvertInputData();
   RunTrackReconstruction();
   ConvertOutputData();
   ClearArrays();
   std::cout << "-I- Event: " << fEventNo++ << std::endl;
}

void CbmLitFindMvdTracks::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("FairBaseParSet");
}

void CbmLitFindMvdTracks::Finish()
{

}

void CbmLitFindMvdTracks::ReadAndCreateDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("Init","CbmRootManager is not instantiated"); }

   // MVD data
   if (fDet.GetDet(kMVD)) {
      fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
      if (NULL == fMvdHits) { Fatal("Init", "No MvdHit array!"); }
   }

   //STS data
   if (fDet.GetDet(kSTS)) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("Init","No StsTrack array!"); }
   }
}

void CbmLitFindMvdTracks::ConvertInputData()
{
   CbmLitConverter::StsTrackArrayToTrackVector(fStsTracks, fLitStsTracks);
   // Change last and first parameters of the track seeds
   for(Int_t iTrack = 0; iTrack < fLitStsTracks.size(); iTrack++) {
      CbmLitTrack* track = fLitStsTracks[iTrack];
      const CbmLitTrackParam* parFirst = track->GetParamFirst();
      track->SetParamLast(parFirst);
   }
   std::cout << "-I- Number of STS tracks: " << fLitStsTracks.size() << std::endl;

   CbmLitConverter::MvdHitArrayToHitVector(fMvdHits, fLitMvdHits);
   // Make reverse order of the hits
   Int_t nofStations = CbmLitTrackingGeometryConstructor::Instance()->GetNofMvdStations();
   for(Int_t iHit = 0; iHit < fLitMvdHits.size(); iHit++) {
      CbmLitHit* hit = fLitMvdHits[iHit];
      hit->SetDetectorId(kLITMVD, nofStations - hit->GetStation() - 1);
   }
   std::cout << "-I- Number of MVD hits: " << fLitMvdHits.size() << std::endl;
}

void CbmLitFindMvdTracks::ConvertOutputData()
{
   for(Int_t iTrack = 0; iTrack < fLitOutputTracks.size(); iTrack++) {
      CbmLitTrack* litTrack = fLitOutputTracks[iTrack];
      Int_t trackId = litTrack->GetPreviousTrackId();
      CbmStsTrack* track = static_cast<CbmStsTrack*>(fStsTracks->At(trackId));
      for (Int_t iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
         const CbmLitHit* litHit = litTrack->GetHit(iHit);
         Int_t refId = litHit->GetRefId();
         FairHit* hit = static_cast<FairHit*>(fMvdHits->At(refId));
         track->AddMvdHit(refId);
      }
      //track->SortMvdHits();

      FairTrackParam parFirst;
      CbmLitConverter::CbmLitTrackParamToFairTrackParam(litTrack->GetParamLast(), &parFirst);
      track->SetParamFirst(&parFirst);
   }
}

void CbmLitFindMvdTracks::ClearArrays()
{
   // Free memory
   for_each(fLitStsTracks.begin(), fLitStsTracks.end(), DeleteObject());
   for_each(fLitMvdHits.begin(), fLitMvdHits.end(), DeleteObject());
   for_each(fLitOutputTracks.begin(), fLitOutputTracks.end(), DeleteObject());
   fLitStsTracks.clear();
   fLitMvdHits.clear();
   fLitOutputTracks.clear();
}

void CbmLitFindMvdTracks::RunTrackReconstruction()
{
   fFinder->DoFind(fLitMvdHits, fLitStsTracks, fLitOutputTracks);
}
