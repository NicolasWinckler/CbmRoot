/** CbmLitFindGlobalTracks.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#include "CbmLitFindGlobalTracks.h"
#include "CbmLitEnvironment.h"
#include "CbmLitConverter.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitTrack.h"
#include "CbmLitToolFactory.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>

CbmLitFindGlobalTracks::CbmLitFindGlobalTracks():
	fEventNo(0),
	fTrackingType("branch"),
	fMergerType("nearest_hit"),
	fFitterType("lit_kalman"),
	fStsTracks(NULL),
	fMuchPixelHits(NULL),
	fMuchStrawHits(NULL),
	fMuchTracks(NULL),
	fTrdHits(NULL),
	fTrdTracks(NULL),
	fTofHits(NULL),
	fGlobalTracks(NULL)
{
}

CbmLitFindGlobalTracks::~CbmLitFindGlobalTracks()
{

}

InitStatus CbmLitFindGlobalTracks::Init()
{
	DetermineSetup();
	ReadAndCreateDataBranches();

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

	RunTrackReconstruction();

	ConvertOutputData();

	ClearArrays();

	std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitFindGlobalTracks::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();

    rtdb->getContainer("CbmGeoMuchPar");
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
		fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
		fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
		if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) Fatal("Init", "No MuchPixelHit or MuchStrawHit array!");
		if (fMuchPixelHits) std::cout << "-I- MuchPixelHit branch found in tree" << std::endl;
		if (fMuchStrawHits) std::cout << "-I- MuchStrawHit branch found in tree" << std::endl;
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
		if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "weight" || fTrackingType == "nn_parallel") {
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

	if (fFitterType == "lit_kalman")
		fFitter = factory->CreateTrackFitter("lit_kalman");
	else
		TObject::Fatal("CbmLitFindGlobalTracks","Fitter type not found");
}

void CbmLitFindGlobalTracks::ConvertInputData()
{
	CbmLitConverter::StsTrackArrayToTrackVector(fStsTracks, fLitStsTracks);
	std::cout << "-I- Number of STS tracks: " << fLitStsTracks.size() << std::endl;
	if (fMuchPixelHits) CbmLitConverter::HitArrayToHitVector(fMuchPixelHits, fLitHits);
	if (fMuchStrawHits) CbmLitConverter::HitArrayToHitVector(fMuchStrawHits, fLitHits);
	if (fTrdHits) {
		CbmLitConverter::HitArrayToHitVector(fTrdHits, fLitHits);
		//If MUCH-TRD setup, than shift plane id for the TRD hits
		if (fIsMuch && fIsTrd) {
			Int_t nofPlanes = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofPlanes();
			for (Int_t i = 0; i < fLitHits.size(); i++) {
				CbmLitHit* hit = fLitHits[i];
				if (hit->GetDetectorId() == kLITTRD) hit->SetPlaneId(hit->GetPlaneId() + nofPlanes);
			}
		}
	}
	std::cout << "-I- Number of hits: " << fLitHits.size() << std::endl;

	if (fTofHits) {
		CbmLitConverter::HitArrayToHitVector(fTofHits, fLitTofHits);
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
	for_each(fLitHits.begin(), fLitHits.end(), DeleteObject());
	for_each(fLitTofHits.begin(), fLitTofHits.end(), DeleteObject());
	fLitStsTracks.clear();
	fLitOutputTracks.clear();
	fLitHits.clear();
	fLitTofHits.clear();
}

void CbmLitFindGlobalTracks::RunTrackReconstruction()
{
	if (fIsMuch || fIsTrd) {
		fTrackingWatch.Start(kFALSE);
		fFinder->DoFind(fLitHits, fLitStsTracks, fLitOutputTracks);
		fTrackingWatch.Stop();
	}
	if (fIsTof){
		fMergerWatch.Start(kFALSE);
//		fMerger->DoMerge(fLitTofHits, fLitOutputTracks);
		fMergerWatch.Stop();
	}

	//Refit found tracks
	for(TrackPtrIterator it = fLitOutputTracks.begin(); it != fLitOutputTracks.end(); it++){
		CbmLitTrack* track = *it;
		fFitter->Fit(track);
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
