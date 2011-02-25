#include "finder_cbm/CbmLitMuchTrackFinderWeight.h"

#include "base/CbmLitEnvironment.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitConverter.h"

#include "TClonesArray.h"

#include <iostream>
#include <algorithm>

CbmLitMuchTrackFinderWeight::CbmLitMuchTrackFinderWeight()
{
}

CbmLitMuchTrackFinderWeight::~CbmLitMuchTrackFinderWeight()
{
}

void CbmLitMuchTrackFinderWeight::Init()
{
	DefaultInit();

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	TrackPropagatorPtr propagator = factory->CreateTrackPropagator("lit");
	SetPropagatorToDet(propagator);
	SetPropagator(propagator);
	SetSeedSelection(factory->CreateTrackSelection("momentum_seed"));
	SetFinalSelection(factory->CreateTrackSelection("empty"));
	SetFitter(factory->CreateTrackFitter("kalman_robust"));
	SetFilter(factory->CreateTrackUpdate("kalman"));

	SetLayout(CbmLitEnvironment::Instance()->GetLayout());

	SetVerbose(1);
	SetNofIter(1);
	SetMaxNofMissingHits(1);
	IsUseFastSearch(true);
	SetSigmaCoef(3.5);
	SetChiSqPixelHitCut(13.86);
	SetChiSqStripHitCut(4.);
	SetPDG(13);
}

Int_t CbmLitMuchTrackFinderWeight::DoFind(
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

//	CbmLitConverter::HitArrayToHitVector(fHitsArray, hits);
//	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

	CbmLitTrackFinderWeight::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

ClassImp(CbmLitMuchTrackFinderWeight);
