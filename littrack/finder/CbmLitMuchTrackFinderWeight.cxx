#include "CbmLitMuchTrackFinderRobust.h"

#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitTrack.h"

#include "TClonesArray.h"

#include <iostream>
#include <algorithm>

CbmLitMuchTrackFinderRobust::CbmLitMuchTrackFinderRobust()
{
}

CbmLitMuchTrackFinderRobust::~CbmLitMuchTrackFinderRobust()
{
}

void CbmLitMuchTrackFinderRobust::Init()
{
	DefaultInit();

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagatorToDet =fPropagator = factory->CreateTrackPropagator("lit");
	fSeedSelection = factory->CreateTrackSelection("momentum_seed");
	fFinalSelection = factory->CreateTrackSelection("empty");
	fFitter = factory->CreateTrackFitter("kalman_robust");
	fFilter = factory->CreateTrackUpdate("kalman");

	fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();

	fVerbose = 1;
	fNofIter = 1;
	fMaxNofMissingHits = 1;
	fUseFastSearch = true;
	fSigmaCoef = 3.5;
	fChiSqPixelHitCut = 13.86;
	fChiSqStripHitCut = 4.;
	fPDG = 13;
}

Int_t CbmLitMuchTrackFinderRobust::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::MuchHitArrayToHitVector(hitArray, hits);
	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

	CbmLitTrackFinderRobust::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

ClassImp(CbmLitMuchTrackFinderRobust);
