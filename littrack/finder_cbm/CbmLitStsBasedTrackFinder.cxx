#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "base/CbmLitDetectorLayout.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitTrack.h"
#include "interface/CbmLitTrackPropagator.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitConverter.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <algorithm>
#include <iostream>

CbmLitStsBasedTrackFinder::CbmLitStsBasedTrackFinder()
{
}

CbmLitStsBasedTrackFinder::~CbmLitStsBasedTrackFinder()
{
}

void CbmLitStsBasedTrackFinder::DefaultInit()
{
	FairRootManager* rootMgr = FairRootManager::Instance();
	if(NULL == rootMgr) {
		std::cout << "CbmLitStsBasedTrackFinder::DefaultInit" << "ROOT manager is not instantiated" << std::endl;
		exit(0);
	   //TObject::Fatal("CbmLitStsBasedTrackFinder::DefaultInit","ROOT manager is not instantiated");
	}

	fTrackSeedsArray = (TClonesArray*) rootMgr->GetObject("StsTrack");
	if(NULL == fTrackSeedsArray){
		std::cout << "CbmLitStsBasedTrackFinder::DefaultInit" << "no STS track array" << std::endl;
		exit(0);
	   //TObject::Fatal("CbmLitStsBasedTrackFinder::DefaultInit","no STS track array");
	}

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagatorToDet = factory->CreateTrackPropagator("lit");
}

int CbmLitStsBasedTrackFinder::DefaultDoFind(
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

    myf Ze = layout.GetSubstation(0, 0, 0).GetZ();
    for (TrackPtrIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {
    	CbmLitTrackParam par = *(*iTrack)->GetParamLast();
    	fPropagatorToDet->Propagate(&par, Ze, pdg);
    	(*iTrack)->SetParamLast(&par);
    	(*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
    	(*iTrack)->SetChi2(0.);
    }
}

ClassImp(CbmLitStsBasedTrackFinder);
