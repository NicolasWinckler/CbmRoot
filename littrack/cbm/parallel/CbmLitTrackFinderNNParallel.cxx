#include "cbm/parallel/CbmLitTrackFinderNNParallel.h"

#include "cbm/base/CbmLitEnvironment.h"
#include "std/data/CbmLitHit.h"
#include "std/data/CbmLitPixelHit.h"
#include "std/data/CbmLitTrackParam.h"
#include "std/data/CbmLitTrack.h"
#include "std/utils/CbmLitMemoryManagment.h"

#include "parallel/LitTypes.h"
#include "parallel/LitHit.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"
#include "parallel/LitTrack.h"
#include "parallel/LitConverter.h"
#include "parallel/muon/LitTrackFinderNNVecMuon.h"
#include "parallel/electron/LitTrackFinderNNVecElectron.h"

#include "TStopwatch.h"

#include <iostream>


CbmLitTrackFinderNNParallel::CbmLitTrackFinderNNParallel(
   const std::string& trackingType):
   fTime(0.),
   fEventNo(1),
   fTrackingType(trackingType)
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();

   if (fTrackingType == "nn_parallel_muon") {
      lit::parallel::LitDetectorLayoutMuonVec layout;
      env->GetMuchLayoutVec(layout);
      fTFParallelMuon = new lit::parallel::LitTrackFinderNNVecMuon();
      fTFParallelMuon->SetDetectorLayout(layout);
   } else if (fTrackingType == "nn_parallel_electron") {
      lit::parallel::LitDetectorLayoutElectronVec layout;
      env->GetTrdLayoutVec(layout);
      fTFVecElectron = new lit::parallel::LitTrackFinderNNVecElectron();
      fTFVecElectron->SetDetectorLayout(layout);
   } else {
      std::cout << "-E- CbmLitTrackFinderNNParallel: TRACKING TYPE NOT FOUND!" << std::endl;
      exit(0);
   }
}

CbmLitTrackFinderNNParallel::~CbmLitTrackFinderNNParallel()
{
}

LitStatus CbmLitTrackFinderNNParallel::DoFind(
   HitPtrVector& hits,
   TrackPtrVector& trackSeeds,
   TrackPtrVector& tracks)
{
   std::vector<lit::parallel::LitScalPixelHit*> lhits;
   std::vector<lit::parallel::LitScalTrack*> lseeds;
   std::vector<lit::parallel::LitScalTrack*> ltracks;

   ConvertHits(hits, lhits);
   ConvertSeeds(trackSeeds, lseeds);

   TStopwatch timer1;
   timer1.Start(kFALSE);

   if (fTrackingType == "nn_parallel_muon") {
      fTFParallelMuon->DoFind(lhits, lseeds, ltracks);
   } else if (fTrackingType == "nn_parallel_electron") {
      fTFVecElectron->DoFind(lhits, lseeds, ltracks);
   }
   timer1.Stop();

   double dtime = timer1.RealTime();

   fTime += dtime;
   std::cout << "Time: " << dtime << " sec" << std::endl;
   std::cout << "Total:" << fTime << " sec / " << fEventNo << " events." << std::endl;
   std::cout << "Average per event:" << fTime / fEventNo << " sec." << std::endl;

   ConvertTracks(ltracks, tracks);

   for_each(lhits.begin(), lhits.end(), DeleteObject());
   for_each(lseeds.begin(), lseeds.end(), DeleteObject());
   for_each(ltracks.begin(), ltracks.end(), DeleteObject());
   lhits.clear();
   lseeds.clear();
   ltracks.clear();

   std::cout << "Event: " << fEventNo++ << std::endl;

   return kLITSUCCESS;
}

void CbmLitTrackFinderNNParallel::ConvertHits(
   HitPtrVector& hits,
   std::vector<lit::parallel::LitScalPixelHit*>& lhits)
{
   for (unsigned int i = 0; i < hits.size(); i++) {
      CbmLitPixelHit* hit = (CbmLitPixelHit*) hits[i];
      lit::parallel::LitScalPixelHit* lhit = new lit::parallel::LitScalPixelHit;
      CbmLitPixelHitToLitScalPixelHit(hit, lhit);
      lhits.push_back(lhit);
   }
}

void CbmLitTrackFinderNNParallel::ConvertSeeds(
   TrackPtrVector& seeds,
   std::vector<lit::parallel::LitScalTrack*>& lseeds)
{
   for (unsigned int i = 0; i < seeds.size(); i++) {
      CbmLitTrack* track = (CbmLitTrack*) seeds[i];
      const CbmLitTrackParam* par = track->GetParamLast();
      lit::parallel::LitScalTrack* ltrack = new lit::parallel::LitScalTrack();

      lit::parallel::LitTrackParamScal lpar;
      CbmLitTrackParamToLitTrackParamScal(par, &lpar);
      ltrack->SetParamFirst(lpar);
      ltrack->SetParamLast(lpar);
      lseeds.push_back(ltrack);
   }
}

void CbmLitTrackFinderNNParallel::ConvertTracks(
   const std::vector<lit::parallel::LitScalTrack*>& ltracks,
   TrackPtrVector& tracks)
{
   for (unsigned int i = 0; i < ltracks.size(); i++) {
      lit::parallel::LitScalTrack* ltrack = ltracks[i];
      CbmLitTrack* track = new CbmLitTrack;
      if (fTrackingType == "nn_parallel_muon") {
         LitScalTrackToCbmLitTrack(ltrack, track, kLITMUCH);
      } else if (fTrackingType == "nn_parallel_electron") {
         LitScalTrackToCbmLitTrack(ltrack, track, kLITTRD);
      }
      tracks.push_back(track);
   }
}
