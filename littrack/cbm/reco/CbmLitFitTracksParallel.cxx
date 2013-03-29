/**
 * \file CbmLitFitTracksParallel.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitFitTracksParallel.h"
#include "cbm/base/CbmLitTrackingGeometryConstructor.h"
#include "cbm/utils/CbmLitConverterParallel.h"
#include "std/utils/CbmLitMemoryManagment.h"
#include "parallel/LitDetectorLayout.h"
#include "parallel/LitScalTrack.h"
#include "parallel/LitScalPixelHit.h"
#include "parallel/LitTrackFitter.h"
#include "FairRootManager.h"
#include "CbmTrack.h"
#include "CbmStsTrack.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

using std::vector;
using std::for_each;

CbmLitFitTracksParallel::CbmLitFitTracksParallel():
   FairTask(),
   fGlobalTracks(NULL),
   fStsTracks(NULL),
   fMuchTracks(NULL),
   fTrdTracks(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fTrdHits(NULL)
{

}

CbmLitFitTracksParallel::~CbmLitFitTracksParallel()
{

}

InitStatus CbmLitFitTracksParallel::Init()
{
   ReadDataBranches();



   return kSUCCESS;
}

void CbmLitFitTracksParallel::Exec(
    Option_t* opt)
{
   static Int_t eventNo = 0;
   std::cout << "CbmLitFitTracksParallel::Exec : eventNo=" << eventNo++ << std::endl;

   DoFit();
}

void CbmLitFitTracksParallel::Finish()
{

}

void CbmLitFitTracksParallel::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
}

void CbmLitFitTracksParallel::DoFit()
{
   static Bool_t firstTime = true;
   static lit::parallel::LitDetectorLayoutScal layout;
   if (firstTime) {
      CbmLitTrackingGeometryConstructor::Instance()->GetTrdLayoutScal(layout);
      firstTime = false;
   }

   // Convert input data
   vector<lit::parallel::LitScalTrack*> ltracks;
   vector<lit::parallel::LitScalPixelHit*> lhits;
   CbmLitConverterParallel::CbmPixelHitArrayToLitScalPixelHitArray(fTrdHits, lhits);
   CbmLitConverterParallel::CbmTrackArrayToLitScalTrackArray(fTrdTracks, lhits, ltracks);

   // Replace first track parameter of the converted tracks with the last STS track parameter
   Int_t nofTracks = ltracks.size();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack& track = *ltracks[iTrack];
      Int_t stsTrackId = track.GetPreviousTrackId();
      CbmStsTrack* stsTrack = static_cast<CbmStsTrack*>(fStsTracks->At(stsTrackId));
      lit::parallel::LitTrackParamScal lpar;
      CbmLitConverterParallel::FairTrackParamToLitTrackParamScal(stsTrack->GetParamLast(), &lpar);
      track.SetParamFirst(lpar);
   }

  // Int_t nofTracks = ltracks.size();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack& track = *ltracks[iTrack];
      LitTrackFitter(track, layout);
   }

   // Replace first and last track parameters for the fitted track
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack& track = *ltracks[iTrack];
      CbmTrack* trdTrack = static_cast<CbmTrack*>(fTrdTracks->At(iTrack));
      FairTrackParam firstParam, lastParam;
      CbmLitConverterParallel::LitTrackParamScalToFairTrackParam(&track.GetParamFirst(), &firstParam);
      CbmLitConverterParallel::LitTrackParamScalToFairTrackParam(&track.GetParamLast(), &lastParam);
      trdTrack->SetParamFirst(&firstParam);
      trdTrack->SetParamLast(&lastParam);
   }

   // Clear memory
   for_each(ltracks.begin(), ltracks.end(), DeleteObject());
   for_each(lhits.begin(), lhits.end(), DeleteObject());
   ltracks.clear();
   lhits.clear();
}

ClassImp(CbmLitFitTracksParallel);
