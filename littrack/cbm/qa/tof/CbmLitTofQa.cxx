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
#include "CbmKFVertex.h"
#include "TClonesArray.h"
#include "TH2F.h"
#include <boost/assign/list_of.hpp>
#include <vector>
#include <cmath>
using std::vector;
using std::pair;
using boost::assign::list_of;
using std::min;
using std::sqrt;

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
   return mcTrack->GetPdgCode() == 2212;
}

Bool_t AntiProtonTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return mcTrack->GetPdgCode() == -2212;
}

Bool_t PionPlusTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return mcTrack->GetPdgCode() == 211;
}

Bool_t PionMinusTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return mcTrack->GetPdgCode() == -211;
}

Bool_t KaonPlusTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return mcTrack->GetPdgCode() == 321;
}

Bool_t KaonMinusTrackAcceptanceFunctionTof(
      const TClonesArray* mcTracks,
      Int_t index)
{
   if (index < 0) return false;
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
   return mcTrack->GetPdgCode() == -321;
}

CbmLitTofQa::CbmLitTofQa():
   fIsFixedBounds(true),
   fOutputDir("./test/"),
   fHM(NULL),
   fPRangeMin(0.),
   fPRangeMax(15.),
   fPRangeBins(400),
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
   FitHistograms();
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
   fTrackCategories = list_of("All")("Electron")("Muon")("Proton")("AntiProton")("PionPlus")("PionMinus")("KaonPlus")("KaonMinus");
   // List of all supported track categories
   fTrackAcceptanceFunctions["All"] = AllTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Electron"] = ElectronTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Muon"] = MuonTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["Proton"] = ProtonTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["AntiProton"] = AntiProtonTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["PionPlus"] = PionPlusTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["PionMinus"] = PionMinusTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["KaonPlus"] = KaonPlusTrackAcceptanceFunctionTof;
   fTrackAcceptanceFunctions["KaonMinus"] = KaonMinusTrackAcceptanceFunctionTof;
}

void CbmLitTofQa::CreateHistograms()
{
   Int_t nofTrackCategories = fTrackCategories.size();
   for (Int_t iCat = 0; iCat < nofTrackCategories; iCat++) {
	  string name = "hmp_Tof_Reco_" + fTrackCategories[iCat] + "_m2p";
      fHM->Add(name, new TH2F(name.c_str(), string(name + ";P [GeV/c];M^{2} [(GeV/c)^{2}]").c_str(), fPRangeBins, fPRangeMin, fPRangeMax, 400, -0.2, 1.8));
      name = "hmp_Tof_RecoMCID_" + fTrackCategories[iCat] + "_m2p";
      fHM->Add(name, new TH2F(name.c_str(), string(name + ";P [GeV/c];M^{2} [(GeV/c)^{2}]").c_str(), fPRangeBins, fPRangeMin, fPRangeMax, 400, -0.2, 1.8));
   }
   string name = "hmp_Tof_dTime";
   fHM->Add(name, new TH1F(name.c_str(), string(name + "dt [ns];Counter").c_str(), 1000, -100., 100.));
   name = "hmp_Tof_TimeZero_a";
   fHM->Add(name, new TH1F(name.c_str(), string(name + "Time [ns];Counter").c_str(), 2000, 34., 36.));
   name = "hmp_Tof_TimeZero_reco";
   fHM->Add(name, new TH1F(name.c_str(), string(name + "Time [ns];Counter").c_str(), 2000, -5.,15. ));
   name = "hmp_Tof_TimeZero_mc";
   fHM->Add(name, new TH1F(name.c_str(), string(name + "Time [ns];Counter").c_str(), 2000, -5.,15. ));
   name = "hmp_Tof_TimeZero_NofTracks";
   fHM->Add(name, new TH1F(name.c_str(), string(name + "Number of tracks;Counter").c_str(), 100, 0., 100.));
   name = "hmp_Tof_Time_FirstTrack";
   fHM->Add(name, new TH1F(name.c_str(), string(name + "Time [ns];Counter").c_str(), 2000, 34., 36.));
}

void CbmLitTofQa::ProcessGlobalTracks()
{
   Double_t timeZeroReco = 0.0;
   Double_t timeZeroMC = 0.0;
   Double_t timeFirstTrack = 100.; // ns
   Double_t timeZeroA = 0.; // ns
   Int_t nofTracksForTimeZero = 0;

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
      Double_t chiSqPrimary = fKFFitter.GetChiToVertex(stsTrack, fPrimVertex);
      Bool_t chiSqPrimaryOk = chiSqPrimary < 3.;
      
      Double_t ctCorrection = -0.007;
      Double_t ctReco = 0.299792458 * tofHit->GetTime() + ctCorrection; // ToF time in ns -> transfrom to ct in m
      Double_t ctMC = 0.299792458 * tofPoint->GetTime() ; // mc time in ns -> transfrom to ct in m
      Double_t trackLengthReco = globalTrack->GetLength() / 100.; //global length
     // Double_t trackLengthMC = tofPoint->GetLength() / 100.; //mc length
      Double_t preco = (vtxTrack.GetQp() != 0) ? std::abs(1. / vtxTrack.GetQp()) : 0;
      Double_t t = (trackLengthReco != 0) ? (ctReco / trackLengthReco) : 0;
      Double_t m2reco = preco * preco * (t * t - 1);
      Double_t radialPos = sqrt(tofHit->GetX() * tofHit->GetX() + tofHit->GetY() * tofHit->GetY());

      if (chiSqPrimaryOk && radialPos < 50.) {
         nofTracksForTimeZero++;
         Double_t beta = 1.;
         if (m2reco > 0.6) beta = preco / sqrt(preco * preco + 0.93827231 * 0.93827231);
         if (m2reco <= 0.6) beta = preco / sqrt(preco * preco + 0.1395679 * 0.1395679);
         timeZeroReco += tofHit->GetTime() - trackLengthReco / (beta * 0.299792458);
         timeZeroMC += tofPoint->GetTime() - trackLengthReco / (beta * 0.299792458);
         timeZeroA += trackLengthReco / 0.299792458;
         timeFirstTrack = std::min(timeFirstTrack, trackLengthReco / 0.299792458);
      }

      Int_t nofTrackCategories = fTrackCategories.size();
      for (Int_t iCat = 0; iCat < nofTrackCategories; iCat++) {
    	  string category = fTrackCategories[iCat];
    	  LitTrackAcceptanceFunction function = fTrackAcceptanceFunctions.find(category)->second;
    	  Bool_t accOk = function(fMCTracks, stsMCTrackId);

    	  if (accOk && chiSqPrimaryOk) {
    		  fHM->H1("hmp_Tof_Reco_" + category + "_m2p")->Fill(preco, m2reco);

    		  if (stsMCTrackId == tofMCTrackId) {
    		     fHM->H1("hmp_Tof_RecoMCID_" + category + "_m2p")->Fill(preco, m2reco);
    		     fHM->H1("hmp_Tof_dTime")->Fill(tofPoint->GetTime() - tofHit->GetTime());
    		  }
    	  }
      }
   }
   if (nofTracksForTimeZero > 0) {
      timeZeroReco /= nofTracksForTimeZero;
      timeZeroMC /= nofTracksForTimeZero;
      timeZeroA /= nofTracksForTimeZero;
   }
   fHM->H1("hmp_Tof_TimeZero_reco")->Fill(timeZeroReco);
   fHM->H1("hmp_Tof_TimeZero_mc")->Fill(timeZeroMC);
   fHM->H1("hmp_Tof_TimeZero_a")->Fill(timeZeroA);
   fHM->H1("hmp_Tof_TimeZero_NofTracks")->Fill(nofTracksForTimeZero);
   fHM->H1("hmp_Tof_Time_FirstTrack")->Fill(timeFirstTrack);
}

void CbmLitTofQa::FitHistograms()
{
	fHM->H2("hmp_Tof_RecoMCID_Pion_m2p")->FitSlicesY();
	TH1* meanHist = (TH1*) gDirectory->Get("hmp_Tof_RecoMCID_Pion_m2p_1"); // mean
	TH1* sigmaHist = (TH1*) gDirectory->Get("hmp_Tof_RecoMCID_Pion_m2p_2"); // sigma
	Int_t nofBins = meanHist->GetNbinsX();
	for (Int_t iBin = 0; iBin <= nofBins; iBin++) {
		Double_t mean = meanHist->GetBinContent(iBin);
		Double_t sigma = sigmaHist->GetBinContent(iBin);
		std::cout << "mean=" << mean << " sigma=" << sigma << std::endl;
	}
}

ClassImp(CbmLitTofQa)
