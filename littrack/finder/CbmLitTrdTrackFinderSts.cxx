#include "CbmLitTrdTrackFinderSts.h"

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

#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <vector>
#include <algorithm>

CbmLitTrdTrackFinderSts::CbmLitTrdTrackFinderSts()
{

}

CbmLitTrdTrackFinderSts::~CbmLitTrdTrackFinderSts()
{
	delete fExtrapolatorToDet;
	delete fPropagatorToDet;
}

void CbmLitTrdTrackFinderSts::Init()
{
   CbmRootManager* rootMgr = CbmRootManager::Instance();
   if(NULL == rootMgr)
      TObject::Fatal("CbmLitTrdTrackFinderSts::Init", "ROOT manager is not instantiated");

   fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("STSTrack");
   if(NULL == fTrackSeedsArray)
      TObject::Fatal("CbmLitTrdTrackFinderSts::Init", "no STS track array");

   fExtrapolatorToDet = new CbmLitRK4TrackExtrapolator();
   fPropagatorToDet = new CbmLitTrackPropagatorImp(fExtrapolatorToDet);
   fPropagatorToDet->Initialize();

   fPropagator = new CbmLitTrackPropagatorImp(new CbmLitLineTrackExtrapolator());
   fPropagator->Initialize();

   fFilter = new CbmLitKalmanFilter();
   fFilter->Initialize();

   fFitter = new CbmLitTrackFitterImp(fPropagator, fFilter);
   fFitter->Initialize();

   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fSeedSelection = factory->CreateTrackSelection("Momentum");
   fStationGroupSelection = factory->CreateTrackSelection("TrdStation");
   fFinalSelection = factory->CreateTrackSelection("TrdFinal");
   fFinalPreSelection = factory->CreateTrackSelection("Empty");

   fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();

   fNofIter = 1;
   fBeginStationGroup = 0;
   fEndStationGroup = fLayout.GetNofStationGroups() - 1;
   fPDG = 11;

   fVerbose = 1;
}

void CbmLitTrdTrackFinderSts::SetIterationParameters(
		Int_t iter)
{
	if (iter == 0) {
		fMaxNofMissingHits = 2;
		fIsAlwaysCreateMissingHit = false;
		fSigmaCoef = 3.7;
	}
}

Int_t CbmLitTrdTrackFinderSts::DoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
	HitPtrVector hits;
	TrackPtrVector trackSeeds;
	TrackPtrVector foundTracks;

	CbmLitConverter::TrkHitArrayToPixelHitVector(hitArray, hits);
	CreateTrackSeeds(fTrackSeedsArray, trackSeeds);

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

void CbmLitTrdTrackFinderSts::CreateTrackSeeds(
		TClonesArray* trackArray,
		TrackPtrVector& trackSeeds)
{
	CbmLitConverter::StsTrackArrayToTrackVector(trackArray, trackSeeds);

    if (fVerbose > 1)
      std::cout << "-I- CbmLitTrdTrackFinderSts::CreateTrackSeeds: " << std::endl
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
       std::cout << "-I- CbmLitTrackFinderStsNew::CreateTrackSeeds: "
                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitTrdTrackFinderSts);
