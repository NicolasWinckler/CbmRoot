#include "CbmLitMuchTrackFinderNN.h"
#include "CbmLitToolFactory.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTrack.h"

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
	fPropagator = factory->CreateTrackPropagator("lit");
	fSeedSelection = factory->CreateTrackSelection("momentum_seed");
	fFinalSelection = factory->CreateTrackSelection("much_final");
	fFilter = factory->CreateTrackUpdate("kalman");

	fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();

	fVerbose = 1;
	fNofIter = 1;
	fMaxNofMissingHits = 1;
	fSigmaCoef = 3.;
	fPDG = 13;
}

Int_t CbmLitMuchTrackFinderNN::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::MuchHitArrayToHitVector(hitArray, hits);
	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

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
