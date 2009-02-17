#include "CbmLitStsBasedTrackFinder.h"
#include "CbmLitTypes.h"
#include "CbmLitToolFactory.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitTrackParam.h"
#include "CbmLitTrack.h"
#include "CbmLitDetectorLayout.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <algorithm>
#include <iostream>

CbmLitStsBasedTrackFinder::CbmLitStsBasedTrackFinder()
{
//	fVerbose = 1;
}

CbmLitStsBasedTrackFinder::~CbmLitStsBasedTrackFinder()
{
	delete fPropagatorToDet;
}

void CbmLitStsBasedTrackFinder::DefaultInit()
{
	FairRootManager* rootMgr = FairRootManager::Instance();
	if(NULL == rootMgr) {
		std::cout << "CbmLitStsBasedTrackFinder::DefaultInit" << "ROOT manager is not instantiated" << std::endl;
		exit(0);
	   //TObject::Fatal("CbmLitStsBasedTrackFinder::DefaultInit","ROOT manager is not instantiated");
	}

	fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("STSTrack");
	if(NULL == fTrackSeedsArray){
		std::cout << "CbmLitStsBasedTrackFinder::DefaultInit" << "no STS track array" << std::endl;
		exit(0);
	   //TObject::Fatal("CbmLitStsBasedTrackFinder::DefaultInit","no STS track array");
	}

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagatorToDet = factory->CreateTrackPropagator("Much");
}

Int_t CbmLitStsBasedTrackFinder::DefaultDoFind(
		TClonesArray* hitArray,
		TClonesArray* trackArray)
{
}

void CbmLitStsBasedTrackFinder::DefaultCreateTrackSeeds(
		TClonesArray* trackArray,
		TrackPtrVector& trackSeeds,
		const CbmLitDetectorLayout& layout,
		Int_t pdg)
{
	CbmLitConverter::StsTrackArrayToTrackVector(trackArray, trackSeeds);

//    if (fVerbose > 1)
//      std::cout << "-I- CbmLitStsBasedTrackFinder::CreateTrackSeeds: " << std::endl
//                << trackArray->GetEntriesFast() << " tracks were loaded, "
//                << trackSeeds.size() << " tracks were created" << std::endl;

    Double_t Ze = layout.GetSubstation(0, 0, 0).GetZ();
    for (TrackPtrIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {
    	CbmLitTrackParam par = *(*iTrack)->GetParamLast();
    	fPropagatorToDet->Propagate(&par, Ze, pdg);
    	(*iTrack)->SetParamLast(&par);
    	(*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
    	(*iTrack)->SetChi2(0.);
    }

//    if (fVerbose > 1)
//       std::cout << "-I- CbmLitStsBasedTrackFinder::CreateTrackSeeds: "
//                 << "Extrapolation to detector finished " << std::endl;
}

ClassImp(CbmLitStsBasedTrackFinder);
