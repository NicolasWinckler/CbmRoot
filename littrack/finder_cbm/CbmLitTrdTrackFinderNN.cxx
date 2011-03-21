#include "finder_cbm/CbmLitTrdTrackFinderNN.h"

#include "base/CbmLitToolFactory.h"
#include "base/CbmLitEnvironment.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"

#include "TClonesArray.h"

#include <algorithm>

CbmLitTrdTrackFinderNN::CbmLitTrdTrackFinderNN()
{
}

CbmLitTrdTrackFinderNN::~CbmLitTrdTrackFinderNN()
{
}

void CbmLitTrdTrackFinderNN::Init()
{
   DefaultInit();

   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   SetPropagator(factory->CreateTrackPropagator("lit"));
   SetSeedSelection(factory->CreateTrackSelection("empty"));//"momentum"));
   SetFinalSelection(factory->CreateTrackSelection("trd_final"));
   SetFilter(factory->CreateTrackUpdate("kalman"));
   SetLayout(CbmLitEnvironment::Instance()->GetLayout());
   SetVerbose(1);
   SetNofIter(1);
   IsUseFastSearch(true);
   SetMaxNofMissingHits(3);
   SetSigmaCoef(5.);
   SetChiSqPixelHitCut(25.);
   SetChiSqStripHitCut(4.);
   SetPDG(11);
   IsProcessSubstationsTogether(true);
}

Int_t CbmLitTrdTrackFinderNN::DoFind(
   TClonesArray* hitArray,
   TClonesArray* trackArray)
{
   HitPtrVector hits;
   TrackPtrVector trackSeeds;
   TrackPtrVector foundTracks;

   CbmLitConverter::HitArrayToHitVector(hitArray, hits);
   DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

   CbmLitTrackFinderNN::DoFind(hits, trackSeeds, foundTracks);

   CbmLitConverter::TrackVectorToTrdTrackArray(foundTracks, trackArray);

   for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
   for_each(hits.begin(), hits.end(), DeleteObject());
   for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
   foundTracks.clear();
   hits.clear();
   trackSeeds.clear();

   return trackArray->GetEntriesFast();
}

ClassImp(CbmLitTrdTrackFinderNN);
