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
using boost::assign::list_of;

Bool_t AllTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   return index > -1;
}

Bool_t ElectronTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return std::abs(mcTrack->GetPdgCode()) == 11;
}

Bool_t MuonTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return std::abs(mcTrack->GetPdgCode()) == 13;
}

Bool_t ProtonTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return std::abs(mcTrack->GetPdgCode()) == 2212;
}

Bool_t PionTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return std::abs(mcTrack->GetPdgCode()) == 211;
}

Bool_t KaonTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return std::abs(mcTrack->GetPdgCode()) == 321;
}

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
   fKFFitter(),
   fTrackCategories(),
   fTrackAcceptanceFunctions()
{
   FillTrackCategoriesAndAcceptanceFunctions();
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
   std::cout << "CbmLitTofQa::Exec: event=" << nofEvents++ << std::endl;
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

void CbmLitTofQa::FillTrackCategoriesAndAcceptanceFunctions()
{
   fTrackCategories = list_of("All")("Electron")("Muon")("Proton")("Pion")("Kaon");
   // List of all supported track categories
   fTrackAcceptanceFunctions["All"] = AllTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Electron"] = ElectronTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Muon"] = MuonTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Proton"] = ProtonTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Pion"] = PionTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Kaon"] = KaonTrackAcceptanceFunctionTof;
}

void CbmLitTofQa::CreateHistograms()
{
   Int_t nofTrackCategories = fTrackCategories.size();
   for (Int_t iCat = 0; iCat < nofTrackCategories; iCat++) {
	  string name = "hmp_Tof_Reco_" + fTrackCategories[iCat] + "_m2p";
      fHM->Add(name, new TH2F(name.c_str(), string(name + ";P [GeV/c];M^{2} [(GeV/c)^{2}]").c_str(), fPRangeBins, fPRangeMin, fPRangeMax, 100, -0.2, 1.8));
      name = "hmp_Tof_RecoMCID_" + fTrackCategories[iCat] + "_m2p";
      fHM->Add(name, new TH2F(name.c_str(), string(name + ";P [GeV/c];M^{2} [(GeV/c)^{2}]").c_str(), fPRangeBins, fPRangeMin, fPRangeMax, 100, -0.2, 1.8));
   }
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

      Int_t nofTrackCategories = fTrackCategories.size();
      for (Int_t iCat = 0; iCat < nofTrackCategories; iCat++) {
    	  string category = fTrackCategories[iCat];
    	  LitTrackAcceptanceFunction function = fTrackAcceptanceFunctions.find(category)->second;
    	  Bool_t accOk = function(fMCTracks, stsMCTrackId);

    	  if (accOk) {
    		  fHM->H1("hmp_Tof_Reco_" + category + "_m2p")->Fill(preco, m2reco);

    		  if (stsMCTrackId == tofMCTrackId) {
    		     fHM->H1("hmp_Tof_RecoMCID_" + category + "_m2p")->Fill(preco, m2reco);
    		  }
    	  }
      }
   }
}

ClassImp(CbmLitTofQa)
