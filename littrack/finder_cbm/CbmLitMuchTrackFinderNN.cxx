#include "CbmLitMuchTrackFinderNN.h"
#include "CbmLitToolFactory.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"

#include "TClonesArray.h"

#include <algorithm>

CbmLitMuchTrackFinderNN::CbmLitMuchTrackFinderNN()
{
}

CbmLitMuchTrackFinderNN::~CbmLitMuchTrackFinderNN()
{
}

void CbmLitMuchTrackFinderNN::Init()
{
	DefaultInit();

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	TrackPropagatorPtr propagator = factory->CreateTrackPropagator("lit");
	SetPropagatorToDet(propagator);
	SetPropagator(propagator);
	SetSeedSelection(factory->CreateTrackSelection("momentum_seed"));
	SetFinalSelection(factory->CreateTrackSelection("much_final"));
	SetFilter(factory->CreateTrackUpdate("kalman"));

	SetLayout(CbmLitEnvironment::Instance()->GetLayout());

	SetVerbose(1);
	SetNofIter(1);
	IsUseFastSearch(true);
	SetMaxNofMissingHits(1);
	SetSigmaCoef(3.5);
	SetChiSqPixelHitCut(13.86);
	SetChiSqStripHitCut(4.);
	SetPDG(13);
}

Int_t CbmLitMuchTrackFinderNN::DoFind(
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

//	CbmLitConverter::HitArrayToHitVector(fHitsArray, hits);
//	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

	CbmLitTrackFinderNN::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

ClassImp(CbmLitMuchTrackFinderNN);
