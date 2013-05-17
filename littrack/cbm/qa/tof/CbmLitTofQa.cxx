/**
 * \file CbmLitTofQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTofQa.h"
#include "CbmLitTofQaReport.h"
#include "CbmHistManager.h"
#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsTrack.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmMCTrack.h"
#include "TClonesArray.h"
#include "TH2F.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using std::vector;
using std::pair;

CbmLitTofQa::CbmLitTofQa():
   fIsFixedBounds(true),
   fOutputDir("./test/"),
   fHM(NULL),
   fPRangeMin(0.),
   fPRangeMax(10.),
   fPRangeBins(20),
   fGlobalTracks(NULL),
   fStsTracks(NULL),
   fStsTrackMatches(NULL),
   fTofHits(NULL),
   fTofPoints(NULL),
   fMCTracks(NULL),
   fPrimVertex(NULL),
   fKFFitter()
{

}

CbmLitTofQa::~CbmLitTofQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitTofQa::Init()
{
   fHM = new CbmHistManager();
   CreateHistograms();
   ReadDataBranches();
   fKFFitter.Init();
   return kSUCCESS;
}

void CbmLitTofQa::Exec(
   Option_t* opt)
{
   static Int_t nofEvents = 0;
   std::cout << "CbmLitFitQaCalculator::Exec: event=" << nofEvents++ << std::endl;
   ProcessGlobalTracks();
}

void CbmLitTofQa::Finish()
{
   fHM->WriteToFile();
   CbmSimulationReport* report = new CbmLitTofQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitTofQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
}

void CbmLitTofQa::CreateHistograms()
{
   fHM->Add("hmp_Tof_Reco_m2p", new TH2F("hmp_Tof_Reco_m2p", "hmp_Tof_Reco_m2p;P [GeV/c];M^{2} [(GeV/c)^{2}]", fPRangeBins, fPRangeMin, fPRangeMax, 100, 0, 1.));
   fHM->Add("hmp_Tof_RecoMCID_m2p", new TH2F("hmp_Tof_RecoMCID_m2p", "hmp_Tof_RecoMCID_m2p;P [GeV/c];M^{2} [(GeV/c)^{2}]", fPRangeBins, fPRangeMin, fPRangeMax, 100, 0, 1.));
}

void CbmLitTofQa::ProcessGlobalTracks()
{
   Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
      Int_t stsId = globalTrack->GetStsTrackIndex();
      Int_t tofId = globalTrack->GetTofHitIndex();
      if (stsId < 0 || tofId < 0) continue; // We need both STS track and TOF hit

      CbmStsTrack* stsTrack = static_cast<CbmStsTrack*>(fStsTracks->At(stsId));
      const CbmTrackMatch* stsMatch = static_cast<const CbmTrackMatch*>(fStsTrackMatches->At(stsId));
      Int_t stsMCTrackId = stsMatch->GetMCTrackId();

      const CbmTofHit* tofHit = static_cast<const CbmTofHit*>(fTofHits->At(tofId));
      Int_t tofMCPointId = tofHit->GetRefId();
      const CbmTofPoint* tofPoint = static_cast<const CbmTofPoint*>(fTofPoints->At(tofMCPointId));
      Int_t tofMCTrackId = tofPoint->GetTrackID();

      FairTrackParam vtxTrack;
      fKFFitter.FitToVertex(stsTrack, fPrimVertex, &vtxTrack);

      Double_t ct = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m
      Double_t trackLength = globalTrack->GetLength() / 100.;
      Double_t preco = (vtxTrack.GetQp() != 0) ? std::abs(1. / vtxTrack.GetQp()) : 0;
      Double_t t = (trackLength != 0) ? (ct / trackLength) : 0;
      Double_t m2reco = preco * preco * (t * t - 1);

      fHM->H1("hmp_Tof_Reco_m2p")->Fill(preco, m2reco);

      if (stsMCTrackId == tofMCTrackId) {
         fHM->H1("hmp_Tof_RecoMCID_m2p")->Fill(preco, m2reco);
      }
   }
}

ClassImp(CbmLitTofQa)
