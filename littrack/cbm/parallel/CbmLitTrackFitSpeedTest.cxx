/* CbmLitTrackFitSpeedTest.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 */
#include "parallel/CbmLitTrackFitSpeedTest.h"

#include "base/CbmLitToolFactory.h"
#include "base/CbmLitEnvironment.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitStripHit.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitConverter.h"
#include "parallel/LitConverter.h"
#include "parallel/LitTrack.h"
#include "parallel/muon/LitTrackFitterMuon.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"
#include "parallel/LitVecPack.h"
#include "parallel/electron/LitTrackFitterElectron.h"

#include "CbmGlobalTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

#include <iostream>
#include <algorithm>

class FastScalarFitClass
{
   lit::parallel::LitTrackScal* fTracks;
   lit::parallel::LitDetectorLayoutMuonScal& fLayout;
public:
   void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
      for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
         lit::parallel::LitTrackFitterMuon(fTracks[iTrack], fLayout);
      }
   }
   FastScalarFitClass(
      lit::parallel::LitTrackScal* tracks,
      lit::parallel::LitDetectorLayoutMuonScal& layout) :
      fTracks(tracks),
      fLayout(layout) {}
};

class SIMDFitClass
{
   lit::parallel::LitTrackVec* fTracks;
   lit::parallel::LitDetectorLayoutMuonVec& fLayout;
public:
   void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
      for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
         lit::parallel::LitTrackFitterMuon(fTracks[iTrack], fLayout);
      }
   }
   SIMDFitClass(
      lit::parallel::LitTrackVec* tracks,
      lit::parallel::LitDetectorLayoutMuonVec& layout) :
      fTracks(tracks),
      fLayout(layout) {}
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

   return kSUCCESS;
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

   std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitTrackFitSpeedTest::Finish()
{
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
   const int ntimes = 100; // repeat each test ntimes

   // Initialization
   // Get the layout for the fast track fit
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();

   // For MUCH track fit
   lit::parallel::LitDetectorLayoutMuonScal layoutScal;
   lit::parallel::LitDetectorLayoutMuonVec layoutVec;
   // For TRD track fit
   lit::parallel::LitDetectorLayoutElectronScal layoutElectronScal;
   lit::parallel::LitDetectorLayoutElectronVec layoutElectronVec;
   if (!fIsElectronSetup) {
      env->GetMuchLayoutScal(layoutScal);
      env->GetMuchLayoutVec(layoutVec);
   } else {
      env->GetTrdLayoutScal(layoutElectronScal);
      env->GetTrdLayoutVec(layoutElectronVec);
   }

   // Convert to LitTracksScal
   lit::parallel::LitTrackScal* tracksFast = new lit::parallel::LitTrackScal[nofTracks];
   lit::parallel::LitTrackScal* tracksSIMD = new lit::parallel::LitTrackScal[nofTracks];
   for(unsigned int i = 0; i < nofTracks; i++) {
      CbmLitTrackToLitTrackScal(fLitTracks[i], &tracksFast[i]);
      CbmLitTrackToLitTrackScal(fLitTracks[i], &tracksSIMD[i]);
   }

   // Test standard fit
   std::cout << "Runing test standard Kalman track fit..." << std::endl;
   TStopwatch timer1;
   timer1.Start();
//    for (int c = 0; c < ntimes; c++) {
//    for(unsigned int i = 0; i < nofTracks; i++){
//       fFitterLitKalman->Fit(fLitTracks[i]);
//    }
//    }
   timer1.Stop();
   // end test standard fit

   // Test fast scalar version of the track fit
   std::cout << "Runing test fast scalar Kalman track fit..." << std::endl;
   TStopwatch timer2;
   timer2.Start();
   if (!fIsElectronSetup) {
      for (int c = 0; c < ntimes; c++) {
         for(unsigned int i = 0; i < nofTracks; i++) {
            lit::parallel::LitTrackFitterMuon(tracksFast[i], layoutScal);
         }
      }
   } else {
      for (int c = 0; c < ntimes; c++) {
         for(unsigned int i = 0; i < nofTracks; i++) {
            lit::parallel::LitTrackFitterElectron(tracksFast[i], layoutElectronScal);
         }
      }
   }
   timer2.Stop();
   // end test fast scalar version of the track fit

   // Test fast SIMD version of the track fit
   std::cout << "Runing test SIMD track fit..." << std::endl;
   // Convert to LitTrackVec
   lit::parallel::LitTrackVec* tracksVec = new lit::parallel::LitTrackVec[nofTracksVec];
   const lit::parallel::LitTrack<fscal>* tmp_trks[fvecLen];
   for(unsigned int i = 0; i < nofTracksVec; i++) {
      for(unsigned char j = 0; j < fvecLen; j++) {
         tmp_trks[j] = &tracksSIMD[fvecLen*i + j];
      }
      for(unsigned short k = 0; k < fLitTracks[0]->GetNofHits(); k++) {
         tracksVec[i].AddHit(new lit::parallel::LitPixelHit<fvec>);
      }
      lit::parallel::PackTrack(tmp_trks, tracksVec[i]);
   }
   TStopwatch timer3;
   timer3.Start();
   if (!fIsElectronSetup) {
      for (int c = 0; c < ntimes; c++) {
         for(unsigned int i = 0; i < nofTracksVec; i++) {
            lit::parallel::LitTrackFitterMuon(tracksVec[i], layoutVec);
         }
      }
   } else {
      for (int c = 0; c < ntimes; c++) {
         for(unsigned int i = 0; i < nofTracksVec; i++) {
            lit::parallel::LitTrackFitterElectron(tracksVec[i], layoutElectronVec);
         }
      }
   }
   timer3.Stop();
   // end test fast scalar version of the track fit

   std::cout << "TStopwatch:" << std::endl;
   std::cout << "LitKalman time, sec (total / per track): Real "
             << timer1.RealTime() << " / " << timer1.RealTime() / nofTracks / ntimes <<  std::endl;
   std::cout << "Fast scalar time, sec (total / per track): Real "
             << timer2.RealTime() << " / " << timer2.RealTime() / nofTracks / ntimes << std::endl;
   std::cout << "Fast SIMD time, sec (total / per track): Real "
             << timer3.RealTime() << " / " << timer3.RealTime() / nofTracks / ntimes <<  std::endl;
}

void CbmLitTrackFitSpeedTest::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmLitTrackFitSpeedTest::Init","CbmRootManager is not instantiated"); }

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if (NULL == fGlobalTracks) { Fatal("CbmLitTrackFitSpeedTest::Init","No GlobalTrack array!"); }

   if (fIsSts) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("CbmLitTrackFitSpeedTest::Init", "No StsTrack array!"); }
   }

   if (fIsMuch) {
      fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
      if (NULL == fMuchTracks) { Fatal("CbmLitTrackFitSpeedTest::Init", "No MuchTrack array!"); }
      fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
      fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
      if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) { Fatal("CbmLitTrackFitSpeedTest::Init", "No MuchPixelHit AND MuchStrawHit arrays!"); }
   }

   if (fIsTrd) {
      fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
      if (NULL == fTrdTracks) { Fatal("CbmLitPropagationAnalysis::Init", "No TrdTrack array!"); }
      fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
      if (NULL == fTrdHits) { Fatal("CbmLitPropagationAnalysis::Init", "No TRDHit array!"); }
   }

//    if (fIsTof) {
//    fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
//    if (NULL == fTofHits) Fatal("CbmLitPropagationAnalysis::Init", "No TofHit array!");
//    }
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

      if (!CheckAcceptance(globalTrack)) { continue; }

      GlobalTrackToLitTrack(globalTrack, litTrack);
      if (fIsElectronSetup) { litTrack->SetPDG(11); }
      else { litTrack->SetPDG(13); }

      if (litTrack->GetNofHits() != fNofPlanes) { continue; }
      litTrack->SortHits();
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
      if (trdTrack->GetNofHits() != fNofTrdHits) { return false; }
   }
   if (fIsMuch && muchId > -1) {
      CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
      if (muchTrack->GetNofHits() != fNofMuchHits) { return false; }
   }

// if (tofId > -1) return true;
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
// //TOF: attach TOF hit
// if (tofId > -1) {
//    CbmPixelHit* tofHit = (CbmPixelHit*) fTofHits->At(tofId);
//    CbmLitPixelHit litHit;
//    CbmLitConverter::PixelHitToLitPixelHit(tofHit, tofId, &litHit);
//    litTrack->AddHit(&litHit);
// }
}

ClassImp(CbmLitTrackFitSpeedTest);
