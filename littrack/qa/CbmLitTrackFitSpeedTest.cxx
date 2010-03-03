/* CbmLitTrackFitSpeedTest.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 */
#include "CbmLitTrackFitSpeedTest.h"

#include "CbmLitTrack.h"
#include "CbmLitToolFactory.h"
#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitConverter.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"
#include "parallel/LitConverter.h"
#include "parallel/LitTrack.h"
#include "parallel/LitTrackFitter.h"
#include "parallel/LitDetectorGeometry.h"
#include "parallel/LitVecPack.h"
//#include "CbmLitTrackFitterTester.h"

//#include "cuda/LitCudaConverter.h"
//#include "cuda/LitCudaDetectorGeometry.h"

#include "CbmGlobalTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTofHit.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/tick_count.h"

#include <iostream>
#include <algorithm>

//extern "C" void LitCudaFitTracks(
//		LitCudaTrack* h_tracks[],
//		const LitCudaDetectorLayout* h_layout,
//		int nofTracks);

class FastScalarFitClass
{
	LitTrackScal* fTracks;
	LitDetectorLayoutScal& fLayout;
public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
			LitTrackFitter(fTracks[iTrack], fLayout);
		}
	}
	FastScalarFitClass(
			LitTrackScal* tracks,
			LitDetectorLayoutScal& layout) :
		fTracks(tracks),
		fLayout(layout){}
};

class SIMDFitClass
{
	LitTrackVec* fTracks;
	LitDetectorLayoutVec& fLayout;
public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
			LitTrackFitter(fTracks[iTrack], fLayout);
		}
	}
	SIMDFitClass(
			LitTrackVec* tracks,
			LitDetectorLayoutVec& layout) :
		fTracks(tracks),
		fLayout(layout){}
};


CbmLitTrackFitSpeedTest::CbmLitTrackFitSpeedTest()
{
	tbb::task_scheduler_init init;
}

CbmLitTrackFitSpeedTest::~CbmLitTrackFitSpeedTest()
{

}

InitStatus CbmLitTrackFitSpeedTest::Init()
{
	DetermineSetup();
	ReadDataBranches();

	// Create tools
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fFitterLitKalman = factory->CreateTrackFitter("lit_kalman");

	CreateHistograms();
}

void CbmLitTrackFitSpeedTest::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitTrackFitSpeedTest::Exec(
		Option_t* opt)
{
	CreateTrackArrays();

	std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitTrackFitSpeedTest::Finish()
{
//	PrintStopwatchStatistics();

	RunTest();

	for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
	fLitTracks.clear();
}

void CbmLitTrackFitSpeedTest::RunTest()
{
	unsigned int nofTracks = fLitTracks.size();
    unsigned int nofTracksVec = nofTracks / fvecLen;
    std::cout << "nofTracks = " << nofTracks << std::endl;
    std::cout << "nofTracksVec = " << nofTracksVec << std::endl;
	// Initialization
	// Get the layout for the fast track fit
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    LitDetectorLayoutScal layoutScal;
    LitDetectorLayoutVec layoutVec;
    std::cout << " LAYOUT FOR SCALAR VERSION " << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
    env->GetMuchLayoutScal(layoutScal);
    std::cout << " LAYOUT FOR PARALLEL VERSION " << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
    env->GetMuchLayoutVec(layoutVec);
    // Convert to LitTracksScal
    std::cout << "Allocate tracksScal2" << std::endl;
	LitTrackScal* tracksScal2 = new LitTrackScal [nofTracks];
	std::cout << "Allocate tracksScal3" << std::endl;
	LitTrackScal* tracksScal3 = new LitTrackScal[nofTracks];
	std::cout << "Allocate tracksScal5" << std::endl;
	LitTrackScal* tracksScal5 = new LitTrackScal[nofTracks];
	for(unsigned int i = 0; i < nofTracks; i++){
		CbmLitTrackToLitTrackScal(fLitTracks[i], &tracksScal2[i]);
		CbmLitTrackToLitTrackScal(fLitTracks[i], &tracksScal3[i]);
		CbmLitTrackToLitTrackScal(fLitTracks[i], &tracksScal5[i]);
	}
	// Convert to LitTrackVec
	std::cout << "Allocate tracksVec" << std::endl;
	LitTrackVec* tracksVec = new LitTrackVec [nofTracksVec];
	LitTrackVec* tracksVec7 = new LitTrackVec [nofTracksVec];
    const LitTrack<fscal>* tmp_trks[fvecLen];
    for(unsigned int i = 0; i < nofTracksVec; i++) {
	    for(unsigned char j = 0; j < fvecLen; j++) {
	    	tmp_trks[j] = &tracksScal2[fvecLen*i + j];
	    }
	    for(unsigned short k = 0; k < fLitTracks[0]->GetNofHits(); k++) {
	    	tracksVec[i].AddHit(new LitPixelHit<fvec>);
	    	tracksVec7[i].AddHit(new LitPixelHit<fvec>);
	    }
	    PackTrack(tmp_trks, tracksVec[i]);
	    PackTrack(tmp_trks, tracksVec7[i]);
	}



	// Test standard fit
	std::cout << "Runing test LitKalman..." << std::endl;
	tbb::tick_count start1 = tbb::tick_count::now();
	TStopwatch timer1;
    timer1.Start();
	for(unsigned int i = 0; i < nofTracks; i++){
		fFitterLitKalman->Fit(fLitTracks[i]);
	}
    timer1.Stop();
	tbb::tick_count stop1 = tbb::tick_count::now();
	Double_t dt1 = (stop1 - start1).seconds();
	// end test standard fit


	// Test fast scalar version of the track fit
	std::cout << "Runing test FastScalar..." << std::endl;
	tbb::tick_count start2 = tbb::tick_count::now();
	TStopwatch timer2;
    timer2.Start();
	for(unsigned int i = 0; i < nofTracks; i++) {
		LitTrackFitter(tracksScal2[i], layoutScal);
	}
    timer2.Stop();
	tbb::tick_count stop2 = tbb::tick_count::now();
	Double_t dt2 = (stop2 - start2).seconds();
    // end test fast scalar version of the track fit


	// Test fast SIMD version of the track fit
	std::cout << "Runing test FastSIMD..." << std::endl;
    tbb::tick_count start3 = tbb::tick_count::now();
	TStopwatch timer3;
    timer3.Start();
	LitTrack<fvec> trk;
    for(unsigned short i = 0; i < fLitTracks[0]->GetNofHits(); i++)
    	trk.AddHit(new LitPixelHit<fvec>);
	const LitTrack<fscal>* trks[fvecLen];
	for(unsigned int i = 0; i < nofTracksVec; i++) {
	    for(unsigned char j = 0; j < fvecLen; j++) {
	    	trks[j] = &tracksScal3[fvecLen*i + j];
	    }
	    PackTrack(trks, trk);
        LitTrackFitter(trk, layoutVec);
	    //unpack track parameters here
	}
    timer3.Stop();
	tbb::tick_count stop3 = tbb::tick_count::now();
	Double_t dt3 = (stop3 - start3).seconds();
	// end test fast SIMD version of the track fit


    // Test fast SIMD version of the track fit
	std::cout << "Runing test FastSIMD (no pack)..." << std::endl;
	tbb::tick_count start4 = tbb::tick_count::now();
	TStopwatch timer4;
    timer4.Start();
	for(unsigned int i = 0; i < nofTracksVec; i++) {
        LitTrackFitter(tracksVec[i], layoutVec);
	}
    timer4.Stop();
	tbb::tick_count stop4 = tbb::tick_count::now();
	Double_t dt4 = (stop4 - start4).seconds();
	// end test fast scalar version of the track fit


	// Test fast scalar track fit with TBB
	std::cout << "Runing test FastScal TBB..." << std::endl;
	tbb::tick_count start5 = tbb::tick_count::now();
	TStopwatch timer5;
    timer5.Start();
	//tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracks),
	//		FastScalarFitClass(tracksScal5, layoutScal), tbb::auto_partitioner());
//    tbb::affinity_partitioner ap;
//    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracks),
//    			FastScalarFitClass(tracksScal5, layoutScal), ap);
    int grainSize = nofTracks / nofTracks;
    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracks, grainSize),
    			FastScalarFitClass(tracksScal5, layoutScal));
    timer5.Stop();
	tbb::tick_count stop5 = tbb::tick_count::now();
	Double_t dt5 = (stop5 - start5).seconds();
	// end test fast scalar track fit with TBB


	// Test fast SIMD track fit with TBB
	std::cout << "Runing test FastSIMD TBB..." << std::endl;
	tbb::tick_count start7 = tbb::tick_count::now();
	TStopwatch timer7;
    timer7.Start();
	//tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracks),
	//		FastScalarFitClass(tracksScal5, layoutScal), tbb::auto_partitioner());
//    tbb::affinity_partitioner ap;
//    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracks),
//    			FastScalarFitClass(tracksScal5, layoutScal), ap);
    grainSize = nofTracksVec / nofTracksVec;
    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracksVec, grainSize),
    			SIMDFitClass(tracksVec7, layoutVec));
    timer7.Stop();
	tbb::tick_count stop7 = tbb::tick_count::now();
	Double_t dt7 = (stop7 - start7).seconds();
	// end test fast scalar track fit with TBB


	std::cout << "nofTracks=" << nofTracks << std::endl;
	std::cout << "nofTracksVec=" << nofTracksVec << std::endl;
	std::cout << "tbb::tick_count" << std::endl;
	std::cout << "LitKalman time, sec (total / per track): " << dt1 << " / " << dt1 / nofTracks << std::endl;
	std::cout << "Fast scalar time, sec (total / per track): " << dt2 << " / " << dt2 / nofTracks << std::endl;
	std::cout << "Fast SIMD time, sec (total / per track): " << dt3 << " / " << dt3 / nofTracks << std::endl;
    std::cout << "Fast SIMD (no pack) time, sec (total / per track): " << dt4 << " / " << dt4 / nofTracks << std::endl;
    std::cout << "Fast Scal TBB time, sec (total / per track): " << dt5 << " / " << dt5 / nofTracks << std::endl;
    std::cout << "Fast SIMD TBB time, sec (total / per track): " << dt7 << " / " << dt7 / nofTracks << std::endl;

	std::cout << "+ TStopwatch:" << std::endl;
    std::cout << "LitKalman time, sec (total / per track): Real " << timer1.RealTime() << " / " << timer1.RealTime() / nofTracks
	    << " CPU " << timer1.CpuTime() << " / " << timer1.CpuTime() / nofTracks <<  std::endl;
	std::cout << "Fast scalar time, sec (total / per track): Real " << timer2.RealTime() << " / " << timer2.RealTime() / nofTracks
	    << " CPU " << timer2.CpuTime() << " / " << timer2.CpuTime() / nofTracks <<  std::endl;
	std::cout << "Fast SIMD time, sec (total / per track): Real " << timer3.RealTime() << " / " << timer3.RealTime() / nofTracks
	    << " CPU " << timer3.CpuTime() << " / " << timer3.CpuTime() / nofTracks <<  std::endl;
    std::cout << "Fast SIMD (no pack) time, sec (total / per track): Real " << timer4.RealTime() << " / " << timer4.RealTime() / nofTracks
	    << " CPU " << timer4.CpuTime() << " / " << timer4.CpuTime() / nofTracks <<  std::endl;
    std::cout << "Fast Scal TBB time, sec (total / per track): Real " << timer5.RealTime() << " / " << timer5.RealTime() / nofTracks
    	    << " CPU " << timer5.CpuTime() << " / " << timer5.CpuTime() / nofTracks <<  std::endl;
    std::cout << "Fast SIMD TBB time, sec (total / per track): Real " << timer7.RealTime() << " / " << timer7.RealTime() / nofTracks
    	    << " CPU " << timer7.CpuTime() << " / " << timer7.CpuTime() / nofTracks <<  std::endl;

	//
}

void CbmLitTrackFitSpeedTest::DetermineSetup()
{
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    fIsElectronSetup = env->IsElectronSetup();
	fIsSts = true;
    fIsTrd = env->IsTrd();
    fIsMuch = env->IsMuch();
    fIsTof = env->IsTof();

    if (fIsElectronSetup) std::cout << "-I- CBM electron setup detected" << std::endl;
    else std::cout << "-I- CBM muon setup detected" << std::endl;
    std::cout << "-I- The following detectors were found in the CBM setup:" << std::endl;
    if (fIsTrd) std::cout << "TRD" << std::endl;
    if (fIsMuch) std::cout << "MUCH" << std::endl;
    if (fIsTof) std::cout << "TOF" << std::endl;
}

void CbmLitTrackFitSpeedTest::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("CbmLitTrackFitSpeedTest::Init","CbmRootManager is not instantiated");

    fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if (NULL == fGlobalTracks) Fatal("CbmLitTrackFitSpeedTest::Init","No GlobalTrack array!");

   	if (fIsSts) {
   		fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   		if (NULL == fStsTracks) Fatal("CbmLitTrackFitSpeedTest::Init", "No STSTrack array!");
   	}

   	if (fIsMuch) {
		fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
		if (NULL == fMuchTracks) Fatal("CbmLitTrackFitSpeedTest::Init", "No MuchTrack array!");
		fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
		fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
		if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) Fatal("CbmLitTrackFitSpeedTest::Init", "No MuchPixelHit AND MuchStrawHit arrays!");
   	}

   	if (fIsTrd) {
		fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
		if (NULL == fTrdTracks) Fatal("CbmLitTrackFitSpeedTest::Init", "No TrdTrack array!");
		fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
		if (NULL == fTrdHits) Fatal("CbmLitTrackFitSpeedTest::Init", "No TRDHit array!");
   	}

   	if (fIsTof) {
		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) Fatal("CbmLitTrackFitSpeedTest::Init", "No TofHit array!");
   	}
}

void CbmLitTrackFitSpeedTest::CreateHistograms()
{

}

void CbmLitTrackFitSpeedTest::CreateTrackArrays()
{
	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
		CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
		CbmLitTrack* litTrack = new CbmLitTrack();

		if (!CheckAcceptance(globalTrack)) continue;

		GlobalTrackToLitTrack(globalTrack, litTrack);
		if (fIsElectronSetup) litTrack->SetPDG(11); else litTrack->SetPDG(13);
//		std::cout << litTrack->GetNofHits() << std::endl;
		if (litTrack->GetNofHits() != fNofPlanes) continue;
		litTrack->SortHits();
//		std::cout << litTrack->GetNofHits() << std::endl;
		fLitTracks.push_back(litTrack);
	}
}

Bool_t CbmLitTrackFitSpeedTest::CheckAcceptance(
		const CbmGlobalTrack* globalTrack)
{
	Int_t trdId = globalTrack->GetTrdTrackIndex();
	Int_t muchId = globalTrack->GetMuchTrackIndex();
	Int_t tofId = globalTrack->GetTofHitIndex();
	if (fIsTrd && trdId > -1) {
		CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
		if (trdTrack->GetNofHits() != fNofTrdHits) return false;
	}
	if (fIsMuch && muchId > -1) {
		CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
		if (muchTrack->GetNofHits() != fNofMuchHits) return false;
	}

//	if (fNofTofHits < 1) return true;
//	if (tofId > -1) return true;
	return true;
}

void CbmLitTrackFitSpeedTest::GlobalTrackToLitTrack(
		const CbmGlobalTrack* globalTrack,
		CbmLitTrack* litTrack)
{
	Int_t stsId = globalTrack->GetStsTrackIndex();
	Int_t trdId = globalTrack->GetTrdTrackIndex();
	Int_t muchId = globalTrack->GetMuchTrackIndex();
	Int_t tofId = globalTrack->GetTofHitIndex();

	// STS: set initial track parameters from STS track
	if (stsId > -1) {
		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsId);
		CbmLitTrackParam par;
		CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamLast(), &par);
		litTrack->SetParamLast(&par);
		litTrack->SetParamFirst(&par);
	}
	//MUCH: attach hits from MUCH track
	if (muchId > -1) {
		CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
		for (int iHit = 0; iHit < muchTrack->GetNofHits(); iHit++) {
			Int_t index = muchTrack->GetHitIndex(iHit);
			HitType type = muchTrack->GetHitType(iHit);
			if (type == kMUCHPIXELHIT) {
				CbmPixelHit* hit = (CbmPixelHit*) fMuchPixelHits->At(index);
				CbmLitPixelHit litHit;
				CbmLitConverter::PixelHitToLitPixelHit(hit, index, &litHit);
				litTrack->AddHit(&litHit);
			} else if (type == kMUCHSTRAWHIT) {
				CbmStripHit* hit = (CbmStripHit*) fMuchStrawHits->At(index);
				CbmLitStripHit litHit;
				CbmLitConverter::StripHitToLitStripHit(hit, index, &litHit);
				litTrack->AddHit(&litHit);
			}
		}
	}
	//TRD: attach hits from TRD track
	if (trdId > -1) {
		CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
		for (int iHit = 0; iHit < trdTrack->GetNofHits(); iHit++) {
			Int_t index = trdTrack->GetHitIndex(iHit);
			CbmPixelHit* hit = (CbmPixelHit*) fTrdHits->At(index);
			CbmLitPixelHit litHit;
			CbmLitConverter::PixelHitToLitPixelHit(hit, index, &litHit);
			litTrack->AddHit(&litHit);
		}
	}
	//TOF: attach TOF hit
//	if (tofId > -1) {
//		CbmPixelHit* tofHit = (CbmPixelHit*) fTofHits->At(tofId);
//		CbmLitPixelHit litHit;
//		CbmLitConverter::PixelHitToLitPixelHit(tofHit, tofId, &litHit);
//		litTrack->AddHit(&litHit);
//	}
}

ClassImp(CbmLitTrackFitSpeedTest);
