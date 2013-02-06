/** CbmAnaDielectronTask.cxx
 * based on class by T.Galatyuk
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 3.0
 **/

#include "CbmAnaDielectronTask.h" 

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "FairMCPoint.h"

#include "CbmMCTrack.h"

#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrackMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"

#include "FairTrackParam.h"
#include "CbmTrackMatch.h"
#include "CbmKF.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmTofPoint.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TProfile.h"
#include "TRandom3.h"
#include "TObject.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "TDatabasePDG.h"

#include "CbmStsHit.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"

#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"

#include <sstream>

#define M2E 2.6112004954086e-7

using namespace std;

ClassImp(CbmAnaDielectronTask);

void CbmAnaDielectronTask::CreateAnalysisStepsH1(
      vector<TH1D*>& hist,
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max)
{
   string hname = "";
   hist.resize(CbmAnaLmvmNames::fNofAnaSteps);
   for (Int_t i = 0; i < CbmAnaLmvmNames::fNofAnaSteps; i++){
      hname = name + "_"+ CbmAnaLmvmNames::fAnaSteps[i];
      hist[i] = new TH1D(hname.c_str(), hname.c_str(), nBins, min, max);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      fHistoList.push_back(hist[i]);
   }
}

void CbmAnaDielectronTask::CreateAnalysisStepsH2(
      vector<TH2D*>& hist,
      const string& name,
      const string& axisX,
      const string& axisY,
      const string& axisZ,
      double nBinsX,
      double minX,
      double maxX,
      double nBinsY,
      double minY,
      double maxY)
{
   string hname = "";
   hist.resize(CbmAnaLmvmNames::fNofAnaSteps);
   for (Int_t i = 0; i < CbmAnaLmvmNames::fNofAnaSteps; i++){
      hname = name + "_"+ CbmAnaLmvmNames::fAnaSteps[i];
      hist[i] = new TH2D(hname.c_str(), hname.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      hist[i]->GetZaxis()->SetTitle(axisZ.c_str());
      fHistoList.push_back(hist[i]);
   }
}

void CbmAnaDielectronTask::CreateSourceTypesH1(
      vector<TH1D*>& hist,
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max)
{
   string hname = "";
   hist.resize(CbmAnaLmvmNames::fNofSourceTypes);
   for (Int_t i = 0; i < CbmAnaLmvmNames::fNofSourceTypes; i++){
      hname = name + "_"+ CbmAnaLmvmNames::fSourceTypes[i];
      hist[i] = new TH1D(hname.c_str(), hname.c_str(), nBins, min, max);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      fHistoList.push_back(hist[i]);
   }
}

void CbmAnaDielectronTask::CreateSourceTypesH2(
      vector<TH2D*>& hist,
      const string& name,
      const string& axisX,
      const string& axisY,
      const string& axisZ,
      double nBinsX,
      double minX,
      double maxX,
      double nBinsY,
      double minY,
      double maxY)
{
   string hname = "";
   hist.resize(CbmAnaLmvmNames::fNofSourceTypes);
   for (Int_t i = 0; i < CbmAnaLmvmNames::fNofSourceTypes; i++){
      hname = name + "_"+ CbmAnaLmvmNames::fSourceTypes[i];
      hist[i] = new TH2D(hname.c_str(), hname.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      hist[i]->GetZaxis()->SetTitle(axisZ.c_str());
      fHistoList.push_back(hist[i]);
   }
}

CbmAnaDielectronTask::CbmAnaDielectronTask()
: FairTask("CbmAnaDielectronTask")
{
   // weight for rho0 = 0.001081; omega_ee = 0.0026866; omega_dalitz = 0.02242; phi = 0.00039552; pi0 = 4.38   ------ for 25 GeV
   fWeight = 0.0;
   fUseRich = true;
   fUseTrd = true;
   fUseTof = true;

   fPionMisidLevel = -1.;
   fRandom3 = new TRandom3(0);

   //identification cuts
   fTrdAnnCut = 0.85;
   fRichAnnCut = 0.0;
   fUseRichAnn = true;
   fMeanA = -1.;
   fMeanB = -1.;
   fRmsA = -1.;
   fRmsB = -1.;
   fRmsCoeff = -1.;
   fDistCut = -1.;
   fMomentumCut = -1.; // if cut < 0 them it is not used
   // analysis cuts
   fPtCut = 0.2;
   fAngleCut = 1.;
   fChiPrimCut = 3.;
   fGammaCut = 0.025;
   fStCutAngle = 1.5;
   fStCutPP = 1.5;
   fTtCutAngle = 0.75;
   fTtCutPP = 4.0;
   fMvd1CutP = 1.2;
   fMvd1CutD = 0.4;
   fMvd2CutP = 1.5;
   fMvd2CutD = 0.5;
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

void CbmAnaDielectronTask::InitHists()
{
   fHistoList.clear();

   // Mother PDG
   fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
   fHistoList.push_back(fh_mc_mother_pdg);
   fh_acc_mother_pdg = new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
   fHistoList.push_back(fh_acc_mother_pdg);

   //vertex of the secondary electrons from gamma conversion
   CreateAnalysisStepsH2(fh_vertex_el_gamma_xz, "fh_vertex_el_gamma_xz","Z [cm]", "X [cm]", "Counter per event", 200, -10., 190., 400, -130.,130.);
   CreateAnalysisStepsH2(fh_vertex_el_gamma_yz, "fh_vertex_el_gamma_yz","Z [cm]", "Y [cm]", "Counter per event", 200, -10., 190., 400, -130., 130.);
   CreateAnalysisStepsH2(fh_vertex_el_gamma_xy, "fh_vertex_el_gamma_xy","X [cm]", "Y [cm]", "Counter per event",  400, -130.,130., 400, -130., 130.);
   CreateAnalysisStepsH2(fh_vertex_el_gamma_rz, "fh_vertex_el_gamma_rz","Z [cm]", "#sqrt{X^{2}+Y^{2}} [cm]", "Counter per event",  300, -10., 190., 300, 0., 150.);

   // Number of BG and signal tracks after each cut
   fh_nof_bg_tracks = new TH1D("fh_nof_bg_tracks","fh_nof_bg_tracks;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_bg_tracks);
   fh_nof_el_tracks = new TH1D("fh_nof_el_tracks","fh_nof_el_tracks;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_el_tracks);
   fh_source_tracks = new TH2D("fh_source_tracks","fh_source_tracks;Analysis steps;Particle", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps, 7, 0., 7.);
   fHistoList.push_back(fh_source_tracks);

   //Number of mismatches and ghosts after each cut
   fh_nof_mismatches = new TH1D("fh_nof_mismatches","fh_nof_mismatches;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches);
   fh_nof_mismatches_rich = new TH1D("fh_nof_mismatches_rich","fh_nof_mismatches_rich;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches_rich);
   fh_nof_mismatches_trd = new TH1D("fh_nof_mismatches_trd","fh_nof_mismatches_trd;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches_trd);
   fh_nof_mismatches_tof = new TH1D("fh_nof_mismatches_tof","fh_nof_mismatches_tof;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches_tof);
   fh_nof_ghosts = new TH1D("fh_nof_ghosts","fh_nof_ghosts;Analysis steps;Tracks/event", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps);
   fHistoList.push_back(fh_nof_ghosts);

   // BG pair source
   fh_source_pairs = new TH2D("fh_source_pairs","fh_source_pairs;Analysis steps;Pair", CbmAnaLmvmNames::fNofAnaSteps, 0., CbmAnaLmvmNames::fNofAnaSteps, CbmAnaLmvmNames::fNofBgPairSources, 0., CbmAnaLmvmNames::fNofBgPairSources);
   fHistoList.push_back(fh_source_pairs);

   // Event number counter
   fh_event_number = new TH1D("fh_event_number","fh_event_number", 1, 0, 1.);
   fHistoList.push_back(fh_event_number);

   CreateSourceTypesH1(fh_richann, "fh_richann", "ANN output", "Yield", 100, -1.1, 1.1);
   CreateSourceTypesH1(fh_trdann, "fh_trdann", "ANN output", "Yield", 100, -1.1, 1.1);
   CreateSourceTypesH2(fh_tofm2, "fh_tofm2", "P [GeV/c]", "m^{2} [GeV/c^{2}]^{2}", "Yield", 100, 0., 4., 600, -0.2, 1.2);

   // Distributions for analysis cuts.
   // Transverse momentum of tracks.
   CreateSourceTypesH1(fh_pt, "fh_pt", "P_{t} [GeV/c]", "Yield", 200, 0., 2.);
   // Momentum of tracks
   CreateSourceTypesH1(fh_mom, "fh_mom", "P [GeV/c]", "Yield", 200, 0., 10.);
   // Chi2 of the STS tracks
   CreateSourceTypesH1(fh_chi2sts, "fh_chi2sts", "#chi^{2}", "Yield", 200, 0., 20.);
   // Chi2 of the primary vertex
   CreateSourceTypesH1(fh_chi2prim, "fh_chi2prim", "#chi^{2}_{prim}", "Yield", 200, 0., 20.);
   // TT cut
   CreateSourceTypesH2(fh_ttcut, "fh_ttcut", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]",
         "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   // ST cut
   CreateSourceTypesH2(fh_stcut, "fh_stcut", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]",
         "#theta_{e^{#pm},rec} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   // AP cut
   CreateSourceTypesH2(fh_apcut, "fh_apcut", "#alpha", "P_{t} [GeV/c]", "Yield", 100, -1., 1., 200, 0., 1.);
   // APM cut
   CreateSourceTypesH2(fh_apmcut, "fh_apmcut", "#alpha", "P_{t} [GeV/c]", "Yield", 100, -1., 1., 200, 0., 1.);
   // MVD cut at the first station
   CreateSourceTypesH2(fh_mvd1cut, "fh_mvd1cut", "d_{MVD} [cm]", "P_{e} [GeV/c]", "Yield", 100, 0., 1., 100, 0., 5.);
   // MVD cut at the second station
   CreateSourceTypesH2(fh_mvd2cut, "fh_mvd2cut", "d_{MVD} [cm]", "P_{e} [GeV/c]", "Yield", 100, 0., 1., 100, 0., 5.);

   fh_ttcut_pion = new TH2D("fh_ttcut_pion", "fh_ttcut_pion;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg];Yield", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_ttcut_pion);
   fh_ttcut_truepair = new TH2D("fh_ttcut_truepair", "fh_ttcut_truepair;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg];Yield", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_ttcut_truepair);
   fh_stcut_pion = new TH2D("fh_stcut_pion", "fh_stcut_pion;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg];Yield", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_stcut_pion);
   fh_stcut_truepair = new TH2D("fh_stcut_truepair", "fh_stcut_truepair;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg];Yield", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_stcut_truepair);


   CreateSourceTypesH1(fh_nofMvdHits, "fh_nofMvdHits", "Number of hits in MVD", "Yield", 5, -0.5, 4.5);
   CreateSourceTypesH1(fh_nofStsHits, "fh_nofStsHits", "Number of hits in STS", "Yield", 9, -0.5, 8.5);
   CreateSourceTypesH2(fh_mvd1xy, "fh_mvd1xy", "X [cm]", "Y [cm]", "Yield", 60, -3., 3., 60, -3., 3.);
   CreateSourceTypesH1(fh_mvd1r, "fh_mvd1r", "#sqrt{X^{2}+Y^{2}} [cm]", "Yield", 60, 0., 3.);
   CreateSourceTypesH2(fh_mvd2xy, "fh_mvd2xy", "X [cm]", "Y [cm]", "Yield", 60, -6., 6., 60, -6., 6.);
   CreateSourceTypesH1(fh_mvd2r, "fh_mvd2r", "#sqrt{X^{2}+Y^{2}} [cm]", "Yield", 60, 0., 6.);

   // Check MVD cut quality. [0.5]-correct, [1.5]-wrong
   CreateSourceTypesH1(fh_mvd1cut_qa, "fh_mvd1cut_qa", "MVD hit assignment", "Yield", 2, 0., 2.);
   CreateSourceTypesH1(fh_mvd2cut_qa, "fh_mvd2cut_qa", "MVD hit assignment", "Yield", 2, 0., 2.);

   //Create invariant mass histograms
   CreateAnalysisStepsH1(fh_signal_minv, "fh_signal_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   CreateAnalysisStepsH1(fh_bg_minv, "fh_bg_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   CreateAnalysisStepsH1(fh_pi0_minv, "fh_pi0_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   CreateAnalysisStepsH1(fh_eta_minv, "fh_eta_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   CreateAnalysisStepsH1(fh_gamma_minv, "fh_gamma_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   // minv for true matched and mismatched tracks
   CreateAnalysisStepsH1(fh_bg_truematch_minv, "fh_bg_truematch_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   CreateAnalysisStepsH1(fh_bg_mismatch_minv, "fh_bg_mismatch_minv", "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   // Minv for different sources
   fh_source_bg_minv.resize(CbmAnaLmvmNames::fNofBgPairSources);
   for (int i = 0; i < CbmAnaLmvmNames::fNofBgPairSources; i++){
      stringstream ss;
      ss << "fh_source_bg_minv_" << i;
      CreateAnalysisStepsH1(fh_source_bg_minv[i], ss.str(), "M_{ee} [GeV/c^{2}]", "Yield", 2000, 0. , 2.);
   }
   // Momentum distribution of the signal
   CreateAnalysisStepsH1(fh_signal_mom, "fh_signal_mom", "P [GeV/c]", "Yield", 100, 0., 15.);
   //Pt/y distibution of the signal
   CreateAnalysisStepsH2(fh_signal_pty, "fh_signal_pty","Rapidity", "P_{t} [GeV/c]", "Yield", 40, 0., 4., 20, 0., 2.);
   //Sources pairs 2D
   CreateAnalysisStepsH2(fh_source_pairs_epem, "fh_source_pairs_epem","mother particle e+", "mother particle e-", "Yield", 3, 0., 3., 3, 0., 3.);
   //Invariant mass vs. Mc Pt
   CreateAnalysisStepsH2(fh_signal_minv_pt, "fh_signal_minv_pt","M_{ee} [GeV/c^{2}]", "P_{t} [GeV/c]", "Yield", 2000, 0., 2., 20, 0., 2.);

   fh_opening_angle.resize(CbmAnaLmvmNames::fNofSourceTypes);
   fh_source_mom.resize(CbmAnaLmvmNames::fNofSourceTypes);
   fh_source_pt.resize(CbmAnaLmvmNames::fNofSourceTypes);
   for (Int_t i = 0; i < CbmAnaLmvmNames::fNofSourceTypes; i++){
      fh_opening_angle[i].resize(CbmAnaLmvmNames::fNofAnaSteps);
      fh_source_mom[i].resize(CbmAnaLmvmNames::fNofAnaSteps);
      fh_source_pt[i].resize(CbmAnaLmvmNames::fNofAnaSteps);
   }

   for (Int_t i = 0; i < CbmAnaLmvmNames::fNofSourceTypes; i++){
      for (Int_t step = 0; step < CbmAnaLmvmNames::fNofAnaSteps; step++){
         string hname = "", htitle = "";
         hname = "fh_opening_angle_" + CbmAnaLmvmNames::fAnaSteps[step] + "_" + CbmAnaLmvmNames::fSourceTypes[i];
         htitle = hname + ";#Theta_{1,2} [deg];Yield";
         fh_opening_angle[i][step] = new TH1D(hname.c_str(), htitle.c_str(), 160, 0., 80.);
         fHistoList.push_back(fh_opening_angle[i][step]);

         hname = "fh_source_mom_" + CbmAnaLmvmNames::fAnaSteps[step] + "_" + CbmAnaLmvmNames::fSourceTypes[i];
         htitle = hname + ";P [GeV/c];Yield";
         fh_source_mom[i][step] = new TH1D(hname.c_str(), htitle.c_str(), 300, 0., 15.);
         fHistoList.push_back(fh_source_mom[i][step]);

         hname = "fh_source_pt_" + CbmAnaLmvmNames::fAnaSteps[step] + "_" + CbmAnaLmvmNames::fSourceTypes[i];
         htitle = hname + ";P_{t} [GeV/c];Yield";
         fh_source_pt[i][step] = new TH1D(hname.c_str(), htitle.c_str(), 100, 0., 5.);
         fHistoList.push_back(fh_source_pt[i][step]);
      }
   }

   fh_elpi_mom_mc.resize(2);
   fh_elpi_mom_acc.resize(2);
   fh_elpi_mom_rec.resize(2);
   fh_elpi_mom_rec_only_sts.resize(2);
   fh_elpi_mom_rec_sts_rich_trd.resize(2);
   fh_elpi_mom_rec_sts_rich_trd_tof.resize(2);
   //electrons and pions vs momentum
   for (int i = 0; i < 2; i++){
      string s = "electron";
      if (i == 1) s = "pion";
      fh_elpi_mom_mc[i] = new TH1D( ("fh_elpi_mom_mc_"+s).c_str(), ("fh_elpi_mom_mc_"+s+";p [GeV/c];dN/dP [1/GeV/c]").c_str(), 120, 0., 12.);
      fHistoList.push_back(fh_elpi_mom_mc[i]);
      fh_elpi_mom_acc[i] = new TH1D( ("fh_elpi_mom_acc_"+s).c_str(), ("fh_elpi_mom_acc_"+s+";p [GeV/c];dN/dP [1/GeV/c]").c_str(), 120, 0., 12.);
      fHistoList.push_back(fh_elpi_mom_acc[i]);
      fh_elpi_mom_rec[i] = new TH1D( ("fh_elpi_mom_rec_"+s).c_str(), ("fh_elpi_mom_rec_"+s+";p [GeV/c];dN/dP [1/GeV/c]").c_str(), 120, 0., 12.);
      fHistoList.push_back(fh_elpi_mom_rec[i]);
      fh_elpi_mom_rec_only_sts[i] = new TH1D( ("fh_elpi_mom_rec_only_sts_"+s).c_str(), ("fh_elpi_mom_rec_only_sts_"+s+";p [GeV/c];dN/dP [1/GeV/c]").c_str(), 120, 0., 12.);
      fHistoList.push_back(fh_elpi_mom_rec_only_sts[i]);
      fh_elpi_mom_rec_sts_rich_trd[i] = new TH1D( ("fh_elpi_mom_rec_sts_rich_trd_"+s).c_str(), ("fh_elpi_mom_rec_sts_rich_trd_"+s+";p [GeV/c];dN/dP [1/GeV/c]").c_str(), 120, 0., 12.);
      fHistoList.push_back(fh_elpi_mom_rec_sts_rich_trd[i]);
      fh_elpi_mom_rec_sts_rich_trd_tof[i] = new TH1D( ("fh_elpi_mom_rec_sts_rich_trd_tof_"+s).c_str(), ("fh_elpi_mom_rec_sts_rich_trd_tof_"+s+";p [GeV/c];dN/dP [1/GeV/c]").c_str(), 120, 0., 12.);
      fHistoList.push_back(fh_elpi_mom_rec_sts_rich_trd_tof[i]);
   }
}

InitStatus CbmAnaDielectronTask::Init()
{
	cout << "InitStatus CbmAnaDielectronTask::Init"<< endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmAnaDielectronTask::Init","No FairRootManager!"); }

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if (NULL == fMCTracks) { Fatal("CbmAnaDielectronTask::Init","No MCTrack array!"); }

	if (fUseRich == true){
		fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
		if (NULL == fRichHits) { Fatal("CbmAnaDielectronTask::Init","No RichHit array!"); }

		fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
		if (NULL == fRichRings) { Fatal("CbmAnaDielectronTask::Init","No RichRing array!"); }

		fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
		if (NULL == fRichPoints) { Fatal("CbmAnaDielectronTask::Init","No RichPoint array!"); }

		fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
		if (NULL == fRichRingMatches) { Fatal("CbmAnaDielectronTask::Init","No RichRingMatch array!"); }

		fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
		if (NULL == fRichProj) { Fatal("CbmAnaDielectronTask::Init","No RichProjection array!"); }
	}//fUseRich

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if (NULL == fStsTrackMatches) { Fatal("CbmAnaDielectronTask::Init","No StsTrackMatch array!"); }

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if (NULL == fStsTracks) { Fatal("CbmAnaDielectronTask::Init","No StsTrack array!"); }

   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   if (NULL == fStsHits) { Fatal("CbmAnaDielectronTask::Init","No StsHit array!"); }

   if (fUseMvd){
      fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
      if (NULL == fMvdHits) { Fatal("CbmAnaDielectronTask::Init","No MvdHit array!"); }

      fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
      if (NULL == fMvdPoints) { Fatal("CbmAnaDielectronTask::Init",": No MvdPoint array!"); }

      fMvdHitMatches = (TClonesArray*) ioman->GetObject("MvdHitMatch");
      if (NULL == fMvdHitMatches) { Fatal("CbmAnaDielectronTask::Init",": No MvdHitMatch array!"); }
   }

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (NULL == fGlobalTracks) { Fatal("CbmAnaDielectronTask::Init","No GlobalTrack array!"); }

	if (fUseTrd == true){
		fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
		if (NULL == fTrdTracks) { Fatal("CbmAnaDielectronTask::Init","No TrdTrack array!"); }

		fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
		if (NULL == fTrdTrackMatches) { Fatal("CbmAnaDielectronTask::Init","No TrdTrackMatch array!"); }
	}//fUseTrd

	if (fUseTof == true){
		fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
		if (NULL == fTofPoints) { Fatal("CbmAnaDielectronTask::Init","No TofPoint array!"); }

		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) { Fatal("CbmAnaDielectronTask::Init","No TofHit array!"); }
   }//fUseTof

   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
   if (NULL == fPrimVertex) { Fatal("CbmAnaDielectronTask::Init","No PrimaryVertex array!"); }

   InitHists();

   fKFFitter.Init();

   if (fUseRichAnn){
      string richANNFile(gSystem->Getenv("VMCWORKDIR"));
      richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
      fElIdAnn = new CbmRichElectronIdAnn(richANNFile);
      fElIdAnn->Init();
   }

   return kSUCCESS;
}

void CbmAnaDielectronTask::Exec(
      Option_t *option)
{
    fh_event_number->Fill(0.5);

    cout << "-I- CbmAnaDielectronTask, event number " << fh_event_number->GetEntries() << endl;
    cout << "Cuts:" << endl <<
    		"fTrdAnnCut = " << fTrdAnnCut << endl <<
    		"fRichAnnCut = " << fRichAnnCut << endl <<
    		"fChiPrimCut = " << fChiPrimCut << endl <<
    		"fPtCut = " << fPtCut << endl <<
    		"fAngleCut = " << fAngleCut << endl <<
    		"fGammaCut = " << fGammaCut << endl <<
    		"fStCut (ang,pp) = (" << fStCutAngle << "," << fStCutPP << ")" << endl <<
    		"fTtCut (ang,pp) = (" << fTtCutAngle << "," << fTtCutPP << ")" << endl <<
    		"fMvd1Cut (p,d) = (" << fMvd1CutP << "," << fMvd1CutD << ")" << endl <<
         "fMvd2Cut (p,d) = (" << fMvd2CutP << "," << fMvd2CutD << ")" << endl;

    cout << "fWeight = " << fWeight << endl;
    cout << "fMomentumCut = "<< fMomentumCut <<  endl;

    FillRichRingNofHits();
    MCPairs();   
    SingleParticleAcceptance();
    PairAcceptance();
    FillSegmentCandidatesArray();
    FillCandidateArray();
    DifferenceSignalAndBg();
    SignalAndBgReco();
    FillElPiMomHist();
}// Exec

void CbmAnaDielectronTask::FillRichRingNofHits()
{
	fNofHitsInRingMap.clear();
    Int_t nofRichHits = fRichHits->GetEntriesFast();
    for (Int_t iHit=0; iHit < nofRichHits; iHit++) {
        CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(iHit));
        if (NULL == hit) continue;

        Int_t iPoint = hit->GetRefId();
        if (iPoint < 0) continue;

        FairMCPoint* point = static_cast<FairMCPoint*>(fRichPoints->At(iPoint));
        if (NULL == point) continue;

        Int_t iMCTrack = point->GetTrackID();
        CbmMCTrack* track = static_cast<CbmMCTrack*>(fMCTracks->At(iMCTrack));
        if (NULL == track) continue;

        Int_t iMother = track->GetMotherId();
        if (iMother == -1) continue;

        fNofHitsInRingMap[iMother]++;
    }
}

void CbmAnaDielectronTask::MCPairs()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++){
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Double_t mom = mctrack->GetP();

        // mother pdg of e-/e+
        Int_t mcMotherPdg = 0;
        if (pdg == 11) {
           // momentum distribution for electrons from signal
           if (motherId == -1) fh_source_mom[kSignal][kMc]->Fill(mom, fWeight);

           // secondary electrons
           if (motherId != -1){
                CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
                if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
                // vertex of secondary electron from gamma conversion
                if (mcMotherPdg == 22) {
                    TVector3 v;
                    mctrack->GetStartVertex(v);
                    fh_vertex_el_gamma_xz[kMc]->Fill(v.Z(),v.X());
                    fh_vertex_el_gamma_yz[kMc]->Fill(v.Z(),v.Y());
                    fh_vertex_el_gamma_xy[kMc]->Fill(v.X(),v.Y());
                    fh_vertex_el_gamma_rz[kMc]->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
                }
            }else {
                mcMotherPdg = 0;
            }
            fh_mc_mother_pdg->Fill(mcMotherPdg);
        }
    } // nMcTracks

    for (Int_t iP = 0; iP < nMcTracks; iP++) {
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
        Int_t motherId  = mctrackP->GetMotherId();
        Int_t pdg = mctrackP->GetPdgCode();
        if ( !(motherId == -1 && pdg == -11)) continue;

        for (Int_t iM = 0; iM < nMcTracks; iM++) {
            if (iP == iM) continue;
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
            motherId = mctrackM->GetMotherId();
            pdg = mctrackM->GetPdgCode();
            if ( !(motherId == -1 && pdg == 11)) continue;

            KinematicParams p = CalculateKinematicParams(mctrackP,mctrackM);

            fh_signal_pty[kMc]->Fill(p.rapidity,p.pt,fWeight);
            fh_signal_mom[kMc]->Fill(p.momentumMag,fWeight);
            fh_signal_minv[kMc]->Fill(p.minv, fWeight);
        } //iM
    } //iP

} //MC Pairs

void CbmAnaDielectronTask::SingleParticleAcceptance()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++) {
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Int_t nMvdPoints = mctrack->GetNPoints(kMVD);
        Int_t nStsPoints = mctrack->GetNPoints(kSTS);
        Int_t nRichPoints = fNofHitsInRingMap[i];

        Bool_t isAcc = ( nMvdPoints+nStsPoints >= 4 && nRichPoints >= 7);

        Int_t mcMotherPdg = 0;
        if (pdg == 11 && isAcc) {
           if (motherId != -1){
               CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
               if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
               // vertex of secondary electron from gamma conversion
               if (mcMotherPdg == 22) {
                  TVector3 v;
                  mctrack->GetStartVertex(v);
                  fh_vertex_el_gamma_xz[kAcc]->Fill(v.Z(),v.X());
                  fh_vertex_el_gamma_yz[kAcc]->Fill(v.Z(),v.Y());
                  fh_vertex_el_gamma_xy[kAcc]->Fill(v.X(),v.Y());
                  fh_vertex_el_gamma_rz[kAcc]->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
               }

            }else {
                mcMotherPdg = 0;
            }
            fh_acc_mother_pdg->Fill(mcMotherPdg);
        }
    }
}

void CbmAnaDielectronTask::PairAcceptance()
{
   Int_t nMcTracks = fMCTracks->GetEntries();
   for (Int_t iP = 0; iP < nMcTracks; iP++) {
      CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
      Int_t motherId = mctrackP->GetMotherId();
      Int_t pdg = mctrackP->GetPdgCode();
      if ( !(motherId == -1 && pdg == -11 )) continue;

      Int_t nMvdPoints = mctrackP->GetNPoints(kMVD);
      Int_t nStsPoints = mctrackP->GetNPoints(kSTS);
      Int_t nRichPoints = fNofHitsInRingMap[iP];
      Int_t nTrdPoints = mctrackP->GetNPoints(kTRD);
      Int_t nTofPoints = mctrackP->GetNPoints(kTOF);

      Bool_t isAccP = (nMvdPoints + nStsPoints >= 4 && nRichPoints >= 7 && nTrdPoints > 8 && nTofPoints > 0) ;
      if (isAccP){
         for (Int_t iM = 0; iM < nMcTracks; iM++) {
            if (iP == iM) continue;
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
            motherId = mctrackM->GetMotherId();
            pdg = mctrackM->GetPdgCode();
            if ( !(motherId == -1 && pdg == 11 )) continue;

            nMvdPoints = mctrackM->GetNPoints(kMVD);
            nStsPoints = mctrackM->GetNPoints(kSTS);
            nRichPoints = fNofHitsInRingMap[iM];
            nTrdPoints = mctrackM->GetNPoints(kTRD);
            nTofPoints = mctrackM->GetNPoints(kTOF);

            Bool_t isAccM = (nMvdPoints + nStsPoints >= 4 && nRichPoints >= 7 && nTrdPoints > 8 && nTofPoints > 0) ;

            KinematicParams p = CalculateKinematicParams(mctrackP,mctrackM);

            if (isAccP && isAccM) {
               fh_signal_pty[kAcc]->Fill(p.rapidity, p.pt,fWeight);
               fh_signal_mom[kAcc]->Fill(p.momentumMag,fWeight);
               fh_signal_minv[kAcc]->Fill(p.minv,fWeight);
            }
         }//iM
      }
   }//iP
} // PairsAcceptance


void CbmAnaDielectronTask::FillElPiMomHist()
{
   Int_t nMcTracks = fMCTracks->GetEntries();
   for (Int_t i = 0; i < nMcTracks; i++) {
       CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
       Int_t motherId = mctrack->GetMotherId();
       Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
       double momentum = mctrack->GetP();
       Int_t nMvdPoints = mctrack->GetNPoints(kMVD);
       Int_t nStsPoints = mctrack->GetNPoints(kSTS);
       Bool_t isAcc = ( nMvdPoints+nStsPoints >= 4);

       if (pdg == 11){
          fh_elpi_mom_mc[0]->Fill(momentum);
          if (isAcc) fh_elpi_mom_acc[0]->Fill(momentum);
       }

       if (pdg == 211){
          fh_elpi_mom_mc[1]->Fill(momentum);
          if (isAcc) fh_elpi_mom_acc[1]->Fill(momentum);
       }
   }

   Int_t ngTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t i = 0; i < ngTracks; i++) {
       CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
       if(NULL == gTrack) continue;
       int stsInd = gTrack->GetStsTrackIndex();
       int richInd = gTrack->GetRichRingIndex();
       int trdInd = gTrack->GetTrdTrackIndex();
       int tofInd = gTrack->GetTofHitIndex();

       if (stsInd < 0) continue;
       CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
       if (stsTrack == NULL) continue;
       CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsTrackMatches->At(stsInd);
       if (stsMatch == NULL) continue;
       CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].stsMcTrackId);
       if (mcTrack1 == NULL) continue;
       int pdg = TMath::Abs(mcTrack1->GetPdgCode());
       int motherId = mcTrack1->GetMotherId();
       double momentum = mcTrack1->GetP();

       if (pdg == 11) {
          fh_elpi_mom_rec[0]->Fill(momentum);

          if (richInd < 0 && trdInd < 0 && tofInd < 0) {
             fh_elpi_mom_rec_only_sts[0]->Fill(momentum);
          }

          if (richInd >= 0 && trdInd >= 0) {
               fh_elpi_mom_rec_sts_rich_trd[0]->Fill(momentum);
           }

          if (richInd >= 0 && trdInd >= 0 && tofInd >= 0) {
             fh_elpi_mom_rec_sts_rich_trd_tof[0]->Fill(momentum);
          }
       }

       if (pdg == 211) {
          fh_elpi_mom_rec[1]->Fill(momentum);

          if (richInd < 0 && trdInd < 0 && tofInd < 0) {
             fh_elpi_mom_rec_only_sts[1]->Fill(momentum);
          }

          if (richInd >= 0 && trdInd >= 0) {
               fh_elpi_mom_rec_sts_rich_trd[1]->Fill(momentum);
           }

          if (richInd >= 0 && trdInd >= 0 && tofInd >= 0) {
             fh_elpi_mom_rec_sts_rich_trd_tof[1]->Fill(momentum);
          }
       }
   }//gTracks
}

void CbmAnaDielectronTask::FillSegmentCandidatesArray()
{
    fSegmentCandidates.clear();
    Int_t ngTracks = fGlobalTracks->GetEntriesFast();
    fSegmentCandidates.reserve(ngTracks);

    for (Int_t i = 0; i < ngTracks; i++) {
        DielectronCandidate cand;
    
        CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
        if(NULL == gTrack) continue;

        //choose tracks which are ONLY in STS -> SEGMENT
        cand.stsInd = gTrack->GetStsTrackIndex();
        if (cand.stsInd < 0) continue;
        CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.stsInd);
        if (stsTrack == NULL) continue;

        cand.richInd = gTrack->GetRichRingIndex();
        if (cand.richInd >= 0) continue;

        cand.trdInd = gTrack->GetTrdTrackIndex();
        if (cand.trdInd >= 0) continue;

        cand.tofInd = gTrack->GetTofHitIndex();
        if (cand.tofInd >= 0) continue;

        fKFFitter.DoFit(stsTrack,11);
        cand.chi2Prim = fKFFitter.GetChiToVertex(stsTrack, fPrimVertex);
        // Fit tracks to the primary vertex
        FairTrackParam vtxTrack;  
        fKFFitter.FitToVertex(stsTrack, fPrimVertex, &vtxTrack);

        vtxTrack.Position(cand.position);
        vtxTrack.Momentum(cand.momentum);

        cand.mass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
        cand.charge = (vtxTrack.GetQp() > 0) ?1 :-1;
        cand.energy = sqrt(cand.momentum.Mag2() + cand.mass * cand.mass);
        cand.rapidity = 0.5*TMath::Log((cand.energy + cand.momentum.Z()) /
                    (cand.energy - cand.momentum.Z()));

        //only primary segment candidates
        if (cand.chi2Prim < fChiPrimCut) fSegmentCandidates.push_back(cand);
    }//gTracks
    cout << "fSegmentCandidates.size() = " << fSegmentCandidates.size() << endl;
}

void CbmAnaDielectronTask::FillCandidateArray()
{
   fCandidates.clear();
   Int_t nGTracks = fGlobalTracks->GetEntriesFast();
   fCandidates.reserve(nGTracks);

   for (Int_t i = 0; i < nGTracks; i++){
      DielectronCandidate cand;
      cand.isElectron = false;
      cand.isGamma = false;
      cand.isStCutElectron = false;
      cand.isTtCutElectron = false;
      cand.isApmCutElectron = false;
      cand.isMvd1CutElectron = true;
      cand.isMvd2CutElectron = true;

      CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
      if(NULL == gTrack) continue;
      // STS
      cand.stsInd = gTrack->GetStsTrackIndex();
      if (cand.stsInd < 0) continue;
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.stsInd);
      if (stsTrack == NULL) continue;

      fKFFitter.DoFit(stsTrack,11);
      cand.chi2sts = stsTrack->GetChi2() / stsTrack->GetNDF();
      cand.chi2Prim = fKFFitter.GetChiToVertex(stsTrack, fPrimVertex);
      FairTrackParam vtxTrack;
      fKFFitter.FitToVertex(stsTrack, fPrimVertex, &vtxTrack);// Fit tracks to the primary vertex

      vtxTrack.Position(cand.position);
      vtxTrack.Momentum(cand.momentum);
      cand.mass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
      cand.charge = (vtxTrack.GetQp() > 0) ?1 :-1;
      cand.energy = sqrt(cand.momentum.Mag2() + cand.mass * cand.mass);
      cand.rapidity = 0.5*TMath::Log((cand.energy + cand.momentum.Z()) / (cand.energy - cand.momentum.Z()));

      // Add all pions from STS for pion misidentification level study
      if (fPionMisidLevel > 0.0){
         CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsTrackMatches->At(cand.stsInd);
         if (stsMatch == NULL) continue;
         cand.stsMcTrackId = stsMatch->GetMCTrackId();
         if (cand.stsMcTrackId < 0) continue;
         CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(cand.stsMcTrackId);
         if (mcTrack1 == NULL) continue;
         Int_t pdg = TMath::Abs(mcTrack1->GetPdgCode());

         if (pdg == 211){
            IsElectron(NULL, cand.momentum.Mag(), NULL, gTrack, &cand);
            fCandidates.push_back(cand);
            continue;
         }
      }

      // RICH
      cand.richInd = gTrack->GetRichRingIndex();
      if (cand.richInd < 0) continue;
      CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(cand.richInd);

      // TRD
      CbmTrdTrack* trdTrack = NULL;
      if (fUseTrd == true) {
         cand.trdInd = gTrack->GetTrdTrackIndex();
         if (cand.trdInd < 0) continue;
         trdTrack = (CbmTrdTrack*) fTrdTracks->At(cand.trdInd);
         if (trdTrack == NULL) continue;
      }

      // ToF
      cand.tofInd = gTrack->GetTofHitIndex();
      if (cand.tofInd < 0) continue;
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(cand.tofInd);
      if (tofHit == NULL) continue;

      IsElectron(richRing, cand.momentum.Mag(), trdTrack, gTrack, &cand);

      if (false) {
         // distance to vertex cut
         double dx = vtxTrack.GetX() - fPrimVertex->GetX();
         double dy = vtxTrack.GetY() - fPrimVertex->GetY();
         double dz = vtxTrack.GetZ() - fPrimVertex->GetZ();
         double r = TMath::Sqrt(dx*dx + dy*dy + dz*dz);
         if(r > 0.0025) continue;
      }

      fCandidates.push_back(cand);
   }// global tracks
   cout << "fCandidates.size() = " << fCandidates.size() << endl;

   AssignMcToCandidates();
}

void CbmAnaDielectronTask::AssignMcToCandidates()
{
   int nCand = fCandidates.size();
   for (int i = 0; i < nCand; i++){
      fCandidates[i].isMcSignalElectron = false;
      fCandidates[i].isMcPi0Electron = false;
      fCandidates[i].isMcGammaElectron = false;
      fCandidates[i].isMcEtaElectron = false;
      fCandidates[i].McMotherId = -1;
      fCandidates[i].stsMcTrackId = -1;
      fCandidates[i].richMcTrackId = -1;
      fCandidates[i].trdMcTrackId = -1;
      fCandidates[i].tofMcTrackId = -1;

      //STS
      //MCTrackId of the candidate is defined by STS track
      int stsInd = fCandidates[i].stsInd;
      CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsTrackMatches->At(stsInd);
      if (stsMatch == NULL) continue;
      fCandidates[i].stsMcTrackId = stsMatch->GetMCTrackId();
      if (fCandidates[i].stsMcTrackId < 0) continue;
      CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].stsMcTrackId);
      if (mcTrack1 == NULL) continue;
      int pdg = TMath::Abs(mcTrack1->GetPdgCode());
      int motherId = mcTrack1->GetMotherId();
      fCandidates[i].McMotherId = motherId;

      if (pdg == 211 && fPionMisidLevel > 0.) continue;

      // RICH
      int richInd = fCandidates[i].richInd;
      CbmTrackMatch* richMatch  = (CbmTrackMatch*) fRichRingMatches->At(richInd);
      if (richMatch == NULL) continue;
      fCandidates[i].richMcTrackId = richMatch->GetMCTrackId();
      //if (fCandidates[i].richMcTrackId < 0) continue;
      //CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].richMcTrackId);
      //if (mcTrack2 == NULL) continue;

      if (pdg == 11 && motherId == -1) fCandidates[i].isMcSignalElectron = true;

      if (motherId >=0){
         CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(motherId);
         int motherPdg = mct1->GetPdgCode();
         if (mct1 != NULL && motherPdg == 111 && pdg == 11) {
            fCandidates[i].isMcPi0Electron = true;
         }
         if (mct1 != NULL && motherPdg == 22 && pdg == 11){
            fCandidates[i].isMcGammaElectron = true;
         }
         if(mct1 != NULL && motherPdg == 221 && pdg == 11){
            fCandidates[i].isMcEtaElectron = true;
         }
      }

      // TRD
      CbmTrdTrack* trdTrack = NULL;
      if (fUseTrd == true) {
         int trdInd = fCandidates[i].trdInd;
         CbmTrackMatch* trdMatch = (CbmTrackMatch*) fTrdTrackMatches->At(trdInd);
         if (trdMatch == NULL) continue;
         fCandidates[i].trdMcTrackId = trdMatch->GetMCTrackId();
         //if (fCandidates[i].trdMcTrackId < 0) continue;
         //CbmMCTrack* mcTrack3 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].trdMcTrackId);
         //if (mcTrack3 == NULL) continue;
      }

      // ToF
      int tofInd = fCandidates[i].tofInd;
      if (tofInd < 0) continue;
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofInd);
      if (tofHit == NULL) continue;
      Int_t tofPointIndex = tofHit->GetRefId();
      if (tofPointIndex < 0) continue;
      FairMCPoint* tofPoint = (FairMCPoint*) fTofPoints->At(tofPointIndex);
      if (tofPoint == NULL) continue;
      fCandidates[i].tofMcTrackId = tofPoint->GetTrackID();
    //  if (cand.tofMcTrackId < 0) continue;
    //  CbmMCTrack* mcTrack4 = (CbmMCTrack*) fMCTracks->At(cand.tofMcTrackId);
    //  if (mcTrack4 == NULL) continue;
   }// candidates
}

void CbmAnaDielectronTask::PairSource(
      DielectronCandidate* candP,
      DielectronCandidate* candM,
      AnalysisSteps step,
      KinematicParams* parRec)
{
   // Fill opening angle histograms
   Bool_t isSignal = candP->isMcSignalElectron && candM->isMcSignalElectron;
   Bool_t isBg = !(candP->isMcSignalElectron || candM->isMcSignalElectron);
   Bool_t isPi0 = (candP->isMcPi0Electron && candM->isMcPi0Electron && candP->McMotherId == candM->McMotherId);
   Bool_t isGamma = (candP->isMcGammaElectron && candM->isMcGammaElectron && candP->McMotherId == candM->McMotherId);

   if (isSignal) fh_opening_angle[kSignal][step]->Fill(parRec->angle, fWeight);
   if (isBg) fh_opening_angle[kBg][step]->Fill(parRec->angle);
   if (isPi0) fh_opening_angle[kPi0][step]->Fill(parRec->angle);
   if (isGamma)fh_opening_angle[kGamma][step]->Fill(parRec->angle);


   int binNum = (double)step + 0.5;
   // Fill BG source pair histograms
   if (isBg){
      TH2D* hsp = fh_source_pairs_epem[step];
      if (candM->isMcGammaElectron) {
         if (candP->isMcGammaElectron && candP->McMotherId != candM->McMotherId){
            hsp->Fill(0.5, 0.5);
            fh_source_bg_minv[0][step]->Fill(parRec->minv);//g-g
            fh_source_pairs->Fill(binNum, 0.5);
         }else if (candP->isMcPi0Electron){
            hsp->Fill(1.5, 0.5);
            fh_source_bg_minv[3][step]->Fill(parRec->minv);//g-p
            fh_source_pairs->Fill(binNum, 3.5);
         }else{
            hsp->Fill(2.5, 0.5);
            fh_source_bg_minv[4][step]->Fill(parRec->minv);//g-o
            fh_source_pairs->Fill(binNum, 4.5);
         }
      }else if (candM->isMcPi0Electron) {
        if (candP->isMcGammaElectron){
           hsp->Fill(0.5, 1.5);
           fh_source_bg_minv[3][step]->Fill(parRec->minv);//g-p
           fh_source_pairs->Fill(binNum, 3.5);
        } else if (candP->isMcPi0Electron && candP->McMotherId != candM->McMotherId){
           hsp->Fill(1.5, 1.5);
           fh_source_bg_minv[1][step]->Fill(parRec->minv);//p-p
           fh_source_pairs->Fill(binNum, 1.5);
        } else {
           hsp->Fill(2.5, 1.5);
           fh_source_bg_minv[5][step]->Fill(parRec->minv);//p-o
           fh_source_pairs->Fill(binNum, 5.5);
        }
      }else {
         if (candP->isMcGammaElectron){
            hsp->Fill(0.5, 2.5);
            fh_source_bg_minv[4][step]->Fill(parRec->minv);//g-o
            fh_source_pairs->Fill(binNum, 4.5);
         }else if (candP->isMcPi0Electron){
            hsp->Fill(1.5, 2.5);
            fh_source_bg_minv[5][step]->Fill(parRec->minv);//p-o
            fh_source_pairs->Fill(binNum, 5.5);
         }else {
            hsp->Fill(2.5, 2.5);
            fh_source_bg_minv[2][step]->Fill(parRec->minv);//o-o
            fh_source_pairs->Fill(binNum, 2.5);
         }
      }
   }
}

void CbmAnaDielectronTask::TrackSource(
		DielectronCandidate* cand,
		AnalysisSteps step,
		Int_t pdg)
{
   int binNum = (double)step + 0.5;
	Double_t mom = cand->momentum.Mag();
	Double_t pt = cand->momentum.Perp();
	if (cand->isMcSignalElectron) {
		fh_nof_el_tracks->Fill(binNum, fWeight);
		fh_source_mom[kSignal][step]->Fill(mom, fWeight);
		fh_source_pt[kSignal][step]->Fill(pt, fWeight);
	} else {
	   if (IsMismatch(cand)) fh_nof_mismatches->Fill(binNum);
	   if (cand->stsMcTrackId != cand->richMcTrackId) fh_nof_mismatches_rich->Fill(binNum);
	   if (fUseTrd && cand->stsMcTrackId != cand->trdMcTrackId) fh_nof_mismatches_trd->Fill(binNum);
	   if (cand->stsMcTrackId != cand->tofMcTrackId) fh_nof_mismatches_tof->Fill(binNum);
	   if (IsGhost(cand)) fh_nof_ghosts->Fill(binNum);
		fh_nof_bg_tracks->Fill(binNum);
		fh_source_mom[kBg][step]->Fill(mom);
		fh_source_pt[kBg][step]->Fill(pt);
		if (cand->isMcGammaElectron) {
		   fh_source_tracks->Fill(binNum, 0.5);
		   fh_source_mom[kGamma][step]->Fill(mom);
		   fh_source_pt[kGamma][step]->Fill(pt);

		   // e+/- from gamma conversion vertex
		   int mcTrackId = cand->stsMcTrackId;
		   CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(mcTrackId);
		   if (NULL != mctrack){
            TVector3 v;
            mctrack->GetStartVertex(v);
            fh_vertex_el_gamma_xz[step]->Fill(v.Z(),v.X());
            fh_vertex_el_gamma_yz[step]->Fill(v.Z(),v.Y());
            fh_vertex_el_gamma_xy[step]->Fill(v.X(),v.Y());
            fh_vertex_el_gamma_rz[step]->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
		   }
		}else if (cand->isMcPi0Electron) {
		   fh_source_tracks->Fill(binNum, 1.5);
		   fh_source_mom[kPi0][step]->Fill(mom);
		   fh_source_pt[kPi0][step]->Fill(pt);
		}else if (pdg == 211 || pdg ==-211) {
		   fh_source_tracks->Fill(binNum, 2.5);
		}else if (pdg == 2212) {
		   fh_source_tracks->Fill(binNum, 3.5);
		}else if (pdg == 321 || pdg == -321) {
		   fh_source_tracks->Fill(binNum, 4.5);
		}else if ( (pdg == 11 || pdg == -11) && !cand->isMcGammaElectron
		      && !cand->isMcPi0Electron && !cand->isMcSignalElectron){
		   fh_source_tracks->Fill(binNum, 5.5);
		}else{
		   fh_source_tracks->Fill(binNum, 6.5);
		}
	}
}

void CbmAnaDielectronTask::FillPairHists(
      DielectronCandidate* candP,
      DielectronCandidate* candM,
      KinematicParams* parMc,
      KinematicParams* parRec,
      AnalysisSteps step)
{
   Bool_t isSignal = candP->isMcSignalElectron && candM->isMcSignalElectron;
   Bool_t isBG = !(candP->isMcSignalElectron || candM->isMcSignalElectron);
   Bool_t isPi0 = (candP->isMcPi0Electron && candM->isMcPi0Electron && candP->McMotherId == candM->McMotherId);
   Bool_t isEta = (candP->isMcEtaElectron && candM->isMcEtaElectron && candP->McMotherId == candM->McMotherId);
   Bool_t isGamma = (candP->isMcGammaElectron && candM->isMcGammaElectron && candP->McMotherId == candM->McMotherId);
   Bool_t isMismatch = (IsMismatch(candP) || IsMismatch(candM));

   if (isSignal) fh_signal_pty[step]->Fill(parMc->rapidity, parMc->pt, fWeight);
   if (isSignal) fh_signal_mom[step]->Fill(parMc->momentumMag, fWeight);
   if (isSignal) fh_signal_minv[step]->Fill(parRec->minv, fWeight);
   if (isSignal) fh_signal_minv_pt[step]->Fill(parRec->minv, parMc->pt, fWeight);
   if (isBG) fh_bg_minv[step]->Fill(parRec->minv);
   PairSource(candP, candM, step, parRec);
   if (isPi0) fh_pi0_minv[step]->Fill(parRec->minv);
   if (isEta) fh_eta_minv[step]->Fill(parRec->minv);
   if (isGamma) fh_gamma_minv[step]->Fill(parRec->minv);
   if (isBG && isMismatch) fh_bg_mismatch_minv[step]->Fill(parRec->minv);
   if (isBG && !isMismatch) fh_bg_truematch_minv[step]->Fill(parRec->minv);
}

void CbmAnaDielectronTask::SignalAndBgReco()
{
   CheckGammaConvAndPi0();
   CheckTrackTopologyCut();
   CheckTrackTopologyRecoCut();
   CheckArmPodModified();
   if (fUseMvd){
      CheckClosestMvdHit(1, fh_mvd1cut, fh_mvd1cut_qa);
      CheckClosestMvdHit(2, fh_mvd2cut, fh_mvd2cut_qa);
   }

   Int_t ncand = fCandidates.size();
   for (Int_t i = 0; i < ncand; i++){
      Int_t pdg = 0;
      if (fCandidates[i].stsMcTrackId > 0){
	  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(fCandidates[i].stsMcTrackId);
	  pdg = mcTrack->GetPdgCode();
      }

      Bool_t isChi2Prim = (fCandidates[i].chi2Prim < fChiPrimCut);
      Bool_t isEl = (fCandidates[i].isElectron);
      Bool_t isGammaCut = (!fCandidates[i].isGamma);
      Bool_t isMvd1Cut = fCandidates[i].isMvd1CutElectron;
      Bool_t isMvd2Cut = fCandidates[i].isMvd2CutElectron;
      Bool_t isStCut = (fCandidates[i].isStCutElectron);
      Bool_t isTtCut = (fCandidates[i].isTtCutElectron);
      Bool_t isPtCut = (fCandidates[i].momentum.Perp() > fPtCut);
      if (!fUseMvd) isMvd1Cut = true;
      if (!fUseMvd) isMvd2Cut = true;

		TrackSource(&fCandidates[i], kReco, pdg);
		if (isChi2Prim) TrackSource(&fCandidates[i], kChi2Prim, pdg);
		if (isChi2Prim && isEl) TrackSource(&fCandidates[i], kElId, pdg);
		if (isChi2Prim && isEl && isGammaCut) TrackSource(&fCandidates[i], kGammaCut, pdg);
      if (isChi2Prim && isEl && isGammaCut && isMvd1Cut) TrackSource(&fCandidates[i], kMvd1Cut, pdg);
      if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut)TrackSource(&fCandidates[i], kMvd2Cut, pdg);
		if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut) TrackSource(&fCandidates[i], kStCut, pdg);
		if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut) TrackSource(&fCandidates[i], kTtCut, pdg);
		if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut && isPtCut) TrackSource(&fCandidates[i], kPtCut, pdg);
    }

    for (Int_t iP = 0; iP < ncand; iP++){
        if (fCandidates[iP].charge < 0) continue;
	CbmMCTrack* mctrackP = NULL;
	if (fCandidates[iP].stsMcTrackId >=0) mctrackP = (CbmMCTrack*) fMCTracks->At(fCandidates[iP].stsMcTrackId);
        for (Int_t iM = 0; iM < ncand; iM++){
            if (fCandidates[iM].charge > 0) continue;
	    CbmMCTrack* mctrackM = NULL;
	    if (fCandidates[iM].stsMcTrackId >=0) mctrackM = (CbmMCTrack*) fMCTracks->At(fCandidates[iM].stsMcTrackId);
            if (iM == iP ) continue;

	    KinematicParams pMC;
	    if (mctrackP != NULL && mctrackM != NULL) pMC = CalculateKinematicParams(mctrackP, mctrackM);

            KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 

            Bool_t isChiPrimary = (fCandidates[iP].chi2Prim < fChiPrimCut && fCandidates[iM].chi2Prim < fChiPrimCut);
            Bool_t isEl = (fCandidates[iP].isElectron && fCandidates[iM].isElectron);
            Bool_t isGammaCut = (!fCandidates[iP].isGamma && !fCandidates[iM].isGamma);
            Bool_t isStCut = (fCandidates[iP].isStCutElectron && fCandidates[iM].isStCutElectron);
            Bool_t isTtCut = (fCandidates[iP].isTtCutElectron && fCandidates[iM].isTtCutElectron);
            Bool_t isPtCut = (fCandidates[iP].momentum.Perp() > fPtCut && fCandidates[iM].momentum.Perp() > fPtCut);
            Bool_t isAngleCut = (pRec.angle > fAngleCut);
            Bool_t isAPMCut = (fCandidates[iP].isApmCutElectron && fCandidates[iM].isApmCutElectron);
            Bool_t isMvd1Cut = (fCandidates[iP].isMvd1CutElectron && fCandidates[iM].isMvd1CutElectron);
            Bool_t isMvd2Cut = (fCandidates[iP].isMvd2CutElectron && fCandidates[iM].isMvd2CutElectron);
            if (!fUseMvd) isMvd1Cut = true;
            if (!fUseMvd) isMvd2Cut = true;

            FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kReco);
            if (isChiPrimary){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kChi2Prim);
            }
            if (isChiPrimary && isEl){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kElId);
            }
            if (isChiPrimary && isEl && isGammaCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kGammaCut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kMvd1Cut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kMvd2Cut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kStCut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kTtCut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut && isPtCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kPtCut);
            }
        } // iM
    } // iP
}

Bool_t CbmAnaDielectronTask::CheckArmPod(
      Double_t alfa,
      Double_t pt)
{
    Double_t a = 1.;
    Double_t b = 0.41;

    if (alfa >= a) return false;
    Double_t y2 = b*b - (b*b/a*a)*alfa;
    if (y2 <= 0.) return false;

    Double_t y = TMath::Sqrt( b*b - (b*b/a*a)*alfa );
    if (pt < y) return true;
    return false;
}

// Armanteros-Podolyanski modified cut
void CbmAnaDielectronTask::CheckArmPodModified() 
{
   Int_t nCand = fCandidates.size();
   vector<Float_t> angles1;
   vector<Int_t> candInd;
   angles1.reserve(50);
   candInd.reserve(50);
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].chi2Prim < fChiPrimCut && fCandidates[iP].isElectron){
         angles1.clear();
         candInd.clear();
         for (Int_t iM = 0; iM < nCand; iM++){
            // different charges, charge Im != charge iP
            if (fCandidates[iM].charge != fCandidates[iP].charge){
               KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]);
               angles1.push_back(pRec.angle);
               candInd.push_back(iM);
            } // if
         }// iM
         if (candInd.size() > 0){
            //find min opening angle
            Double_t minAng = 360.;
            Int_t minInd = 0;
            for (Int_t i = 0; i < angles1.size(); i++){
               if (minAng > angles1[i]){
               minAng = angles1[i];
               minInd = i;
               }
            }
            Double_t alfa = 0.;
            Double_t ptt = 0.;
            CalculateArmPodParams(&fCandidates[iP], &fCandidates[candInd[minInd]], alfa, ptt);

            if (fCandidates[iP].isMcSignalElectron) fh_apmcut[kSignal]->Fill(alfa, ptt, fWeight);
            else fh_apmcut[kBg]->Fill(alfa, ptt);
            if (fCandidates[iP].isMcGammaElectron) fh_apmcut[kGamma]->Fill(alfa, ptt);
            if (fCandidates[iP].isMcPi0Electron) fh_apmcut[kPi0]->Fill(alfa, ptt);

            if (ptt > 0.025) fCandidates[iP].isApmCutElectron = true;
         }
      }//if electron
   } //iP
}

void CbmAnaDielectronTask::CheckGammaConvAndPi0()
{
   Int_t ncand = fCandidates.size();
   for (Int_t iP = 0; iP < ncand; iP++){
      if (fCandidates[iP].charge < 0) continue;
      for (Int_t iM = 0; iM < ncand; iM++){
         if (fCandidates[iM].charge > 0) continue;
         if (iM == iP ) continue;
         if ((fCandidates[iP].chi2Prim < fChiPrimCut && fCandidates[iP].isElectron) &&
         (fCandidates[iM].chi2Prim < fChiPrimCut && fCandidates[iM].isElectron )) {
            KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]);
            if (pRec.minv < fGammaCut) {
               fCandidates[iM].isGamma = true;
               fCandidates[iP].isGamma = true;
            }
         }
      }
   }
}

//ST cut, cut for only segment tracks
void CbmAnaDielectronTask::CheckTrackTopologyCut() 
{
   vector<Float_t> angles, mom;
   angles.reserve(50);
   mom.reserve(50);
   Int_t nCand = fCandidates.size();
   Int_t nSegmentCand = fSegmentCandidates.size();
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].chi2Prim < fChiPrimCut && fCandidates[iP].isElectron){
         angles.clear();
         mom.clear();
         for (Int_t iM = 0; iM < nSegmentCand; iM++){
            // different charges, charge Im != charge iP
            if (fSegmentCandidates[iM].charge != fCandidates[iP].charge){
               KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fSegmentCandidates[iM]);
               angles.push_back(pRec.angle);
               mom.push_back(fSegmentCandidates[iM].momentum.Mag());
            } // if
         }// iM
         //find min opening angle
         Double_t minAng = 360.;
         Int_t minInd = 0;
         for (Int_t i = 0; i < angles.size(); i++){
            if (minAng > angles[i]){
               minAng = angles[i];
               minInd = i;
            }
         }
         Double_t sqrt_mom = TMath::Sqrt(fCandidates[iP].momentum.Mag()*mom[minInd]);

         if (fCandidates[iP].isMcSignalElectron) fh_stcut[kSignal]->Fill(sqrt_mom, minAng, fWeight);
         else{
            fh_stcut[kBg]->Fill(sqrt_mom, minAng);
            fh_stcut_pion;
            fh_stcut_truepair;
         }
         if (fCandidates[iP].isMcPi0Electron) fh_stcut[kPi0]->Fill(sqrt_mom, minAng);
         if (fCandidates[iP].isMcGammaElectron) fh_stcut[kGamma]->Fill(sqrt_mom, minAng);


         Double_t val = -1.*(fStCutAngle/fStCutPP)*sqrt_mom + fStCutAngle;
         if ( !(sqrt_mom < fStCutPP && val > minAng) ) fCandidates[iP].isStCutElectron = true;
      }//if electron
   } //iP
}

//TT cut, cut only for full reco tracks
void CbmAnaDielectronTask::CheckTrackTopologyRecoCut()
{
   vector<Float_t> angles1, mom1;
   angles1.reserve(50);
   mom1.reserve(50);
   Int_t nCand = fCandidates.size();
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].chi2Prim < fChiPrimCut && fCandidates[iP].isElectron){
         angles1.clear();
         mom1.clear();
         for (Int_t iM = 0; iM < nCand; iM++){
            // different charges, charge Im != charge iP
            if (fCandidates[iM].charge != fCandidates[iP].charge && fCandidates[iM].chi2Prim < fChiPrimCut && !fCandidates[iM].isElectron){
               KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]);
               angles1.push_back(pRec.angle);
               mom1.push_back(fCandidates[iM].momentum.Mag());
            } // if
         }// iM
         //find min opening angle
         Double_t minAng = 360.;
         Int_t minInd = 0;
         for (Int_t i = 0; i < angles1.size(); i++){
            if (minAng > angles1[i]){
               minAng = angles1[i];
               minInd = i;
            }
         }
         Double_t sqrt_mom = TMath::Sqrt(fCandidates[iP].momentum.Mag()*mom1[minInd]);
         if (fCandidates[iP].isMcSignalElectron) fh_ttcut[kSignal]->Fill(sqrt_mom, minAng, fWeight);
         else{
            fh_ttcut[kBg]->Fill(sqrt_mom, minAng);
            fh_ttcut_pion;
            fh_ttcut_truepair;
         }
         if (fCandidates[iP].isMcPi0Electron) fh_ttcut[kPi0]->Fill(sqrt_mom, minAng);
         if (fCandidates[iP].isMcGammaElectron) fh_ttcut[kGamma]->Fill(sqrt_mom, minAng);

         Double_t val = -1.*(fTtCutAngle/fTtCutPP)*sqrt_mom + fTtCutAngle;
         if ( !(sqrt_mom < fTtCutPP && val > minAng) ) fCandidates[iP].isTtCutElectron = true;
      }//if electron
   } //iP
}

Bool_t CbmAnaDielectronTask::IsMismatch(
      DielectronCandidate* cand)
{
   if (cand->stsMcTrackId == cand->richMcTrackId && cand->stsMcTrackId == cand->trdMcTrackId &&
         cand->stsMcTrackId == cand->tofMcTrackId && cand->stsMcTrackId !=-1) return false;
   return true;
}

Bool_t CbmAnaDielectronTask::IsGhost(
      DielectronCandidate* cand)
{
   if (cand->stsMcTrackId == -1 || cand->richMcTrackId == -1 || cand->trdMcTrackId == -1 ||
         cand->tofMcTrackId == -1) return true;
   return false;
}

void CbmAnaDielectronTask::IsElectron(
      CbmRichRing * ring,
      Double_t momentum,
      CbmTrdTrack* trdTrack,
      CbmGlobalTrack * gTrack,
      DielectronCandidate* cand)
{
   if (fPionMisidLevel < 0.){
      Bool_t richEl = IsRichElectron(ring, momentum, cand);
      Bool_t trdEl = (trdTrack != NULL)?IsTrdElectron(trdTrack, cand):true;
      Double_t annRich = cand->richAnn;
      Double_t annTrd = cand->trdAnn;
      Bool_t tofEl = IsTofElectron(gTrack, momentum, cand);
      Bool_t momCut = (fMomentumCut > 0.)?(momentum < fMomentumCut):true;

      if (richEl && trdEl && tofEl && momCut) {
         cand->isElectron = true;
      } else {
         cand->isElectron = false;
      }
   } else {
      // PID using MC information, a certain pi supression level can be set
       if (cand->stsMcTrackId < 0){
	   cand->isElectron = false;
       } else {
	   CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(cand->stsMcTrackId);
	   Int_t pdg = mcTrack->GetPdgCode();
	   if (pdg == 11 || pdg == -11){
	       cand->isElectron = true;
	   } else {
	       Double_t r = fRandom3->Rndm();
	       if (r < fPionMisidLevel){
		   cand->isElectron = true;
	       } else {
		   cand->isElectron = false;
	       }
	   }
       }
   }
}

Bool_t CbmAnaDielectronTask::IsRichElectron(
      CbmRichRing * ring,
      Double_t momentum,
      DielectronCandidate* cand)
{
    if (fUseRichAnn == false){
        Double_t axisA = ring->GetAaxis();
        Double_t axisB = ring->GetBaxis();
        Double_t dist = ring->GetDistance();
        if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && dist < fDistCut){
            return true;
        } else {
            return false;
        }
    } else {
        Double_t ann = fElIdAnn->DoSelect(ring, momentum);
        cand->richAnn = ann;
        if (ann > fRichAnnCut) return true;
        else  return false;
    }
}


Bool_t CbmAnaDielectronTask::IsTrdElectron(
      CbmTrdTrack* trdTrack,
      DielectronCandidate* cand)
{
    Double_t ann = trdTrack->GetPidANN();
    cand->trdAnn = ann;
    if (ann > fTrdAnnCut) return true;
    else return false;

}

Bool_t CbmAnaDielectronTask::IsTofElectron(
      CbmGlobalTrack* gTrack,
      Double_t momentum,
      DielectronCandidate* cand)
{
    Double_t trackLength = gTrack->GetLength() / 100.;

    // Calculate time of flight from TOF hit
    Int_t tofIndex = gTrack->GetTofHitIndex();
    CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
    if (NULL == tofHit){
        cand->mass2 = 100.;
        return false;
    }
    Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

    // Calculate mass squared
    Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);
    cand->mass2 =mass2;

    if (momentum >= 1.) {
        if (mass2 < (0.013*momentum - 0.003)){
            return true;
        }
    } else {
        if (mass2 < 0.01){
            return true;//fTofM2
        }
    }
    return false;
}

KinematicParams CbmAnaDielectronTask::CalculateKinematicParams(
      CbmMCTrack* mctrackP,
      CbmMCTrack* mctrackM)
{
    KinematicParams params;
    
    TVector3 momP;  //momentum e+
    mctrackP->GetMomentum(momP);
    Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E);
    TLorentzVector lorVecP(momP, energyP); 

    TVector3 momM;  //momentum e-
    mctrackM->GetMomentum(momM);
    Double_t energyM = TMath::Sqrt(momM.Mag2() + M2E);
    TLorentzVector lorVecM(momM, energyM); 

    TVector3 momPair = momP + momM;
    Double_t energyPair = energyP + energyM;
    Double_t ptPair = momPair.Perp();
    Double_t pzPair = momPair.Pz();
    Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
    Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
    Double_t theta = 180.*anglePair/TMath::Pi();
    Double_t minv = 2.*TMath::Sin(anglePair / 2.)*TMath::Sqrt(momM.Mag()*momP.Mag());

    params.momentumMag = momPair.Mag();
    params.pt = ptPair;
    params.rapidity = yPair;
    params.minv = minv;
    params.angle = theta;
    return params;
}

KinematicParams CbmAnaDielectronTask::CalculateKinematicParams(
      DielectronCandidate* candP,
      DielectronCandidate* candM)
{
    KinematicParams params;
    
    TLorentzVector lorVecP(candP->momentum, candP->energy); 
    TLorentzVector lorVecM(candM->momentum, candM->energy); 

    TVector3 momPair = candP->momentum +candM->momentum ;
    Double_t energyPair = candP->energy + candM->energy;
    Double_t ptPair = momPair.Perp();
    Double_t pzPair = momPair.Pz();
    Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
    Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
    Double_t theta = 180.*anglePair/TMath::Pi();
    Double_t minv = 2.*TMath::Sin(anglePair / 2.)*TMath::Sqrt(candM->momentum.Mag() * candP->momentum.Mag());

    params.momentumMag = momPair.Mag();
    params.pt = ptPair;
    params.rapidity = yPair;
    params.minv = minv;
    params.angle = theta;
    return params;
}

void CbmAnaDielectronTask::DifferenceSignalAndBg()
{
//ID cuts
    Int_t nCand = fCandidates.size(); 
    for (Int_t i = 0; i < nCand; i++){
        if (fCandidates[i].isMcSignalElectron){
            fh_chi2prim[kSignal]->Fill(fCandidates[i].chi2Prim, fWeight);
        } else {
            fh_chi2prim[kBg]->Fill(fCandidates[i].chi2Prim);
        }
        if (fCandidates[i].isMcGammaElectron) fh_chi2prim[kGamma]->Fill(fCandidates[i].chi2Prim);
        if (fCandidates[i].isMcPi0Electron) fh_chi2prim[kPi0]->Fill(fCandidates[i].chi2Prim);

        if (fCandidates[i].chi2Prim > fChiPrimCut ) continue;

        if (fCandidates[i].isMcSignalElectron){
            fh_richann[kSignal]->Fill(fCandidates[i].richAnn, fWeight);
            fh_trdann[kSignal]->Fill(fCandidates[i].trdAnn, fWeight);
            fh_tofm2[kSignal]->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2, fWeight);
        } else {
            fh_richann[kBg]->Fill(fCandidates[i].richAnn);
            fh_trdann[kBg]->Fill(fCandidates[i].trdAnn);
            fh_tofm2[kBg]->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2);
       }
       if (fCandidates[i].isMcGammaElectron){
          fh_richann[kGamma]->Fill(fCandidates[i].richAnn);
          fh_trdann[kGamma]->Fill(fCandidates[i].trdAnn);
          fh_tofm2[kGamma]->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2);
       }
       if (fCandidates[i].isMcPi0Electron){
          fh_richann[kPi0]->Fill(fCandidates[i].richAnn);
          fh_trdann[kPi0]->Fill(fCandidates[i].trdAnn);
          fh_tofm2[kPi0]->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2);
       }
    } // loop over candidates

    for (Int_t i = 0; i < nCand; i++){
        if (!(fCandidates[i].chi2Prim < fChiPrimCut && fCandidates[i].isElectron )) continue;

        if (fCandidates[i].isMcSignalElectron){
            fh_pt[kSignal]->Fill(fCandidates[i].momentum.Perp(), fWeight);
            fh_mom[kSignal]->Fill(fCandidates[i].momentum.Mag(), fWeight);
            fh_chi2sts[kSignal]->Fill(fCandidates[i].chi2sts, fWeight);
        } else {
            fh_pt[kBg]->Fill(fCandidates[i].momentum.Perp());
            fh_mom[kBg]->Fill(fCandidates[i].momentum.Mag());
            fh_chi2sts[kBg]->Fill(fCandidates[i].chi2sts);
        }
        if (fCandidates[i].isMcGammaElectron){
           fh_pt[kGamma]->Fill(fCandidates[i].momentum.Perp());
           fh_mom[kGamma]->Fill(fCandidates[i].momentum.Mag());
           fh_chi2sts[kGamma]->Fill(fCandidates[i].chi2sts);
        }
        if (fCandidates[i].isMcPi0Electron){
           fh_pt[kPi0]->Fill(fCandidates[i].momentum.Perp());
           fh_mom[kPi0]->Fill(fCandidates[i].momentum.Mag());
           fh_chi2sts[kPi0]->Fill(fCandidates[i].chi2sts);
        }
    } // loop over candidates

   if (fUseMvd){
      //number of STS and MVD hits and distributions for the MVD
      for (int i = 0; i < nCand; i++){
         if (!(fCandidates[i].chi2Prim < fChiPrimCut && fCandidates[i].isElectron )) continue;

         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].stsInd);
         if (NULL == track) continue;
         int nofMvdHits = track->GetNMvdHits();
         int nofStsHits = track->GetNStsHits();
         double mvd1x = 0., mvd1y = 0., mvd2x = 0., mvd2y = 0.;

         for(Int_t ith = 0; ith < nofMvdHits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
            Int_t stationNum = pmh->GetStationNr();
            if(NULL == pmh) continue;
            if (stationNum == 1){
               mvd1x = pmh->GetX();
               mvd1y = pmh->GetY();
            } else if (stationNum == 2){
               mvd2x = pmh->GetX();
               mvd2y = pmh->GetY();
            }
         }
         double mvd1r = sqrt(mvd1x*mvd1x + mvd1y*mvd1y);
         double mvd2r = sqrt(mvd2x*mvd2x + mvd2y*mvd2y);

         if (fCandidates[i].isMcSignalElectron){
            fh_nofMvdHits[kSignal]->Fill(nofMvdHits, fWeight);
            fh_nofStsHits[kSignal]->Fill(nofStsHits, fWeight);
            fh_mvd1xy[kSignal]->Fill(mvd1x, mvd1y, fWeight);
            fh_mvd1r[kSignal]->Fill(mvd1r, fWeight);
            fh_mvd2xy[kSignal]->Fill(mvd2x, mvd2y, fWeight);
            fh_mvd2r[kSignal]->Fill(mvd2r, fWeight);
         } else {
            fh_nofMvdHits[kBg]->Fill(nofMvdHits);
            fh_nofStsHits[kBg]->Fill(nofStsHits);
            fh_mvd1xy[kBg]->Fill(mvd1x, mvd1y);
            fh_mvd1r[kBg]->Fill(mvd1r);
            fh_mvd2xy[kBg]->Fill(mvd2x, mvd2y);
            fh_mvd2r[kBg]->Fill(mvd2r);
         }
         if (fCandidates[i].isMcGammaElectron){
            fh_nofMvdHits[kGamma]->Fill(nofMvdHits);
            fh_nofStsHits[kGamma]->Fill(nofStsHits);
            fh_mvd1xy[kGamma]->Fill(mvd1x, mvd1y);
            fh_mvd1r[kGamma]->Fill(mvd1r);
            fh_mvd2xy[kGamma]->Fill(mvd2x, mvd2y);
            fh_mvd2r[kGamma]->Fill(mvd2r);
         }
         if (fCandidates[i].isMcPi0Electron){
            fh_nofMvdHits[kPi0]->Fill(nofMvdHits);
            fh_nofStsHits[kPi0]->Fill(nofStsHits);
            fh_mvd1xy[kPi0]->Fill(mvd1x, mvd1y);
            fh_mvd1r[kPi0]->Fill(mvd1r);
            fh_mvd2xy[kPi0]->Fill(mvd2x, mvd2y);
            fh_mvd2r[kPi0]->Fill(mvd2r);
         }
      }
   }

   // Armenteros - Podolansky plot
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].charge > 0){
         if (fCandidates[iP].chi2Prim < fChiPrimCut && fCandidates[iP].isElectron){
            for (Int_t iM = 0; iM < nCand; iM++){
               if ( iM != iP && fCandidates[iM].charge < 0){
                  if (fCandidates[iM].chi2Prim < fChiPrimCut && fCandidates[iM].isElectron){
                     Double_t alfa = 0.;
                     Double_t ptt = 0.;
                     CalculateArmPodParams(&fCandidates[iP], &fCandidates[iM], alfa, ptt);
                     if (fCandidates[iP].isMcSignalElectron && fCandidates[iM].isMcSignalElectron){
                        fh_apcut[kSignal]->Fill(alfa, ptt, fWeight);
                     }
                     if ( !(fCandidates[iP].isMcSignalElectron || fCandidates[iM].isMcSignalElectron) ){
                        fh_apcut[kBg]->Fill(alfa, ptt);
                     }
                     if (fCandidates[iP].isMcPi0Electron && fCandidates[iM].isMcPi0Electron &&
                           fCandidates[iP].McMotherId == fCandidates[iM].McMotherId)
                        fh_apcut[kPi0]->Fill(alfa, ptt);
                     if (fCandidates[iP].isMcGammaElectron && fCandidates[iM].isMcGammaElectron &&
                           fCandidates[iP].McMotherId == fCandidates[iM].McMotherId)
                        fh_apcut[kGamma]->Fill(alfa, ptt);
                  } // if electron
               }// if charge < 0
            }// iM
         }// if electron
      }// charge >0
   }// iP
}

// Armenteros - Podolansky plot
void CbmAnaDielectronTask::CalculateArmPodParams(
      DielectronCandidate* cand1,
      DielectronCandidate* cand2,
      Double_t &alpha,
      Double_t &ptt)
{
    alpha = ptt = 0.;
    Double_t spx = cand1->momentum.X() + cand2->momentum.X();
    Double_t spy = cand1->momentum.Y() + cand2->momentum.Y();
    Double_t spz = cand1->momentum.Z() + cand2->momentum.Z();
    Double_t sp  = sqrt(spx*spx + spy*spy + spz*spz);

    if( sp == 0.0) return;
    Double_t pn, pp, pln, plp;
    if(cand1->charge < 0.) {
        pn = cand1->momentum.Mag();
        pp = cand2->momentum.Mag();
        pln  = (cand1->momentum.X()*spx + cand1->momentum.Y()*spy + cand1->momentum.Z()*spz)/sp;
        plp  = (cand2->momentum.X()*spx + cand2->momentum.Y()*spy + cand2->momentum.Z()*spz)/sp;
    }else{
        pn = cand2->momentum.Mag();
        pp = cand1->momentum.Mag();
        pln  = (cand2->momentum.X()*spx + cand2->momentum.Y()*spy + cand2->momentum.Z()*spz)/sp;
        plp  = (cand1->momentum.X()*spx + cand1->momentum.Y()*spy + cand1->momentum.Z()*spz)/sp;
    }
    if( pn == 0.0) return;
    Double_t ptm  = (1.-((pln/pn)*(pln/pn)));
    ptt= (ptm>=0.)?  pn*sqrt(ptm) :0;
    alpha = (plp-pln)/(plp+pln);
}

void CbmAnaDielectronTask::CheckClosestMvdHit(
		Int_t mvdStationNum,
		vector<TH2D*>& hist,
		vector<TH1D*>& histQa)
{
   vector<float> mvdX;
   vector<float> mvdY;
   vector<int> mvdInd;
   vector<float> candX;
   vector<float> candY;
   vector<int> candInd;

   CbmKF *KF = CbmKF::Instance();

   Int_t nMvdHit = fMvdHits->GetEntriesFast();
   for(Int_t iHit = 0; iHit < nMvdHit; iHit++) {
      CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
      if(NULL == pmh) continue;
      Int_t stationNum = pmh->GetStationNr();
      if (stationNum == mvdStationNum){
         if ( MvdPlaneCut(pmh->GetX(), pmh->GetY(), mvdStationNum) ){
            mvdX.push_back(pmh->GetX());
            mvdY.push_back(pmh->GetY());
            mvdInd.push_back(iHit);
         }
      }
   }

   Int_t nCand = fCandidates.size();
   for (Int_t i = 0; i < nCand; i++){
      if (fCandidates[i].chi2Prim < fChiPrimCut && fCandidates[i].isElectron){
         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].stsInd);
         if(NULL == track) continue;
         Int_t nhits = track->GetNMvdHits();
         for(Int_t ith = 0; ith < nhits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
            Int_t stationNum = pmh->GetStationNr();
            if(NULL == pmh) continue;
            if (stationNum == mvdStationNum){
               if ( MvdPlaneCut(pmh->GetX(), pmh->GetY(), mvdStationNum) ){
                  candX.push_back(pmh->GetX());
                  candY.push_back(pmh->GetY());
                  candInd.push_back(i);
               }
            }
         }
      }
   }// iCand

   for(Int_t iT = 0; iT < candInd.size(); iT++) {
      Float_t mind = 9999999.;
      Int_t minMvdInd = -1;
      for(Int_t iH = 0; iH < mvdX.size(); iH++) {
         Float_t dx = mvdX[iH] - candX[iT];
         Float_t dy = mvdY[iH] - candY[iT];
         Float_t d2 = dx*dx + dy*dy;
         if(d2 < 1.e-9) continue;
         if(d2 < mind){
            minMvdInd = mvdInd[iH];
            mind = d2;
         }
      } // iHit
      Double_t dmvd = sqrt(mind);

      // Check MVD cut quality
      double bin = -1.;
      const CbmMvdHitMatch* hitMatch = static_cast<const CbmMvdHitMatch*>(fMvdHitMatches->At(minMvdInd));
      if (NULL != hitMatch){
         int mcMvdHitId = hitMatch->GetTrackId();
         CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(mcMvdHitId);
         int mcMvdHitPdg = TMath::Abs(mct1->GetPdgCode());
         int mvdMotherId = mct1->GetMotherId();

	 int stsMcTrackId = fCandidates[candInd[iT]].stsMcTrackId;
         int stsMotherId = -2;
	 if (stsMcTrackId >=0){
	     CbmMCTrack* mct2 = (CbmMCTrack*) fMCTracks->At(stsMcTrackId);
	     stsMotherId = mct2->GetMotherId();
	 }

         //cout << mvdStationNum << " " << mvdMotherId << " " << stsMotherId << endl;
         if ( mvdMotherId != -1 && mvdMotherId == stsMotherId){
            bin = 0.5; // correct assignment
         } else {
            bin = 1.5; // not correct assignment
         }

         if (fCandidates[candInd[iT]].isMcSignalElectron){
            if (mvdMotherId == stsMotherId && mcMvdHitPdg == 11){
               bin = 0.5; // correct assignment
            }else{
               bin = 1.5;
            }
         }
      }
      //cout << "MVD cut correctness " << bin << endl;

      // Fill histograms
      fCandidates[candInd[iT]].dSts = dmvd;
      if (fCandidates[candInd[iT]].isMcSignalElectron){
         hist[kSignal]->Fill(dmvd, fCandidates[candInd[iT]].momentum.Mag(), fWeight);
         histQa[kSignal]->Fill(bin, fWeight);
      }else {
         hist[kBg]->Fill(dmvd, fCandidates[candInd[iT]].momentum.Mag());
         histQa[kBg]->Fill(bin);
      }
      if (fCandidates[candInd[iT]].isMcGammaElectron){
         hist[kGamma]->Fill(dmvd, fCandidates[candInd[iT]].momentum.Mag());
         histQa[kGamma]->Fill(bin);
      }
      if (fCandidates[candInd[iT]].isMcPi0Electron){
         hist[kPi0]->Fill(dmvd, fCandidates[candInd[iT]].momentum.Mag());
         histQa[kPi0]->Fill(bin);
      }

      // Apply MVD cut
      if (mvdStationNum == 1){
         Double_t mom = fCandidates[candInd[iT]].momentum.Mag();
         Double_t val = -1.*(fMvd1CutP/fMvd1CutD)*dmvd + fMvd1CutP;
         if ( !(dmvd < fMvd1CutD && val > mom) ) {
            fCandidates[candInd[iT]].isMvd1CutElectron = true;
         } else {
            fCandidates[candInd[iT]].isMvd1CutElectron = false;
         }
      }
      if (mvdStationNum == 2){
         Double_t mom = fCandidates[candInd[iT]].momentum.Mag();
         Double_t val = -1.*(fMvd2CutP/fMvd2CutD)*dmvd + fMvd2CutP;
         if ( !(dmvd < fMvd2CutD && val > mom) ){
            fCandidates[candInd[iT]].isMvd2CutElectron = true;
         }else {
            fCandidates[candInd[iT]].isMvd2CutElectron = false;
         }
      }
   } // iTrack
}

bool CbmAnaDielectronTask::MvdPlaneCut(
      double x,
      double y,
      int stationNum)
{
   return true;
   if (stationNum == 1){
      if ( !(y < -0.5*x + 0.6 && 0.5*x - 0.6 < y && x < 0)) return true;
   }

   if (stationNum == 2){
      if ( !(y > 0.43*x - 0.6 && -0.43*x + 0.6 < y && x < 0) ) return true;
   }

   return false;
}

void CbmAnaDielectronTask::MvdCutMcDistance()
{
   if (!fUseMvd) return;
   Int_t nCand = fCandidates.size();
   for (Int_t i = 0; i < nCand; i++){
      if (fCandidates[i].chi2Prim < fChiPrimCut && fCandidates[i].isElectron){
         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].stsInd);
         if(NULL == track) continue;
         int stsMcTrackId = fCandidates[i].stsMcTrackId;
         Int_t nhits = track->GetNMvdHits();
         for(Int_t ith = 0; ith < nhits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh1 = (CbmMvdHit*) fMvdHits->At(iHit);
            if (NULL == pmh1) continue;
            Int_t stationNum = pmh1->GetStationNr();

            int nofMvdHits = fMvdHitMatches->GetEntriesFast();
            for (int iMvd = 0; iMvd < nofMvdHits; iMvd++){
               const CbmMvdHitMatch* hitMatch = static_cast<const CbmMvdHitMatch*>(fMvdHitMatches->At(iMvd));
               if (NULL == hitMatch) continue;
               int mcMvdHitId = hitMatch->GetTrackId();
               if (stsMcTrackId != mcMvdHitId) continue;
               CbmMvdHit *pmh2 = (CbmMvdHit*) fMvdHits->At(iMvd);
               if ( pmh2->GetStationNr() != stationNum) continue;
               double dx = pmh1->GetX() - pmh2->GetX();
               double dy = pmh1->GetY() - pmh2->GetY();
               double d = sqrt(dx*dx + dy*dy);
               if (stationNum == 1){
                  if (fCandidates[i].isMcGammaElectron) fh_mvd1cut_mc_dist_gamma->Fill(d);
                  if (fCandidates[i].isMcPi0Electron) fh_mvd1cut_mc_dist_pi0->Fill(d);
               }else if (stationNum == 1){
                  if (fCandidates[i].isMcGammaElectron) fh_mvd2cut_mc_dist_gamma->Fill(d);
                  if (fCandidates[i].isMcPi0Electron) fh_mvd2cut_mc_dist_pi0->Fill(d);
               }

            }
         }
      }
   }// iCan
}

void CbmAnaDielectronTask::Finish()
{
   // Write histograms to a file
   for (Int_t i = 0; i < fHistoList.size(); i++){
     fHistoList[i]->Write();
   }
}
