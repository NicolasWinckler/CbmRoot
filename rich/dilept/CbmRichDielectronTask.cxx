#include <iostream>
#include <map>
using namespace std;

#include "CbmRichDielectronTask.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
#include "CbmStsKFTrackFitter.h"
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

#include "CbmGeoStsPar.h"
#include "CbmGeoPassivePar.h"

#include "CbmStsHit.h"
#include "CbmMvdHit.h"

#include "TString.h"
#include "TSystem.h"

#define M2E 2.6112004954086e-7
#define cutChiPrimary 3

ClassImp(CbmRichDielectronTask);

/*

CbmRichDielectronTask::CbmRichDielectronTask()
    : FairTask("DielectronTask")
{
    fEvents = 0;
    fCoutBunch = 1;

    fRichMom        = 5.5;
    fRichDist       = 0.;
    fRichNN         = -0.5;
    fRichRadial     = 140.;
    fRichAxisAmean  = 4.95;
    fRichAxisAsigma = 0.30;
    fRichAxisBmean  = 4.54;
    fRichAxisBsigma = 0.22;
    fRichCoeff      = 3.5;     // +-3.5 * sigma
    fTofM2    = 0.;
    fDcut   = 0.2;
    fGcut   = 0.03;
    f2Dtcut = 2.5;
    f2Dpcut = 1.;
    fPtcut  = 0.15;
    fTcut   = 1.;
    fPi0cut = 0.2;
    fWeight = 1.;

    fArrayMCTrack = NULL;
    fArrayStsTrack = NULL;
    fArrayStsMatch = NULL;
    fArrayMvdHit   = NULL;
    fArrayStsHit   = NULL;
    fArrayRichRing  = NULL;
    fArrayRichMatch = NULL;
    fArrayGlobalTrack = NULL;
}

*/

CbmRichDielectronTask::CbmRichDielectronTask(const char *name, const char *title)
: FairTask(name)
{
    fEvents = 0;
    fCoutBunch = 1;

    fDcut   = 0.2;
    fGcut   = 0.03;
    f2Dtcut = 2.5;
    f2Dpcut = 1.;
    fPtcut  = 0.15;
    fTcut   = 1.;
    fPi0cut = 0.2;
    fWeight = 1.;

    fArrayMCTrack = NULL;

    fArrayStsTrack = NULL;
    fArrayStsMatch = NULL;

    fArrayMvdHit = NULL;
    fArrayStsHit = NULL;

    fArrayRichRing  = NULL;
    fArrayRichMatch = NULL;

    fArrayGlobalTrack = NULL;
}

CbmRichDielectronTask::~CbmRichDielectronTask()
{

}

void CbmRichDielectronTask::CreateHistogramms()
{
    fh_chi2_prim_all = new TH1D("h_chi2_prim_all", "", 100, 0., 10.);
    fh_chi2_prim_e   = new TH1D("h_chi2_prim_e",   "", 100, 0., 10.);

    fh_mc_all_zr = new TH2D("h_mc_all_zr", "", 400, 0., 400., 400, -200., 200.);

    // single track momentum ---------------------------------------------------
    fh_mc_all_lept_mom  = new TH1D("h_mc_all_lept_mom", "", 1000, 0., 10.);
    fh_mc_rec_lept_mom  = new TH1D("h_mc_rec_lept_mom", "", 1000, 0., 10.);
    fh_rich_id_lept_mom = new TH1D("h_rich_id_lept_mom","", 1000, 0., 10.);
    fh_rich_id_ep_mom   = new TH1D("h_rich_id_ep_mom","", 1000, 0., 10.);
    fh_rich_id_em_mom   = new TH1D("h_rich_id_em_mom","", 1000, 0., 10.);
    fh_rich_id_no_cut_mom = new TH1D("h_rich_id_no_cut_mom","", 1000, 0., 10.);
    fh_rich_id_mom_cut_mom = new TH1D("h_rich_id_mom_cut_mom","", 1000, 0., 10.);
  //  fh_rich_id_dist_cut_mom = new TH1D("h_rich_id_dist_cut_mom","", 1000, 0., 10.);
    fh_rich_id_ann_cut_mom = new TH1D("h_rich_id_ann_cut_mom","", 1000, 0., 10.);
  //  fh_rich_id_xy_cut_mom = new TH1D("h_rich_id_xy_cut_mom","", 1000, 0., 10.);
  //  fh_rich_id_axisAB_cut_mom = new TH1D("h_rich_id_axisAB_cut_mom","", 1000, 0., 10.);

    // pair momentum -----------------------------------------------------------
    fh_mc_all_mom = new TH1D("h_mc_all_mom", "", 1000, 0., 10.);
    fh_mc_rec_mom = new TH1D("h_mc_rec_mom", "", 1000, 0., 10.);
    fh_rich_id_mom= new TH1D("h_rich_id_mom","", 1000, 0., 10.);
    // omega
    fh_mc_all_omega_mom  = new TH1D("h_mc_all_omega_mom", "", 1000, 0., 10.);
    fh_mc_rec_omega_mom  = new TH1D("h_mc_rec_omega_mom", "", 1000, 0., 10.);
    fh_rich_id_omega_mom = new TH1D("h_rich_id_omega_mom","", 1000, 0., 10.);
    // eta
    fh_mc_all_eta_mom  = new TH1D("h_mc_all_eta_mom", "", 1000, 0., 10.);
    fh_mc_rec_eta_mom  = new TH1D("h_mc_rec_eta_mom", "", 1000, 0., 10.);
    fh_rich_id_eta_mom = new TH1D("h_rich_id_eta_mom","", 1000, 0., 10.);
    // pi0
    fh_mc_all_pi0_mom  = new TH1D("h_mc_all_pi0_mom", "", 1000, 0., 10.);
    fh_mc_rec_pi0_mom  = new TH1D("h_mc_rec_pi0_mom", "", 1000, 0., 10.);
    fh_rich_id_pi0_mom = new TH1D("h_rich_id_pi0_mom","", 1000, 0., 10.);
    // gamma
    fh_mc_all_gamma_mom  = new TH1D("h_mc_all_gamma_mom", "", 1000, 0., 10.);
    fh_mc_rec_gamma_mom  = new TH1D("h_mc_rec_gamma_mom", "", 1000, 0., 10.);
    fh_rich_id_gamma_mom = new TH1D("h_rich_id_gamma_mom","", 1000, 0., 10.);

    // opening angle ------------------------------------------------------------
    fh_mc_all_theta = new TH1D("h_mc_all_theta", "", 1800, 0., 180.);
    fh_mc_rec_theta = new TH1D("h_mc_rec_theta", "", 1800, 0., 180.);
    fh_rich_id_theta= new TH1D("h_rich_id_theta","", 1800, 0., 180.);
    // omega
    fh_mc_all_omega_theta  = new TH1D("h_mc_all_omega_theta",  "", 1800, 0., 180.);
    fh_mc_rec_omega_theta  = new TH1D("h_mc_rec_omega_theta",  "", 1800, 0., 180.);
    fh_rich_id_omega_theta = new TH1D("h_rich_id_omega_theta", "", 1800, 0., 180.);
    // eta
    fh_mc_all_eta_theta  = new TH1D("h_mc_all_eta_theta", "", 1800, 0., 180.);
    fh_mc_rec_eta_theta  = new TH1D("h_mc_rec_eta_theta", "", 1800, 0., 180.);
    fh_rich_id_eta_theta = new TH1D("h_rich_id_eta_theta","", 1800, 0., 180.);
    // pi0
    fh_mc_all_pi0_theta  = new TH1D("h_mc_all_pi0_theta", "", 1800, 0., 180.);
    fh_mc_rec_pi0_theta  = new TH1D("h_mc_rec_pi0_theta", "", 1800, 0., 180.);
    fh_rich_id_pi0_theta = new TH1D("h_rich_id_pi0_theta","", 1800, 0., 180.);
    // gamma
    fh_mc_all_gamma_theta  = new TH1D("h_mc_all_gamma_theta", "", 1800, 0., 180.);
    fh_mc_rec_gamma_theta  = new TH1D("h_mc_rec_gamma_theta", "", 1800, 0., 180.);
    fh_rich_id_gamma_theta = new TH1D("h_rich_id_gamma_theta","", 1800, 0., 180.);

    // RICH ring ring information -------------------------------------------------
    fh_rich_ring_dist    = new TH1D("h_rich_ring_dist",   "", 100, 0., 10.);
    fh_rich_ring_dist_e  = new TH1D("h_rich_ring_dist_e", "", 100, 0., 10.);
    fh_rich_dist_mom     = new TH2D("h_rich_dist_mom",    "", 100, 0., 10., 100, 0., 10);
    fh_rich_dist_mom_e   = new TH2D("h_rich_dist_mom_e",  "", 100, 0., 10., 100, 0., 10);
    fh_rich_dist_mom_cut = new TH2D("h_rich_dist_mom_cut","", 100, 0., 10., 100, 0., 5.);

    fh_rich_ring_axisA_mom_all = new TH2D("h_rich_ring_axisA_mom_all",  "", 100, 0., 10., 100, 0., 20);
    fh_rich_ring_axisB_mom_all = new TH2D("h_rich_ring_axisB_mom_all",  "", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisA_mom     = new TH2D("h_rich_ring_axisA_mom",  "", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisB_mom     = new TH2D("h_rich_ring_axisB_mom",  "", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisA         = new TH1D("h_rich_ring_axisA",      "", 120, 2., 10.);
 //   fh_rich_ring_axisB         = new TH1D("h_rich_ring_axisB",      "", 120, 2., 10.);
 //   fh_rich_ring_axisA_mom_e   = new TH2D("h_rich_ring_axisA_mom_e","", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisB_mom_e   = new TH2D("h_rich_ring_axisB_mom_e","", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisA_e       = new TH1D("h_rich_ring_axisA_e",    "", 120, 2., 10.);
 //   fh_rich_ring_axisB_e       = new TH1D("h_rich_ring_axisB_e",    "", 120, 2., 10.);

    // TRD and TOF information -------------------------------------------------------
  //  fh_trd_eloss      = new TH1D("h_trd_eloss",   "", 200, 0., 0.2);
 //   fh_trd_eloss_e    = new TH1D("h_trd_eloss_e", "", 200, 0., 0.2);
 //   fh_trd_ann        = new TH1D("h_trd_ann",   "", 80,-2.,2.);
 //   fh_trd_ann_e      = new TH1D("h_trd_ann_e", "", 80,-2.,2.);
 //   fh_trd_ann_mom    = new TH2D("h_trd_ann_mom",  "",50,0,5,80,-2.,2.);
 //   fh_trd_ann_mom_e  = new TH2D("h_trd_ann_mom_e","",50,0,5,80,-2.,2.);
    fh_tof_m2_mom          = new TH2D("h_tof_m2_mom",         "", 120, 0., 6., 600, 0., 1.5);
    fh_tof_m2_mom_zoom     = new TH2D("h_tof_m2_mom_zoom",    "", 120, 0., 6., 600, 0., 0.15);
    fh_tof_m2_mom_zoom_cut = new TH2D("h_tof_m2_mom_zoom_cut","", 120, 0., 6., 600, 0., 0.15);


    // d sts to the closest neighbour -------------------------------------------
    fh_dsts_bg    = new TH1D("h_dsts_bg",    "", 1000, 0., 1.);
    fh_dsts_omega = new TH1D("h_dsts_omega", "", 1000, 0., 1.);
    fh_dsts_pi0   = new TH1D("h_dsts_pi0",   "", 1000, 0., 1.);
    fh_dsts_gamma = new TH1D("h_dsts_gamma", "", 1000, 0., 1.);
    fh_dsts_mom_bg    = new TH2D("h_dsts_mom_bg",    "", 50, 0., 1., 100, 0., 10.);
    fh_dsts_mom_omega = new TH2D("h_dsts_mom_omega", "", 50, 0., 1., 100, 0., 10.);
    fh_dsts_mom_pi0   = new TH2D("h_dsts_mom_pi0",   "", 50, 0., 1., 100, 0., 10.);
    fh_dsts_mom_gamma = new TH2D("h_dsts_mom_gamma", "", 50, 0., 1., 100, 0., 10.);

    // (theta + mom) cut --------------------------------------------------------
    fh_dcut_theta_mom_omega = new TH2D("h_dcut_theta_mom_omega", "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_pi0   = new TH2D("h_dcut_theta_mom_pi0",   "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_gamma = new TH2D("h_dcut_theta_mom_gamma", "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_bg    = new TH2D("h_dcut_theta_mom_bg",    "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_omega_true = new TH2D("h_dcut_theta_mom_omega_true", "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_gamma_true = new TH2D("h_dcut_theta_mom_gamma_true", "", 100, 0., 5., 500, 0., 50.);

    // pt cut -------------------------------------------------------------------
    fh_tpcut_pt_lept_omega = new TH1D("h_tpcut_pt_lept_omega", "", 1000, 0., 10.);
    fh_tpcut_pt_lept_bg    = new TH1D("h_tpcut_pt_lept_bg",    "", 1000, 0., 10.);

    // opening angle ------------------------------------------------------------
    fh_ptcut_theta_omega = new TH1D("h_ptcut_theta_omega", "", 1000, 0., 50.);
    fh_ptcut_theta_omega_true = new TH1D("h_ptcut_theta_omega_true", "", 1000, 0., 50.);
    fh_ptcut_theta_pi0   = new TH1D("h_ptcut_theta_pi0",   "", 1000, 0., 50.);
    fh_ptcut_theta_gamma = new TH1D("h_ptcut_theta_gamma", "", 1000, 0., 50.);
    fh_ptcut_theta_bg    = new TH1D("h_ptcut_theta_bg",    "", 1000, 0., 50.);

    // invariant mass -----------------------------------------------------------
    fh_mc_all_minv  = new TH1D("h_mc_all_minv", "", 2000, 0., 2.);
    fh_mc_rec_minv  = new TH1D("h_mc_rec_minv", "", 2000, 0., 2.);
    fh_rich_id_minv = new TH1D("h_rich_id_minv","", 2000, 0., 2.);
    fh_gcut_minv    = new TH1D("h_gcut_minv",   "", 2000, 0., 2.);
    fh_dcut_minv    = new TH1D("h_dcut_minv",   "", 2000, 0., 2.);
    fh_tpcut_minv   = new TH1D("h_tpcut_minv",  "", 2000, 0., 2.);
    fh_ptcut_minv   = new TH1D("h_ptcut_minv",  "", 2000, 0., 2.);
    fh_tcut_minv    = new TH1D("h_tcut_minv",   "", 2000, 0., 2.);
    fh_pi0cut_minv  = new TH1D("h_pi0cut_minv", "", 2000, 0., 2.);
    fh_rich_id_mc_minv = new TH1D("h_rich_id_mc_minv","", 2000, 0., 2.);
    fh_gcut_mc_minv    = new TH1D("h_gcut_mc_minv",   "", 2000, 0., 2.);
    fh_dcut_mc_minv    = new TH1D("h_dcut_mc_minv",   "", 2000, 0., 2.);
    fh_tpcut_mc_minv   = new TH1D("h_tpcut_mc_minv",  "", 2000, 0., 2.);
    fh_ptcut_mc_minv   = new TH1D("h_ptcut_mc_minv",  "", 2000, 0., 2.);
    fh_tcut_mc_minv    = new TH1D("h_tcut_mc_minv",   "", 2000, 0., 2.);
    fh_pi0cut_mc_minv  = new TH1D("h_pi0cut_mc_minv", "", 2000, 0., 2.);
    fh_mc_all_omega_bg_minv  = new TH1D("h_mc_all_omega_bg_minv", "", 2000, 0., 2.);
    fh_mc_rec_omega_bg_minv  = new TH1D("h_mc_rec_omega_bg_minv", "", 2000, 0., 2.);
    fh_rich_id_omega_bg_minv = new TH1D("h_rich_id_omega_bg_minv","", 2000, 0., 2.);
    fh_gcut_omega_bg_minv    = new TH1D("h_gcut_omega_bg_minv",   "", 2000, 0., 2.);
    fh_dcut_omega_bg_minv    = new TH1D("h_dcut_omega_bg_minv",   "", 2000, 0., 2.);
    fh_tpcut_omega_bg_minv   = new TH1D("h_tpcut_omega_bg_minv",  "", 2000, 0., 2.);
    fh_ptcut_omega_bg_minv   = new TH1D("h_ptcut_omega_bg_minv",  "", 2000, 0., 2.);
    fh_tcut_omega_bg_minv    = new TH1D("h_tcut_omega_bg_minv",   "", 2000, 0., 2.);
    fh_pi0cut_omega_bg_minv  = new TH1D("h_pi0cut_omega_bg_minv", "", 2000, 0., 2.);
    fh_rich_id_mc_omega_bg_minv = new TH1D("h_rich_id_mc_omega_bg_minv","", 2000, 0., 2.);
    fh_gcut_mc_omega_bg_minv    = new TH1D("h_gcut_mc_omega_bg_minv",   "", 2000, 0., 2.);
    fh_dcut_mc_omega_bg_minv    = new TH1D("h_dcut_mc_omega_bg_minv",   "", 2000, 0., 2.);
    fh_tpcut_mc_omega_bg_minv   = new TH1D("h_tpcut_mc_omega_bg_minv",  "", 2000, 0., 2.);
    fh_ptcut_mc_omega_bg_minv   = new TH1D("h_ptcut_mc_omega_bg_minv",  "", 2000, 0., 2.);
    fh_tcut_mc_omega_bg_minv    = new TH1D("h_tcut_mc_omega_bg_minv",   "", 2000, 0., 2.);
    fh_pi0cut_mc_omega_bg_minv  = new TH1D("h_pi0cut_mc_omega_bg_minv", "", 2000, 0., 2.);

    fh_pi0cut_epep_minv = new TH1D("h_pi0cut_epep_minv", "", 2000, 0., 2.);
    fh_pi0cut_emem_minv = new TH1D("h_pi0cut_emem_minv", "", 2000, 0., 2.);

    fh_mc_all_pt_minv_omega  = new TH2D("h_mc_all_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_mc_all_pt_minv_pairs  = new TH2D("h_mc_all_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);
    fh_mc_rec_pt_minv_omega  = new TH2D("h_mc_rec_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_mc_rec_pt_minv_pairs  = new TH2D("h_mc_rec_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);
    fh_rich_id_pt_minv_omega = new TH2D("h_rich_id_pt_minv_omega","", 100, 0., 2., 200, 0., 5.);
    fh_rich_id_pt_minv_pairs = new TH2D("h_rich_id_pt_minv_pairs","", 100, 0., 2., 200, 0., 5.);
    fh_tpcut_pt_minv_omega   = new TH2D("h_tpcut_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_tpcut_pt_minv_pairs   = new TH2D("h_tpcut_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);
    fh_pi0cut_pt_minv_omega  = new TH2D("h_pi0cut_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_pi0cut_pt_minv_pairs  = new TH2D("h_pi0cut_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);

    // omega
    fh_mc_all_omega_minv  = new TH1D("h_mc_all_omega_minv", "", 2000, 0., 2.);
    fh_mc_rec_omega_minv  = new TH1D("h_mc_rec_omega_minv", "", 2000, 0., 2.);
    fh_rich_id_omega_minv = new TH1D("h_rich_id_omega_minv","", 2000, 0., 2.);
    fh_gcut_omega_minv    = new TH1D("h_gcut_omega_minv",   "", 2000, 0., 2.);
    fh_dcut_omega_minv    = new TH1D("h_dcut_omega_minv",   "", 2000, 0., 2.);
    fh_tpcut_omega_minv   = new TH1D("h_tpcut_omega_minv",  "", 2000, 0., 2.);
    fh_ptcut_omega_minv   = new TH1D("h_ptcut_omega_minv",  "", 2000, 0., 2.);
    fh_tcut_omega_minv    = new TH1D("h_tcut_omega_minv",   "", 2000, 0., 2.);
    fh_pi0cut_omega_minv  = new TH1D("h_pi0cut_omega_minv", "", 2000, 0., 2.);
    // eta
    fh_mc_all_eta_minv  = new TH1D("h_mc_all_eta_minv", "", 2000, 0., 2.);
    fh_mc_rec_eta_minv  = new TH1D("h_mc_rec_eta_minv", "", 2000, 0., 2.);
    fh_rich_id_eta_minv = new TH1D("h_rich_id_eta_minv","", 2000, 0., 2.);
    fh_gcut_eta_minv    = new TH1D("h_gcut_eta_minv",   "", 2000, 0., 2.);
    fh_dcut_eta_minv    = new TH1D("h_dcut_eta_minv",   "", 2000, 0., 2.);
    fh_tpcut_eta_minv   = new TH1D("h_tpcut_eta_minv",  "", 2000, 0., 2.);
    fh_ptcut_eta_minv   = new TH1D("h_ptcut_eta_minv",  "", 2000, 0., 2.);
    fh_tcut_eta_minv    = new TH1D("h_tcut_eta_minv",   "", 2000, 0., 2.);
    fh_pi0cut_eta_minv  = new TH1D("h_pi0cut_eta_minv", "", 2000, 0., 2.);
    // pi0
    fh_mc_all_pi0_minv  = new TH1D("h_mc_all_pi0_minv", "", 2000, 0., 2.);
    fh_mc_rec_pi0_minv  = new TH1D("h_mc_rec_pi0_minv", "", 2000, 0., 2.);
    fh_rich_id_pi0_minv = new TH1D("h_rich_id_pi0_minv","", 2000, 0., 2.);
    fh_gcut_pi0_minv    = new TH1D("h_gcut_pi0_minv",   "", 2000, 0., 2.);
    fh_dcut_pi0_minv    = new TH1D("h_dcut_pi0_minv",   "", 2000, 0., 2.);
    fh_tpcut_pi0_minv   = new TH1D("h_tpcut_pi0_minv",  "", 2000, 0., 2.);
    fh_ptcut_pi0_minv   = new TH1D("h_ptcut_pi0_minv",  "", 2000, 0., 2.);
    fh_tcut_pi0_minv    = new TH1D("h_tcut_pi0_minv",   "", 2000, 0., 2.);
    fh_pi0cut_pi0_minv  = new TH1D("h_pi0cut_pi0_minv", "", 2000, 0., 2.);
    fh_pi0cut_fake_pi0_minv = new TH1D("h_pi0cut_fake_pi0_minv","", 2000, 0., 2.);
    fh_pi0cut_true_pi0_minv = new TH1D("h_pi0cut_true_pi0_minv","", 2000, 0., 2.);
    // gamma
    fh_mc_all_gamma_minv = new TH1D("h_mc_all_gamma_minv", "", 2000, 0., 2.);
    fh_mc_rec_gamma_minv = new TH1D("h_mc_rec_gamma_minv", "", 2000, 0., 2.);
    fh_rich_id_gamma_minv = new TH1D("h_rich_id_gamma_minv","", 2000, 0., 2.);
    fh_gcut_gamma_minv    = new TH1D("h_gcut_gamma_minv",   "", 2000, 0., 2.);
    fh_dcut_gamma_minv    = new TH1D("h_dcut_gamma_minv",   "", 2000, 0., 2.);
    fh_tpcut_gamma_minv   = new TH1D("h_tpcut_gamma_minv",  "", 2000, 0., 2.);
    fh_ptcut_gamma_minv   = new TH1D("h_ptcut_gamma_minv",  "", 2000, 0., 2.);
    fh_tcut_gamma_minv    = new TH1D("h_tcut_gamma_minv",   "", 2000, 0., 2.);
    fh_pi0cut_gamma_minv  = new TH1D("h_pi0cut_gamma_minv", "", 2000, 0., 2.);

    // pair pt-y ----------------------------------------------------------------
    fh_mc_all_pty  = new TH2D("h_mc_all_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_pty  = new TH2D("h_mc_rec_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_pty = new TH2D("h_rich_id_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_pty    = new TH2D("h_gcut_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_pty    = new TH2D("h_dcut_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_pty   = new TH2D("h_tpcut_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_pty   = new TH2D("h_ptcut_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_pty    = new TH2D("h_tcut_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_pty  = new TH2D("h_pi0cut_pty", "", 40, 0., 4., 20, 0., 2.);
    // omega
    fh_mc_all_omega_pty = new TH2D("h_mc_all_omega_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_omega_pty = new TH2D("h_mc_rec_omega_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_omega_pty = new TH2D("h_rich_id_omega_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_omega_pty   = new TH2D("h_gcut_omega_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_omega_pty   = new TH2D("h_dcut_omega_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_omega_pty  = new TH2D("h_tpcut_omega_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_omega_pty  = new TH2D("h_ptcut_omega_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_omega_pty   = new TH2D("h_tcut_omega_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_omega_pty = new TH2D("h_pi0cut_omega_pty", "", 40, 0., 4., 20, 0., 2.);
    // eta
    fh_mc_all_eta_pty  = new TH2D("h_mc_all_eta_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_eta_pty  = new TH2D("h_mc_rec_eta_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_eta_pty = new TH2D("h_rich_id_eta_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_eta_pty    = new TH2D("h_gcut_eta_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_eta_pty    = new TH2D("h_dcut_eta_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_eta_pty   = new TH2D("h_tpcut_eta_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_eta_pty   = new TH2D("h_ptcut_eta_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_eta_pty    = new TH2D("h_tcut_eta_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_eta_pty  = new TH2D("h_pi0cut_eta_pty", "", 40, 0., 4., 20, 0., 2.);
    // pi0
    fh_mc_all_pi0_pty  = new TH2D("h_mc_all_pi0_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_pi0_pty  = new TH2D("h_mc_rec_pi0_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_pi0_pty = new TH2D("h_rich_id_pi0_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_pi0_pty    = new TH2D("h_gcut_pi0_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_pi0_pty    = new TH2D("h_dcut_pi0_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_pi0_pty   = new TH2D("h_tpcut_pi0_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_pi0_pty   = new TH2D("h_ptcut_pi0_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_pi0_pty    = new TH2D("h_tcut_pi0_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_pi0_pty  = new TH2D("h_pi0cut_pi0_pty", "", 40, 0., 4., 20, 0., 2.);
    // gamma
    fh_mc_all_gamma_pty  = new TH2D("h_mc_all_gamma_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_gamma_pty  = new TH2D("h_mc_rec_gamma_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_gamma_pty = new TH2D("h_rich_id_gamma_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_gamma_pty    = new TH2D("h_gcut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_gamma_pty    = new TH2D("h_dcut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_gamma_pty   = new TH2D("h_tpcut_gamma_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_gamma_pty   = new TH2D("h_ptcut_gamma_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_gamma_pty    = new TH2D("h_tcut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_gamma_pty  = new TH2D("h_pi0cut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);

    fh_mom_rich = new TH1D("h_mom_rich","",1000,0,10);
 //   fh_mom_trd  = new TH1D("h_mom_trd", "",1000,0,10);
    fh_mom_tof  = new TH1D("h_mom_tof", "",1000,0,10);

    fh_mom_w_sts  = new TH1D("h_mom_w_sts", "",1000,0,10);
    fh_mom_w_rich = new TH1D("h_mom_w_rich","",1000,0,10);
//    fh_mom_w_trd  = new TH1D("h_mom_w_trd", "",1000,0,10);
    fh_mom_w_tof  = new TH1D("h_mom_w_tof", "",1000,0,10);

    fh_mom_e_sts  = new TH1D("h_mom_e_sts", "",1000,0,10);
    fh_mom_e_rich = new TH1D("h_mom_e_rich","",1000,0,10);
//    fh_mom_e_trd  = new TH1D("h_mom_e_trd", "",1000,0,10);
    fh_mom_e_tof  = new TH1D("h_mom_e_tof", "",1000,0,10);

    // background cocktail
    fh_rich_id_pi0_pi0_minv    = new TH1D("h_rich_id_pi0_pi0_minv",    "",2000, 0., 2.);
    fh_rich_id_pi0_gamma_minv  = new TH1D("h_rich_id_pi0_gamma_minv",  "",2000, 0., 2.);
    fh_rich_id_gamma_gamma_minv= new TH1D("h_rich_id_gamma_gamma_minv","",2000, 0., 2.);
    fh_rich_id_eta_gamma_minv  = new TH1D("h_rich_id_eta_gamma_minv",  "",2000, 0., 2.);
    fh_rich_id_eta_eta_minv    = new TH1D("h_rich_id_eta_eta_minv",    "",2000, 0., 2.);
    fh_rich_id_eta_pi0_minv    = new TH1D("h_rich_id_eta_pi0_minv",    "",2000, 0., 2.);

//    fh_trd_id_pi0_pi0_minv    = new TH1D("h_trd_id_pi0_pi0_minv",    "",2000, 0., 2.);
//   fh_trd_id_pi0_gamma_minv  = new TH1D("h_trd_id_pi0_gamma_minv",  "",2000, 0., 2.);
//    fh_trd_id_gamma_gamma_minv= new TH1D("h_trd_id_gamma_gamma_minv","",2000, 0., 2.);
//    fh_trd_id_eta_gamma_minv  = new TH1D("h_trd_id_eta_gamma_minv",  "",2000, 0., 2.);
//    fh_trd_id_eta_eta_minv    = new TH1D("h_trd_id_eta_eta_minv",    "",2000, 0., 2.);
//    fh_trd_id_eta_pi0_minv    = new TH1D("h_trd_id_eta_pi0_minv",    "",2000, 0., 2.);

    fh_tof_id_pi0_pi0_minv    = new TH1D("h_tof_id_pi0_pi0_minv",    "",2000, 0., 2.);
    fh_tof_id_pi0_gamma_minv  = new TH1D("h_tof_id_pi0_gamma_minv",  "",2000, 0., 2.);
    fh_tof_id_gamma_gamma_minv= new TH1D("h_tof_id_gamma_gamma_minv","",2000, 0., 2.);
    fh_tof_id_eta_gamma_minv  = new TH1D("h_tof_id_eta_gamma_minv",  "",2000, 0., 2.);
    fh_tof_id_eta_eta_minv    = new TH1D("h_tof_id_eta_eta_minv",    "",2000, 0., 2.);
    fh_tof_id_eta_pi0_minv    = new TH1D("h_tof_id_eta_pi0_minv",    "",2000, 0., 2.);

    fh_pi0cut_pi0_pi0_minv    = new TH1D("h_pi0cut_pi0_pi0_minv",    "",2000, 0., 2.);
    fh_pi0cut_pi0_gamma_minv  = new TH1D("h_pi0cut_pi0_gamma_minv",  "",2000, 0., 2.);
    fh_pi0cut_gamma_gamma_minv= new TH1D("h_pi0cut_gamma_gamma_minv","",2000, 0., 2.);
    fh_pi0cut_eta_gamma_minv  = new TH1D("h_pi0cut_eta_gamma_minv",  "",2000, 0., 2.);
    fh_pi0cut_eta_eta_minv    = new TH1D("h_pi0cut_eta_eta_minv",    "",2000, 0., 2.);
    fh_pi0cut_eta_pi0_minv    = new TH1D("h_pi0cut_eta_pi0_minv",    "",2000, 0., 2.);

   fh_rich_tof_id_cut_mom = new TH1D("h_rich_tof_id_cut_mom", "", 1000, 0., 10.);
}

void CbmRichDielectronTask::SetParContainers()
{
    // Get Base Container
    FairRunAna *run = FairRunAna::Instance();
    FairRuntimeDb *rtdb = run->GetRuntimeDb();
    fBasePar = (FairBaseParSet*)(rtdb->getContainer("FairBaseParSet"));
}

Bool_t CbmRichDielectronTask::SetParameters()
{

	if (fTrdAnn == -1.){
		cout << "-E- You must set TRD ANN cut, current value is " << fTrdAnn <<endl;
		return false;
	}
	if (fUseRichAnn && fRichAnnCut == -1.) {
		cout << "-E- You must set RICH ANN cut, because you are using RICH ANN. Current value is "<< fRichAnnCut <<endl;
		return false;
	}
	if (fRichAxisAmean == -1.) {
		cout << "-E- You must set RICH Axis A Mean, current value is "<< fRichAxisAmean <<endl;
		return false;
	}
	if (fRichAxisBmean == -1.) {
		cout << "-E- You must set RICH Axis B Mean, current value is "<< fRichAxisBmean <<endl;
		return false;
	}
	if (fRichAxisAsigma == -1.) {
		cout << "-E- You must set RICH Axis A sigma, current value is " << fRichAxisAsigma << endl;
		return false;
	}
	if (fRichAxisBsigma == -1.) {
		cout << "-E- You must set RICH Axis B sigma, current value is " << fRichAxisBsigma << endl;
		return false;
	}
	if (fRichCoeff == -1.) {
		cout << "-E- You must set RmsCoeff, current value is " << fRichCoeff <<endl;
		return false;
	}
	if (fRichDist == -1.) {
		cout << "-E- You must set Distance Cut, current value is "<< fRichDist <<endl;
		return false;
	}
	if (fUseRichAnn) {
		TString richANNFile = gSystem->Getenv("VMCWORKDIR");
		richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";

		fElIdAnn = new CbmRichElectronIdAnn(0, richANNFile);
		fElIdAnn->Init();
	}
	return true;
}

InitStatus CbmRichDielectronTask::Init()
{
    // get list of generators
    fPrimGen = fBasePar->GetPriGen();
//    fPrimGen->GetListOfGenerators()->ls();

    // Get pointers to root manager
    fRootManager = FairRootManager::Instance();

    // Get MCTrack
    fArrayMCTrack = (TClonesArray*) fRootManager->GetObject("MCTrack");
    if (! fArrayMCTrack) {
		cout << "-E- CbmRichDielectronTask::Init: No MCTrack array!" << endl;
		return kERROR;
    }
    // Get RichPoint
    fArrayRichPoint = (TClonesArray*) fRootManager->GetObject("RichPoint");
    if (! fArrayRichPoint) {
		cout << "-E- CbmRichDielectronTask::Init: No RichPoint array!" << endl;
		return kERROR;
    }
    // Get STSTrack Array
    fArrayStsTrack = (TClonesArray*) fRootManager->GetObject("StsTrack");
    if (! fArrayStsTrack) {
		cout << "-E- CbmRichDielectronTask::Init: No STSTrack array!" << endl;
		return kERROR;
    }
    // Get STSTrackMatchArray
    fArrayStsMatch = (TClonesArray*) fRootManager->GetObject("StsTrackMatch");
    if (! fArrayStsMatch) {
		cout << "-E- CbmRichDielectronTask::Init: No STSTrackMatch array!" << endl;
		return kERROR;
    }
    // Get STSHit Array
    fArrayStsHit = (TClonesArray*) fRootManager->GetObject("StsHit");
    if (! fArrayStsHit) {
		cout << "-E- CbmRichDielectronTask::Init: No STSHit array!" << endl;
		return kERROR;
    }
    // Get STSMvdHit Array
    fArrayMvdHit = (TClonesArray*) fRootManager->GetObject("MvdHit");
    if (! fArrayMvdHit) {
		cout << "-E- CbmRichDielectronTask::Init: No MvdHit array!" << endl;
		return kERROR;
    }
    // Get Primary Vertex
    fPrimVertex = (CbmVertex*) fRootManager->GetObject("PrimaryVertex");
    if (! fPrimVertex) {
		cout << "-E- CbmRichDielectronTask::Init: No Primary Vertex!" << endl;
		return kERROR;
    }
    // Get RichTrack Array
    fArrayRichRing = (TClonesArray*) fRootManager->GetObject("RichRing");
    if (! fArrayRichRing) {
		cout << "-E- CbmRichDielectronTask::Init: No RichRing array!" << endl;
		return kERROR;
    }
    // Get RichProj Array
    fArrayRichProj = (TClonesArray*) fRootManager->GetObject("RichProjection");
    if (! fArrayRichProj) {
		cout << "-E- CbmRichDielectronTask::Init: No RichProjection array!" << endl;
		return kERROR;
    }
    // Get RichMatch Array
    fArrayRichMatch = (TClonesArray*) fRootManager->GetObject("RichRingMatch");
    if (! fArrayRichMatch) {
		cout << "-E- CbmRichDielectronTask::Init: No RichRingMatch array!" << endl;
		return kERROR;
    }
    // Get TrdTrack array
/*    fArrayTrdTrack = (TClonesArray*) fRootManager->GetObject("TrdTrack");
    if (! fArrayTrdTrack) {
	cout << "-E- CbmRichDielectronTask::Init:! No TrdTrack array!" << endl;
	return kERROR;
    }
    // Get TrdHit array
    fArrayTrdHit = (TClonesArray*) fRootManager->GetObject("TrdHit");
    if (! fArrayTrdHit) {
	cout << "-E- CbmRichDielectronTask::Init:! No TrdHit array!" << endl;
	return kERROR;
    }
*/
    // Get TofHit array
    fArrayTofHit = (TClonesArray*) fRootManager->GetObject("TofHit");
    if (! fArrayTofHit) {
		cout << "-E- CbmRichDielectronTask::Init:! No TofHit array!" << endl;
		return kERROR;
    }
    // Get GlobalTrack Array
    fArrayGlobalTrack = (TClonesArray*) fRootManager->GetObject("GlobalTrack");
    if (! fArrayGlobalTrack) {
		cout << "-E- CbmRichDielectronTask::Init: No GlobalTrack array!" << endl;
		return kERROR;
    }

    Bool_t isSetOk = SetParameters();
    if (!isSetOk) return kFATAL;
    
    // initialize KF-Fitter
    fFitter.Init();

    CreateHistogramms();

    fn_rich_id_ep = 0;
    fn_rich_id_em = 0;
    fn_gcut_ep = 0;
    fn_gcut_em = 0;
    fn_dcut_ep = 0;
    fn_dcut_em = 0;
    fn_tpcut_ep = 0;
    fn_tpcut_em = 0;
    fn_ptcut_ep = 0;
    fn_ptcut_em = 0;
    fn_tcut_ep = 0;
    fn_tcut_em = 0;
    fn_pi0cut_ep = 0;
    fn_pi0cut_em = 0;

    cout << "-I- CbmRichDielectronTask::Init() : " << "initialisation completed." << endl;
    return kSUCCESS;
}

Bool_t mapGamma[500000];
Bool_t mapDsts[500000];
Bool_t mapTp[500000];
Bool_t mapPt[500000];
Bool_t mapTheta[500000];
Bool_t mapPi0[500000];

Double_t ArrayThetaClosest[500000];
Double_t ArrayMomClosest[500000];
Double_t ArrayMotherIDClosest[500000];
Double_t ArrayMotherPdgClosest[500000];
Int_t    ArrayTrackIdClosest[500000];

void CbmRichDielectronTask::FillDiGlobalTracks()
{
    Int_t nGlobalTrack = fArrayGlobalTrack->GetEntriesFast();
    fDiGlobalTracks.reserve(nGlobalTrack);

	for(Int_t i = 0; i < nGlobalTrack; i++) {
		DiGlobalTrack di;
		fDiGlobalTracks.push_back(di);
	}
	for(Int_t i = 0; i < nGlobalTrack; i++) {
		DiGlobalTrack &G = fDiGlobalTracks[i];

		G.is_sts  = 0;
		G.is_rich = 0;
	    G.is_trd  = 0;
		G.is_tof  = 0;
		G.is_e    = 0;

		G.is_fake   = 0;
		G.is_wrong  = 0;
		G.is_pion   = 0;
		G.is_proton = 0;

		Bool_t isDist = 0;

		G.fGlobalTrack = (CbmGlobalTrack*) fArrayGlobalTrack->At(i);
		if(NULL == G.fGlobalTrack) continue;
		//-----------------------------------------------------------------
		// get STS information --------------------------------------------
		//-----------------------------------------------------------------
		G.stsInd  = G.fGlobalTrack->GetStsTrackIndex();
		if(G.stsInd < 0) continue;
		CbmStsTrack* stsTrack = (CbmStsTrack*)fArrayStsTrack->At(G.stsInd);
		if(NULL == stsTrack) continue;
		CbmTrackMatch* stsMatch  = (CbmTrackMatch*)fArrayStsMatch->At(G.stsInd);
		if(NULL == stsMatch) continue;
		G.stsMCid = stsMatch->GetMCTrackId();
		if(G.stsMCid < 0) continue;
		CbmMCTrack* mcTrack = (CbmMCTrack*)fArrayMCTrack->At(G.stsMCid);
		if(NULL == mcTrack) continue;

		// Select tracks from the target region
		G.chiPrimary = fFitter.GetChiToVertex(stsTrack);
		fh_chi2_prim_all->Fill(G.chiPrimary);
		if(G.chiPrimary > cutChiPrimary) continue;

		// Fit tracks to the primary vertex
		FairTrackParam VtxTrack;
		fFitter.FitToVertex(stsTrack, fPrimVertex, &VtxTrack);

		VtxTrack.Position(G.position);
		VtxTrack.Momentum(G.momentum);

		G.mass     = TDatabasePDG::Instance()->GetParticle(11)->Mass();
		G.charge   = (VtxTrack.GetQp() > 0) ?1 :-1;
		G.energy   = sqrt(G.momentum.Mag2() + G.mass * G.mass);
		G.rapidity = 0.5*TMath::Log((G.energy + G.momentum.Z()) /
					(G.energy - G.momentum.Z()));

		G.is_sts = true;

		//-----------------------------------------------------------------
		// RICH identification --------------------------------------------
		//-----------------------------------------------------------------
		G.richInd = G.fGlobalTrack->GetRichRingIndex();

		if(G.richInd < 0) continue;
		CbmRichRing* richRing = (CbmRichRing*)fArrayRichRing->At(G.richInd);
		if(NULL == richRing) continue;
		CbmRichRingMatch* richMatch = (CbmRichRingMatch*)fArrayRichMatch->At(G.richInd);
		if(NULL == richMatch) continue;
		G.richMCid = richMatch->GetMCTrackID();
		if(G.richMCid < 0) continue;


		// BEGIN --- check for correct match
		if(G.stsMCid == G.richMCid){
			CbmMCTrack* mcTrack = (CbmMCTrack*)fArrayMCTrack->At(G.stsMCid);
			G.mcPdg = mcTrack->GetPdgCode();
			G.mcMotherID = mcTrack->GetMotherId();
			G.mcMotherPdg = 0;
			if(G.mcMotherID >= 0){
				CbmMCTrack *mother = (CbmMCTrack*) fArrayMCTrack->At(G.mcMotherID);
				if(mother) G.mcMotherPdg = mother->GetPdgCode();
			} else {
				G.mcMotherPdg = 0;
			}
		}// END --- check for correct match

		// check for wrong match
		if(G.stsMCid != G.richMCid){
			if (211 == TMath::Abs(G.mcPdg)) {
				G.is_pion = true;
			} else if (2212 == TMath::Abs(G.mcPdg)) {
				G.is_proton = true;
			} else {
				G.is_wrong = true;
			}
		}

		// map fakes
		if(((Double_t)(richMatch->GetNofTrueHits()))/((Double_t)(richRing->GetNofHits())) < 0.6) {
			G.is_fake = true;
		}

		if (fUseRichAnn == false){
			// fill ring radius for all particle in rich
			G.axisA = richRing->GetAaxis();
			G.axisB = richRing->GetBaxis();

			fh_rich_ring_axisA_mom_all->Fill(G.momentum.Mag(), G.axisA);
			fh_rich_ring_axisB_mom_all->Fill(G.momentum.Mag(), G.axisB);
			fh_rich_id_no_cut_mom->Fill(G.momentum.Mag());

			// 1 RICH ID cut : cut on high momentum
			if(G.momentum.Mag() > fRichMom) continue;
			fh_rich_id_mom_cut_mom->Fill(G.momentum.Mag());

			// 2 RICH ID cut : use distance to track attached to ring
			G.distance = richRing->GetDistance();
			fh_rich_ring_dist->Fill(G.distance);
			fh_rich_dist_mom->Fill(G.momentum.Mag(), G.distance);
			if((G.stsMCid == G.richMCid) && (11 == TMath::Abs(G.mcPdg))) {
				fh_rich_ring_dist_e->Fill(G.distance);
				fh_rich_dist_mom_e->Fill(G.momentum.Mag(), G.distance);
			}
			// define 2D distance cut (BestCut2)
			if (fRichDist == 0.){
				if (G.momentum.Mag() < 0.5 && G.distance < 2.) isDist = true;
				if (G.momentum.Mag() >= 0.5 && G.momentum.Mag() < 2. ){
					if (G.distance < (7./3.-2./3.* G.momentum.Mag())) isDist = true;
				}
				if (G.momentum.Mag() >= 2. && G.distance < 1.) isDist = true;
			} else {
				if (G.distance < fRichDist) isDist = true;
			}

			if(isDist == false) continue;
			fh_rich_dist_mom_cut->Fill(G.momentum.Mag(), G.distance);
			fh_rich_id_dist_cut_mom->Fill(G.momentum.Mag());

			// 5 RICH ID cut : use dependence of radius on total p
			fh_rich_ring_axisA_mom->Fill(G.momentum.Mag(), G.axisA);
			fh_rich_ring_axisB_mom->Fill(G.momentum.Mag(), G.axisB);

			fh_rich_ring_axisA->Fill(G.axisA);
			fh_rich_ring_axisB->Fill(G.axisB);

			if((G.stsMCid == G.richMCid) && (11 == TMath::Abs(G.mcPdg))) {
				G.is_e = 1;
				fh_rich_ring_axisA_mom_e->Fill(G.momentum.Mag(), G.axisA);
				fh_rich_ring_axisB_mom_e->Fill(G.momentum.Mag(), G.axisB);

				fh_rich_ring_axisA_e->Fill(G.axisA);
				fh_rich_ring_axisB_e->Fill(G.axisB);
			}

			if(fabs(G.axisA - fRichAxisAmean) > fRichCoeff*fRichAxisAsigma ||
			   fabs(G.axisB - fRichAxisBmean) > fRichCoeff*fRichAxisBsigma) continue;
			fh_rich_id_axisAB_cut_mom->Fill(G.momentum.Mag());
		} else { // fUseRichAnn
		   if (fElIdAnn->DoSelect(richRing, G.momentum.Mag()) < fRichAnnCut) continue;
		   fh_rich_id_ann_cut_mom->Fill(G.momentum.Mag());
		   G.is_rich = true;

		   if(G.is_rich == false) continue;
		}
	/*
		// TRD ID cut
		G.trdInd  = G.fGlobalTrack->GetTrdTrackIndex();
		if(G.trdInd < 0) continue;

		CbmTrdTrack* trdTrack = (CbmTrdTrack*)fArrayTrdTrack->At(G.trdInd);
		if(NULL == trdTrack) continue;

		Double_t Eloss = 1000.*trdTrack->GetELoss();
		fh_trd_eloss->Fill(Eloss);

		fh_trd_ann->Fill(trdTrack->GetPidANN());
		fh_trd_ann_mom->Fill(G.momentum.Mag(),trdTrack->GetPidANN());

		if(11==TMath::Abs(G.mcPdg))
		{
		fh_trd_eloss_e->Fill(Eloss);
		fh_trd_ann_e->Fill(trdTrack->GetPidANN());
		fh_trd_ann_mom_e->Fill(G.momentum.Mag(),trdTrack->GetPidANN());
		}
		if(trdTrack->GetPidANN() < fTrdAnn) continue;

		G.is_trd = true;
		if(G.is_trd == false) continue;

	*/

		// TOF ID cut
		G.tofInd  = G.fGlobalTrack->GetTofHitIndex();
		if(G.tofInd < 0) continue;
		// Get the tracklength
		Double_t trackLength = G.fGlobalTrack->GetLength()/100.;

		// Calculate time of flight from TOF hit
		CbmTofHit* tofHit = (CbmTofHit*)fArrayTofHit->At(G.tofInd);
		if(NULL == tofHit) continue;
		Double_t time = 0.2998*tofHit->GetTime();  // time in ns -> transfrom to ct in m

		// Calculate mass squared
		Double_t mass2 = TMath::Power(G.momentum.Mag(),2.)*(TMath::Power(time/trackLength,2) - 1);
		fh_tof_m2_mom->Fill(G.momentum.Mag(),mass2);
		fh_tof_m2_mom_zoom->Fill(G.momentum.Mag(),mass2);
		if(fTofM2 == 0.){
			if (G.momentum.Mag() < 1. && mass2 < 0.01) G.is_tof = true;
			if (G.momentum.Mag() >= 1. ){
				if (mass2 < (0.01+(G.momentum.Mag()-1.)*0.09)) G.is_tof = true;
			}
		} else {
			if (mass2 < fTofM2) G.is_tof = true;
		}

		if(G.is_tof == true) {
			fh_tof_m2_mom_zoom_cut->Fill(G.momentum.Mag(),mass2);
			fh_rich_tof_id_cut_mom->Fill(G.momentum.Mag());
		}
		if(G.is_tof == false) continue;

		// Fill finally chi2
		fh_chi2_prim_e->Fill(G.chiPrimary);
	}
}



void CbmRichDielectronTask::Exec(Option_t *option)
{
	CbmTrackMatch* stsMatchNeg = NULL;
	CbmTrackMatch* stsMatchPos = NULL;
    Int_t nGlobalTrack = fArrayGlobalTrack->GetEntriesFast();

    // ------------------------------------------------------------------
    // ---- Reset the maps ----------------------------------------------
    // ------------------------------------------------------------------
    for(Int_t i = 0; i < nGlobalTrack; i++) {
		mapGamma[i] = kTRUE;
		mapDsts[i]  = kTRUE;
		mapTp[i]    = kTRUE;
		mapPt[i]    = kTRUE;
		mapTheta[i] = kTRUE;
		mapPi0[i]   = kTRUE;
    }

    MCPairs();

    FillDiGlobalTracks();

    SingleParticleAcceptance();

    PairAcceptance();

    DStsMomCut();


    // ------------------------------------------------------------------
    // ---- Prepare (theta + mom) cut -----------------------------------
    // ------------------------------------------------------------------
    Double_t sqrt_pp_cut;
    Double_t b = -1. * (f2Dtcut/ f2Dpcut);

    // Loop over negatives
    for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];

		ArrayThetaClosest[iNeg] = 360.;

		if (!globalTrackNeg.is_tof)  continue;

		if( !mapGamma[iNeg] ) continue;
		if( !mapDsts[iNeg]  ) continue;

		// --------------------------------------------
		// --- Loop over STS track not accepted in RICH
		// ---
		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (globalTrackPos.is_tof)  continue;
			if (!globalTrackPos.is_sts) continue;

			if( !mapGamma[iPos] ) continue;
			if( !mapDsts[iPos] ) continue;

			// Select opposite charge
			if(((globalTrackPos.charge < 0) &&
			(globalTrackNeg.charge < 0)) ||
			   ((globalTrackPos.charge > 0) &&
			(globalTrackNeg.charge > 0))) continue;

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
			Double_t theta = 180*angle/TMath::Pi();
			Double_t sqrt_mom = TMath::Sqrt(globalTrackNeg.momentum.Mag()*
							globalTrackPos.momentum.Mag());

			if(theta < 1.e-8) continue;
			// Find closest track by opening angle
			if(theta < ArrayThetaClosest[iNeg]) {
				ArrayThetaClosest[iNeg] = theta;
				ArrayMomClosest[iNeg] = sqrt_mom;
				ArrayTrackIdClosest[iNeg] = iPos;
				ArrayMotherIDClosest[iNeg] = globalTrackPos.mcMotherID;
				ArrayMotherPdgClosest[iNeg] = globalTrackPos.mcMotherPdg;
			}
		}// loop over sts tracks

		if((-1 == globalTrackNeg.mcMotherID) || (-1 == ArrayMotherIDClosest[iNeg])) {
			fh_dcut_theta_mom_omega->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg], fWeight);
		} else if((111 == globalTrackNeg.mcMotherPdg) || (111 == ArrayMotherPdgClosest[iNeg])) {
			fh_dcut_theta_mom_pi0->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		} else if((22 == globalTrackNeg.mcMotherPdg) || (22 == ArrayMotherPdgClosest[iNeg])) {
			fh_dcut_theta_mom_gamma->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		} else {
			fh_dcut_theta_mom_bg->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		}

		if((-1 == globalTrackNeg.mcMotherID) && (-1 == ArrayMotherIDClosest[iNeg])) {
			fh_dcut_theta_mom_omega_true->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg], fWeight);
		}
		if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==ArrayMotherIDClosest[iNeg])) {
			fh_dcut_theta_mom_gamma_true->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		}

		// --- ( theta + sqrt(p*p) ) cut ---
		sqrt_pp_cut = b * ArrayThetaClosest[iNeg] + f2Dtcut;
		if((ArrayThetaClosest[iNeg] <= f2Dtcut) &&
		   (ArrayMomClosest[iNeg]   <= sqrt_pp_cut)) {
			Int_t iPos = ArrayTrackIdClosest[iNeg];
			mapTp[iNeg] = kFALSE;
			mapTp[iPos] = kFALSE;
		}
    }//loop over global tracks


    // ------------------------------------------------------------------
    // ---- Loop over global tracks -------------------------------------
    // ------------------------------------------------------------------
    // Loop over negatives
    for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];

		if (!globalTrackNeg.is_tof)  continue;

		if( !mapGamma[iNeg] ) continue;
		if( !mapDsts[iNeg]  ) continue;
		if( !mapTp[iNeg]    ) continue;
		if(globalTrackNeg.charge > 0) fn_tpcut_em++;
		if(globalTrackNeg.charge < 0) fn_tpcut_ep++;


		// Fill pt rec tracks
		if(-1 == globalTrackNeg.mcMotherID) {
			fh_tpcut_pt_lept_omega->Fill(globalTrackNeg.momentum.Perp(), fWeight);
		} else {
			fh_tpcut_pt_lept_bg->Fill(globalTrackNeg.momentum.Perp());
		}

		// Select negatives
		if(globalTrackNeg.charge > 0) continue;

		if(globalTrackNeg.momentum.Perp() < fPtcut)  {
			mapPt[iNeg] = kFALSE;
		}

		// -------------------------
		// --- Loop over positives
		// ---
		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (!globalTrackPos.is_tof)  continue;

			if( !mapGamma[iPos] ) continue;
			if( !mapDsts[iPos]  ) continue;
			if( !mapTp[iPos]    ) continue;

			// Select positives
			if(globalTrackPos.charge < 0) continue;

			if(globalTrackPos.momentum.Perp() < fPtcut)  {
				mapPt[iPos] = kFALSE;
			}

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t energy = globalTrackNeg.energy + globalTrackPos.energy;
			Double_t pt = momentum.Perp();
			Double_t pz = momentum.Pz();
			Double_t y = 0.5*TMath::Log((energy+pz)/(energy-pz));
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
	//	    Double_t theta = 180*angle/TMath::Pi();
			Double_t minv = 2*TMath::Sin(angle/2)*TMath::Sqrt(globalTrackNeg.momentum.Mag()*globalTrackPos.momentum.Mag());

			// Fill histograms after theta + mom cut
			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_tpcut_omega_minv->Fill(minv, fWeight);
				fh_tpcut_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_tpcut_eta_minv->Fill(minv);
				fh_tpcut_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_tpcut_gamma_minv->Fill(minv);
				fh_tpcut_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_tpcut_pi0_minv->Fill(minv);
				fh_tpcut_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_tpcut_omega_bg_minv->Fill(minv, fWeight);
				fh_tpcut_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_tpcut_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_tpcut_minv->Fill(minv);
				fh_tpcut_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_tpcut_mc_minv->Fill(minv);
				}
			}

			// Pair acceptance
			if(globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID) {
				if(-1 == globalTrackNeg.mcMotherID) {
					fh_tpcut_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
				} else {
					fh_tpcut_pt_minv_pairs->Fill(minv, momentum.Perp());
				}
			}

			// Fill histograms after transverse momentum cut
			if( !mapPt[iNeg] ) continue;
			if( !mapPt[iPos] ) continue;

			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_ptcut_omega_minv->Fill(minv, fWeight);
				fh_ptcut_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_ptcut_eta_minv->Fill(minv);
				fh_ptcut_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_ptcut_gamma_minv->Fill(minv);
				fh_ptcut_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_ptcut_pi0_minv->Fill(minv);
				fh_ptcut_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_ptcut_omega_bg_minv->Fill(minv, fWeight);
				fh_ptcut_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_ptcut_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_ptcut_minv->Fill(minv);
				fh_ptcut_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_ptcut_mc_minv->Fill(minv);
				}
			}
		}//loop over positives
    }//loop over negatives
    // ------------------------------------------------------------------



    // ------------------------------------------------------------------
    // ---- Prepare theta cut -------------------------------------------
    // ------------------------------------------------------------------
    // Loop over negatives
    for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];

		ArrayThetaClosest[iNeg] = 360.;

		if (!globalTrackNeg.is_tof)  continue;

		if( !mapGamma[iNeg] ) continue;
		if( !mapDsts[iNeg]  ) continue;
		if( !mapTp[iNeg]    ) continue;
		if( !mapPt[iNeg]    ) continue;
		if(globalTrackNeg.charge > 0) fn_ptcut_em++;
		if(globalTrackNeg.charge < 0) fn_ptcut_ep++;

		// Select negatives
		if(globalTrackNeg.charge > 0) continue;

		// --- Loop over positives
		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (!globalTrackPos.is_tof)  continue;

			// Select positives
			if(globalTrackPos.charge < 0) continue;

			if( !mapGamma[iPos] ) continue;
			if( !mapDsts[iPos]  ) continue;
			if( !mapTp[iPos]    ) continue;
			if( !mapPt[iPos]    ) continue;

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
			Double_t theta = 180*angle/TMath::Pi();
			Double_t sqrt_mom = TMath::Sqrt(globalTrackNeg.momentum.Mag()*globalTrackPos.momentum.Mag());

			if((-1 == globalTrackNeg.mcMotherID) &&
			   (-1 == globalTrackPos.mcMotherID)) {
				fh_ptcut_theta_omega_true->Fill(theta, fWeight);
			}

			// Find closest track by opening angle
			if(theta < ArrayThetaClosest[iNeg]) {
				ArrayThetaClosest[iNeg] = theta;
				ArrayMomClosest[iNeg] = sqrt_mom;
				ArrayTrackIdClosest[iNeg] = iPos;
				ArrayMotherIDClosest[iNeg] = globalTrackPos.mcMotherID;
				ArrayMotherPdgClosest[iNeg] = globalTrackPos.mcMotherPdg;
			}
		}// loop over positives

		if((-1 == globalTrackNeg.mcMotherID) || (-1 == ArrayMotherIDClosest[iNeg])) {
			fh_ptcut_theta_omega->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		} else if((111 == globalTrackNeg.mcMotherPdg) || (111 == ArrayMotherPdgClosest[iNeg])) {
			fh_ptcut_theta_pi0->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		} else if((22 == globalTrackNeg.mcMotherPdg) || (22 == ArrayMotherPdgClosest[iNeg])) {
			fh_ptcut_theta_gamma->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		} else {
			fh_ptcut_theta_bg->Fill(ArrayMomClosest[iNeg], ArrayThetaClosest[iNeg]);
		}

		if(ArrayThetaClosest[iNeg] < fTcut) {
			Int_t iPos = ArrayTrackIdClosest[iNeg];
			mapTheta[iNeg] = kFALSE;
			mapTheta[iPos] = kFALSE;
		}

    }//loop over negatives


    // ------------------------------------------------------------------
    // ---- Loop over global tracks -------------------------------------
    // ------------------------------------------------------------------
    // Loop over negatives
    for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];

		if (!globalTrackNeg.is_tof)  continue;

		// Select negatives
		if(globalTrackNeg.charge > 0) continue;

		if( !mapGamma[iNeg] ) continue;
		if( !mapDsts[iNeg]  ) continue;
		if( !mapTp[iNeg]    ) continue;
		if( !mapPt[iNeg]    ) continue;
		if( !mapTheta[iNeg] ) continue;

		// --- Loop over positives
		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (!globalTrackPos.is_tof)  continue;

			// Select positives
			if(globalTrackPos.charge < 0) continue;

			if( !mapGamma[iPos] ) continue;
			if( !mapDsts[iPos]  ) continue;
			if( !mapTp[iPos]    ) continue;
			if( !mapPt[iPos]    ) continue;
			if( !mapTheta[iPos] ) continue;

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t energy = globalTrackNeg.energy + globalTrackPos.energy;
			Double_t pt = momentum.Perp();
			Double_t pz = momentum.Pz();
			Double_t y = 0.5*TMath::Log((energy+pz)/(energy-pz));
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
	//	    Double_t theta = 180*angle/TMath::Pi();
			Double_t minv = 2*TMath::Sin(angle/2)*TMath::Sqrt(globalTrackNeg.momentum.Mag()*globalTrackPos.momentum.Mag());

			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_tcut_omega_minv->Fill(minv, fWeight);
				fh_tcut_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_tcut_eta_minv->Fill(minv);
				fh_tcut_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_tcut_gamma_minv->Fill(minv);
				fh_tcut_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_tcut_pi0_minv->Fill(minv);
				fh_tcut_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_tcut_omega_bg_minv->Fill(minv, fWeight);
				fh_tcut_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_tcut_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_tcut_minv->Fill(minv);
				fh_tcut_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_tcut_mc_minv->Fill(minv);
				}
			}
		}//loop over positives
    }//loop over negatives

    // ------------------------------------------------------------------
    // ---- pi0-Dalitz reconstruction cut -------------------------------
    // ------------------------------------------------------------------
    // Loop over negatives
    for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];

		if (!globalTrackNeg.is_tof)  continue;

		if( !mapGamma[iNeg] ) continue;
		if( !mapDsts[iNeg]  ) continue;
		if( !mapTp[iNeg]    ) continue;
		if( !mapPt[iNeg]    ) continue;
		if( !mapTheta[iNeg] ) continue;
		if(globalTrackNeg.charge > 0) fn_tcut_em++;
		if(globalTrackNeg.charge < 0) fn_tcut_ep++;

		if(! mapPi0[iNeg]) continue;
		if(globalTrackNeg.charge > 0) fn_pi0cut_em++;
		if(globalTrackNeg.charge < 0) fn_pi0cut_ep++;

		// Select negatives
		if(globalTrackNeg.charge > 0) continue;

		// --- Loop over positives
		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (!globalTrackPos.is_tof)  continue;

			// Select positives
			if(globalTrackPos.charge < 0) continue;

			if( !mapGamma[iPos] ) continue;
			if( !mapDsts[iPos]  ) continue;
			if( !mapTp[iPos]    ) continue;
			if( !mapPt[iPos]    ) continue;
			if( !mapTheta[iPos] ) continue;

			// Skip, if small minv
			if(! mapPi0[iNeg]) continue;
			if(! mapPi0[iPos]) continue;

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t energy = globalTrackNeg.energy + globalTrackPos.energy;
			Double_t pt = momentum.Perp();
			Double_t pz = momentum.Pz();
			Double_t y = 0.5*TMath::Log((energy+pz)/(energy-pz));
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
			Double_t minv = 2*TMath::Sin(angle/2)*TMath::Sqrt(globalTrackNeg.momentum.Mag()*globalTrackPos.momentum.Mag());

			// Fill histograms after pi0-Dalitz reconstruction cut
			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_pi0cut_omega_minv->Fill(minv, fWeight);
				fh_pi0cut_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_pi0cut_eta_minv->Fill(minv);
				fh_pi0cut_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_pi0cut_gamma_minv->Fill(minv);
				fh_pi0cut_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_pi0cut_pi0_minv->Fill(minv);
				fh_pi0cut_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_pi0cut_omega_bg_minv->Fill(minv, fWeight);
				fh_pi0cut_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_pi0cut_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_pi0cut_minv->Fill(minv);
				fh_pi0cut_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_pi0cut_mc_minv->Fill(minv);
				}
			}

			// Pair acceptance
			if(globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID) {
				if(-1 == globalTrackNeg.mcMotherID) {
					fh_pi0cut_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
				} else {
					fh_pi0cut_pt_minv_pairs->Fill(minv, momentum.Perp());
				}
			}

			// ----- pi0-Dalitz reconstruction --------
			if(minv < fPi0cut) {
				fh_pi0cut_fake_pi0_minv->Fill(minv);
				if((111==globalTrackNeg.mcMotherPdg) &&
				   (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
					fh_pi0cut_true_pi0_minv->Fill(minv);
				}
				mapPi0[iNeg] = kFALSE;
				mapPi0[iPos] = kFALSE;
				continue;
			}
		}//loop over positives
    }//loop over negatives

    fEvents += 1;
    cout << "-I- event number " << fEvents << endl;
    cout << "fn_rich_id_ep = " << fn_rich_id_ep << endl;
    cout << "fn_rich_id_em = " << fn_rich_id_em << endl;
    cout << "fn_gcut_ep = " << fn_gcut_ep << endl;
    cout << "fn_gcut_em = " << fn_gcut_em << endl;
    cout << "fn_dcut_ep = " << fn_dcut_ep << endl;
    cout << "fn_dcut_em = " << fn_dcut_em << endl;
    cout << "fn_tpcut_ep = " << fn_tpcut_ep << endl;
    cout << "fn_tpcut_em = " << fn_tpcut_em << endl;
    cout << "fn_ptcut_ep = " << fn_ptcut_ep << endl;
    cout << "fn_ptcut_em = " << fn_ptcut_em << endl;
    cout << "fn_tcut_ep = " << fn_tcut_ep << endl;
    cout << "fn_tcut_em = " << fn_tcut_em << endl;
    cout << "fn_pi0cut_ep = " << fn_pi0cut_ep << endl;
    cout << "fn_pi0cut_em = " << fn_pi0cut_em << endl;

}// Exec

void CbmRichDielectronTask::MCPairs()
{
    Int_t nMCTrack = fArrayMCTrack->GetEntries();

    // Loop over negatives
    for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
		CbmMCTrack* trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
		Int_t pdgCode_neg = trackNeg->GetPdgCode();
		Int_t motherID_neg = trackNeg->GetMotherId();

		// If primary, motherPdg = 0
		Int_t motherPdg_neg = 0;
		if(motherID_neg > -1) {
			motherPdg_neg =	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
		}

		// Get vertex
		TVector3 vertex_neg;
		trackNeg->GetStartVertex(vertex_neg);

		// Calculate radial
		Double_t R = TMath::Sqrt(TMath::Power(vertex_neg.X(),2) +
					 TMath::Power(vertex_neg.Y(),2));
		if(vertex_neg.Y() > 0.) {
			R = R;
		} else {
			R = -1*R;
		}

		// Fill radial vs z of e+(e-) from gamma
		if((11==TMath::Abs(pdgCode_neg)) && (22==motherPdg_neg)) {
			fh_mc_all_zr->Fill(vertex_neg.Z(), R);
		}

		// Select tracks only from the target region (target 25 mkm)
		Double_t r = TMath::Sqrt(vertex_neg.X()*vertex_neg.X() +
					 vertex_neg.Y()*vertex_neg.Y() + vertex_neg.Z()*vertex_neg.Z());
		if(r > 0.0025) continue;

		// Get momentum
		TVector3 momentum_neg;
		trackNeg->GetMomentum(momentum_neg);
		Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
		TLorentzVector p_neg(momentum_neg, energy_neg);

		// Get number of STS points
		Int_t nPoint  = trackNeg->GetNPoints(kMVD)+trackNeg->GetNPoints(kSTS);

		// Select leptons
		if(11 != TMath::Abs(pdgCode_neg)) continue;

		// Fill all lepton momentum
		fh_mc_all_lept_mom->Fill(momentum_neg.Mag());

		// Fill reconstructed lepton momentum
		if(nPoint > 4) {
			fh_mc_rec_lept_mom->Fill(momentum_neg.Mag());
		}

		// Select e-
		if(11 != pdgCode_neg) continue;

		// Loop over positives
		for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
			CbmMCTrack* trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

			// Select e+
			Int_t pdgCode_pos = trackPos->GetPdgCode();
			if(-11 != pdgCode_pos) continue;

			// Get vertex
			TVector3 vertex_pos;
			trackPos->GetStartVertex(vertex_pos);

			// Select tracks only from the target region
			Double_t r = TMath::Sqrt(vertex_pos.X()*vertex_pos.X() +
						 vertex_pos.Y()*vertex_pos.Y() + vertex_pos.Z()*vertex_pos.Z());
			if(r > 0.0025) continue;

			// Get motherID
			Int_t motherID_pos = trackPos->GetMotherId();

			// If primary, motherPdg = 0
			Int_t motherPdg_pos = 0;
			if(motherID_pos > -1) {
				motherPdg_pos =((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
			}

			// Get momentum
			TVector3 momentum_pos;
			trackPos->GetMomentum(momentum_pos);
			Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
			TLorentzVector p_pos(momentum_pos, energy_pos);

			// Calculate kinematic parameters of the pair
			TVector3 momentum = momentum_neg + momentum_pos;
			Double_t energy = energy_neg + energy_pos;
			Double_t pt = momentum.Perp();
			Double_t pz = momentum.Pz();
			Double_t y = 0.5*TMath::Log((energy+pz)/ (energy-pz));
			Double_t angle = p_neg.Angle(p_pos.Vect());
			Double_t theta = 180*angle/ TMath::Pi();
			Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

			// Fill histograms for all mc tracks
			if((-1==motherID_neg) && (-1==motherID_pos)) {
				fh_mc_all_omega_mom->Fill(momentum.Mag(), fWeight);
				fh_mc_all_omega_theta->Fill(theta, fWeight);
				fh_mc_all_omega_minv->Fill(minv, fWeight);
				fh_mc_all_omega_pty->Fill(y, pt, fWeight);
			} else if((-1==motherID_neg) || (-1==motherID_pos)) {
				fh_mc_all_mom->Fill(momentum.Mag(), fWeight);
				fh_mc_all_theta->Fill(theta, fWeight);
				fh_mc_all_omega_bg_minv->Fill(minv, fWeight);
				fh_mc_all_pty->Fill(y, pt, fWeight);
			} else {
				fh_mc_all_mom->Fill(momentum.Mag());
				fh_mc_all_theta->Fill(theta);
				fh_mc_all_minv->Fill(minv);
				fh_mc_all_pty->Fill(y, pt);
			}
			if((221==motherPdg_neg) && (motherID_neg==motherID_pos)) {
				fh_mc_all_eta_mom->Fill(momentum.Mag());
				fh_mc_all_eta_theta->Fill(theta);
				fh_mc_all_eta_minv->Fill(minv);
				fh_mc_all_eta_pty->Fill(y, pt);
			}
			if((22==motherPdg_neg) && (motherID_neg==motherID_pos)) {
				fh_mc_all_gamma_mom->Fill(momentum.Mag());
				fh_mc_all_gamma_theta->Fill(theta);
				fh_mc_all_gamma_minv->Fill(minv);
				fh_mc_all_gamma_pty->Fill(y, pt);
			}
			if((111==motherPdg_neg) && (motherID_neg==motherID_pos)) {
				fh_mc_all_pi0_mom->Fill(momentum.Mag());
				fh_mc_all_pi0_theta->Fill(theta);
				fh_mc_all_pi0_minv->Fill(minv);
				fh_mc_all_pi0_pty->Fill(y, pt);
			}

			// Pair acceptance
			if(motherID_neg == motherID_pos) {
				if(-1 == motherID_neg) {
					fh_mc_all_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
				} else {
					fh_mc_all_pt_minv_pairs->Fill(minv, momentum.Perp());
				}
			}

			// Fill MC reconstructed lepton pairs (tracks with 4 sts points)
			Int_t n_sts_neg = trackNeg->GetNPoints(kMVD)+trackNeg->GetNPoints(kSTS);// id 1-mvd, 2-sts
			Int_t n_sts_pos = trackPos->GetNPoints(kMVD)+trackPos->GetNPoints(kSTS);// id 1-mvd, 2-sts
			if(n_sts_neg < 4) continue;
			if(n_sts_pos < 4) continue;
			if(trackNeg->GetNPoints(kRICH) < 0) continue;
			if(trackPos->GetNPoints(kRICH) < 0) continue;

			// Fill histograms for "mc rec" tracks
			if((-1==motherID_neg) && (-1==motherID_pos)) {
				fh_mc_rec_omega_mom->Fill(momentum.Mag(), fWeight);
				fh_mc_rec_omega_theta->Fill(theta, fWeight);
				fh_mc_rec_omega_minv->Fill(minv, fWeight);
				fh_mc_rec_omega_pty->Fill(y, pt, fWeight);
			} else if((221==motherPdg_neg) && (motherID_neg==motherID_pos)) {
				fh_mc_rec_eta_mom->Fill(momentum.Mag());
				fh_mc_rec_eta_theta->Fill(theta);
				fh_mc_rec_eta_minv->Fill(minv);
				fh_mc_rec_eta_pty->Fill(y, pt);
			} else if((22==motherPdg_neg) && (motherID_neg==motherID_pos)) {
				fh_mc_rec_gamma_mom->Fill(momentum.Mag());
				fh_mc_rec_gamma_theta->Fill(theta);
				fh_mc_rec_gamma_minv->Fill(minv);
				fh_mc_rec_gamma_pty->Fill(y, pt);
			} else if((111==motherPdg_neg) && (motherID_neg==motherID_pos)) {
				fh_mc_rec_pi0_mom->Fill(momentum.Mag());
				fh_mc_rec_pi0_theta->Fill(theta);
				fh_mc_rec_pi0_minv->Fill(minv);
				fh_mc_rec_pi0_pty->Fill(y, pt);
			} else if((-1==motherID_neg) || (-1==motherID_pos)) {
				fh_mc_rec_mom->Fill(momentum.Mag(), fWeight);
				fh_mc_rec_theta->Fill(theta, fWeight);
				fh_mc_rec_omega_bg_minv->Fill(minv, fWeight);
				fh_mc_rec_pty->Fill(y, pt, fWeight);
			} else {
				fh_mc_rec_mom->Fill(momentum.Mag());
				fh_mc_rec_theta->Fill(theta);
				fh_mc_rec_minv->Fill(minv);
				fh_mc_rec_pty->Fill(y, pt);
			}

			// Pair acceptance
			if(motherID_neg == motherID_pos) {
				if(-1 == motherID_neg) {
					fh_mc_rec_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
				} else {
					fh_mc_rec_pt_minv_pairs->Fill(minv, momentum.Perp());
				}
			}
		}//loop over positives
    }//loop over negatives
} //MC Pairs

void CbmRichDielectronTask::SingleParticleAcceptance()
{
    Int_t nGlobalTrack = fArrayGlobalTrack->GetEntriesFast();
	for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++)
	{
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];
		if (!globalTrackNeg.is_sts)  continue;
		if(11==globalTrackNeg.mcPdg) {
			fh_mom_e_sts->Fill(globalTrackNeg.momentum.Mag());
		}
		if(11==globalTrackNeg.mcPdg && -1==globalTrackNeg.mcMotherID) {
			fh_mom_w_sts->Fill(globalTrackNeg.momentum.Mag());
		}

		if (!globalTrackNeg.is_rich)  continue;
		fh_mom_rich->Fill(globalTrackNeg.momentum.Mag());
		if(11==globalTrackNeg.mcPdg) {
			fh_mom_e_rich->Fill(globalTrackNeg.momentum.Mag());
		}
		if(11==globalTrackNeg.mcPdg && -1==globalTrackNeg.mcMotherID) {
			fh_mom_w_rich->Fill(globalTrackNeg.momentum.Mag());
		}

	//	if (!globalTrackNeg.is_trd)  continue;
	//	fh_mom_trd->Fill(globalTrackNeg.momentum.Mag());
	//	if(11==globalTrackNeg.mcPdg) {
	//	    fh_mom_e_trd->Fill(globalTrackNeg.momentum.Mag());
	//	}
	//	if((11==globalTrackNeg.mcPdg)&&
	//	   (-1==globalTrackNeg.mcMotherID)) {
	//	    fh_mom_w_trd->Fill(globalTrackNeg.momentum.Mag());
	//	}

		if (!globalTrackNeg.is_tof)  continue;
		fh_mom_tof->Fill(globalTrackNeg.momentum.Mag());
		if(11==globalTrackNeg.mcPdg) {
			fh_mom_e_tof->Fill(globalTrackNeg.momentum.Mag());
		}
		if(11==globalTrackNeg.mcPdg && -1==globalTrackNeg.mcMotherID) {
			fh_mom_w_tof->Fill(globalTrackNeg.momentum.Mag());
		}
	}
}

void CbmRichDielectronTask::PairAcceptance()
{
    Int_t nGlobalTrack = fArrayGlobalTrack->GetEntriesFast();
// Loop over negatives
    for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];
		//cout << globalTrackNeg.charge << endl;
		if (!globalTrackNeg.is_tof)  continue;

		// Fill momentum rec tracks
		fh_rich_id_lept_mom->Fill(globalTrackNeg.momentum.Mag());

		// Count ep em

		if(globalTrackNeg.charge > 0){
			fn_rich_id_em++;
			fh_rich_id_em_mom->Fill(globalTrackNeg.momentum.Mag());
		}
		if(globalTrackNeg.charge < 0){
			fn_rich_id_ep++;
			fh_rich_id_ep_mom->Fill(globalTrackNeg.momentum.Mag());
		}
		// Select negatives
		if(globalTrackNeg.charge > 0) continue;

		// Loop over positives
		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (!globalTrackPos.is_tof)  continue;

			// Select positives
			if(globalTrackPos.charge < 0) continue;

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t energy = globalTrackNeg.energy + globalTrackPos.energy;
			Double_t pt = momentum.Perp();
			Double_t pz = momentum.Pz();
			Double_t y = 0.5*TMath::Log((energy+pz)/(energy-pz));
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
			Double_t theta = 180*angle/TMath::Pi();
			Double_t minv = 2*TMath::Sin(angle/2)*TMath::Sqrt(globalTrackNeg.momentum.Mag()*globalTrackPos.momentum.Mag());

			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_rich_id_omega_mom->Fill(momentum.Mag(), fWeight);
				fh_rich_id_omega_theta->Fill(theta, fWeight);
				fh_rich_id_omega_minv->Fill(minv, fWeight);
				fh_rich_id_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_rich_id_eta_mom->Fill(momentum.Mag());
				fh_rich_id_eta_theta->Fill(theta);
				fh_rich_id_eta_minv->Fill(minv);
				fh_rich_id_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_rich_id_gamma_mom->Fill(momentum.Mag());
				fh_rich_id_gamma_theta->Fill(theta);
				fh_rich_id_gamma_minv->Fill(minv);
				fh_rich_id_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_rich_id_pi0_mom->Fill(momentum.Mag());
				fh_rich_id_pi0_theta->Fill(theta);
				fh_rich_id_pi0_minv->Fill(minv);
				fh_rich_id_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_rich_id_mom->Fill(momentum.Mag(), fWeight);
				fh_rich_id_theta->Fill(theta, fWeight);
				fh_rich_id_omega_bg_minv->Fill(minv, fWeight);
				fh_rich_id_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_rich_id_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_rich_id_mom->Fill(momentum.Mag());
				fh_rich_id_theta->Fill(theta);
				fh_rich_id_minv->Fill(minv);
				fh_rich_id_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_rich_id_mc_minv->Fill(minv);
				}
			}
			// Pair acceptance
			if(globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID) {
				if(-1 == globalTrackNeg.mcMotherID) {
					fh_rich_id_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
				} else {
					fh_rich_id_pt_minv_pairs->Fill(minv, momentum.Perp());
				}
			}
			// gamma conversion cut ----------------------------------------------------
			if(minv < fGcut) {
				mapGamma[iNeg] = kFALSE;
				mapGamma[iPos] = kFALSE;
				continue;
			}

			if( !mapGamma[iNeg] ) continue;
			if( !mapGamma[iPos] ) continue;

			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_gcut_omega_minv->Fill(minv, fWeight);
				fh_gcut_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_gcut_eta_minv->Fill(minv);
				fh_gcut_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_gcut_gamma_minv->Fill(minv);
				fh_gcut_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_gcut_pi0_minv->Fill(minv);
				fh_gcut_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_gcut_omega_bg_minv->Fill(minv, fWeight);
				fh_gcut_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_gcut_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_gcut_minv->Fill(minv);
				fh_gcut_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_gcut_mc_minv->Fill(minv);
				}
			}
		}//loop over positives
    }//loop over negatives
} // PairsAcceptance

void CbmRichDielectronTask::DStsMomCut()
{
	// Find closest neighbor hit in STS1
    Int_t nGlobalTrack = fArrayGlobalTrack->GetEntriesFast();
	Int_t nMvdHit = fArrayMvdHit->GetEntriesFast();
	Int_t nHits = 0;
	Float_t ArrayMvdX[nMvdHit];
	Float_t ArrayMvdY[nMvdHit];
	Float_t ArrayTrackX[nGlobalTrack];
	Float_t ArrayTrackY[nGlobalTrack];
	Float_t ArrayDsts[nGlobalTrack];
	Int_t  hitStation[nMvdHit];
	Bool_t hitBusy[nMvdHit];

	CbmKF *KF = CbmKF::Instance();

	for(Int_t iHit = 0; iHit < nMvdHit; iHit++) {
		hitBusy[iHit] = 0;
		hitStation[iHit] = 1;
		CbmMvdHit *pmh = (CbmMvdHit*) fArrayMvdHit->At(iHit);
		if(!pmh) continue;
		hitStation[iHit] = KF->StsStationIDMap[pmh->GetDetectorID()];
		ArrayMvdX[nHits] = pmh->GetX();
		ArrayMvdY[nHits] = pmh->GetY();
		nHits++;
	}

	Int_t nofStsTracks = fArrayStsTrack->GetEntriesFast();
	for(Int_t iTrack = 0; iTrack < nofStsTracks; iTrack++) {
		ArrayTrackX[iTrack] = 1000;
		ArrayTrackY[iTrack] = 1000;
		ArrayDsts[iTrack] = 0;
		CbmStsTrack *track = (CbmStsTrack*) fArrayStsTrack->At(iTrack);
		if(!track) continue;

		Int_t nthits = track->GetNMvdHits();
		for(Int_t ith = 0; ith < nthits; ith++) {
			Int_t iHit = track->GetMvdHitIndex(ith);
			if(iHit < 0 || iHit > nMvdHit) continue;
			hitBusy[ iHit ] = 1;
			if(hitStation[iHit] == 0) {
				CbmMvdHit *pmh = (CbmMvdHit*) fArrayMvdHit->At(iHit);
				if(!pmh) continue;
				ArrayTrackX[iTrack]= pmh->GetX();
				ArrayTrackY[iTrack]= pmh->GetY();
			}
		}
	}

	for(Int_t iTrack = 0; iTrack < nGlobalTrack; iTrack++) {

		ArrayDsts[iTrack] = 1.E8;
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iTrack];
		if (!globalTrackNeg.is_tof) continue;
		if (!mapGamma[iTrack]) continue;

		for(Int_t iHit = 0; iHit < nHits; iHit++) {
			Float_t dx = ArrayMvdX[iHit] - ArrayTrackX[iTrack];
			Float_t dy = ArrayMvdY[iHit] - ArrayTrackY[iTrack];
			Float_t d2 = dx*dx + dy*dy;
			if(d2 < 1.e-8) continue;
			if(d2 < ArrayDsts[iTrack]) ArrayDsts[iTrack] = d2;
		}
		ArrayDsts[iTrack] = sqrt( ArrayDsts[iTrack] );
	}

	for(Int_t iTrack1 = 0; iTrack1 < nGlobalTrack; iTrack1++)
	{
		DiGlobalTrack &T1 = fDiGlobalTracks[iTrack1];

		if(!T1.is_tof) continue;
		if(!mapGamma[iTrack1] ) continue;
		if(T1.charge > 0) fn_gcut_em++;
		if(T1.charge < 0) fn_gcut_ep++;

		if(T1.mcMotherID < 0) {
			fh_dsts_omega->Fill(ArrayDsts[iTrack1], fWeight);
			fh_dsts_mom_omega->Fill(ArrayDsts[iTrack1], T1.momentum.Mag(), fWeight);
		} else{
			fh_dsts_bg->Fill(ArrayDsts[iTrack1]);
			fh_dsts_mom_bg->Fill(ArrayDsts[iTrack1], T1.momentum.Mag());
			if( T1.mcMotherPdg==22 ) {
				fh_dsts_gamma->Fill(ArrayDsts[iTrack1]);
				fh_dsts_mom_gamma->Fill(ArrayDsts[iTrack1], T1.momentum.Mag());
			}
			if( T1.mcMotherPdg==111 ) {
				fh_dsts_pi0->Fill(ArrayDsts[iTrack1]);
				fh_dsts_mom_pi0->Fill(ArrayDsts[iTrack1], T1.momentum.Mag());
			}
		}

		// --- ( dsts + p) cut ---
		Double_t dsts_cut;
		Double_t f2Dpcut = 1.1;
		Double_t f2Ddcut = 0.5;
		Double_t b = -1. * (f2Dpcut/ f2Ddcut);
		dsts_cut = b * ArrayDsts[iTrack1] + f2Dpcut;

		if((T1.momentum.Mag()  <= f2Dpcut) &&
		   (ArrayDsts[iTrack1] <= dsts_cut)) {
			mapDsts[iTrack1] = kFALSE;
		}
	}

	// ---- Loop over global tracks (apply dsts cut ) -------------------
	// Loop over negatives
	for(Int_t iNeg = 0; iNeg < nGlobalTrack; iNeg++) {
		DiGlobalTrack &globalTrackNeg = fDiGlobalTracks[iNeg];

		if (!globalTrackNeg.is_tof)  continue;

		if( !mapGamma[iNeg] ) continue;
		if( !mapDsts[iNeg]  ) continue;

		if(globalTrackNeg.charge > 0) fn_dcut_em++;
		if(globalTrackNeg.charge < 0) fn_dcut_ep++;

		// Select negatives
		if(globalTrackNeg.charge > 0) continue;

		for(Int_t iPos = 0; iPos < nGlobalTrack; iPos++) {
			DiGlobalTrack &globalTrackPos = fDiGlobalTracks[iPos];

			if (!globalTrackPos.is_tof)  continue;

			if( !mapGamma[iPos] ) continue;
			if( !mapDsts[iPos]  ) continue;

			// Select positives
			if(globalTrackPos.charge < 0) continue;

			// Calculate kinematic parameters of the pair
			TVector3 momentum = globalTrackNeg.momentum + globalTrackPos.momentum;
			Double_t energy = globalTrackNeg.energy + globalTrackPos.energy;
			Double_t pt = momentum.Perp();
			Double_t pz = momentum.Pz();
			Double_t y = 0.5*TMath::Log((energy+pz)/(energy-pz));
			Double_t angle = globalTrackNeg.momentum.Angle(globalTrackPos.momentum);
			Double_t minv = 2*TMath::Sin(angle/2)*TMath::Sqrt(globalTrackNeg.momentum.Mag()*globalTrackPos.momentum.Mag());

			// Fill histograms after dsts + mom cut
			if((-1==globalTrackNeg.mcMotherID) && (-1==globalTrackPos.mcMotherID)) {
				fh_dcut_omega_minv->Fill(minv, fWeight);
				fh_dcut_omega_pty->Fill(y, pt, fWeight);
			} else if((221==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_dcut_eta_minv->Fill(minv);
				fh_dcut_eta_pty->Fill(y, pt);
			} else if((22==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_dcut_gamma_minv->Fill(minv);
				fh_dcut_gamma_pty->Fill(y, pt);
			} else if((111==globalTrackNeg.mcMotherPdg) && (globalTrackNeg.mcMotherID==globalTrackPos.mcMotherID)) {
				fh_dcut_pi0_minv->Fill(minv);
				fh_dcut_pi0_pty->Fill(y, pt);
			} else if((-1==globalTrackNeg.mcMotherID) || (-1==globalTrackPos.mcMotherID)) {
				fh_dcut_omega_bg_minv->Fill(minv, fWeight);
				fh_dcut_pty->Fill(y, pt, fWeight);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_dcut_mc_omega_bg_minv->Fill(minv, fWeight);
				}
			} else {
				fh_dcut_minv->Fill(minv);
				fh_dcut_pty->Fill(y, pt);
				if((globalTrackNeg.is_e) && (globalTrackPos.is_e)) {
					fh_dcut_mc_minv->Fill(minv);
				}
			}
		}//loop over positives
	}//loop over negatives
}// DStsMomCut

void CbmRichDielectronTask::Finish()
{

}

void CbmRichDielectronTask::WriteOutput()
{
    // --- Normalize histogramms ---
    Double_t cNev = 1./(Double_t)fEvents;

    fh_chi2_prim_all->Scale(cNev);
    fh_chi2_prim_e->Scale(cNev);
    fh_mc_all_zr->Scale(cNev);

    fh_mc_all_lept_mom->Scale(cNev);
    fh_mc_rec_lept_mom->Scale(cNev);
    fh_rich_id_lept_mom->Scale(cNev);
    fh_rich_id_ep_mom->Scale(cNev);
    fh_rich_id_em_mom->Scale(cNev);

    fh_mc_all_mom->Scale(cNev);
    fh_mc_rec_mom->Scale(cNev);
    fh_rich_id_mom->Scale(cNev);
    fh_rich_id_no_cut_mom->Scale(cNev);
    fh_rich_id_mom_cut_mom->Scale(cNev);
   // fh_rich_id_dist_cut_mom->Scale(cNev);
    fh_rich_id_ann_cut_mom->Scale(cNev);
 //   fh_rich_id_xy_cut_mom->Scale(cNev);
 //   fh_rich_id_axisAB_cut_mom->Scale(cNev);

    fh_mc_all_omega_mom->Scale(cNev);
    fh_mc_rec_omega_mom->Scale(cNev);
    fh_rich_id_omega_mom->Scale(cNev);

    fh_mc_all_eta_mom->Scale(cNev);
    fh_mc_rec_eta_mom->Scale(cNev);
    fh_rich_id_eta_mom->Scale(cNev);

    fh_mc_all_pi0_mom->Scale(cNev);
    fh_mc_rec_pi0_mom->Scale(cNev);
    fh_rich_id_pi0_mom->Scale(cNev);

    fh_mc_all_gamma_mom->Scale(cNev);
    fh_mc_rec_gamma_mom->Scale(cNev);
    fh_rich_id_gamma_mom->Scale(cNev);

    fh_mc_all_theta->Scale(cNev);
    fh_mc_rec_theta->Scale(cNev);
    fh_rich_id_theta->Scale(cNev);

    fh_mc_all_omega_theta->Scale(cNev);
    fh_mc_rec_omega_theta->Scale(cNev);
    fh_rich_id_omega_theta->Scale(cNev);

    fh_mc_all_eta_theta->Scale(cNev);
    fh_mc_rec_eta_theta->Scale(cNev);
    fh_rich_id_eta_theta->Scale(cNev);

    fh_mc_all_pi0_theta->Scale(cNev);
    fh_mc_rec_pi0_theta->Scale(cNev);
    fh_rich_id_pi0_theta->Scale(cNev);

    fh_mc_all_gamma_theta->Scale(cNev);
    fh_mc_rec_gamma_theta->Scale(cNev);
    fh_rich_id_gamma_theta->Scale(cNev);

  //  fh_rich_ring_dist->Scale(cNev);
//    fh_rich_ring_dist_e->Scale(cNev);
//    fh_rich_dist_mom->Scale(cNev);
//    fh_rich_dist_mom_e->Scale(cNev);
//    fh_rich_dist_mom_cut->Scale(cNev);
//    fh_selectNN_true_e->Scale(cNev);
//    fh_selectNN_wrong_e->Scale(cNev);
//    fh_rich_ring_xy->Scale(cNev);
//    fh_rich_ring_xy_e->Scale(cNev);
//    fh_rich_ring_xy_cut->Scale(cNev);
 //   fh_rich_ring_radi->Scale(cNev);
 //   fh_rich_ring_radi_e->Scale(cNev);
 //   fh_rich_ring_axisA_mom_all->Scale(cNev);
 //   fh_rich_ring_axisA_mom->Scale(cNev);
  //  fh_rich_ring_axisA->Scale(cNev);
  //  fh_rich_ring_axisA_mom_e->Scale(cNev);
  //  fh_rich_ring_axisA_e->Scale(cNev);
  //  fh_rich_ring_axisB_mom_all->Scale(cNev);
  //  fh_rich_ring_axisB_mom->Scale(cNev);
  //  fh_rich_ring_axisB->Scale(cNev);
  //  fh_rich_ring_axisB_mom_e->Scale(cNev);
  //  fh_rich_ring_axisB_e->Scale(cNev);

//    fh_trd_eloss->Scale(cNev);
//    fh_trd_eloss_e->Scale(cNev);
 //   fh_trd_ann->Scale(cNev);
//    fh_trd_ann_e->Scale(cNev);
//    fh_trd_ann_mom->Scale(cNev);
 //   fh_trd_ann_mom_e->Scale(cNev);
    fh_tof_m2_mom->Scale(cNev);
    fh_tof_m2_mom_zoom->Scale(cNev);
    fh_tof_m2_mom_zoom_cut->Scale(cNev);

    fh_dsts_bg->Scale(cNev);
    fh_dsts_omega->Scale(cNev);
    fh_dsts_pi0->Scale(cNev);
    fh_dsts_gamma->Scale(cNev);
    fh_dsts_mom_bg->Scale(cNev);
    fh_dsts_mom_omega->Scale(cNev);
    fh_dsts_mom_pi0->Scale(cNev);
    fh_dsts_mom_gamma->Scale(cNev);

    fh_dcut_theta_mom_omega->Scale(cNev);
    fh_dcut_theta_mom_omega_true->Scale(cNev);
    fh_dcut_theta_mom_pi0->Scale(cNev);
    fh_dcut_theta_mom_gamma->Scale(cNev);
    fh_dcut_theta_mom_gamma_true->Scale(cNev);
    fh_dcut_theta_mom_bg->Scale(cNev);
    fh_tpcut_pt_lept_omega->Scale(cNev);
    fh_tpcut_pt_lept_bg->Scale(cNev);
    fh_ptcut_theta_omega->Scale(cNev);
    fh_ptcut_theta_omega_true->Scale(cNev);
    fh_ptcut_theta_pi0->Scale(cNev);
    fh_ptcut_theta_gamma->Scale(cNev);
    fh_ptcut_theta_bg->Scale(cNev);

    fh_mc_all_minv->Scale(cNev);
    fh_mc_rec_minv->Scale(cNev);
    fh_rich_id_minv->Scale(cNev);
    fh_gcut_minv->Scale(cNev);
    fh_dcut_minv->Scale(cNev);
    fh_tpcut_minv->Scale(cNev);
    fh_ptcut_minv->Scale(cNev);
    fh_tcut_minv->Scale(cNev);
    fh_pi0cut_minv->Scale(cNev);

    fh_pi0cut_epep_minv->Scale(cNev);
    fh_pi0cut_emem_minv->Scale(cNev);

    fh_mc_all_pt_minv_omega->Scale(cNev);
    fh_mc_all_pt_minv_pairs->Scale(cNev);
    fh_mc_rec_pt_minv_omega->Scale(cNev);
    fh_mc_rec_pt_minv_pairs->Scale(cNev);
    fh_rich_id_pt_minv_omega->Scale(cNev);
    fh_rich_id_pt_minv_pairs->Scale(cNev);
    fh_tpcut_pt_minv_omega->Scale(cNev);
    fh_tpcut_pt_minv_pairs->Scale(cNev);
    fh_pi0cut_pt_minv_omega->Scale(cNev);
    fh_pi0cut_pt_minv_pairs->Scale(cNev);

    fh_rich_id_mc_minv->Scale(cNev);
    fh_gcut_mc_minv->Scale(cNev);
    fh_dcut_mc_minv->Scale(cNev);
    fh_tpcut_mc_minv->Scale(cNev);
    fh_ptcut_mc_minv->Scale(cNev);
    fh_tcut_mc_minv->Scale(cNev);
    fh_pi0cut_mc_minv->Scale(cNev);
    fh_mc_all_omega_bg_minv->Scale(cNev);
    fh_mc_rec_omega_bg_minv->Scale(cNev);
    fh_rich_id_omega_bg_minv->Scale(cNev);
    fh_gcut_omega_bg_minv->Scale(cNev);
    fh_dcut_omega_bg_minv->Scale(cNev);
    fh_tpcut_omega_bg_minv->Scale(cNev);
    fh_ptcut_omega_bg_minv->Scale(cNev);
    fh_tcut_omega_bg_minv->Scale(cNev);
    fh_pi0cut_omega_bg_minv->Scale(cNev);
    fh_rich_id_mc_omega_bg_minv->Scale(cNev);
    fh_gcut_mc_omega_bg_minv->Scale(cNev);
    fh_dcut_mc_omega_bg_minv->Scale(cNev);
    fh_tpcut_mc_omega_bg_minv->Scale(cNev);
    fh_ptcut_mc_omega_bg_minv->Scale(cNev);
    fh_tcut_mc_omega_bg_minv->Scale(cNev);
    fh_pi0cut_mc_omega_bg_minv->Scale(cNev);

    fh_ptcut_omega_minv->Scale(cNev);
    fh_mc_all_omega_minv->Scale(cNev);
    fh_mc_rec_omega_minv->Scale(cNev);
    fh_rich_id_omega_minv->Scale(cNev);
    fh_gcut_omega_minv->Scale(cNev);
    fh_dcut_omega_minv->Scale(cNev);
    fh_tpcut_omega_minv->Scale(cNev);
    fh_tcut_omega_minv->Scale(cNev);
    fh_pi0cut_omega_minv->Scale(cNev);

    fh_mc_all_eta_minv->Scale(cNev);
    fh_mc_rec_eta_minv->Scale(cNev);
    fh_rich_id_eta_minv->Scale(cNev);
    fh_gcut_eta_minv->Scale(cNev);
    fh_dcut_eta_minv->Scale(cNev);
    fh_tpcut_eta_minv->Scale(cNev);
    fh_ptcut_eta_minv->Scale(cNev);
    fh_tcut_eta_minv->Scale(cNev);
    fh_pi0cut_eta_minv->Scale(cNev);

    fh_mc_all_pi0_minv->Scale(cNev);
    fh_mc_rec_pi0_minv->Scale(cNev);
    fh_rich_id_pi0_minv->Scale(cNev);
    fh_gcut_pi0_minv->Scale(cNev);
    fh_dcut_pi0_minv->Scale(cNev);
    fh_tpcut_pi0_minv->Scale(cNev);
    fh_ptcut_pi0_minv->Scale(cNev);
    fh_tcut_pi0_minv->Scale(cNev);
    fh_pi0cut_pi0_minv->Scale(cNev);
    fh_pi0cut_fake_pi0_minv->Scale(cNev);
    fh_pi0cut_true_pi0_minv->Scale(cNev);

    fh_mc_all_gamma_minv->Scale(cNev);
    fh_mc_rec_gamma_minv->Scale(cNev);
    fh_rich_id_gamma_minv->Scale(cNev);
    fh_gcut_gamma_minv->Scale(cNev);
    fh_dcut_gamma_minv->Scale(cNev);
    fh_tpcut_gamma_minv->Scale(cNev);
    fh_ptcut_gamma_minv->Scale(cNev);
    fh_tcut_gamma_minv->Scale(cNev);
    fh_pi0cut_gamma_minv->Scale(cNev);

    fh_mc_all_pty->Scale(cNev);
    fh_mc_rec_pty->Scale(cNev);
    fh_rich_id_pty->Scale(cNev);
    fh_gcut_pty->Scale(cNev);
    fh_dcut_pty->Scale(cNev);
    fh_tpcut_pty->Scale(cNev);
    fh_ptcut_pty->Scale(cNev);
    fh_tcut_pty->Scale(cNev);
    fh_pi0cut_pty->Scale(cNev);

    fh_mc_all_omega_pty->Scale(cNev);
    fh_mc_rec_omega_pty->Scale(cNev);
    fh_rich_id_omega_pty->Scale(cNev);
    fh_gcut_omega_pty->Scale(cNev);
    fh_dcut_omega_pty->Scale(cNev);
    fh_tpcut_omega_pty->Scale(cNev);
    fh_ptcut_omega_pty->Scale(cNev);
    fh_tcut_omega_pty->Scale(cNev);
    fh_pi0cut_omega_pty->Scale(cNev);

    fh_mc_all_eta_pty->Scale(cNev);
    fh_mc_rec_eta_pty->Scale(cNev);
    fh_rich_id_eta_pty->Scale(cNev);
    fh_gcut_eta_pty->Scale(cNev);
    fh_dcut_eta_pty->Scale(cNev);
    fh_tpcut_eta_pty->Scale(cNev);
    fh_ptcut_eta_pty->Scale(cNev);
    fh_tcut_eta_pty->Scale(cNev);
    fh_pi0cut_eta_pty->Scale(cNev);

    fh_mc_all_pi0_pty->Scale(cNev);
    fh_mc_rec_pi0_pty->Scale(cNev);
    fh_rich_id_pi0_pty->Scale(cNev);
    fh_gcut_pi0_pty->Scale(cNev);
    fh_dcut_pi0_pty->Scale(cNev);
    fh_tpcut_pi0_pty->Scale(cNev);
    fh_ptcut_pi0_pty->Scale(cNev);
    fh_tcut_pi0_pty->Scale(cNev);
    fh_pi0cut_pi0_pty->Scale(cNev);

    fh_mc_all_gamma_pty->Scale(cNev);
    fh_mc_rec_gamma_pty->Scale(cNev);
    fh_rich_id_gamma_pty->Scale(cNev);
    fh_gcut_gamma_pty->Scale(cNev);
    fh_dcut_gamma_pty->Scale(cNev);
    fh_tpcut_gamma_pty->Scale(cNev);
    fh_ptcut_gamma_pty->Scale(cNev);
    fh_tcut_gamma_pty->Scale(cNev);
    fh_pi0cut_gamma_pty->Scale(cNev);

    fh_mom_rich->Scale(cNev);
 //   fh_mom_trd->Scale(cNev);
    fh_mom_tof->Scale(cNev);

    fh_mom_w_sts->Scale(cNev);
    fh_mom_w_rich->Scale(cNev);
 //   fh_mom_w_trd->Scale(cNev);
    fh_mom_w_tof->Scale(cNev);

    fh_mom_e_sts->Scale(cNev);
    fh_mom_e_rich->Scale(cNev);
//    fh_mom_e_trd->Scale(cNev);
    fh_mom_e_tof->Scale(cNev);

    fh_rich_id_pi0_pi0_minv->Scale(cNev);
    fh_rich_id_pi0_gamma_minv->Scale(cNev);
    fh_rich_id_gamma_gamma_minv->Scale(cNev);
    fh_rich_id_eta_gamma_minv->Scale(cNev);
    fh_rich_id_eta_eta_minv->Scale(cNev);
    fh_rich_id_eta_pi0_minv->Scale(cNev);

  //  fh_trd_id_pi0_pi0_minv->Scale(cNev);
  //  fh_trd_id_pi0_gamma_minv->Scale(cNev);
  //  fh_trd_id_gamma_gamma_minv->Scale(cNev);
  //  fh_trd_id_eta_gamma_minv->Scale(cNev);
  //  fh_trd_id_eta_eta_minv->Scale(cNev);
  //  fh_trd_id_eta_pi0_minv->Scale(cNev);

    fh_tof_id_pi0_pi0_minv->Scale(cNev);
    fh_tof_id_pi0_gamma_minv->Scale(cNev);
    fh_tof_id_gamma_gamma_minv->Scale(cNev);
    fh_tof_id_eta_gamma_minv->Scale(cNev);
    fh_tof_id_eta_eta_minv->Scale(cNev);
    fh_tof_id_eta_pi0_minv->Scale(cNev);

    fh_pi0cut_pi0_pi0_minv->Scale(cNev);
    fh_pi0cut_pi0_gamma_minv->Scale(cNev);
    fh_pi0cut_gamma_gamma_minv->Scale(cNev);
    fh_pi0cut_eta_gamma_minv->Scale(cNev);
    fh_pi0cut_eta_eta_minv->Scale(cNev);
    fh_pi0cut_eta_pi0_minv->Scale(cNev);


    fh_rich_tof_id_cut_mom->Scale(cNev);


    // --- Write histogramms ---
    fh_chi2_prim_all->Write();
    fh_chi2_prim_e->Write();
    fh_mc_all_zr->Write();

    fh_mc_all_lept_mom->Write();
    fh_mc_rec_lept_mom->Write();
    fh_rich_id_lept_mom->Write();
    fh_rich_id_ep_mom->Write();
    fh_rich_id_em_mom->Write();

    fh_rich_id_no_cut_mom->Write();
    fh_rich_id_mom_cut_mom->Write();
//    fh_rich_id_dist_cut_mom->Write();
    fh_rich_id_ann_cut_mom->Write();
//    fh_rich_id_xy_cut_mom->Write();
//    fh_rich_id_axisAB_cut_mom->Write();

    fh_mc_all_omega_mom->Write();
    fh_mc_rec_omega_mom->Write();
    fh_rich_id_omega_mom->Write();

    fh_mc_all_eta_mom->Write();
    fh_mc_rec_eta_mom->Write();
    fh_rich_id_eta_mom->Write();

    fh_mc_all_pi0_mom->Write();
    fh_mc_rec_pi0_mom->Write();
    fh_rich_id_pi0_mom->Write();

    fh_mc_all_gamma_mom->Write();
    fh_mc_rec_gamma_mom->Write();
    fh_rich_id_gamma_mom->Write();

    fh_mc_all_mom->Write();
    fh_mc_rec_mom->Write();
    fh_rich_id_mom->Write();

    fh_mc_all_theta->Write();
    fh_mc_rec_theta->Write();
    fh_rich_id_theta->Write();

    fh_mc_all_omega_theta->Write();
    fh_mc_rec_omega_theta->Write();
    fh_rich_id_omega_theta->Write();

    fh_mc_all_eta_theta->Write();
    fh_mc_rec_eta_theta->Write();
    fh_rich_id_eta_theta->Write();

    fh_mc_all_pi0_theta->Write();
    fh_mc_rec_pi0_theta->Write();
    fh_rich_id_pi0_theta->Write();

    fh_mc_all_gamma_theta->Write();
    fh_mc_rec_gamma_theta->Write();
    fh_rich_id_gamma_theta->Write();
/*
    fh_rich_ring_dist->Write();
    fh_rich_ring_dist_e->Write();
    fh_rich_dist_mom->Write();
    fh_rich_dist_mom_e->Write();
    fh_rich_dist_mom_cut->Write();
*/
    fh_rich_ring_axisA_mom_all->Write();
 //   fh_rich_ring_axisA_mom->Write();
 /*   fh_rich_ring_axisA->Write();
    fh_rich_ring_axisA_mom_e->Write();
    fh_rich_ring_axisA_e->Write();
    fh_rich_ring_axisB_mom_all->Write();
    fh_rich_ring_axisB_mom->Write();
    fh_rich_ring_axisB->Write();
    fh_rich_ring_axisB_mom_e->Write();
    fh_rich_ring_axisB_e->Write();
*/
//    fh_trd_eloss->Write();
//    fh_trd_eloss_e->Write();
//    fh_trd_ann->Write();
//    fh_trd_ann_e->Write();
//    fh_trd_ann_mom->Write();
//    fh_trd_ann_mom_e->Write();
    fh_tof_m2_mom->Write();
    fh_tof_m2_mom_zoom->Write();
    fh_tof_m2_mom_zoom_cut->Write();

    fh_dsts_bg->Write();
    fh_dsts_omega->Write();
    fh_dsts_pi0->Write();
    fh_dsts_gamma->Write();
    fh_dsts_mom_bg->Write();
    fh_dsts_mom_omega->Write();
    fh_dsts_mom_pi0->Write();
    fh_dsts_mom_gamma->Write();

    fh_dcut_theta_mom_omega->Write();
    fh_dcut_theta_mom_omega_true->Write();
    fh_dcut_theta_mom_pi0->Write();
    fh_dcut_theta_mom_gamma->Write();
    fh_dcut_theta_mom_gamma_true->Write();
    fh_dcut_theta_mom_bg->Write();
    fh_tpcut_pt_lept_omega->Write();
    fh_tpcut_pt_lept_bg->Write();

    fh_ptcut_theta_omega->Write();
    fh_ptcut_theta_omega_true->Write();
    fh_ptcut_theta_pi0->Write();
    fh_ptcut_theta_gamma->Write();
    fh_ptcut_theta_bg->Write();

    fh_mc_all_minv->Write();
    fh_mc_rec_minv->Write();
    fh_rich_id_minv->Write();
    fh_gcut_minv->Write();
    fh_dcut_minv->Write();
    fh_tpcut_minv->Write();
    fh_ptcut_minv->Write();
    fh_tcut_minv->Write();
    fh_pi0cut_minv->Write();

    fh_pi0cut_epep_minv->Write();
    fh_pi0cut_emem_minv->Write();

    fh_mc_all_pt_minv_omega->Write();
    fh_mc_all_pt_minv_pairs->Write();
    fh_mc_rec_pt_minv_omega->Write();
    fh_mc_rec_pt_minv_pairs->Write();
    fh_rich_id_pt_minv_omega->Write();
    fh_rich_id_pt_minv_pairs->Write();
    fh_tpcut_pt_minv_omega->Write();
    fh_tpcut_pt_minv_pairs->Write();
    fh_pi0cut_pt_minv_omega->Write();
    fh_pi0cut_pt_minv_pairs->Write();

    fh_rich_id_mc_minv->Write();
    fh_gcut_mc_minv->Write();
    fh_dcut_mc_minv->Write();
    fh_tpcut_mc_minv->Write();
    fh_ptcut_mc_minv->Write();
    fh_tcut_mc_minv->Write();
    fh_pi0cut_mc_minv->Write();
    fh_mc_all_omega_bg_minv->Write();
    fh_mc_rec_omega_bg_minv->Write();
    fh_rich_id_omega_bg_minv->Write();
    fh_gcut_omega_bg_minv->Write();
    fh_dcut_omega_bg_minv->Write();
    fh_tpcut_omega_bg_minv->Write();
    fh_ptcut_omega_bg_minv->Write();
    fh_tcut_omega_bg_minv->Write();
    fh_pi0cut_omega_bg_minv->Write();
    fh_rich_id_mc_omega_bg_minv->Write();
    fh_gcut_mc_omega_bg_minv->Write();
    fh_dcut_mc_omega_bg_minv->Write();
    fh_tpcut_mc_omega_bg_minv->Write();
    fh_ptcut_mc_omega_bg_minv->Write();
    fh_tcut_mc_omega_bg_minv->Write();
    fh_pi0cut_mc_omega_bg_minv->Write();

    fh_mc_all_omega_minv->Write();
    fh_mc_rec_omega_minv->Write();
    fh_rich_id_omega_minv->Write();
    fh_gcut_omega_minv->Write();
    fh_dcut_omega_minv->Write();
    fh_tpcut_omega_minv->Write();
    fh_ptcut_omega_minv->Write();
    fh_tcut_omega_minv->Write();
    fh_pi0cut_omega_minv->Write();

    fh_mc_all_eta_minv->Write();
    fh_mc_rec_eta_minv->Write();
    fh_rich_id_eta_minv->Write();
    fh_gcut_eta_minv->Write();
    fh_dcut_eta_minv->Write();
    fh_tpcut_eta_minv->Write();
    fh_ptcut_eta_minv->Write();
    fh_tcut_eta_minv->Write();
    fh_pi0cut_eta_minv->Write();

    fh_mc_all_pi0_minv->Write();
    fh_mc_rec_pi0_minv->Write();
    fh_rich_id_pi0_minv->Write();
    fh_gcut_pi0_minv->Write();
    fh_dcut_pi0_minv->Write();
    fh_tpcut_pi0_minv->Write();
    fh_ptcut_pi0_minv->Write();
    fh_tcut_pi0_minv->Write();
    fh_pi0cut_pi0_minv->Write();
    fh_pi0cut_fake_pi0_minv->Write();
    fh_pi0cut_true_pi0_minv->Write();

    fh_mc_all_gamma_minv->Write();
    fh_mc_rec_gamma_minv->Write();
    fh_rich_id_gamma_minv->Write();
    fh_gcut_gamma_minv->Write();
    fh_dcut_gamma_minv->Write();
    fh_tpcut_gamma_minv->Write();
    fh_ptcut_gamma_minv->Write();
    fh_tcut_gamma_minv->Write();
    fh_pi0cut_gamma_minv->Write();

    fh_mc_all_pty->Write();
    fh_mc_rec_pty->Write();
    fh_rich_id_pty->Write();
    fh_gcut_pty->Write();
    fh_dcut_pty->Write();
    fh_tpcut_pty->Write();
    fh_ptcut_pty->Write();
    fh_tcut_pty->Write();
    fh_pi0cut_pty->Write();

    fh_mc_all_omega_pty->Write();
    fh_mc_rec_omega_pty->Write();
    fh_rich_id_omega_pty->Write();
    fh_gcut_omega_pty->Write();
    fh_dcut_omega_pty->Write();
    fh_tpcut_omega_pty->Write();
    fh_ptcut_omega_pty->Write();
    fh_tcut_omega_pty->Write();
    fh_pi0cut_omega_pty->Write();

    fh_mc_all_eta_pty->Write();
    fh_mc_rec_eta_pty->Write();
    fh_rich_id_eta_pty->Write();
    fh_gcut_eta_pty->Write();
    fh_dcut_eta_pty->Write();
    fh_tpcut_eta_pty->Write();
    fh_ptcut_eta_pty->Write();
    fh_tcut_eta_pty->Write();
    fh_pi0cut_eta_pty->Write();

    fh_mc_all_pi0_pty->Write();
    fh_mc_rec_pi0_pty->Write();
    fh_rich_id_pi0_pty->Write();
    fh_gcut_pi0_pty->Write();
    fh_dcut_pi0_pty->Write();
    fh_tpcut_pi0_pty->Write();
    fh_ptcut_pi0_pty->Write();
    fh_tcut_pi0_pty->Write();
    fh_pi0cut_pi0_pty->Write();

    fh_mc_all_gamma_pty->Write();
    fh_mc_rec_gamma_pty->Write();
    fh_rich_id_gamma_pty->Write();
    fh_gcut_gamma_pty->Write();
    fh_dcut_gamma_pty->Write();
    fh_tpcut_gamma_pty->Write();
    fh_ptcut_gamma_pty->Write();
    fh_tcut_gamma_pty->Write();
    fh_pi0cut_gamma_pty->Write();

    fh_mom_rich->Write();
//    fh_mom_trd->Write();
    fh_mom_tof->Write();

    fh_mom_w_sts->Write();
    fh_mom_w_rich->Write();
 //   fh_mom_w_trd->Write();
    fh_mom_w_tof->Write();

    fh_mom_e_sts->Write();
    fh_mom_e_rich->Write();
//    fh_mom_e_trd->Write();
    fh_mom_e_tof->Write();

    fh_rich_id_pi0_pi0_minv->Write();
    fh_rich_id_pi0_gamma_minv->Write();
    fh_rich_id_gamma_gamma_minv->Write();
    fh_rich_id_eta_gamma_minv->Write();
    fh_rich_id_eta_eta_minv->Write();
    fh_rich_id_eta_pi0_minv->Write();

//    fh_trd_id_pi0_pi0_minv->Write();
//    fh_trd_id_pi0_gamma_minv->Write();
//    fh_trd_id_gamma_gamma_minv->Write();
//    fh_trd_id_eta_gamma_minv->Write();
//    fh_trd_id_eta_eta_minv->Write();
//    fh_trd_id_eta_pi0_minv->Write();

    fh_tof_id_pi0_pi0_minv->Write();
    fh_tof_id_pi0_gamma_minv->Write();
    fh_tof_id_gamma_gamma_minv->Write();
    fh_tof_id_eta_gamma_minv->Write();
    fh_tof_id_eta_eta_minv->Write();
    fh_tof_id_eta_pi0_minv->Write();

    fh_pi0cut_pi0_pi0_minv->Write();
    fh_pi0cut_pi0_gamma_minv->Write();
    fh_pi0cut_gamma_gamma_minv->Write();
    fh_pi0cut_eta_gamma_minv->Write();
    fh_pi0cut_eta_eta_minv->Write();
    fh_pi0cut_eta_pi0_minv->Write();

    fh_rich_tof_id_cut_mom->Write();
}
