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
	SetSeedSelection(factory->CreateTrackSelection("momentum"));
	SetStationGroupSelection(factory->CreateTrackSelection("trd_station"));
	SetFinalSelection(factory->CreateTrackSelection("trd_final"));
	SetFinalPreSelection(factory->CreateTrackSelection("empty"));
	TrackPropagatorPtr propagator = factory->CreateTrackPropagator("lit");
	SetPropagatorToDet(propagator);
	SetPropagator(propagator);
	SetFilter(factory->CreateTrackUpdate("kalman"));
	SetFitter(factory->CreateTrackFitter("lit_kalman"));

	SetLayout(CbmLitEnvironment::Instance()->GetTrdLayout());

	SetVerbose(1);
	SetNofIter(1);
	IsUseFastSearch(true);
	SetBeginStationGroup(0);
	SetEndStationGroup(fLayout.GetNofStationGroups() - 1);
	SetPDG(11);
}

void CbmLitTrdTrackFinderBranch::SetIterationParameters(
		Int_t iter)
{
	if (iter == 0) {
		SetMaxNofMissingHits(4);
		IsAlwaysCreateMissingHit(false);
		SetSigmaCoef(10.);
		SetChiSqPixelHitCut(20.);//15.84;
		SetChiSqStripHitCut(4.);
	}
}

Int_t CbmLitTrdTrackFinderBranch::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::TrdHitArrayToPixelHitVector(hitArray, hits);
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
