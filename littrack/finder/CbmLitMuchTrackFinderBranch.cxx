#include "CbmLitMuchTrackFinderBranch.h"

#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"

#include "TClonesArray.h"

#include <vector>
#include <iostream>
#include <algorithm>


CbmLitMuchTrackFinderBranch::CbmLitMuchTrackFinderBranch()
{
}

CbmLitMuchTrackFinderBranch::~CbmLitMuchTrackFinderBranch()
{
	if (fPropagatorToDet) delete fPropagatorToDet;
}

void CbmLitMuchTrackFinderBranch::Init()
{
	DefaultInit();

	fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagatorToDet = fPropagator = factory->CreateTrackPropagator("lit");
	fFilter = factory->CreateTrackUpdate("kalman");
	fSeedSelection = factory->CreateTrackSelection("momentum_seed");
	fStationGroupSelection = factory->CreateTrackSelection("momentum");
	fFinalSelection = factory->CreateTrackSelection("much_final");
	fFinalPreSelection = factory->CreateTrackSelection("empty");
	fFitter = factory->CreateTrackFitter("lit_kalman");

	fVerbose = 1;
	fNofIter = 1;
	fBeginStationGroup = 0;
	fEndStationGroup = fLayout.GetNofStationGroups() - 1;
	fMaxNofMissingHits = 1;
	fIsAlwaysCreateMissingHit = false;
	fSigmaCoef = 3.;
	fPDG = 13;
}

Int_t CbmLitMuchTrackFinderBranch::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::MuchHitArrayToHitVector(hitArray, hits);
	DefaultCreateTrackSeeds(fTrackSeedsArray, trackSeeds, fLayout, fPDG);

	CbmLitTrackFinderBranch::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToMuchTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

ClassImp(CbmLitMuchTrackFinderBranch);
