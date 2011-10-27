/**
 * \file CbmLitPropagationQa.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 **/
#include "qa/propagation/CbmLitPropagationQa.h"

#include "base/CbmLitEnvironment.h"
#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitStripHit.h"
#include "interface/CbmLitTrackUpdate.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackFitter.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmTrackMatch.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchPoint.h"
#include "CbmTrdPoint.h"
#include "CbmMuchGeoScheme.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "TF1.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>
#include <fstream>


CbmLitPropagationQa::CbmLitPropagationQa():
   fIsElectronSetup(false),
   fIsSts(false),
   fIsTrd(false),
   fIsMuch(false),
   fIsTof(false),

   fLitTracks(),
   fLitMcTracks(),

   fNofPlanes(14),
   fPDGCode(-1),

   fNofTrdHits(0),
   fNofMuchHits(13),
   fNofTofHits(1),

   fMCTracks(NULL),
   fGlobalTracks(NULL),
   fStsTracks(NULL),
// fStsTrackMatches(NULL),
   fMuchTracks(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchPoints(NULL),
   fMuchTrackMatches(NULL),
   fMuchPixelDigiMatches(NULL),
   fMuchStrawDigiMatches(NULL),
   fMuchClusters(NULL),
   fTrdTracks(NULL),
   fTrdHits(NULL),
   fTrdPoints(NULL),
   fTrdTrackMatches(NULL),
   fTofPoints(NULL),
   fTofHits(NULL),

   fPropagator(),
   fFilter(),
   fFitter(),
   fSmoother(),
   fParallelFitter(),

   fPropagationHistos(),
   fFilterHistos(),
   fSmootherHistos(),

   fNofParams(12),

   fStsHistos(),

   fPropagationWatch(),
   fFitterWatch(),
   fSmootherWatch(),

   fVerbose(1),
   fEvents(0),

   fIsTestFastPropagation(true),

   fIsDrawPropagation(false),
   fIsDrawFilter(true),
   fIsDrawSmoother(false),

   fIsCloseCanvas(true),

   fOutputDir(""),

   fIsFixedBounds(true),

   fSigma(),
   fRms()
{
}

CbmLitPropagationQa::~CbmLitPropagationQa()
{
}

InitStatus CbmLitPropagationQa::Init()
{
   DetermineSetup();
   ReadDataBranches();

   // Create tools
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   if (!fIsTestFastPropagation) {
      fPropagator = factory->CreateTrackPropagator("lit");
      fFilter = factory->CreateTrackUpdate("kalman");
      fFitter = factory->CreateTrackFitter("lit_kalman");
      fSmoother = factory->CreateTrackFitter("kalman_smoother");
   } else {
      fPropagator = factory->CreateTrackPropagator("lit");
      if (fIsMuch) { fParallelFitter = factory->CreateTrackFitter("kalman_parallel_muon"); }
      else { fParallelFitter = factory->CreateTrackFitter("kalman_parallel_electron"); }
   }

   CreateHistograms();
}

void CbmLitPropagationQa::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitPropagationQa::Exec(
   Option_t* opt)
{
   CreateTrackArrays();
   RunTest();
   DeleteTrackArrays();

   std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitPropagationQa::Finish()
{
   for(Int_t i = 0; i < fNofPlanes; i++) {
      for(Int_t j = 0; j < fNofParams; j++) {
         fPropagationHistos[i][j]->Write();
         fFilterHistos[i][j]->Write();
         fSmootherHistos[i][j]->Write();
      }
   }

   PrintStopwatchStatistics();
   Draw();
}

void CbmLitPropagationQa::DetermineSetup()
{
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fIsElectronSetup = env->IsElectronSetup();
   fIsSts = env->IsSts();
   fIsTrd = env->IsTrd();
   fIsMuch = env->IsMuch();
   fIsTof = env->IsTof();

   if (fIsElectronSetup) { std::cout << "-I- CBM electron setup detected" << std::endl; }
   else { std::cout << "-I- CBM muon setup detected" << std::endl; }
   std::cout << "-I- The following detectors were found in the CBM setup:";
   if (fIsSts) { std::cout << " STS"; }
   if (fIsMuch) { std::cout << " MUCH"; }
   if (fIsTrd) { std::cout << " TRD"; }
   if (fIsTof) { std::cout << " TOF"; }
   std::cout << std::endl;
}

void CbmLitPropagationQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmLitPropagationQa::Init","CbmRootManager is not instantiated"); }

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) { Fatal("CbmLitPropagationQa::Init","No MCTrack array!"); }

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if (NULL == fGlobalTracks) { Fatal("CbmLitPropagationQa::Init","No GlobalTrack array!"); }

   if (fIsSts) {
      fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
      if (NULL == fStsTracks) { Fatal("CbmLitPropagationQa::Init", "No STSTrack array!"); }
//       fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
//       if (NULL == fStsTrackMatches) Fatal("CbmLitPropagationQa::Init", "No StsTrackMatch array!");
   }

   if (fIsMuch) {
      fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
      if (NULL == fMuchTracks) { Fatal("CbmLitPropagationQa::Init", "No MuchTrack array!"); }
      fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
      fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
      if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) { Fatal("CbmLitPropagationQa::Init", "No MuchPixelHit AND MuchStrawHit arrays!"); }
      fMuchTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchTrackMatches) { Fatal("CbmLitPropagationQa::Init", "No MuchTrackMatch array!"); }
      fMuchPixelDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
      fMuchStrawDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");
      if (NULL == fMuchPixelDigiMatches && NULL == fMuchStrawDigiMatches) { Fatal("CbmLitPropagationQa::Init", "No MuchDigiMatches array!"); }
      fMuchClusters  = (TClonesArray*) ioman->GetObject("MuchCluster");
      if (NULL == fMuchClusters) { Fatal("CbmLitPropagationQa::Init", "No MuchCluster array!"); }
      fMuchPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
      if (NULL == fMuchPoints) { Fatal("CbmLitPropagationQa::Init", "No MuchPoint array!"); }
   }

   if (fIsTrd) {
      fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
      if (NULL == fTrdTracks) { Fatal("CbmLitPropagationQa::Init", "No TrdTrack array!"); }
      fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
      if (NULL == fTrdHits) { Fatal("CbmLitPropagationQa::Init", "No TRDHit array!"); }
      fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (!fTrdTrackMatches) { Fatal("CbmLitPropagationQa::Init", "No TrdTrackMatch array!"); }
      fTrdPoints  = (TClonesArray*) ioman->GetObject("TrdPoint");
      if (NULL == fTrdPoints) { Fatal("CbmLitPropagationQa::Init", "No TRDPoint array!"); }
   }

   if (fIsTof) {
      fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
      if (NULL == fTofPoints) { Fatal("CbmLitPropagationQa::Init", "No TofPoint array!"); }
      fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
      if (NULL == fTofHits) { Fatal("CbmLitPropagationQa::Init", "No TofHit array!"); }
   }
}

void CbmLitPropagationQa::CreateHistograms()
{
   //0,1,2,3,4 - residuals (x,y,tx,ty,q/p)
   //5,6,7,8,9 - pulls (x,y,tx,ty,q/p)
   //11 - relative momentum error
   //12 - chi-square of the track param and hit

   fPropagationHistos.resize(fNofPlanes);
   fFilterHistos.resize(fNofPlanes);
   fSmootherHistos.resize(fNofPlanes);
   for(Int_t i = 0; i < fNofPlanes; i++) {
      fPropagationHistos[i].resize(fNofParams);
      fFilterHistos[i].resize(fNofParams);
      fSmootherHistos[i].resize(fNofParams);
   }

   std::string names[] = { "h_resx", "h_resy", "h_restx", "h_resty", "h_resqp",
                           "h_pullx", "h_pully", "h_pulltx", "h_pullty", "h_pullqp",
                           "h_resp", "h_chisq"
                         };

   Int_t bins[] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
   Double_t boundL[fNofParams];
   Double_t boundR[fNofParams];
   if (fIsFixedBounds) {
      boundL[0] = -10.;
      boundL[1] = -10.;
      boundL[2] = -.15;
      boundL[3] = -.15;
      boundL[4] = -.1;
      boundL[5] =-5.;
      boundL[6] = -5.;
      boundL[7] = -5.;
      boundL[8] = -5.;
      boundL[9] =-7.;
      boundL[10] =-30.;
      boundL[11] =-1.;
      boundR[0] =  10.;
      boundR[1] = 10.;
      boundR[2] = .15;
      boundR[3] = .15;
      boundR[4] = .1;
      boundR[5] = 5.;
      boundR[6] = 5.;
      boundR[7] = 5.;
      boundR[8] = 5.;
      boundR[9] = 7.;
      boundR[10] = 30.;
      boundR[11] = 20.;
   } else {
      for (Int_t i = 0; i < fNofParams; i++) {
         boundL[i] = 0.;
         boundR[i] = 0.;
      }
   }
   std::string var[] = {"p", "f", "s"};
   std::string var2[] = {"[propagation]", "[filter]", "[smoother]"};

   for (Int_t v = 0; v < 3; v++) {
      for (Int_t i = 0; i < fNofPlanes; i++) {
         for (Int_t j = 0; j < fNofParams; j++) {
            std::stringstream histName;
            histName << names[j] << "_" << var[v] << "_" << i;
            TH1F* hist = new TH1F(histName.str().c_str(), histName.str().c_str(),
                                  bins[j], boundL[j], boundR[j]);
            if (v == 0) { fPropagationHistos[i][j] = hist; }
            if (v == 1) { fFilterHistos[i][j] = hist; }
            if (v == 2) { fSmootherHistos[i][j] = hist; }
         }
      }
   }

   fStsHistos.resize(2);
   fStsHistos[0].resize(fNofParams);
   fStsHistos[1].resize(fNofParams);
   std::string vr[] = {"stsl","stsf"};
   for (Int_t v = 0; v < 2; v++) {
      for (Int_t j = 0; j < fNofParams; j++) {
         std::stringstream histName;
         histName << names[j] << "_" << vr[v];
         TH1F* hist = new TH1F(histName.str().c_str(), histName.str().c_str(),
                               bins[j], boundL[j], boundR[j]);
         fStsHistos[v][j] = hist;
      }
   }
}

void CbmLitPropagationQa::CreateTrackArrays()
{
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
      CbmLitTrack* litTrack = new CbmLitTrack();
      CbmLitTrack* mcLitTrack = new CbmLitTrack();

      if (!CheckAcceptance(globalTrack)) { continue; }

      GlobalTrackToLitTrack(globalTrack, litTrack);
      if (fPDGCode < 0) {
         if (fIsElectronSetup) { litTrack->SetPDG(11); }
         else { litTrack->SetPDG(13); }
      } else { litTrack->SetPDG(fPDGCode); }

      GlobalTrackToMCLitTrack(globalTrack, mcLitTrack);

      fLitTracks.push_back(litTrack);
      fLitMcTracks.push_back(mcLitTrack);
   }

   if (fLitTracks.size() != fLitMcTracks.size()) {
      Fatal("CbmLitPropagationQa", "LitTrack vector size != LitMcVector size");
   }
}

Bool_t CbmLitPropagationQa::CheckAcceptance(
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

   if (tofId > -1) { return true; }
   return true;
}

void CbmLitPropagationQa::GlobalTrackToLitTrack(
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
   if (tofId > -1) {
      CbmPixelHit* tofHit = (CbmPixelHit*) fTofHits->At(tofId);
      CbmLitPixelHit litHit;
      CbmLitConverter::PixelHitToLitPixelHit(tofHit, tofId, &litHit);
      litTrack->AddHit(&litHit);
   }
}

void CbmLitPropagationQa::GlobalTrackToMCLitTrack(
   const CbmGlobalTrack* globalTrack,
   CbmLitTrack* litTrack)
{
   Int_t trdId = globalTrack->GetTrdTrackIndex();
   Int_t muchId = globalTrack->GetMuchTrackIndex();
   Int_t tofId = globalTrack->GetTofHitIndex();

   CbmTrdTrack* trdTrack;
   CbmMuchTrack* muchTrack;
   Int_t nofTrdHits = 0, nofMuchHits = 0, nofTofHits = 0;

   if (muchId > -1) {
      muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
      nofMuchHits = muchTrack->GetNofHits();
   }
   if (trdId > -1) {
      trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
      nofTrdHits = trdTrack->GetNofHits();
   }
   if (tofId > -1) { nofTofHits = 1; }

   FitNodeVector nodes(nofTrdHits + nofMuchHits + nofTofHits);
   Int_t counter = 0;

   //MUCH
   if (muchId > -1) {
      for (Int_t i = 0; i < nofMuchHits; i++) {
         Int_t hitIndex = muchTrack->GetHitIndex(i);
         HitType hitType = muchTrack->GetHitType(i);
         if (hitType == kMUCHPIXELHIT) {
            CbmPixelHit* hit = (CbmPixelHit*) fMuchPixelHits->At(hitIndex);
            Int_t clusterIndex = hit->GetRefId();
            CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(clusterIndex);
            Int_t digiIndex = cluster->GetDigiIndex(0);
            CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchPixelDigiMatches->At(digiIndex);
            Int_t pointIndex = digiMatch->GetRefIndex(0);
            FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(pointIndex);
            McPointToLitFitNode(point, &nodes[counter++]);
         } else if (hitType == kMUCHSTRAWHIT) {
            CbmStripHit* hit = (CbmStripHit*) fMuchStrawHits->At(hitIndex);
            Int_t digiIndex = hit->GetRefId();
            CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchStrawDigiMatches->At(digiIndex);
            Int_t pointIndex = digiMatch->GetRefIndex(0);
            FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(pointIndex);
            McPointToLitFitNode(point, &nodes[counter++]);
         }
      }
   }

   //TRD
   if (trdId > -1) {
      for (Int_t i = 0; i < nofTrdHits; i++) {
         Int_t hitIndex = trdTrack->GetHitIndex(i);
         CbmPixelHit* hit = (CbmPixelHit*) fTrdHits->At(hitIndex);
         Int_t pointIndex = hit->GetRefId();
         FairMCPoint* point = (FairMCPoint*) fTrdPoints->At(pointIndex);
         McPointToLitFitNode(point, &nodes[counter++]);
      }
   }

   //TOF
   if (tofId > -1) {
      CbmPixelHit* hit = (CbmPixelHit*) fTofHits->At(tofId);
      Int_t pointIndex = hit->GetRefId();
      FairMCPoint* point = (FairMCPoint*) fTofPoints->At(pointIndex);
      McPointToLitFitNode(point, &nodes[counter++]);
   }

   litTrack->SetFitNodes(nodes);
}

void CbmLitPropagationQa::DeleteTrackArrays()
{
   for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
   for_each(fLitMcTracks.begin(), fLitMcTracks.end(), DeleteObject());
   fLitTracks.clear();
   fLitMcTracks.clear();
}

void CbmLitPropagationQa::RunTest()
{
   for(Int_t i = 0; i < fLitTracks.size(); i++) {
      if (fLitTracks[i]->GetNofHits() != fNofPlanes) { continue; }
      fLitTracks[i]->SortHits();
      if (!fIsTestFastPropagation) {
         TestPropagation(fLitTracks[i], fLitMcTracks[i]);
         TestFitter(fLitTracks[i], fLitMcTracks[i]);
      } else {
         TestFastPropagation(fLitTracks[i], fLitMcTracks[i]);
      }
   }
}

void CbmLitPropagationQa::TestPropagation(
   const CbmLitTrack* track,
   const CbmLitTrack* mcTrack)
{
   CbmLitTrackParam par(*track->GetParamLast());
   fPropagationWatch.Start(kFALSE);
   for (Int_t i = 0; i < track->GetNofHits(); i++) {
      Double_t zOut = track->GetHit(i)->GetZ();
      if (fPropagator->Propagate(&par, zOut, track->GetPDG()) == kLITERROR) { continue; }
//         if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
      FillHistosPropagation(&par, mcTrack->GetFitNode(i)->GetPredictedParam(), track->GetHit(i), i);
//         if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
   }
   fPropagationWatch.Stop();
}

void CbmLitPropagationQa::TestFitter(
   CbmLitTrack* track,
   const CbmLitTrack* mcTrack)
{
   LitStatus fitStatus, smoothStatus;
   fFitterWatch.Start(kFALSE);
   fitStatus = fFitter->Fit(track);
   fFitterWatch.Stop();
   fSmootherWatch.Start(kFALSE);
   if (fitStatus == kLITSUCCESS) { smoothStatus = fSmoother->Fit(track); }
   fSmootherWatch.Stop();
   if (fitStatus == kLITSUCCESS && smoothStatus == kLITSUCCESS) {
      FillHistosFitter(track, mcTrack);
   }
}

void CbmLitPropagationQa::TestFastPropagation(
   CbmLitTrack* track,
   CbmLitTrack* mcTrack)
{
   // Propagate tracks to the last STS station
   CbmLitTrackParam par(*track->GetParamLast());
   fPropagator->Propagate(&par, 100., track->GetPDG());
   track->SetParamLast(&par);
   LitStatus fitStatus = fParallelFitter->Fit(track);
   if (fitStatus == kLITSUCCESS) {
      for (Int_t i = 0; i < track->GetNofHits(); i++) {
         FillHistosPropagation(track->GetFitNode(i)->GetPredictedParam(), mcTrack->GetFitNode(i)->GetPredictedParam(), track->GetHit(i), i);
         FillHistosFilter(track->GetFitNode(i)->GetUpdatedParam(), mcTrack->GetFitNode(i)->GetPredictedParam(), track->GetHit(i), i, track->GetFitNode(i)->GetChiSqFiltered());
      }
   }
}
void CbmLitPropagationQa::FillHistosPropagation(
   const CbmLitTrackParam* par,
   const CbmLitTrackParam* mcPar,
   const CbmLitHit* hit,
   Int_t plane)
{
   std::vector<Double_t> r = CalcResidualsAndPulls(par, mcPar);
   for (Int_t i = 0; i < 11; i++) {
      fPropagationHistos[plane][i]->Fill(r[i]);
   }
   fPropagationHistos[plane][11]->Fill(0.);//ChiSq(par, hit));
}

void CbmLitPropagationQa::FillHistosFilter(
   const CbmLitTrackParam* par,
   const CbmLitTrackParam* mcPar,
   const CbmLitHit* hit,
   Int_t plane,
   float chisq)
{
   std::vector<Double_t> r = CalcResidualsAndPulls(par, mcPar);
   for (Int_t i = 0; i < 11; i++) {
      fFilterHistos[plane][i]->Fill(r[i]);
   }
   fFilterHistos[plane][11]->Fill(chisq);//ChiSq(par, hit));
}

void CbmLitPropagationQa::FillHistosFitter(
   const CbmLitTrack* track,
   const CbmLitTrack* mcTrack)
{
   Int_t nofHits = track->GetNofHits();
   for (Int_t i = 0; i < nofHits; i++) {
      const CbmLitFitNode* node = track->GetFitNode(i);
      std::vector<Double_t> rFilter = CalcResidualsAndPulls(node->GetUpdatedParam(), mcTrack->GetFitNode(i)->GetPredictedParam());
      std::vector<Double_t> rSmoother = CalcResidualsAndPulls(node->GetSmoothedParam(), mcTrack->GetFitNode(i)->GetPredictedParam());
      for (Int_t j = 0; j < 11; j++) {
         fFilterHistos[i][j]->Fill(rFilter[j]);
         fSmootherHistos[i][j]->Fill(rSmoother[j]);
      }
      fFilterHistos[i][11]->Fill(node->GetChiSqFiltered());
      fSmootherHistos[i][11]->Fill(node->GetChiSqSmoothed());
   }
}

void CbmLitPropagationQa::McPointToLitFitNode(
   FairMCPoint* point,
   CbmLitFitNode* node)
{
   Int_t mcId = point->GetTrackID();
   CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcId);
   Int_t pdg = mcTrack->GetPdgCode();
   Double_t q;
   if (pdg > 0) { q = -1.; }
   else { q = 1; }
   if (pdg == 13) { q = -1.; }
   if (pdg == -13) { q = 1.; }
   if (pdg == 11) { q = -1.; }
   if (pdg == -11) { q = 1.; }
   if (pdg == 211) { q = 1.; }
   if (pdg == -211) { q = -1.; }

   TVector3 mom;
   point->Momentum(mom);
   Double_t qp = q / mom.Mag();

   CbmLitTrackParam par;

   CbmMuchPoint* muchPoint = dynamic_cast<CbmMuchPoint*>(point);
   if (muchPoint != NULL) {
      par.SetX(point->GetX());
      par.SetY(point->GetY());
      par.SetZ(point->GetZ());
   }

   CbmTrdPoint* trdPoint = dynamic_cast<CbmTrdPoint*>(point);
   if (trdPoint != NULL) {
      par.SetX(trdPoint->GetXOut());
      par.SetY(trdPoint->GetYOut());
      par.SetZ(trdPoint->GetZOut());
   }

   //TODO: temporarily done to check the straw tubes
//    CbmMuchPoint* p = (CbmMuchPoint*) point;
//    par.SetX((p->GetXIn()+p->GetXOut())/2.);
//    par.SetY((p->GetYIn()+p->GetYOut())/2.);
//    par.SetZ((p->GetZIn()+p->GetZOut())/2.);
   //
   par.SetTx(point->GetPx()/point->GetPz());
   par.SetTy(point->GetPy()/point->GetPz());
   par.SetQp(qp);

   node->SetPredictedParam(&par);
}

std::vector<Double_t> CbmLitPropagationQa::CalcResidualsAndPulls(
   const CbmLitTrackParam* par,
   const CbmLitTrackParam* mcPar)
{
   std::vector<Double_t> r(11);
   r[0] = par->GetX() - mcPar->GetX();
   r[1] = par->GetY() - mcPar->GetY();
   r[2] = par->GetTx() - mcPar->GetTx();
   r[3] = par->GetTy() - mcPar->GetTy();
   r[4] = par->GetQp() - mcPar->GetQp();
   if (par->GetCovariance(0) > 0.) { r[5] = (r[0]) / (std::sqrt(par->GetCovariance(0))); }
   if (par->GetCovariance(5) > 0.) { r[6] = (r[1]) / (std::sqrt(par->GetCovariance(5))); }
   if (par->GetCovariance(9) > 0.) { r[7] = (r[2]) / (std::sqrt(par->GetCovariance(9))); }
   if (par->GetCovariance(12) > 0.) { r[8] = (r[3]) / (std::sqrt(par->GetCovariance(12))); }
   if (par->GetCovariance(14) > 0.) { r[9] = (r[4]) / (std::sqrt(par->GetCovariance(14))); }
//    r[10] = 100 * ((1./par->GetQp() - 1./mcPar->GetQp()) / (1./mcPar->GetQp()));
   r[10] = 100 * ((1./par->GetQp() - 1./mcPar->GetQp()) / (std::abs(1./mcPar->GetQp())));
   return r;
}

void CbmLitPropagationQa::PrintStopwatchStatistics()
{
   std::cout << "Stopwatch: " << std::endl;
   std::cout << "propagation: counts=" << fPropagationWatch.Counter()
             << ", real=" << fPropagationWatch.RealTime()/fPropagationWatch.Counter()
             << "/" << fPropagationWatch.RealTime()
             << " s, cpu=" << fPropagationWatch.CpuTime()/fPropagationWatch.Counter()
             << "/" << fPropagationWatch.CpuTime() << std::endl;
   std::cout << "fitter: real=" << fFitterWatch.Counter()
             << ", real=" << fFitterWatch.RealTime()/fFitterWatch.Counter()
             << "/" << fFitterWatch.RealTime()
             << " s, cpu=" << fFitterWatch.CpuTime()/fFitterWatch.Counter()
             << "/" << fFitterWatch.CpuTime() << std::endl;
   std::cout << "smoother: real=" << fSmootherWatch.Counter()
             << ", real=" << fSmootherWatch.RealTime()/fSmootherWatch.Counter()
             << "/" << fSmootherWatch.RealTime()
             << " s, cpu=" << fSmootherWatch.CpuTime()/fSmootherWatch.Counter()
             << "/" << fSmootherWatch.CpuTime() << std::endl;
}

void CbmLitPropagationQa::Draw()
{
   SetStyles();

   // Canvases for drawing.
   // First index is parameter (0-propagation, 1-filter, 2-smoother).
   // Second index is a plane number.
   TCanvas* canvas[3][fNofPlanes];
   std::string cnames[] = {"c_propagation_", "c_filter_", "c_smoother_"};
   for (int c = 0; c < 3; c++) {
      if (c == 0 && !fIsDrawPropagation) { continue; }
      if (c == 1 && !fIsDrawFilter) { continue; }
      if (c == 2 && !fIsDrawSmoother) { continue; }
      for (int i = 0; i < fNofPlanes; i++) {
         std::string name = cnames[c] + lit::ToString<int>(i);
         canvas[c][i] = new TCanvas(name.c_str(),name.c_str(),1200,800);
         canvas[c][i]->Divide(4, 3);
      }
   }

   // resize vectors
   fSigma.resize(3);
   fRms.resize(3);
   for (int c = 0; c < 3; c++) {
      fSigma[c].resize(fNofPlanes);
      fRms[c].resize(fNofPlanes);
      for (int i = 0; i < fNofPlanes; i++) {
         fSigma[c][i].resize(fNofParams);
         fRms[c][i].resize(fNofParams);
      }
   }

   if (fIsDrawPropagation) {
      DrawHistos(canvas[0], 0);
   }
   if (fIsDrawFilter) {
      DrawHistos(canvas[1], 1);
   }
   if (fIsDrawSmoother) {
      DrawHistos(canvas[2], 2);
   }

   std::ofstream fout(std::string(fOutputDir + "propagation_analysis.txt").c_str());
   if (fIsDrawPropagation) {
      PrintResults(std::cout, 0);
      PrintResults(fout, 0);
   }
   if (fIsDrawFilter) {
      PrintResults(std::cout, 1);
      PrintResults(fout, 1);
   }
   if (fIsDrawSmoother) {
      PrintResults(std::cout, 2);
      PrintResults(fout, 2);
   }

//   DrawForPhd();
}

void CbmLitPropagationQa::DrawHistos(
   TCanvas* c[],
   int v)
{
   std::string xtitles[] = {"Residual X [cm]", "Residual Y [cm]", "Residual Tx",
                            "Residual Ty", "Residual q/p [(GeV/c)^{-1}]", "Pull X", "Pull Y", "Pull Tx", "Pull Ty",
                            "Pull q/p", "Resolution p [%]", "Chi-square"
                           };
   for (int plane = 0; plane < fNofPlanes; plane++) {
      for (int i = 0; i < fNofParams; i++) {
         c[plane]->cd(i+1);
         TH1F* hist = NULL;
         if (v == 0) { hist = fPropagationHistos[plane][i]; }
         if (v == 1) { hist = fFilterHistos[plane][i]; }
         if (v == 2) { hist = fSmootherHistos[plane][i]; }
         if (i != 11) { hist->Fit("gaus"); }
         hist->SetMaximum(hist->GetMaximum() * 1.50);
         DrawHist1D(hist, xtitles[i], "Counter",
                    LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
                    LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "");

         TF1* fit = hist->GetFunction("gaus");
         Double_t sigma = 0., rms = 0.;
         if (fit) {
            fSigma[v][plane][i] = sigma = fit->GetParameter(2);
            fRms[v][plane][i] = rms = hist->GetRMS();
         }
         DrawHistSigmaRMS(i, sigma, rms);
      }
      lit::SaveCanvasAsImage(c[plane], fOutputDir);
      if (fIsCloseCanvas) { c[plane]->Close(); }
   }
}

//void CbmLitPropagationQa::DrawForPhd()
//{
//   const Int_t nofHist = 4;
//   TCanvas* canvas[3];
//   std::string cnames[] = {"c_phd_propagation_", "c_phd_filter_", "c_phd_smoother_"};
//   for (int c = 0; c < 3; c++) {
//      if (c == 0 && !fIsDrawPropagation) { continue; }
//      if (c == 1 && !fIsDrawFilter) { continue; }
//      if (c == 2 && !fIsDrawSmoother) { continue; }
//      std::string name = cnames[c] + lit::ToString<int>(fPlaneNoPhd);
//      canvas[c] = new TCanvas(name.c_str(),name.c_str(),1600,450);
//      canvas[c]->Divide(nofHist, 1);
//   }
//
//   if (fIsDrawPropagation) { DrawForPhd(canvas[0], 0); }
//   if (fIsDrawFilter) { DrawForPhd(canvas[1], 1); }
//   if (fIsDrawSmoother) { DrawForPhd(canvas[2], 2); }
//}
//
//void CbmLitPropagationQa::DrawForPhd(
//   TCanvas* canvas,
//   Int_t v)
//{
//   Int_t param[] = {0, 5, 8, 4}; // indecis of the parameters that will be drawn
//   std::string xtitles[] = {"Residual X [cm]", "Residual Y [cm]", "Residual Tx",
//                            "Residual Ty", "Residual q/p [(GeV/c)^{-1}]", "Pull X", "Pull Y", "Pull Tx", "Pull Ty",
//                            "Pull q/p", "Resolution p [%]", "Chi-square"
//                           };
//   for (int j = 0; j < 4; j++) {
//      canvas->cd(j+1);
//      TH1F* hist = NULL;
//      if (v == 0) { hist = fPropagationHistos[fPlaneNoPhd][param[j]]; }
//      if (v == 1) { hist = fFilterHistos[fPlaneNoPhd][param[j]]; }
//      if (v == 2) { hist = fSmootherHistos[fPlaneNoPhd][param[j]]; }
//      DrawHist1D(hist, xtitles[param[j]], "Counter",
//                 LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
//                 LIT_MARKER_STYLE1, false, false, "");
//
//      hist->Fit("gaus");
//      TF1* fit = hist->GetFunction("gaus");
//      Double_t sigma = fit->GetParameter(2);
//      Double_t rms = hist->GetRMS();
//      DrawHistSigmaRMS(j, sigma, rms);
//   }
//   lit::SaveCanvasAsImage(canvas, fOutputDir);
//   if (fIsCloseCanvas) { canvas->Close(); }
//}

void CbmLitPropagationQa::PrintResults(
   std::ostream& out,
   int v)
{
   std::string var[] = {"[propagation]", "[filter]", "[smoother]"};
   out << var[v] << std::endl;

   out.precision(3);
   out << "plane     resx             resy           restx             resty           resqp            pullx         pully        pulltx       pullty      pullqp      resmom        chisq" << std::endl;
   for (int i = 0; i < fNofPlanes; i++) {
      out << std::setw(3) << i << " ";
      for (int j = 0; j < fNofParams; j++) {
         int w = (j < 5)? 7 : 5;
         out << std::setw(w) << fSigma[v][i][j] << "(" << std::setw(w) << fRms[v][i][j] << ") ";
      }
      out << std::endl;
   }
}

ClassImp(CbmLitPropagationQa);