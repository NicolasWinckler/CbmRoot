#include "CbmLitTrdTrackFinderBranch.h"

#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"

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
	fSeedSelection = factory->CreateTrackSelection("momentum");
	fStationGroupSelection = factory->CreateTrackSelection("trd_station");
	fFinalSelection = factory->CreateTrackSelection("trd_final");
	fFinalPreSelection = factory->CreateTrackSelection("empty");
	fPropagatorToDet = factory->CreateTrackPropagator("lit");
	fPropagator = factory->CreateTrackPropagator("lit");
	fFilter = factory->CreateTrackUpdate("kalman");
	fFitter = factory->CreateTrackFitter("lit_kalman");

	fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();

	fVerbose = 1;
	fNofIter = 1;
	fBeginStationGroup = 0;
	fEndStationGroup = fLayout.GetNofStationGroups() - 1;
	fPDG = 11;
}

void CbmLitTrdTrackFinderBranch::SetIterationParameters(
		Int_t iter)
{
	if (iter == 0) {
		fMaxNofMissingHits = 2;
		fIsAlwaysCreateMissingHit = false;
		fSigmaCoef = 3.7;
	}
}

Int_t CbmLitTrdTrackFinderBranch::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::TrkHitArrayToPixelHitVector(hitArray, hits);
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
