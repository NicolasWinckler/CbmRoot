#include "CbmLitTrdTrackFinderNN.h"

#include "CbmLitConverter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitToolFactory.h"
#include "CbmLitEnvironment.h"
#include "CbmLitTrack.h"

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
	fPropagatorToDet = fPropagator = factory->CreateTrackPropagator("lit");
	fSeedSelection = factory->CreateTrackSelection("momentum");
	fFinalSelection = factory->CreateTrackSelection("trd_final");
	fFilter = factory->CreateTrackUpdate("kalman");

	fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();

	fVerbose = 1;
	fNofIter = 1;
	fUseFastSearch = true;
	fMaxNofMissingHits = 4;
	fSigmaCoef = 10.;
	fChiSqPixelHitCut = 20.;
	fChiSqStripHitCut = 4.;
	fPDG = 11;
}

Int_t CbmLitTrdTrackFinderNN::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::TrkHitArrayToPixelHitVector(hitArray, hits);
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
