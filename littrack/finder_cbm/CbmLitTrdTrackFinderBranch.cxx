#include "CbmLitTrdTrackFinderBranch.h"

#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"
#include "CbmLitPtrTypes.h"

#include "TClonesArray.h"

#include <vector>
#include <algorithm>

CbmLitTrdTrackFinderBranch::CbmLitTrdTrackFinderBranch()
{
}

CbmLitTrdTrackFinderBranch::~CbmLitTrdTrackFinderBranch()
{
}

void CbmLitTrdTrackFinderBranch::Init()
{
	DefaultInit();

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	SetPropagator(factory->CreateTrackPropagator("lit"));
	SetSeedSelection(factory->CreateTrackSelection("empty"));
	SetStationGroupSelection(factory->CreateTrackSelection("trd_station"));
	SetFinalSelection(factory->CreateTrackSelection("trd_final"));
	SetFilter(factory->CreateTrackUpdate("kalman"));
	SetLayout(CbmLitEnvironment::Instance()->GetLayout());
	SetVerbose(1);
	SetNofIter(1);
	IsUseFastSearch(true);
	SetPDG(11);
	SetMaxNofMissingHits(3);
	IsAlwaysCreateMissingHit(false);
	SetSigmaCoef(5.);
	SetChiSqPixelHitCut(25.);
	SetChiSqStripHitCut(4.);
}

Int_t CbmLitTrdTrackFinderBranch::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::HitArrayToHitVector(hitArray, hits);
	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

	CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToTrdTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

ClassImp(CbmLitTrdTrackFinderBranch);
