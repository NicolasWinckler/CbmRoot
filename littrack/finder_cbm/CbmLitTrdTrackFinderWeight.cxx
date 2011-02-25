#include "finder_cbm/CbmLitTrdTrackFinderWeight.h"

#include "base/CbmLitToolFactory.h"
#include "base/CbmLitEnvironment.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"

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
	SetPropagator(factory->CreateTrackPropagator("lit"));
	SetSeedSelection(factory->CreateTrackSelection("momentum"));
	SetFinalSelection(factory->CreateTrackSelection("empty"));
	SetFitter(factory->CreateTrackFitter("kalman_robust"));
	SetFilter(factory->CreateTrackUpdate("kalman"));
	SetLayout(CbmLitEnvironment::Instance()->GetLayout());
	SetVerbose(1);
	SetNofIter(1);
	SetMaxNofMissingHits(0);
	IsUseFastSearch(true);
	SetSigmaCoef(10.);
	SetChiSqPixelHitCut(20.);
	SetChiSqStripHitCut(4.);
	SetPDG(11);
}

Int_t CbmLitTrdTrackFinderWeight::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::HitArrayToHitVector(hitArray, hits);
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
