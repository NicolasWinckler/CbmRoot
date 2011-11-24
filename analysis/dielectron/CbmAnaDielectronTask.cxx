/** CbmAnaDielectronTask.cxx
 * based on class by T.Galatyuk
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 2.0
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
#include "CbmRichRingMatch.h"
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

#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"


#define M2E 2.6112004954086e-7

using namespace std;

ClassImp(CbmAnaDielectronTask);

TH1D* CbmAnaDielectronTask::CreateMinvHisto(
      const string& name)
{
   TH1D* h = new TH1D(name.c_str(), name.c_str(), 2000, 0., 2.);
   h->GetXaxis()->SetTitle("M_{ee} [GeV/c^{2}]");
   h->GetYaxis()->SetTitle("Yield");
   fHistoList.push_back(h);
   return h;
}

TH1D* CbmAnaDielectronTask::CreateSignalMomHisto(
      const string& name)
{
   TH1D* h = new TH1D(name.c_str(), name.c_str(), 100, 0., 15.);
   h->GetXaxis()->SetTitle("p [GeV/c]");
   h->GetYaxis()->SetTitle("Yield");
   fHistoList.push_back(h);
   return h;
}

TH2D* CbmAnaDielectronTask::CreatePtYHisto(
      const string& name)
{
   TH2D* h = new TH2D(name.c_str(), name.c_str(), 40, 0., 4., 20, 0., 2.);
   h->GetXaxis()->SetTitle("Rapidity");
   h->GetYaxis()->SetTitle("p_{t} [GeV/c]");
   fHistoList.push_back(h);
   return h;
}

CbmAnaDielectronTask::CbmAnaDielectronTask(
      const char *name,
      const char *title)
: FairTask(name)
{
   // weight for rho0 = 0.001081; omega_ee = 0.0026866; omega_dalitz = 0.02242; phi = 0.00039552; pi0 = 4.38   ------ for 25 GeV
   fWeight = 0.0;
   fUseRich = true;
   fUseTrd = true;
   fUseTof = true;

   fHistoList.clear();

   //signal momentum distribution
   fh_mc_signal_mom = CreateSignalMomHisto("fh_mc_signal_mom");
   fh_acc_signal_mom = CreateSignalMomHisto("fh_acc_signal_mom");
   fh_reco_signal_mom = CreateSignalMomHisto("fh_reco_signal_mom");
   fh_chi_prim_signal_mom = CreateSignalMomHisto("fh_chi_prim_signal_mom");
   fh_el_id_signal_mom = CreateSignalMomHisto("fh_el_id_signal_mom");
   fh_ptcut_signal_mom = CreateSignalMomHisto("fh_ptcut_signal_mom");
   fh_anglecut_signal_mom = CreateSignalMomHisto("fh_anglecut_signal_mom");
   fh_gammacut_signal_mom = CreateSignalMomHisto("fh_gammacut_signal_mom");
   fh_ttcut_signal_mom = CreateSignalMomHisto("fh_ttcut_signal_mom");
   fh_stcut_signal_mom = CreateSignalMomHisto("fh_stcut_signal_mom");
   fh_apmcut_signal_mom = CreateSignalMomHisto("fh_apmcut_signal_mom");
   fh_dstscut_signal_mom = CreateSignalMomHisto("fh_dstscut_signal_mom");
   fh_dsts2cut_signal_mom = CreateSignalMomHisto("fh_dsts2cut_signal_mom");

   // Mother PDG
   fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
   fHistoList.push_back(fh_mc_mother_pdg);
   fh_acc_mother_pdg = new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
   fHistoList.push_back(fh_acc_mother_pdg);

   fh_mc_vertex_gamma_xz = new TH2D("fh_mc_vertex_gamma_xz","fh_mc_vertex_gamma_xz;Z [cm];X [cm]",200, -10., 110., 200, -100.,100.);
   fHistoList.push_back(fh_mc_vertex_gamma_xz);
   fh_mc_vertex_gamma_yz = new TH2D("fh_mc_vertex_gamma_yz", "fh_mc_vertex_gamma_yz;Z [cm];Y [cm]", 200, -10., 110., 200, -100., 100.);
   fHistoList.push_back(fh_mc_vertex_gamma_yz);
   fh_mc_vertex_gamma_xy = new TH2D("fh_mc_vertex_gamma_xy","fh_mc_vertex_gamma_xy;X [cm];Y [cm]", 200, -100.,100., 200, -100., 100.);
   fHistoList.push_back(fh_mc_vertex_gamma_xy);

   // signal minv
   fh_mc_signal_minv = CreateMinvHisto("fh_mc_signal_minv");
   fh_acc_signal_minv = CreateMinvHisto("fh_acc_signal_minv");
   fh_rec_signal_minv = CreateMinvHisto("fh_rec_signal_minv");
   fh_chi_prim_signal_minv = CreateMinvHisto("fh_chi_prim_signal_minv");
   fh_el_id_signal_minv = CreateMinvHisto("fh_el_id_signal_minv");
   fh_gammacut_signal_minv = CreateMinvHisto("fh_gammacut_signal_minv");
   fh_stcut_signal_minv = CreateMinvHisto("fh_stcut_signal_minv");
   fh_ttcut_signal_minv = CreateMinvHisto("fh_ttcut_signal_minv");
   fh_ptcut_signal_minv = CreateMinvHisto("fh_ptcut_signal_minv");
   fh_anglecut_signal_minv = CreateMinvHisto("fh_anglecut_signal_minv");
   fh_apmcut_signal_minv = CreateMinvHisto("fh_apmcut_signal_minv");
   fh_dstscut_signal_minv = CreateMinvHisto("fh_dstscut_signal_minv");
   fh_dsts2cut_signal_minv = CreateMinvHisto("fh_dsts2cut_signal_minv");

   // signal minv vs. Pt
   fh_ttcut_signal_minv_pt = new TH2D("fh_ttcut_signal_minv_pt","fh_ttcut_signal_minv_pt;M_{ee} [GeV/c^{2}];p_{t} [GeV/c]",2000, 0., 2., 20, 0., 2.);
   fHistoList.push_back(fh_ttcut_signal_minv_pt);
   fh_ptcut_signal_minv_pt = new TH2D("fh_ptcut_signal_minv_pt","fh_ptcut_signal_minv_pt;M_{ee} [GeV/c^{2}];p_{t} [GeV/c]",2000, 0., 2., 20, 0., 2.);
   fHistoList.push_back(fh_ptcut_signal_minv_pt);

   //BG minv
   fh_rec_bg_minv = CreateMinvHisto("fh_rec_bg_minv");
   fh_chi_prim_bg_minv = CreateMinvHisto("fh_chi_prim_bg_minv");
   fh_el_id_bg_minv = CreateMinvHisto("fh_el_id_bg_minv");
   fh_ptcut_bg_minv = CreateMinvHisto("fh_ptcut_bg_minv");
   fh_anglecut_bg_minv = CreateMinvHisto("fh_anglecut_bg_minv");
   fh_gammacut_bg_minv = CreateMinvHisto("fh_gammacut_bg_minv");
   fh_ttcut_bg_minv = CreateMinvHisto("fh_ttcut_bg_minv");
   fh_stcut_bg_minv = CreateMinvHisto("fh_stcut_bg_minv");
   fh_apmcut_bg_minv = CreateMinvHisto("fh_apmcut_bg_minv");
   fh_dstscut_bg_minv = CreateMinvHisto("fh_dstscut_bg_minv");
   fh_dsts2cut_bg_minv = CreateMinvHisto("fh_dsts2cut_bg_minv");

   //pi0 minv
   fh_rec_pi0_minv = CreateMinvHisto("fh_rec_pi0_minv");
   fh_chi_prim_pi0_minv = CreateMinvHisto("fh_chi_prim_pi0_minv");
   fh_el_id_pi0_minv = CreateMinvHisto("fh_el_id_pi0_minv");
   fh_ptcut_pi0_minv = CreateMinvHisto("fh_ptcut_pi0_minv");
   fh_anglecut_pi0_minv = CreateMinvHisto("fh_anglecut_pi0_minv");
   fh_gammacut_pi0_minv = CreateMinvHisto("fh_gammacut_pi0_minv");
   fh_ttcut_pi0_minv = CreateMinvHisto("fh_ttcut_pi0_minv");
   fh_stcut_pi0_minv = CreateMinvHisto("fh_stcut_pi0_minv");
   fh_apmcut_pi0_minv = CreateMinvHisto("fh_apmcut_pi0_minv");
   fh_dstscut_pi0_minv = CreateMinvHisto("fh_dstscut_pi0_minv");
   fh_dsts2cut_pi0_minv = CreateMinvHisto("fh_dsts2cut_pi0_minv");

   //eta minv
   fh_rec_eta_minv = CreateMinvHisto("fh_rec_eta_minv");
   fh_chi_prim_eta_minv = CreateMinvHisto("fh_chi_prim_eta_minv");
   fh_el_id_eta_minv = CreateMinvHisto("fh_el_id_eta_minv");
   fh_ptcut_eta_minv = CreateMinvHisto("fh_ptcut_eta_minv");
   fh_anglecut_eta_minv = CreateMinvHisto("fh_anglecut_eta_minv");
   fh_gammacut_eta_minv = CreateMinvHisto("fh_gammacut_eta_minv");
   fh_ttcut_eta_minv = CreateMinvHisto("fh_ttcut_eta_minv");
   fh_stcut_eta_minv = CreateMinvHisto("fh_stcut_eta_minv");
   fh_apmcut_eta_minv = CreateMinvHisto("fh_apmcut_eta_minv");
   fh_dstscut_eta_minv = CreateMinvHisto("fh_dstscut_eta_minv");
   fh_dsts2cut_eta_minv = CreateMinvHisto("fh_dsts2cut_eta_minv");

   //gamma minv
//  fh_rec_gamma_minv = new TH1D("fh_rec_gamma_minv","fh_rec_gamma_minv;M_{ee} [GeV/c^{2}];yeild", nofBinsMinv, 0., 2.);
//  fh_chi_prim_gamma_minv = new TH1D("fh_chi_prim_gamma_minv","fh_chi_prim_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);
//  fh_el_id_gamma_minv = new TH1D("fh_el_id_gamma_minv","fh_el_id_gamma_minv;M_{ee} [GeV/c^{2}];yeild", nofBinsMinv, 0., 2.);
//  fh_ptcut_gamma_minv = new TH1D("fh_ptcut_gamma_minv","fh_ptcut_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);
//  fh_anglecut_gamma_minv = new TH1D("fh_anglecut_gamma_minv","fh_anglecut_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);
//  fh_gammacut_gamma_minv = new TH1D("fh_gammacut_gamma_minv","fh_gammacut_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);
//  fh_ttcut_gamma_minv = new TH1D("fh_ttcut_gamma_minv","fh_ttcut_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);
//  fh_stcut_gamma_minv = new TH1D("fh_stcut_gamma_minv","fh_stcut_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);
//  fh_apmcut_gamma_minv = new TH1D("fh_apmcut_gamma_minv","fh_apmcut_gamma_minv;M_{ee} [GeV/c^{2}];yeild",nofBinsMinv, 0., 2.);

   //pty distribution of the signal
   fh_mc_signal_pty = CreatePtYHisto("fh_mc_signal_pty");
   fh_acc_signal_pty = CreatePtYHisto("fh_acc_signal_pty");
   fh_reco_signal_pty = CreatePtYHisto("fh_reco_signal_pty");
   fh_chi_prim_signal_pty = CreatePtYHisto("fh_chi_prim_signal_pty");
   fh_el_id_signal_pty = CreatePtYHisto("fh_el_id_signal_pty");
   fh_ptcut_signal_pty = CreatePtYHisto("fh_ptcut_signal_pty");
   fh_anglecut_signal_pty = CreatePtYHisto("fh_anglecut_signal_pty");
   fh_gammacut_signal_pty = CreatePtYHisto("fh_gammacut_signal_pty");
   fh_ttcut_signal_pty = CreatePtYHisto("fh_ttcut_signal_pty");
   fh_stcut_signal_pty = CreatePtYHisto("fh_stcut_signal_pty");
   fh_apmcut_signal_pty = CreatePtYHisto("fh_apmcut_signal_pty");
   fh_dstscut_signal_pty = CreatePtYHisto("fh_dstscut_signal_pty");
   fh_dsts2cut_signal_pty = CreatePtYHisto("fh_dsts2cut_signal_pty");

   // analysis cut distributions
   fh_angle_signal = new TH1D("fh_angle_signal","fh_angle_signal;#Theta_{1,2} [deg];Yield", 160, 0., 80.);
   fHistoList.push_back(fh_angle_signal);
   fh_angle_bg = new TH1D("fh_angle_bg","fh_angle_bg;#Theta_{1,2} [deg];Yield", 160, 0., 80.);
   fHistoList.push_back(fh_angle_bg);
   fh_pt_signal = new TH1D("fh_pt_signal","fh_pt_signal;p_{t} [GeV/c];Yield",200, 0., 2.);
   fHistoList.push_back(fh_pt_signal);
   fh_pt_bg = new TH1D("fh_pt_bg","fh_pt_bg;p_{t} [GeV/c];Yield",200, 0., 2.);
   fHistoList.push_back(fh_pt_bg);
   fh_mom_signal = new TH1D("fh_mom_signal","fh_mom_signal;p [GeV/c];Yield",200, 0., 10.);
   fHistoList.push_back(fh_mom_signal);
   fh_mom_bg = new TH1D("fh_mom_bg","fh_mom_bg;p [GeV/c];Yield",200, 0., 10.);
   fHistoList.push_back(fh_mom_bg);
   fh_chi2_prim_signal = new TH1D("fh_chi2_prim_signal", "fh_chi2_prim_signal;#chi^{2}_{prim};Yield", 200, 0., 20.);
   fHistoList.push_back(fh_chi2_prim_signal);
   fh_chi2_prim_bg = new TH1D("fh_chi2_prim_bg","fh_chi2_prim_bg;#chi^{2}_{prim};Yield", 200, 0., 20.);
   fHistoList.push_back(fh_chi2_prim_bg);
   fh_chi2sts_signal = new TH1D("fh_chi2sts_signal", "fh_chi2sts_signal;#chi^{2};Yield", 200, 0., 20.);
   fHistoList.push_back(fh_chi2sts_signal);
   fh_chi2sts_bg = new TH1D("fh_chi2sts_bg","fh_chi2sts_bg;#chi^{2};Yield", 200, 0., 20.);
   fHistoList.push_back(fh_chi2sts_bg);
   // TT cut
   fh_ttcut_signal = new TH2D("fh_ttcut_signal", "fh_ttcut_signal;#sqrt{p_{e^{+}} p_{e^{-}}} [GeV/c];#theta_{e^{+},e^{-}} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_ttcut_signal);
   fh_ttcut_bg = new TH2D("fh_ttcut_bg","fh_ttcut_bg;#sqrt{p_{e^{+}} p_{e^{-}}} [GeV/c];#theta_{e^{+},e^{-}} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_ttcut_bg);
   fh_ttcut_pi0 = new TH2D("fh_ttcut_pi0","fh_ttcut_pi0;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_ttcut_pi0);
   fh_ttcut_gamma = new TH2D("fh_ttcut_gamma","fh_ttcut_gamma;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_ttcut_gamma);
   // ST cut
   fh_stcut_signal = new TH2D("fh_stcut_signal", "fh_stcut_signal;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_stcut_signal);
   fh_stcut_bg = new TH2D("fh_stcut_bg","fh_stcut_bg;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_stcut_bg);
   fh_stcut_pi0 = new TH2D("fh_stcut_pi0","fh_stcut_pi0;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_stcut_pi0);
   fh_stcut_gamma = new TH2D("fh_stcut_gamma","fh_stcut_gamma;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
   fHistoList.push_back(fh_stcut_gamma);
   // AP cut
   fh_apcut_signal = new TH2D("fh_apcut_signal", "fh_apcut_signal;#alpha;p_{t} [GeV/c]", 100, -1., 1., 200, 0., 1.);
   fHistoList.push_back(fh_apcut_signal);
   fh_apcut_bg = new TH2D("fh_apcut_bg","fh_apcut_bg;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);
   fHistoList.push_back(fh_apcut_bg);
   fh_apcut_pi0 = new TH2D("fh_apcut_pi0","fh_apcut_pi0;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);
   fHistoList.push_back(fh_apcut_pi0);
   fh_apcut_gamma = new TH2D("fh_apcut_gamma","fh_apcut_gamma;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);
   fHistoList.push_back(fh_apcut_gamma);
   // APM cut
   fh_apmcut_signal = new TH2D("fh_apmcut_signal", "fh_apmcut_signal;#alpha;p_{t} [GeV/c]", 100, -1., 1., 200, 0., 1.);
   fHistoList.push_back(fh_apmcut_signal);
   fh_apmcut_bg = new TH2D("fh_apmcut_bg","fh_apmcut_bg;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);
   fHistoList.push_back(fh_apmcut_bg);
   //dmvd cut for the 1st MVD
   fh_dsts_signal = new TH2D("fh_dsts_signal","fh_dsts_signal;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts_signal);
   fh_dsts_bg = new TH2D("fh_dsts_bg", "fh_dsts_bg;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts_bg);
   fh_dsts_gamma = new TH2D("fh_dsts_gamma","fh_dsts_gamma;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts_gamma);
   fh_dsts_pi0 = new TH2D("fh_dsts_pi0","fh_dsts_pi0;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts_pi0);
   fh_dsts_eta = new TH2D("fh_dsts_eta","fh_dsts_eta;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts_eta);
   //dmvd cut for the 2nd MVD
   fh_dsts2_signal = new TH2D("fh_dsts2_signal","fh_dsts2_signal;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts2_signal);
   fh_dsts2_bg = new TH2D("fh_dsts2_bg", "fh_dsts2_bg;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts2_bg);
   fh_dsts2_gamma = new TH2D("fh_dsts2_gamma","fh_dsts2_gamma;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts2_gamma);
   fh_dsts2_pi0 = new TH2D("fh_dsts2_pi0","fh_dsts2_pi0;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts2_pi0);
   fh_dsts2_eta = new TH2D("fh_dsts2_eta","fh_dsts2_eta;d_{MVD} [cm];p_{e} [GeV/c]", 100, 0., 1., 100, 0., 5.);
   fHistoList.push_back(fh_dsts2_eta);

   // sources of BG pairs 2D
   fh_source_pair_reco = new TH2D("fh_source_pair_reco","fh_source_pair_reco;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_reco);
   fh_source_pair_chi_prim = new TH2D("fh_source_pair_chi_prim","fh_source_pair_chi_prim;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_chi_prim);
   fh_source_pair_el_id = new TH2D("fh_source_pair_el_id","fh_source_pair_el_id;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_el_id);
   fh_source_pair_ptcut = new TH2D("fh_source_pair_ptcut","fh_source_pair_ptcut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_ptcut);
   fh_source_pair_anglecut = new TH2D("fh_source_pair_anglecut","fh_source_pair_anglecut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_anglecut);
   fh_source_pair_gammacut = new TH2D("fh_source_pair_gammacut","fh_source_pair_gammacut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_gammacut);
   fh_source_pair_ttcut = new TH2D("fh_source_pair_ttcut","fh_source_pair_ttcut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_ttcut);
   fh_source_pair_stcut = new TH2D("fh_source_pair_stcut","fh_source_pair_stcut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_stcut);
   fh_source_pair_apmcut = new TH2D("fh_source_pair_apmcut","fh_source_pair_apmcut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_apmcut);
   fh_source_pair_dstscut = new TH2D("fh_source_pair_dstscut","fh_source_pair_dstscut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_dstscut);
   fh_source_pair_dsts2cut = new TH2D("fh_source_pair_dsts2cut","fh_source_pair_dsts2cut;mother particle e+;mother particle e-", 3, 0., 3., 3, 0., 3.);
   fHistoList.push_back(fh_source_pair_dsts2cut);

   // Number of BG and signal tracks after each cut
   fh_nof_bg_tracks = new TH1D("fh_nof_bg_tracks","fh_nof_bg_tracks;analysis steps;tracks/event", 7, 0., 7.);
   fHistoList.push_back(fh_nof_bg_tracks);
   fh_nof_el_tracks = new TH1D("fh_nof_el_tracks","fh_nof_el_tracks;analysis steps;tracks/event", 7, 0., 7.);
   fHistoList.push_back(fh_nof_el_tracks);
   fh_source_tracks = new  TH2D("fh_source_tracks","fh_source_tracks;analysis steps;particle",7, 0., 7., 6, 0., 6.);
   fHistoList.push_back(fh_source_tracks);

   // ID cut distributions
   fh_rich_ann_signal = new TH1D("fh_rich_ann_signal", "fh_rich_ann_signal;ann output;Yield", 100, -1.1, 1.1);
   fHistoList.push_back(fh_rich_ann_signal);
   fh_rich_ann_bg = new TH1D("fh_rich_ann_bg", "fh_rich_ann_bg;ann output;Yield", 100, -1.1, 1.1);
   fHistoList.push_back(fh_rich_ann_bg);
   fh_trd_ann_signal = new TH1D("fh_trd_ann_signal", "fh_trd_ann_signal;ann output;Yield", 100, -1.1, 1.1);
   fHistoList.push_back(fh_trd_ann_signal);
   fh_trd_ann_bg = new TH1D("fh_trd_ann_bg", "fh_trd_ann_bg;ann output;Yield", 100, -1.1, 1.1);
   fHistoList.push_back(fh_trd_ann_bg);
   fh_tof_m2_signal = new TH2D("fh_tof_m2_signal","fh_tof_m2_signal;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2);
   fHistoList.push_back(fh_tof_m2_signal);
   fh_tof_m2_bg = new TH2D("fh_tof_m2_bg","fh_tof_m2_bg;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2);
   fHistoList.push_back(fh_tof_m2_bg);

   // Event number counter
   fh_event_number = new TH1D("fh_event_number","fh_event_number", 1, 0, 1.);
   fHistoList.push_back(fh_event_number);

   // Momentum distributions of the BG tracks
   fh_gamma_mom = new TH1D("fh_gamma_mom","fh_gamma_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_gamma_mom);
   fh_pi0_mom = new TH1D("fh_pi0_mom","fh_pi0_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_pi0_mom);
   fh_pions_mom = new TH1D("fh_pions_mom","fh_pions_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_pions_mom);
   fh_proton_mom = new TH1D("fh_proton_mom","fh_proton_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_proton_mom);
   fh_kaons_mom = new TH1D("fh_kaons_mom","fh_kaons_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_kaons_mom);
   fh_other_mom = new TH1D("fh_other_mom","fh_other_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_other_mom);

   fh_gamma_id_mom = new TH1D("fh_gamma_id_mom","fh_gamma_id_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_gamma_id_mom);
   fh_pi0_id_mom = new TH1D("fh_pi0_id_mom","fh_pi0_id_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_pi0_id_mom);
   fh_pions_id_mom = new TH1D("fh_pions_id_mom","fh_pions_id_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_pions_id_mom);
   fh_proton_id_mom = new TH1D("fh_proton_id_mom","fh_proton_id_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_proton_id_mom);
   fh_kaons_id_mom = new TH1D("fh_kaons_id_mom","fh_kaons_id_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_kaons_id_mom);
   fh_other_id_mom = new TH1D("fh_other_id_mom","fh_other_id_mom;p [GeV/c];Yield",100, 0., 10);
   fHistoList.push_back(fh_other_id_mom);

   SetDefaultIdParameters();
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

void CbmAnaDielectronTask::SetDefaultIdParameters()
{
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
   // analysis cuts
   fPtCut = 0.2;
   fAngleCut = 1.;
   fChiPrimCut = 2.;
   fGammaCut = 0.025;
   fSTCutAngle = 1.5;
   fSTCutPP = 1.5;
   fTTCutAngle = 0.75;
   fTTCutPP = 4.0;
   fDSTSCutP = 1.;
   fDSTSCutD = 0.4;
   fDSTSCut2P = 1.5;
   fDSTSCut2D = 0.5;
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

   fKFFitter.Init();

   if (fUseRichAnn){
      TString richANNFile = gSystem->Getenv("VMCWORKDIR");
      richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
      fElIdAnn = new CbmRichElectronIdAnn(0, richANNFile);
      fElIdAnn->Init();
   }

   return kSUCCESS;
}


void CbmAnaDielectronTask::Exec(Option_t *option)
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
    		"fSTCut (ang,pp) = (" << fSTCutAngle << "," << fSTCutPP << ")" << endl <<
    		"fTTCut (ang,pp) = (" << fTTCutAngle << "," << fTTCutPP << ")" << endl <<
    		"fDstsCut (p,d) = (" << fDSTSCutP << "," << fDSTSCutD << ")" <<endl;
    FillRichRingNofHits();
    MCPairs();   
    SingleParticleAcceptance();
    PairAcceptance();
    FillSegmentCandidatesArray();
    FillCandidateArray();
    DifferenceSignalAndBg();
    SignalAndBgReco();
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
        Int_t pdg = mctrack->GetPdgCode();

        // mother pdg of e-/e+
        Int_t mcMotherPdg = 0;
        if (pdg == -11 || pdg == 11) {
            if (motherId != -1){
                CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
                if (mother) mcMotherPdg = mother->GetPdgCode();
                // vertex of gamma
                if (mcMotherPdg == 22) {
                    TVector3 vertex;
                    mother->GetStartVertex(vertex);
                    fh_mc_vertex_gamma_xz->Fill(vertex.Z(),vertex.X());
                    fh_mc_vertex_gamma_yz->Fill(vertex.Z(),vertex.Y());
                    fh_mc_vertex_gamma_xy->Fill(vertex.X(),vertex.Y());
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

            fh_mc_signal_pty->Fill(p.rapidity,p.pt,fWeight);
            fh_mc_signal_mom->Fill(p.momentumMag,fWeight);
            fh_mc_signal_minv->Fill(p.minv, fWeight);
        } //iM
    } //iP

} //MC Pairs


void CbmAnaDielectronTask::SingleParticleAcceptance()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++) {
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = mctrack->GetPdgCode();
        Int_t nMvdPoints = mctrack->GetNPoints(kMVD);
        Int_t nStsPoints = mctrack->GetNPoints(kSTS);
        Int_t nRichPoints = fNofHitsInRingMap[i];

        Bool_t isAcc = ( nMvdPoints+nStsPoints >= 4 && nRichPoints >= 7);

        Int_t mcMotherPdg = 0;

        // mother PDG accepted e-/e+
        if ((pdg == -11 && isAcc) || (pdg == 11 && isAcc)) {
            if (motherId != -1){
                CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
                if (mother) mcMotherPdg = mother->GetPdgCode();
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
               fh_acc_signal_pty->Fill(p.rapidity, p.pt,fWeight);
               fh_acc_signal_mom->Fill(p.momentumMag,fWeight);
               fh_acc_signal_minv->Fill(p.minv,fWeight);
            }
         }//iM
      }
   }//iP
} // PairsAcceptance

void CbmAnaDielectronTask::FillSegmentCandidatesArray()
{
    fSegmentCandidates.clear();
    Int_t ngTracks = fGlobalTracks->GetEntriesFast();
    fSegmentCandidates.reserve(ngTracks);

    for (Int_t i = 0; i < ngTracks; i++) {
        DielectronCandidate cand;
    
        CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
        if(!gTrack) continue;

//choose tracks which are ONLY in STS - SEGMENT
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
        cand.chiPrimary = fKFFitter.GetChiToVertex(stsTrack, fPrimVertex);
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
        if (cand.chiPrimary < fChiPrimCut) fSegmentCandidates.push_back(cand);
    }//gTracks
}

void CbmAnaDielectronTask::FillCandidateArray()
{
    fCandidates.clear();
    Int_t nGTracks = fGlobalTracks->GetEntriesFast();
    fCandidates.reserve(nGTracks);

    for (Int_t i = 0; i < nGTracks; i++){
        DielectronCandidate cand;
        cand.isMCSignalElectron = false;
        cand.isMCPi0Electron = false;
        cand.isMCGammaElectron = false;
        cand.isMCEtaElectron = false;
        cand.MCMotherId = -1;
        cand.isElectron = false;
        cand.isGamma = false;
        cand.isSTCutElectron = false;
        cand.isTTCutElectron = false;
        cand.isAPMCutElectron = false;
        cand.isDSTSCutElectron = true;
        cand.isDSTSCut2Electron = true;

        CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
        if(!gTrack) continue;

// STS
	    cand.stsInd = gTrack->GetStsTrackIndex();
	    if (cand.stsInd < 0) continue;
	    CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.stsInd);
	    if (stsTrack == NULL) continue;
	    CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsTrackMatches->At(cand.stsInd);
	    if (stsMatch == NULL) continue;
	    cand.stsMCTrackId = stsMatch->GetMCTrackId();
	    if (cand.stsMCTrackId < 0) continue;
	    CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(cand.stsMCTrackId);
	    if (mcTrack1 == NULL) continue;

        fKFFitter.DoFit(stsTrack,11);
	    cand.chi2sts = stsTrack->GetChi2() / stsTrack->GetNDF();
	    cand.chiPrimary = fKFFitter.GetChiToVertex(stsTrack, fPrimVertex);
        // Fit tracks to the primary vertex
        FairTrackParam vtxTrack;
        fKFFitter.FitToVertex(stsTrack, fPrimVertex, &vtxTrack);

        vtxTrack.Position(cand.position);
        vtxTrack.Momentum(cand.momentum);

        cand.mass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
        cand.charge = (vtxTrack.GetQp() > 0) ?1 :-1;
        cand.energy = sqrt(cand.momentum.Mag2() + cand.mass * cand.mass);
        cand.rapidity = 0.5*TMath::Log((cand.energy + cand.momentum.Z()) / (cand.energy - cand.momentum.Z()));

        Double_t MCmoment = mcTrack1->GetP();
        Int_t Pdg = TMath::Abs(mcTrack1->GetPdgCode());
        Int_t Pdg1 = mcTrack1->GetPdgCode();
        Int_t MotherId = mcTrack1->GetMotherId();
        cand.MCMotherId = MotherId;

// RICH
	    cand.richInd = gTrack->GetRichRingIndex();
        if (cand.richInd < 0) continue;
        CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(cand.richInd);

        CbmRichRingMatch* richMatch  = (CbmRichRingMatch*) fRichRingMatches->At(cand.richInd);
	    if (richMatch == NULL) continue;
	    cand.richMCTrackId = richMatch->GetMCTrackID();
	    if (cand.richMCTrackId < 0) continue;
	    CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMCTracks->At(cand.richMCTrackId);
	    if (mcTrack2 == NULL) continue;
// check RICH ring - STS track matches
	   if ( cand.stsMCTrackId == cand.richMCTrackId){
	        Int_t pdg = TMath::Abs( mcTrack1->GetPdgCode() );
	        Int_t motherId = mcTrack1->GetMotherId();
	        cand.MCMotherId = motherId;
            if (pdg == 11 && motherId == -1) cand.isMCSignalElectron = true;

			if (motherId >=0){
				CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(cand.MCMotherId);
				if (mct1 != NULL && mct1->GetPdgCode() == 111 && pdg == 11) {
					cand.isMCPi0Electron = true;
				}
				if (mct1 != NULL && mct1->GetPdgCode() == 22 && pdg == 11){
					cand.isMCGammaElectron = true;
				}
				if(mct1 != NULL && mct1->GetPdgCode() == 221 && pdg == 11){
					cand.isMCEtaElectron = true;
				}
			}
	    }
//        IsRichElectron(richRing, cand.momentum.Mag(), &cand);

// TRD

        cand.trdInd = gTrack->GetTrdTrackIndex();
        if (cand.trdInd < 0) continue;
        CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(cand.trdInd);
        if (trdTrack == NULL) continue;
        CbmTrackMatch* trdMatch = (CbmTrackMatch*) fTrdTrackMatches->At(cand.trdInd);
        if (trdMatch == NULL) continue;
        cand.trdMCTrackId = trdMatch->GetMCTrackId();
        if (cand.trdMCTrackId < 0) continue;
        CbmMCTrack* mcTrack3 = (CbmMCTrack*) fMCTracks->At(cand.trdMCTrackId);
        if (mcTrack3 == NULL) continue;

 //       IsTrdElectron(trdTrack, &cand);

// ToF
        cand.tofInd = gTrack->GetTofHitIndex();
        if (cand.tofInd < 0) continue;
        CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(cand.tofInd);
        if (tofHit == NULL) continue;
        Int_t tofPointIndex = tofHit->GetRefId();
        if (tofPointIndex < 0) continue;
        FairMCPoint* tofPoint = (FairMCPoint*) fTofPoints->At(tofPointIndex);
        if (tofPoint == NULL) continue;
        cand.tofMCTrackId = tofPoint->GetTrackID();
        if (cand.tofMCTrackId < 0) continue;
        CbmMCTrack* mcTrack4 = (CbmMCTrack*) fMCTracks->At(cand.tofMCTrackId);
        if (mcTrack4 == NULL) continue;

//        IsTofElectron(gTrack, cand.momentum.Mag(), &cand);
        IsElectron(richRing, cand.momentum.Mag(), trdTrack, gTrack, &cand);

        fCandidates.push_back(cand);
    }// global tracks
}

void CbmAnaDielectronTask::SourcePairs(
      DielectronCandidate* candP,
      DielectronCandidate* candM,
      TH2D* h_source_pair)
{
    if (candM->isMCGammaElectron) {
        if (candP->isMCGammaElectron && candP->MCMotherId != candM->MCMotherId) h_source_pair->Fill(0.5, 0.5);
        if (candP->isMCPi0Electron) h_source_pair->Fill(1.5, 0.5);
        if (!candP->isMCGammaElectron && !candP->isMCPi0Electron) h_source_pair->Fill(2.5, 0.5);
    } else if (candM->isMCPi0Electron) {
        if (candP->isMCGammaElectron) h_source_pair->Fill(0.5, 1.5);
        if (candP->isMCPi0Electron && candP->MCMotherId != candM->MCMotherId) h_source_pair->Fill(1.5, 1.5);
        if (!candP->isMCGammaElectron && !candP->isMCPi0Electron) h_source_pair->Fill(2.5, 1.5);
    } else if (!candM->isMCGammaElectron && !candM->isMCPi0Electron) {
        if (candP->isMCGammaElectron) h_source_pair->Fill(0.5, 2.5);
        if (candP->isMCPi0Electron) h_source_pair->Fill(1.5, 2.5);
        else h_source_pair->Fill(2.5, 2.5);
    }
}

void CbmAnaDielectronTask::TrackSource(
		DielectronCandidate* cand,
		Double_t binNum,
		Int_t pdg)
{
	Double_t mom = cand->momentum.Mag();
	if (cand->isMCSignalElectron) {
		fh_nof_el_tracks->Fill(binNum, fWeight);
	} else {
		fh_nof_bg_tracks->Fill(binNum);
		if (cand->isMCGammaElectron) {fh_source_tracks->Fill(binNum,0.5); fh_gamma_mom->Fill(mom);}
		else if (cand->isMCPi0Electron) {fh_source_tracks->Fill(binNum,1.5); fh_pi0_mom->Fill(mom);}
		else if (pdg == 211 || pdg ==-211) {fh_source_tracks->Fill(binNum,2.5); fh_pions_mom->Fill(mom);}
		else if (pdg == 2212) {fh_source_tracks->Fill(binNum,3.5); fh_proton_mom->Fill(mom);}
		else if (pdg == 321 || pdg == -321) {fh_source_tracks->Fill(binNum,4.5); fh_kaons_mom->Fill(mom);}
		else {fh_source_tracks->Fill(binNum,5.5); fh_other_mom->Fill(mom);}
	}
}

void CbmAnaDielectronTask::SignalAndBgReco()
{
   CheckGammaConvAndPi0();
   CheckTrackTopologyCut();
   CheckTrackTopologyRecoCut();
   CheckArmPodModified();
   if (fUseMvd){
      CheckClosestMvdHit(1, fh_dsts_signal, fh_dsts_bg, fh_dsts_gamma, fh_dsts_pi0, fh_dsts_eta);
      CheckClosestMvdHit(2, fh_dsts2_signal, fh_dsts2_bg, fh_dsts2_gamma, fh_dsts2_pi0, fh_dsts2_eta);
   }

   Int_t ncand = fCandidates.size();
   for (Int_t i = 0; i < ncand; i++){
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(fCandidates[i].stsMCTrackId);
		Int_t pdg = mcTrack->GetPdgCode();

      Bool_t isChiPrimary = (fCandidates[i].chiPrimary < fChiPrimCut);
      Bool_t isEl = (fCandidates[i].isElectron);
      Bool_t isGammaCut = (!fCandidates[i].isGamma);
      Bool_t isStCut = (fCandidates[i].isSTCutElectron);
      Bool_t isTtCut = (fCandidates[i].isTTCutElectron);
      Bool_t isPtCut = (fCandidates[i].momentum.Perp() > fPtCut);

      Double_t mom = fCandidates[i].momentum.Mag();
      if (isChiPrimary && isEl) {
         if (fCandidates[i].isMCGammaElectron) fh_gamma_id_mom->Fill(mom);
         else if (fCandidates[i].isMCPi0Electron) fh_pi0_id_mom->Fill(mom);
         else if (pdg == 211 || pdg ==-211) fh_pions_id_mom->Fill(mom);
         else if (pdg == 2212)  fh_proton_id_mom->Fill(mom);
         else if (pdg == 321 || pdg == -321) fh_kaons_id_mom->Fill(mom);
         else fh_other_id_mom->Fill(mom);
      }

		TrackSource(&fCandidates[i], 0.5, pdg);
		if (isChiPrimary) TrackSource(&fCandidates[i], 1.5, pdg);
		if (isChiPrimary && isEl) TrackSource(&fCandidates[i], 2.5, pdg);
		if (isChiPrimary && isEl && isGammaCut)TrackSource(&fCandidates[i], 3.5, pdg);
		if (isChiPrimary && isEl && isGammaCut && isStCut) TrackSource(&fCandidates[i], 4.5, pdg);
		if (isChiPrimary && isEl && isGammaCut && isStCut && isTtCut) TrackSource(&fCandidates[i], 5.5, pdg);
		if (isChiPrimary && isEl && isGammaCut && isStCut && isTtCut && isPtCut) TrackSource(&fCandidates[i], 6.5, pdg);
    }

    for (Int_t iP = 0; iP < ncand; iP++){
        if (fCandidates[iP].charge < 0) continue;
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(fCandidates[iP].stsMCTrackId);
        for (Int_t iM = 0; iM < ncand; iM++){
            if (fCandidates[iM].charge > 0) continue;
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(fCandidates[iM].stsMCTrackId);
            if (iM == iP ) continue;

            KinematicParams pMC = CalculateKinematicParams(mctrackP,mctrackM);

            KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 

            Bool_t isSignal = fCandidates[iP].isMCSignalElectron && fCandidates[iM].isMCSignalElectron;

            Bool_t isBG = !(fCandidates[iP].isMCSignalElectron || fCandidates[iM].isMCSignalElectron);

            Bool_t isPi0 = (fCandidates[iP].isMCPi0Electron && fCandidates[iM].isMCPi0Electron
                            && fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId);

            Bool_t isEta = (fCandidates[iP].isMCEtaElectron && fCandidates[iM].isMCEtaElectron
                            && fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId);

            Bool_t isChiPrimary = (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iM].chiPrimary < fChiPrimCut);
            Bool_t isEl = (fCandidates[iP].isElectron && fCandidates[iM].isElectron);
            Bool_t isGammaCut = (!fCandidates[iP].isGamma && !fCandidates[iM].isGamma);
            Bool_t isStCut = (fCandidates[iP].isSTCutElectron && fCandidates[iM].isSTCutElectron);
            Bool_t isTtCut = (fCandidates[iP].isTTCutElectron && fCandidates[iM].isTTCutElectron);
            Bool_t isPtCut = (fCandidates[iP].momentum.Perp() > fPtCut && fCandidates[iM].momentum.Perp() > fPtCut);
            Bool_t isAngleCut = (pRec.angle > fAngleCut);
            Bool_t isAPMCut = (fCandidates[iP].isAPMCutElectron && fCandidates[iM].isAPMCutElectron);
            Bool_t isDSTSCut = (fCandidates[iP].isDSTSCutElectron && fCandidates[iM].isDSTSCutElectron);
            Bool_t isDSTS2Cut = (fCandidates[iP].isDSTSCut2Electron && fCandidates[iM].isDSTSCut2Electron);
            if (!fUseMvd) isDSTSCut = true;
            if (!fUseMvd) isDSTS2Cut = true;

            if (isSignal) fh_reco_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
            if (isSignal) fh_reco_signal_mom->Fill(pMC.momentumMag,fWeight);
            if (isSignal) fh_rec_signal_minv->Fill(pRec.minv, fWeight);
            if (isBG) fh_rec_bg_minv->Fill(pRec.minv);
            if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_reco);
            if (isPi0) fh_rec_pi0_minv->Fill(pRec.minv);
            if (isEta) fh_rec_eta_minv->Fill(pRec.minv);

            if (isChiPrimary){
                if (isSignal) fh_chi_prim_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_chi_prim_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_chi_prim_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isBG) fh_chi_prim_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_chi_prim);
                if (isPi0) fh_chi_prim_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_chi_prim_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl){
                if (isSignal) fh_el_id_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_el_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_el_id_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isSignal) fh_angle_signal->Fill(pRec.angle);
                if (isBG) fh_angle_bg->Fill(pRec.angle);
                if (isBG) fh_el_id_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_el_id);
                if (isPi0) fh_el_id_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_el_id_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut){
                if (isSignal) fh_gammacut_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_gammacut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_gammacut_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isBG) fh_gammacut_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_gammacut);
                if (isPi0) fh_gammacut_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_gammacut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut){
               if (isSignal) fh_dstscut_signal_minv->Fill(pRec.minv, fWeight);
               if (isSignal) fh_dstscut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
               if (isSignal) fh_dstscut_signal_mom->Fill(pMC.momentumMag, fWeight);
               if (isBG) fh_dstscut_bg_minv->Fill(pRec.minv);
               if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_dstscut);
               if (isPi0) fh_dstscut_pi0_minv->Fill(pRec.minv);
               if (isEta) fh_dstscut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut && isDSTS2Cut){
            	if (isSignal) fh_dsts2cut_signal_minv->Fill(pRec.minv, fWeight);
            	if (isSignal) fh_dsts2cut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
            	if (isSignal) fh_dsts2cut_signal_mom->Fill(pMC.momentumMag, fWeight);
               if (isBG) fh_dsts2cut_bg_minv->Fill(pRec.minv);
               if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_dsts2cut);
               if (isPi0) fh_dsts2cut_pi0_minv->Fill(pRec.minv);
               if (isEta) fh_dsts2cut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut && isDSTS2Cut && isStCut){
                if (isSignal) fh_stcut_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_stcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_stcut_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isBG) fh_stcut_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_stcut);
                if (isPi0) fh_stcut_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_stcut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut && isDSTS2Cut && isStCut && isTtCut){
                if (isSignal) fh_ttcut_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_ttcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_ttcut_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isSignal) fh_ttcut_signal_minv_pt->Fill(pRec.minv, pMC.pt, fWeight);
                if (isBG) fh_ttcut_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_ttcut);
                if (isPi0) fh_ttcut_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_ttcut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut && isDSTS2Cut && isStCut && isTtCut && isPtCut){
                if (isSignal) fh_ptcut_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_ptcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_ptcut_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isSignal) fh_ptcut_signal_minv_pt->Fill(pRec.minv, pMC.pt, fWeight);
                if (isBG) fh_ptcut_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_ptcut);
                if (isPi0) fh_ptcut_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_ptcut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut && isDSTS2Cut && isStCut && isTtCut && isPtCut && isAngleCut){
                if (isSignal) fh_anglecut_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_anglecut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_anglecut_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isBG) fh_anglecut_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_anglecut);
                if (isPi0) fh_anglecut_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_anglecut_eta_minv->Fill(pRec.minv);
            }

            if (isChiPrimary && isEl && isGammaCut && isDSTSCut && isDSTS2Cut && isStCut && isTtCut && isPtCut && isAngleCut && isAPMCut){
                if (isSignal) fh_apmcut_signal_minv->Fill(pRec.minv, fWeight);
                if (isSignal) fh_apmcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                if (isSignal) fh_apmcut_signal_mom->Fill(pMC.momentumMag, fWeight);
                if (isBG) fh_apmcut_bg_minv->Fill(pRec.minv);
                if (isBG) SourcePairs(&fCandidates[iP], &fCandidates[iM], fh_source_pair_apmcut);
                if (isPi0) fh_apmcut_pi0_minv->Fill(pRec.minv);
                if (isEta) fh_apmcut_eta_minv->Fill(pRec.minv);
            }
        } // iM
    } // iP
}

Bool_t CbmAnaDielectronTask::CheckArmPod(Double_t alfa, Double_t pt)
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
        if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iP].isElectron){

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
                //APM
                Double_t alfa = 0.;
                Double_t ptt = 0.;
                CalculateArmPodParams(&fCandidates[iP], &fCandidates[candInd[minInd]], alfa, ptt);

			    if (fCandidates[iP].isMCSignalElectron){
                    fh_apmcut_signal->Fill(alfa, ptt, fWeight);
                }else {
                    fh_apmcut_bg->Fill(alfa, ptt, fWeight);
                }

                if (ptt > 0.025) fCandidates[iP].isAPMCutElectron = true;
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
            if ((fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iP].isElectron) &&
               (fCandidates[iM].chiPrimary < fChiPrimCut && fCandidates[iM].isElectron )) {

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
        //if (fCandidates[iP].charge > 0){
            if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iP].isElectron){

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

                if (fCandidates[iP].isMCSignalElectron) fh_stcut_signal->Fill(sqrt_mom, minAng, fWeight);
                else fh_stcut_bg->Fill(sqrt_mom, minAng);
                if (fCandidates[iP].isMCPi0Electron) fh_stcut_pi0->Fill(sqrt_mom, minAng);
                if (fCandidates[iP].isMCGammaElectron) fh_stcut_gamma->Fill(sqrt_mom, minAng);

                Double_t val = -1.*(fSTCutAngle/fSTCutPP)*sqrt_mom + fSTCutAngle;
                if ( !(sqrt_mom < fSTCutPP && val > minAng) ) fCandidates[iP].isSTCutElectron = true;
                //if ( sqrt_mom > 0.65  ) fCandidates[iP].isSTCutElectron = true;
            }//if electron
        //}// charge >0
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
        //if (fCandidates[iP].charge > 0){
            if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iP].isElectron){
                angles1.clear();
                mom1.clear();
                for (Int_t iM = 0; iM < nCand; iM++){
                    // different charges, charge Im != charge iP
                    if (fCandidates[iM].charge != fCandidates[iP].charge){
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

			    if (fCandidates[iP].isMCSignalElectron){
                    fh_ttcut_signal->Fill(sqrt_mom, minAng, fWeight);
                }else {
                    fh_ttcut_bg->Fill(sqrt_mom, minAng);
                }
			    if (fCandidates[iP].isMCPi0Electron) fh_ttcut_pi0->Fill(sqrt_mom, minAng);
			    if (fCandidates[iP].isMCGammaElectron) fh_ttcut_gamma->Fill(sqrt_mom, minAng);

                Double_t val = -1.*(fTTCutAngle/fTTCutPP)*sqrt_mom + fTTCutAngle;
                if ( !(sqrt_mom < fTTCutPP && val > minAng) ) fCandidates[iP].isTTCutElectron = true;
                //if ( minAng > 1. && sqrt_mom > 1.25  ) fCandidates[iP].isTTCutElectron = true;
            }//if electron
        //}// charge >0
    } //iP
}

void CbmAnaDielectronTask::IsElectron(
      CbmRichRing * ring,
      Double_t momentum,
      CbmTrdTrack* trdTrack,
      CbmGlobalTrack * gTrack,
      DielectronCandidate* cand)
{
	Bool_t richEl = IsRichElectron(ring, momentum, cand);
	Bool_t trdEl = IsTrdElectron(trdTrack, cand);
	Double_t annRich = cand->richAnn;
	Double_t annTrd = cand->trdAnn;
	Bool_t tofEl = IsTofElectron(gTrack, momentum, cand);

	if (richEl && trdEl && tofEl) {
			cand->isElectron = true;
	} else {
		cand->isElectron = false;
	}

/*
	if (annRich > 0.85){
		cand->isElectron = true;
		return;
	} else if (annTrd > 1.95) {
		cand->isElectron = true;
		return;
	} else 	if (annRich > 0.4 && annTrd > 0.92) {
		cand->isElectron = true;
		return;
	} else if (momentum < 0.7 && tofEl && annRich > 0.5) {
		cand->isElectron = true;
		return;
	} else if (momentum < 0.7 && tofEl && annTrd > 0.92){
		cand->isElectron = true;
		return;
	} else if (richEl && trdEl && tofEl) {
		cand->isElectron = true;
		return;
	} else {
		cand->isElectron = false;
		return;
	}*/
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
    // Get the tracklength
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

        if (fCandidates[i].isMCSignalElectron){
            fh_chi2_prim_signal->Fill(fCandidates[i].chiPrimary, fWeight);
        } else {
            fh_chi2_prim_bg->Fill(fCandidates[i].chiPrimary);
        }

        if (fCandidates[i].chiPrimary > fChiPrimCut ) continue;

        if (fCandidates[i].isMCSignalElectron){
            fh_rich_ann_signal->Fill(fCandidates[i].richAnn, fWeight);
            fh_trd_ann_signal->Fill(fCandidates[i].trdAnn, fWeight);
            fh_tof_m2_signal->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2, fWeight);
        } else {
            fh_rich_ann_bg->Fill(fCandidates[i].richAnn);
            fh_trd_ann_bg->Fill(fCandidates[i].trdAnn);
            fh_tof_m2_bg->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2);
       }
    } // loop over candidates

    for (Int_t i = 0; i < nCand; i++){
        if (!(fCandidates[i].chiPrimary < fChiPrimCut &&
             fCandidates[i].isElectron )) continue;

        if (fCandidates[i].isMCSignalElectron){
            fh_pt_signal->Fill(fCandidates[i].momentum.Perp(), fWeight);
            fh_mom_signal->Fill(fCandidates[i].momentum.Mag(), fWeight);
            fh_chi2sts_signal->Fill(fCandidates[i].chi2sts);
        } else {
            fh_pt_bg->Fill(fCandidates[i].momentum.Perp());
            fh_mom_bg->Fill(fCandidates[i].momentum.Mag());
            fh_chi2sts_bg->Fill(fCandidates[i].chi2sts);
        }
    } // loop over candidates

// Armenteros - Podolansky plot
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].charge > 0){
         if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iP].isElectron){
            for (Int_t iM = 0; iM < nCand; iM++){
               if ( iM != iP && fCandidates[iM].charge < 0){
                  if (fCandidates[iM].chiPrimary < fChiPrimCut && fCandidates[iM].isElectron){
                     Double_t alfa = 0.;
                     Double_t ptt = 0.;
                     CalculateArmPodParams(&fCandidates[iP], &fCandidates[iM], alfa, ptt);
                     if (fCandidates[iP].isMCSignalElectron && fCandidates[iM].isMCSignalElectron){
                        fh_apcut_signal->Fill(alfa, ptt, fWeight);
                     }
                     if ( !(fCandidates[iP].isMCSignalElectron || fCandidates[iM].isMCSignalElectron) ){
                        fh_apcut_bg->Fill(alfa, ptt);
                     }
                     if (fCandidates[iP].isMCPi0Electron && fCandidates[iM].isMCPi0Electron &&
                           fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId)
                        fh_apcut_pi0->Fill(alfa, ptt);
                     if (fCandidates[iP].isMCGammaElectron && fCandidates[iM].isMCGammaElectron &&
                           fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId)
                        fh_apcut_gamma->Fill(alfa, ptt);
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
		TH2D* signal,
		TH2D* bg,
		TH2D* gamma,
		TH2D* pi0,
		TH2D* eta)
{
    std::vector<Float_t> mvdX;
    std::vector<Float_t> mvdY;
    std::vector<Float_t> candX;
    std::vector<Float_t> candY;
    std::vector<Int_t> candInd;

    mvdX.reserve(200);
    mvdY.reserve(200);
    candX.reserve(100);
    candY.reserve(100);
    candInd.reserve(100);

    CbmKF *KF = CbmKF::Instance();

    Int_t nMvdHit = fMvdHits->GetEntriesFast();
   for(Int_t iHit = 0; iHit < nMvdHit; iHit++) {
      CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
      if(!pmh) continue;
      Int_t stationNum = pmh->GetStationNr();
      if (stationNum == mvdStationNum){
         Double_t X = pmh->GetX();
         Double_t Y = pmh->GetY();
         if (mvdStationNum == 1 && !(Y < -0.5*X + 0.6 && 0.5*X - 0.6 < Y && X < 0)){
            mvdX.push_back(pmh->GetX());
            mvdY.push_back(pmh->GetY());
         }
         if (mvdStationNum == 2 && !(Y > 0.43*X - 0.6 && -0.43*X + 0.6 < Y && X < 0)){
            mvdX.push_back(pmh->GetX());
            mvdY.push_back(pmh->GetY());
         }
      }
   }

    Int_t nCand = fCandidates.size(); 
    Int_t NofCand = 0;
   for (Int_t i = 0; i < nCand; i++){
      if (fCandidates[i].chiPrimary < fChiPrimCut && fCandidates[i].isElectron){
         NofCand++;
         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].stsInd);
         if(!track) continue;
         Int_t nthits = track->GetNMvdHits();
         for(Int_t ith = 0; ith < nthits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
            Int_t stationNum = pmh->GetStationNr();
            if(!pmh) continue;
            if (stationNum == mvdStationNum){
               Double_t X = pmh->GetX();
               Double_t Y = pmh->GetY();
               if (mvdStationNum == 1){
                  if(Y < -0.5*X + 0.6 && 0.5*X - 0.6 < Y && X < 0){ // inside
                  //fCandidates[i].isDSTSCut2Electron = true;
                  } else { // outside
                     candX.push_back(pmh->GetX());
                     candY.push_back(pmh->GetY());
                     candInd.push_back(i);
                  }
               }
               if (mvdStationNum == 2){
                  if (Y > 0.43*X - 0.6 && -0.43*X + 0.6 < Y && X < 0){
                     //fCandidates[i].isDSTSCut2Electron = true;
                  } else {
                     candX.push_back(pmh->GetX());
                     candY.push_back(pmh->GetY());
                     candInd.push_back(i);
                  }
               }
            }
         }
      }
   }// iCand

   for(Int_t iTrack = 0; iTrack < candInd.size(); iTrack++) {
      Float_t mind = 9999999.;
      for(Int_t iHit = 0; iHit < mvdX.size(); iHit++) {
         Float_t dx = mvdX[iHit] - candX[iTrack];
         Float_t dy = mvdY[iHit] - candY[iTrack];
         Float_t d2 = dx*dx + dy*dy;
         if(d2 < 1.e-9) continue;
         if(d2 < mind) mind = d2;
      } // iHit
      Double_t dsts = sqrt(mind);
      fCandidates[candInd[iTrack]].dSts = dsts;
      if (fCandidates[candInd[iTrack]].isMCSignalElectron){
         signal->Fill(sqrt(mind),fCandidates[candInd[iTrack]].momentum.Mag());
      }else {
         bg->Fill(sqrt(mind),fCandidates[candInd[iTrack]].momentum.Mag());
      }
      if (fCandidates[candInd[iTrack]].isMCGammaElectron){gamma->Fill(dsts,fCandidates[candInd[iTrack]].momentum.Mag());}
      if (fCandidates[candInd[iTrack]].isMCPi0Electron){pi0->Fill(dsts,fCandidates[candInd[iTrack]].momentum.Mag());}
      if (fCandidates[candInd[iTrack]].isMCEtaElectron){eta->Fill(dsts,fCandidates[candInd[iTrack]].momentum.Mag());}

      if (mvdStationNum == 1){
         Double_t mom = fCandidates[candInd[iTrack]].momentum.Mag();
         Double_t val = -1.*(fDSTSCutP/fDSTSCutD)*dsts + fDSTSCutP;
         if ( !(dsts < fDSTSCutD && val > mom) ) {
            fCandidates[candInd[iTrack]].isDSTSCutElectron = true;
         } else {
            fCandidates[candInd[iTrack]].isDSTSCutElectron = false;
         }
      }
      if (mvdStationNum == 2){
         Double_t mom = fCandidates[candInd[iTrack]].momentum.Mag();
         Double_t val = -1.*(fDSTSCut2P/fDSTSCut2D)*dsts + fDSTSCut2P;
         if ( !(dsts < fDSTSCut2D && val > mom) ){
            fCandidates[candInd[iTrack]].isDSTSCut2Electron = true;
         }else {
            fCandidates[candInd[iTrack]].isDSTSCut2Electron = false;
         }
      }
   } // iTrack
} // FindClosestMvdHit

void CbmAnaDielectronTask::Finish()
{
///WRITE HISTOGRAMS TO THE FILE
    for (Int_t i = 0; i < fHistoList.size(); i++){
        fHistoList[i]->Write();
    }
}
