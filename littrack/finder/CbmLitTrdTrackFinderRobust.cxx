#include "CbmLitTrdTrackFinderRobust.h"

#include "CbmLitTrackPropagator.h"
#include "CbmLitToolFactory.h"
#include "CbmLitEnvironment.h"
#include "CbmLitConverter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitHit.h"
#include "CbmLitTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <algorithm>

CbmLitTrdTrackFinderRobust::CbmLitTrdTrackFinderRobust()
{
}

CbmLitTrdTrackFinderRobust::~CbmLitTrdTrackFinderRobust()
{
	if (fPropagatorToDet) delete fPropagatorToDet;
}

void CbmLitTrdTrackFinderRobust::Init()
{
   FairRootManager* rootMgr = FairRootManager::Instance();
   if(NULL == rootMgr)
      TObject::Fatal("CbmLitTrdTrackFinderRobust::Init","ROOT manager is not instantiated");

   fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("STSTrack");
   if(NULL == fTrackSeedsArray)
      TObject::Fatal("CbmLitTrdTrackFinderRobust::Init","no STS track array");

   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fPropagatorToDet = factory->CreateTrackPropagator("rk4");
   fPropagator = factory->CreateTrackPropagator("line");

   fSeedSelection = factory->CreateTrackSelection("Momentum");
   fFinalSelection = factory->CreateTrackSelection("MuchRobust");

   fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();

   fVerbose = 1;
   fNofIter = 1;
   fMaxNofMissingHits = 1;
   fSigmaCoef = 3.5;
   fPDG = 211;
}

Int_t CbmLitTrdTrackFinderRobust::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::TrkHitArrayToPixelHitVector(hitArray, hits);
	CreateTrackSeeds(fTrackSeedsArray, trackSeeds);

	CbmLitTrackFinderRobust::DoFind(hits, trackSeeds, foundTracks);

	CbmLitConverter::TrackVectorToTrdTrackArray(foundTracks, trackArray);

	for_each(foundTracks.begin(), foundTracks.end(), DeleteObject());
	for_each(hits.begin(), hits.end(), DeleteObject());
	for_each(trackSeeds.begin(), trackSeeds.end(), DeleteObject());
	foundTracks.clear();
	hits.clear();
	trackSeeds.clear();

	return trackArray->GetEntriesFast();
}

void CbmLitTrdTrackFinderRobust::CreateTrackSeeds(
		TClonesArray* trackArray,
		TrackPtrVector& trackSeeds)
{
	CbmLitConverter::StsTrackArrayToTrackVector(trackArray, trackSeeds);

    if (fVerbose > 1)
      std::cout << "-I- CbmLitTrdTrackFinderRobust::CreateTrackSeeds: " << std::endl
                << trackArray->GetEntriesFast() << " tracks were loaded, "
                << trackSeeds.size() << " tracks were created" << std::endl;

    Double_t Ze = fLayout.GetSubstation(0, 0, 0).GetZ();
    for (TrackPtrIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {
    	CbmLitTrackParam par = *(*iTrack)->GetParamLast();
       fPropagatorToDet->Propagate(&par, Ze, fPDG);
       (*iTrack)->SetParamLast(&par);
       (*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
       (*iTrack)->SetChi2( 0.0 );
    }

    if (fVerbose > 1)
       std::cout << "-I- CbmLitTrdTrackFinderRobust::CreateTrackSeeds: "
                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitTrdTrackFinderRobust);
