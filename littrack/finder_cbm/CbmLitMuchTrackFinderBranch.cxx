#include "finder_cbm/CbmLitMuchTrackFinderBranch.h"

#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitEnvironment.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "interface/CbmLitTrackFitter.h"
#include "interface/CbmLitTrackSelection.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitConverter.h"

#include "TClonesArray.h"

#include <vector>
#include <iostream>
#include <algorithm>

CbmLitMuchTrackFinderBranch::CbmLitMuchTrackFinderBranch()
{
}

CbmLitMuchTrackFinderBranch::~CbmLitMuchTrackFinderBranch()
{
}

void CbmLitMuchTrackFinderBranch::Init()
{
   DefaultInit();

   SetLayout(CbmLitEnvironment::Instance()->GetLayout());

   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   TrackPropagatorPtr propagator = factory->CreateTrackPropagator("lit");
   SetPropagatorToDet(propagator);
   SetPropagator(propagator);
   SetFilter(factory->CreateTrackUpdate("kalman"));
   SetSeedSelection(factory->CreateTrackSelection("momentum_seed"));
   SetStationGroupSelection(factory->CreateTrackSelection("momentum"));
   SetFinalSelection(factory->CreateTrackSelection("much_final"));

   SetVerbose(1);
   SetNofIter(1);
   SetMaxNofMissingHits(2);
   IsAlwaysCreateMissingHit(false);
   IsUseFastSearch(true);
   SetSigmaCoef(3.5);
   SetChiSqPixelHitCut(13.86);
   SetChiSqStripHitCut(4.);
   SetPDG(13);
}

Int_t CbmLitMuchTrackFinderBranch::DoFind(
   TClonesArray* trackArray)
{
   HitPtrVector hits;
   TrackPtrVector trackSeeds;
   TrackPtrVector foundTracks;

// CbmLitConverter::HitArrayToHitVector(fHitsArray, hits);
// DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

   CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);

   CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);

   for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
   for_each(hits.begin(), hits.end(), DeleteObject());
   for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
   foundTracks.clear();
   hits.clear();
   trackSeeds.clear();

   return trackArray->GetEntriesFast();
}

ClassImp(CbmLitMuchTrackFinderBranch);

