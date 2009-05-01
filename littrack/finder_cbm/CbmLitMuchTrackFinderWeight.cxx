#include "CbmLitMuchTrackFinderWeight.h"

#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitTrack.h"

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

	SetLayout(CbmLitEnvironment::Instance()->GetMuchLayout());

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
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::MuchHitArrayToHitVector(hitArray, hits);
	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

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
