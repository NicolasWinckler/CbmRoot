/**
 * \file CbmLitFitTracksParallel.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitFitTracksParallel.h"
#include "FairRootManager.h"

#include <cassert>

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

ClassImp(CbmLitFitTracksParallel);
