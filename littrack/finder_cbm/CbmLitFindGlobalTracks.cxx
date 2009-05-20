#include "CbmLitFindGlobalTracks.h"
#include "CbmLitEnvironment.h"
#include "CbmLitConverter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitTrack.h"
#include "CbmLitToolFactory.h"
//#include "CbmLitSimpleGeometryConstructor.h"

#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrdHit.h"
#include "CbmMuchHit.h"
#include "CbmTofHit.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>

CbmLitFindGlobalTracks::CbmLitFindGlobalTracks()
{
	fEventNo = 0;
	fTrackingType = "branch";
	fMergerType = "nearest_hit";
}

CbmLitFindGlobalTracks::~CbmLitFindGlobalTracks()
{

}

InitStatus CbmLitFindGlobalTracks::Init()
{
	DetermineSetup();
	ReadAndCreateDataBranches();

//	CbmLitSimpleGeometryConstructor geoConstructor;
//	geoConstructor.ConstructGeometry();

	InitTrackReconstruction();

	return kSUCCESS;
}

void CbmLitFindGlobalTracks::Exec(
		Option_t* opt)
{
	if (fIsTrd)	fTrdTracks->Clear();
	if (fIsMuch) fMuchTracks->Clear();
	fGlobalTracks->Clear();

	ConvertInputData();

	InitStsTrackSeeds();

	RunTrackReconstruction();

	ConvertOutputData();

	ClearArrays();

	std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitFindGlobalTracks::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();

//    rtdb->getContainer("FairBaseParSet");
//    rtdb->getContainer("CbmGeoPassivePar");
//    rtdb->getContainer("CbmGeoStsPar");
//    rtdb->getContainer("CbmGeoRichPar");
    rtdb->getContainer("CbmGeoMuchPar");
//    rtdb->getContainer("CbmGeoTrdPar");
//    rtdb->getContainer("CbmFieldPar");
}

void CbmLitFindGlobalTracks::Finish()
{
	PrintStopwatchStatistics();
}

void CbmLitFindGlobalTracks::DetermineSetup()
{
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    fIsElectronSetup = env->IsElectronSetup();
    fIsTrd = env->IsTrd();
    fIsMuch = env->IsMuch();
    fIsTof = env->IsTof();

    if (fIsElectronSetup) std::cout << "-I- CBM electron setup detected" << std::endl;
    else std::cout << "-I- CBM muon setup detected" << std::endl;
    std::cout << "-I- The following detectors were found in the CBM setup and will be used for global tracking:" << std::endl;
    if (fIsTrd) std::cout << "TRD" << std::endl;
    if (fIsMuch) std::cout << "MUCH" << std::endl;
    if (fIsTof) std::cout << "TOF" << std::endl;
}

void CbmLitFindGlobalTracks::ReadAndCreateDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) Fatal("Init","CbmRootManager is not instantiated");

	//STS data
	fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
	if (NULL == fStsTracks) Fatal("Init","No STSTrack array!");
	std::cout << "-I- STSTrack branch found in tree" << std::endl;

	//MUCH data
	if (fIsMuch) {
		fMuchHits = (TClonesArray*) ioman->GetObject("MuchHit");
		if (NULL == fMuchHits) Fatal("Init", "No MuchHit array!");
		std::cout << "-I- MuchHit branch found in tree" << std::endl;
	}

	//TRD data
	if (fIsTrd) {
		fTrdHits = (TClonesArray*) ioman->GetObject("TRDHit");
		if (NULL == fTrdHits) Fatal("Init", "No TRDHit array!");
		std::cout << "-I- TRDHit branch found in tree" << std::endl;
	}

	//TOF data
	if (fIsTof) {
		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) Fatal("Init", "No TofHit array!");
		std::cout << "-I- TofHit branch found in tree" << std::endl;
	}

	// Create and register track arrays
	fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
	ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);

	if (fIsMuch) {
		fMuchTracks = new TClonesArray("CbmMuchTrack", 100);
		ioman->Register("MuchTrack", "Much", fMuchTracks, kTRUE);
	}

	if (fIsTrd) {
		fTrdTracks = new TClonesArray("CbmTrdTrack", 100);
		ioman->Register("TRDTrack", "Trd", fTrdTracks, kTRUE);
	}
}

void CbmLitFindGlobalTracks::InitTrackReconstruction()
{
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	if (fIsElectronSetup) {
		if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "weight") {
			std::string st("e_");
			st += fTrackingType;
			fFinder = factory->CreateTrackFinder(st);
		} else
			TObject::Fatal("CbmLitFindGlobalTracks","Tracking type not found");
	} else {
		if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "weight") {
			std::string st("mu_");
			st += fTrackingType;
			fFinder = factory->CreateTrackFinder(st);
		} else
			TObject::Fatal("CbmLitFindGlobalTracks","Tracking type not found");
	}

	if (fIsTof) {
		if (fMergerType == "nearest_hit")
			fMerger = factory->CreateHitToTrackMerger("tof_nearest_hit");
		else
			TObject::Fatal("CbmLitFindGlobalTracks","Merger type not found");
	}
}

void CbmLitFindGlobalTracks::ConvertInputData()
{
	CbmLitConverter::StsTrackArrayToTrackVector(fStsTracks, fLitStsTracks);
	std::cout << "-I- Number of STS tracks: " << fLitStsTracks.size() << std::endl;
	if (fIsMuch) {
		CbmLitConverter::MuchHitArrayToHitVector(fMuchHits, fLitMuchHits);
		std::cout << "-I- Number of MUCH hits: " << fLitMuchHits.size() << std::endl;
	}
	if (fIsTrd) {
		CbmLitConverter::TrdHitArrayToPixelHitVector(fTrdHits, fLitTrdHits);
		std::cout << "-I- Number of TRD hits: " << fLitTrdHits.size() << std::endl;
	}
	if (fIsTof) {
		CbmLitConverter::TofHitArrayToPixelHitVector(fTofHits, fLitTofHits);
		std::cout << "-I- Number of TOF hits: " << fLitTofHits.size() << std::endl;
	}
}

void CbmLitFindGlobalTracks::ConvertOutputData()
{
	CbmLitConverter::LitTrackVectorToGlobalTrackArray(fLitOutputTracks, fGlobalTracks, fStsTracks, fTrdTracks, fMuchTracks);
}

void CbmLitFindGlobalTracks::ClearArrays()
{
	// Free memory
	for_each(fLitStsTracks.begin(), fLitStsTracks.end(), DeleteObject());
	for_each(fLitOutputTracks.begin(), fLitOutputTracks.end(), DeleteObject());
	for_each(fLitMuchHits.begin(), fLitMuchHits.end(), DeleteObject());
	for_each(fLitTrdHits.begin(), fLitTrdHits.end(), DeleteObject());
	for_each(fLitTofHits.begin(), fLitTofHits.end(), DeleteObject());
	fLitStsTracks.clear();
	fLitOutputTracks.clear();
	fLitMuchHits.clear();
	fLitTrdHits.clear();
	fLitTofHits.clear();
}

void CbmLitFindGlobalTracks::InitStsTrackSeeds()
{
//    Double_t Ze = layout.GetSubstation(0, 0, 0).GetZ();
//    for (TrackPtrIterator iTrack = trackSeeds.begin(); iTrack != trackSeeds.end(); iTrack++) {
//    	CbmLitTrackParam par = *(*iTrack)->GetParamLast();
//    	fPropagatorToDet->Propagate(&par, Ze, pdg);
//    	(*iTrack)->SetParamLast(&par);
//    	(*iTrack)->SetParamFirst((*iTrack)->GetParamLast());
//    	(*iTrack)->SetChi2(0.);
//    }
}

void CbmLitFindGlobalTracks::RunTrackReconstruction()
{
	if (fIsElectronSetup && fIsTrd) {
		fTrackingWatch.Start(kFALSE);
		fFinder->DoFind(fLitTrdHits, fLitStsTracks, fLitOutputTracks);
		fTrackingWatch.Stop();
	}
	if (fIsMuch) {
		fTrackingWatch.Start(kFALSE);
		fFinder->DoFind(fLitMuchHits, fLitStsTracks, fLitOutputTracks);
		fTrackingWatch.Stop();
	}
	if (fIsTof){
		fMergerWatch.Start(kFALSE);
		fMerger->DoMerge(fLitTofHits, fLitOutputTracks);
		fMergerWatch.Stop();
	}
}

void CbmLitFindGlobalTracks::PrintStopwatchStatistics()
{
	std::cout << "Stopwatch: " << std::endl;
	std::cout << "tracking: counts=" << fTrackingWatch.Counter()
		<< ", real=" << fTrackingWatch.RealTime()/fTrackingWatch.Counter()
		<< "/" << fTrackingWatch.RealTime()
		<< " s, cpu=" << fTrackingWatch.CpuTime()/fTrackingWatch.Counter()
		<< "/" << fTrackingWatch.CpuTime() << std::endl;
	std::cout << "fitter: real=" << fMergerWatch.Counter()
		<< ", real=" << fMergerWatch.RealTime()/fMergerWatch.Counter()
		<< "/" << fMergerWatch.RealTime()
		<< " s, cpu=" << fMergerWatch.CpuTime()/fMergerWatch.Counter()
		<< "/" << fMergerWatch.CpuTime() << std::endl;
}

ClassImp(CbmLitFindGlobalTracks);
