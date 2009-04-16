#include "CbmLitTrdTrackFinderWeight.h"

#include "CbmLitToolFactory.h"
#include "CbmLitEnvironment.h"
#include "CbmLitConverter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTrack.h"

#include "TClonesArray.h"

#include <algorithm>

CbmLitTrdTrackFinderWeight::CbmLitTrdTrackFinderWeight()
{
}

CbmLitTrdTrackFinderWeight::~CbmLitTrdTrackFinderWeight()
{
}

void CbmLitTrdTrackFinderWeight::Init()
{
	DefaultInit();

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagatorToDet = fPropagator = factory->CreateTrackPropagator("lit");
	fSeedSelection = factory->CreateTrackSelection("momentum");
	fFinalSelection = factory->CreateTrackSelection("empty");
	fFitter = factory->CreateTrackFitter("kalman_robust");
	fFilter = factory->CreateTrackUpdate("kalman");

	fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();

	fVerbose = 1;
	fNofIter = 1;
	fMaxNofMissingHits = 0;
	fUseFastSearch = true;
	fSigmaCoef = 10.;
	fChiSqPixelHitCut = 20.;
	fChiSqStripHitCut = 4.;
	fPDG = 11;
}

Int_t CbmLitTrdTrackFinderWeight::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::TrkHitArrayToPixelHitVector(hitArray, hits);
	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

	CbmLitTrackFinderWeight::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToTrdTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

ClassImp(CbmLitTrdTrackFinderWeight);
