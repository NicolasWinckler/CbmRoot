/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 Fit performance 
 *
 *====================================================================
 */
#include "CbmL1.h"

#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"

#include "CbmKF.h"
#include "CbmKFMath.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>

using std::cout;
using std::endl;
using std::ios;
using std::vector;
using std::pair;
using std::map;


static int L1_NMTRA_D0              = 0;
static int L1_NMTRA_TOTAL           = 0;
static int L1_NMTRA_FAST            = 0;
static int L1_NMTRA_FAST_PRIM       = 0;
static int L1_NMTRA_FAST_SEC        = 0;
static int L1_NMTRA_SLOW            = 0;
static int L1_NMTRA_SLOW_PRIM       = 0;
static int L1_NMTRA_SLOW_SEC        = 0;

static int L1_NMTRA_D0_RECO         = 0;
static int L1_NMTRA_TOTAL_RECO      = 0;
static int L1_NMTRA_FAST_RECO       = 0;
static int L1_NMTRA_FAST_PRIM_RECO  = 0;
static int L1_NMTRA_FAST_SEC_RECO   = 0;
static int L1_NMTRA_SLOW_RECO       = 0;
static int L1_NMTRA_SLOW_PRIM_RECO  = 0;
static int L1_NMTRA_SLOW_SEC_RECO   = 0;
static int L1_NTRACKS               = 0;
static int L1_NCLONES               = 0;
static int L1_NGHOSTS               = 0;

static int L1_NMTRA_D0_KILLED         = 0;
static int L1_NMTRA_TOTAL_KILLED      = 0;
static int L1_NMTRA_FAST_KILLED       = 0;
static int L1_NMTRA_FAST_PRIM_KILLED  = 0;
static int L1_NMTRA_FAST_SEC_KILLED   = 0;
static int L1_NMTRA_SLOW_KILLED       = 0;
static int L1_NMTRA_SLOW_PRIM_KILLED  = 0;
static int L1_NMTRA_SLOW_SEC_KILLED   = 0;

static int L1_NHITS[20], L1_NFAKES[20];

static int L1_NEVENTS               = 0;
static double L1_CATIME             = 0.0;


void CbmL1::Performance()
{
  typedef  map<CbmL1MCTrack*, CbmL1Track*>  Mtrartramap;
  typedef  map<CbmL1MCTrack*, CbmL1Track*>::iterator MtrartramapIt;
  Mtrartramap   mtrartramap, mtrartradistortmap;
  typedef  map<CbmL1Track*, CbmL1MCTrack*>           Rtramtramap;
  typedef  map<CbmL1Track*, CbmL1MCTrack*>::iterator RtramtramapIt;
  Rtramtramap   rtramtramap, rtramtradistortmap;

  //CbmKF &KF = *CbmKF::Instance();
  // histogramms 

  static TProfile *p_eff_all_vs_mom, *p_eff_prim_vs_mom, *p_eff_sec_vs_mom, *p_eff_d0_vs_mom; 
  
  static TH1F *h_reg_mom_prim, *h_reg_mom_sec, *h_reg_nhits_prim, *h_reg_nhits_sec;
  static TH1F *h_acc_mom_prim, *h_acc_mom_sec, *h_acc_nhits_prim, *h_acc_nhits_sec;
  static TH1F *h_reco_mom_prim, *h_reco_mom_sec, *h_reco_nhits_prim, *h_reco_nhits_sec;
  static TH1F *h_rest_mom_prim, *h_rest_mom_sec, *h_rest_nhits_prim, *h_rest_nhits_sec;

  //static TH1F *h_hit_density[10];

  static TH1F 
    *h_ghost_mom, *h_ghost_nhits, *h_ghost_fstation, 
    *h_ghost_chi2, *h_ghost_prob, *h_ghost_tx, *h_ghost_ty;
  static TH1F *h_reco_mom,  *h_reco_d0_mom, *h_reco_nhits, *h_reco_station, 
    *h_reco_chi2, *h_reco_prob, *h_reco_clean, *h_reco_time;
  static TProfile *h_reco_timeNtr, *h_reco_timeNhit ;
  static TProfile *h_reco_fakeNtr, *h_reco_fakeNhit ;
  static TH1F *h_tx, *h_ty, *h_sec_r, *h_ghost_r;

  static TH1F *h_notfound_mom, *h_notfound_nhits, *h_notfound_station, *h_notfound_r, *h_notfound_tx, *h_notfound_ty;
  
  static TH1F *h_mcp, *h_nmchits, *h_chi2, *h_prob, *MC_vx, *MC_vy, *MC_vz, *VtxChiPrim, *VtxChiSec;

  static TH2F *P_vs_P ;

  static TH2F *h2_vertex, *h2_prim_vertex, *h2_sec_vertex;
  //static TH3F *h3_vertex, *h3_prim_vertex, *h3_sec_vertex;

  static TH2F *h2_reg_nhits_vs_mom_prim, *h2_reg_nhits_vs_mom_sec, 
    *h2_reg_fstation_vs_mom_prim, *h2_reg_fstation_vs_mom_sec, *h2_reg_lstation_vs_fstation_prim, *h2_reg_lstation_vs_fstation_sec;
  static TH2F *h2_acc_nhits_vs_mom_prim, *h2_acc_nhits_vs_mom_sec, 
    *h2_acc_fstation_vs_mom_prim, *h2_acc_fstation_vs_mom_sec, *h2_acc_lstation_vs_fstation_prim, *h2_acc_lstation_vs_fstation_sec;
  static TH2F *h2_reco_nhits_vs_mom_prim, *h2_reco_nhits_vs_mom_sec, 
    *h2_reco_fstation_vs_mom_prim, *h2_reco_fstation_vs_mom_sec, *h2_reco_lstation_vs_fstation_prim, *h2_reco_lstation_vs_fstation_sec;
  static TH2F *h2_ghost_nhits_vs_mom, *h2_ghost_fstation_vs_mom, *h2_ghost_lstation_vs_fstation;
  static TH2F *h2_rest_nhits_vs_mom_prim, *h2_rest_nhits_vs_mom_sec, 
    *h2_rest_fstation_vs_mom_prim, *h2_rest_fstation_vs_mom_sec, *h2_rest_lstation_vs_fstation_prim, *h2_rest_lstation_vs_fstation_sec;

  static TH1F *h_fit[10], *h_fitL[10], *h_fit_chi2;

  static bool first_call = 1;

  if ( first_call )
    {
      first_call = 0;

      //TDirectory *maindir = gDirectory;
      //histodir->cd();

      TDirectory *curdir = gDirectory;
      //histodir = gROOT->mkdir("L1");
      histodir->cd();
      histodir->mkdir("Fit");
      gDirectory->cd("Fit");
      {
	struct {    
	  char *name;
	  char *title;
	  Int_t n;
	  Double_t l,r;
	} Table[10]= 
	  { 
	    {"x",  "Residual X [#mum]",                   100, -100., 100.},
	    {"y",  "Residual Y [#mum]",                   100, -100., 100.},
	    {"tx", "Residual Tx [mrad]",                  100,   -2.,   2.},
	    {"ty", "Residual Ty [mrad]",                  100,   -2.,   2.},
	    {"P",  "Resolution P/Q [100%]",               100,   -.1,  .1 },
	    {"px", "Pull X [residual/estimated_error]",   100,  -10.,  10.},
	    {"py", "Pull Y [residual/estimated_error]",   100,  -10.,  10.},
	    {"ptx","Pull Tx [residual/estimated_error]",  100,  -10.,  10.},
	    {"pty","Pull Ty [residual/estimated_error]",  100,  -10.,  10.},
	    {"pQP","Pull Q/P [residual/estimated_error]", 100,  -10.,  10.}
	  };
	
	for( int i=0; i<10; i++ ){
	  char n[225], t[255];
	  sprintf(n,"fst_%s",Table[i].name);
	  sprintf(t,"First point %s",Table[i].title);
	  h_fit[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
	  sprintf(n,"lst_%s",Table[i].name);
	  sprintf(t,"Last point %s",Table[i].title);
	  h_fitL[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
	}
	h_fit_chi2 = new TH1F("h_fit_chi2", "Chi2/NDF", 50, -0.5, 10.0);
      }
      gDirectory->cd("..");

      p_eff_all_vs_mom = new TProfile("p_eff_all_vs_mom", "AllSet Efficiency vs Momentum",
				     100, 0.0, 5.0, 0.0, 100.0);
      p_eff_prim_vs_mom = new TProfile("p_eff_prim_vs_mom", "All Primary Set Efficiency vs Momentum",
				     100, 0.0, 5.0, 0.0, 100.0);
      p_eff_sec_vs_mom = new TProfile("p_eff_sec_vs_mom", "All Secondary Set Efficiency vs Momentum",
				     100, 0.0, 5.0, 0.0, 100.0);
      p_eff_d0_vs_mom = new TProfile("p_eff_d0_vs_mom", "D0 Secondary Tracks Efficiency vs Momentum",
				     150, 0.0, 15.0, 0.0, 100.0);

      h_reg_mom_prim   = new TH1F("h_reg_mom_prim", "Momentum of registered primary tracks", 500, 0.0, 5.0);
      h_reg_mom_sec   = new TH1F("h_reg_mom_sec", "Momentum of registered secondary tracks", 500, 0.0, 5.0);
      h_acc_mom_prim   = new TH1F("h_acc_mom_prim", "Momentum of accepted primary tracks", 500, 0.0, 5.0);
      h_acc_mom_sec   = new TH1F("h_acc_mom_sec", "Momentum of accepted secondary tracks", 500, 0.0, 5.0);
      h_reco_mom_prim   = new TH1F("h_reco_mom_prim", "Momentum of reconstructed primary tracks", 500, 0.0, 5.0);
      h_reco_mom_sec   = new TH1F("h_reco_mom_sec", "Momentum of reconstructed secondary tracks", 500, 0.0, 5.0);
      h_rest_mom_prim   = new TH1F("h_rest_mom_prim", "Momentum of not found primary tracks", 500, 0.0, 5.0);
      h_rest_mom_sec   = new TH1F("h_rest_mom_sec", "Momentum of not found secondary tracks", 500, 0.0, 5.0);

      h_reg_nhits_prim = new TH1F("h_reg_nhits_prim", "Number of hits in registered primary track", 51, -0.1, 10.1);
      h_reg_nhits_sec = new TH1F("h_reg_nhits_sec", "Number of hits in registered secondary track", 51, -0.1, 10.1);
      h_acc_nhits_prim = new TH1F("h_acc_nhits_prim", "Number of hits in accepted primary track", 51, -0.1, 10.1);
      h_acc_nhits_sec = new TH1F("h_acc_nhits_sec", "Number of hits in accepted secondary track", 51, -0.1, 10.1);
      h_reco_nhits_prim = new TH1F("h_reco_nhits_prim", "Number of hits in reconstructed primary track", 51, -0.1, 10.1);
      h_reco_nhits_sec = new TH1F("h_reco_nhits_sec", "Number of hits in reconstructed secondary track", 51, -0.1, 10.1);
      h_rest_nhits_prim = new TH1F("h_rest_nhits_prim", "Number of hits in not found primary track", 51, -0.1, 10.1);
      h_rest_nhits_sec = new TH1F("h_rest_nhits_sec", "Number of hits in not found secondary track", 51, -0.1, 10.1);

      h_ghost_mom = new TH1F("h_ghost_mom", "Momentum of ghost track", 500, 0.0, 5.0);
      h_ghost_nhits = new TH1F("h_ghost_nhits", "Number of hits in ghost track", 51, -0.1, 10.1);
      h_ghost_fstation = new TH1F("h_ghost_fstation", "First station of ghost track", 50, -0.5, 10.0);
      h_ghost_chi2 = new TH1F("h_ghost_chi2", "Chi2/NDF of ghost track", 50, -0.5, 10.0);
      h_ghost_prob = new TH1F("h_ghost_prob", "Prob of ghost track", 50, 0., 1.01);
      h_ghost_r = new TH1F("h_ghost_r", "R of ghost track at the first hit", 50, 0.0, 15.0);
      h_ghost_tx = new TH1F("h_ghost_tx", "tx of ghost track at the first hit", 50, -5.0, 5.0);
      h_ghost_ty = new TH1F("h_ghost_ty", "ty of ghost track at the first hit", 50, -1.0, 1.0);

      h_reco_mom = new TH1F("h_reco_mom", "Momentum of reco track", 50, 0.0, 5.0);
      h_reco_nhits = new TH1F("h_reco_nhits", "Number of hits in reco track", 50, 0.0, 10.0);
      h_reco_station = new TH1F("h_reco_station", "First station of reco track", 50, -0.5, 10.0);
      h_reco_chi2 = new TH1F("h_reco_chi2", "Chi2/NDF of reco track", 50, -0.5, 10.0);
      h_reco_prob = new TH1F("h_reco_prob", "Prob of reco track", 50, 0., 1.01);
      h_reco_clean = new TH1F("h_reco_clean", "Percentage of correct hits", 100, -0.5, 100.5);
      h_reco_time = new TH1F("h_reco_time", "CA Track Finder Time (s/ev)", 20, 0.0, 20.0);
      h_reco_timeNtr = new TProfile("h_reco_timeNtr", 
				    "CA Track Finder Time (s/ev) vs N Tracks", 
				    200, 0.0, 1000.0);
      h_reco_timeNhit = new TProfile("h_reco_timeNhit", 
				    "CA Track Finder Time (s/ev) vs N Hits", 
				    200, 0.0, 30000.0);
      h_reco_fakeNtr = new TProfile("h_reco_fakeNtr", 
				    "N Fake hits vs N Tracks", 
				    200, 0.0, 1000.0);
      h_reco_fakeNhit = new TProfile("h_reco_fakeNhit", 
				    "N Fake hits vs N Hits", 
				    200, 0.0, 30000.0);

      h_reco_d0_mom = new TH1F("h_reco_d0_mom", "Momentum of reco D0 track", 150, 0.0, 15.0);
      /*
      h_hit_density[0] = new TH1F("h_hit_density0", "Hit density station 1", 50, 0.0,  5.00);
      h_hit_density[1] = new TH1F("h_hit_density1", "Hit density station 2", 100, 0.0, 10.00);
      h_hit_density[2] = new TH1F("h_hit_density2", "Hit density station 3", 140, 0.0, 13.99);
      h_hit_density[3] = new TH1F("h_hit_density3", "Hit density station 4", 180, 0.0, 18.65);
      h_hit_density[4] = new TH1F("h_hit_density4", "Hit density station 5", 230, 0.0, 23.32);
      h_hit_density[5] = new TH1F("h_hit_density5", "Hit density station 6", 270, 0.0, 27.98);
      h_hit_density[6] = new TH1F("h_hit_density6", "Hit density station 7", 340, 0.0, 34.97);
      h_hit_density[7] = new TH1F("h_hit_density7", "Hit density station 8", 460, 0.0, 46.63);
      h_hit_density[8] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
      h_hit_density[9] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
      h_hit_density[10] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
      */
      h_tx = new TH1F("h_tx", "tx of MC track at the vertex", 50, -0.5, 0.5);
      h_ty = new TH1F("h_ty", "ty of MC track at the vertex", 50, -0.5, 0.5);

      h_sec_r = new TH1F("h_sec_r", "R of sec MC track at the first hit", 50, 0.0, 15.0);

      h_notfound_mom = new TH1F("h_notfound_mom", "Momentum of not found track", 50, 0.0, 5.0);
      h_notfound_nhits = new TH1F("h_notfound_nhits", "Num hits of not found track", 50, 0.0, 10.0);
      h_notfound_station = new TH1F("h_notfound_station", "First station of not found track", 50, 0.0, 10.0);
      h_notfound_r = new TH1F("h_notfound_r", "R at first hit of not found track", 50, 0.0, 15.0);
      h_notfound_tx = new TH1F("h_notfound_tx", "tx of not found track at the first hit", 50, -5.0, 5.0);
      h_notfound_ty = new TH1F("h_notfound_ty", "ty of not found track at the first hit", 50, -1.0, 1.0);

      h_chi2 = new TH1F("chi2", "Chi^2", 100, 0.0, 10.);
      h_prob = new TH1F("prob", "Prob", 100, 0.0, 1.01);
      VtxChiPrim = new TH1F("vtxChiPrim", "Chi to primary vtx for primary tracks", 100, 0.0, 10.);
      VtxChiSec = new TH1F("vtxChiSec", "Chi to primary vtx for secondary tracks", 100, 0.0, 10.);
      h_mcp = new TH1F("h_mcp", "MC P ", 500, 0.0, 5.0);
      MC_vx = new TH1F("MC_vx", "MC Vertex X", 100, -.05, .05);
      MC_vy = new TH1F("MC_vy", "MC Vertex Y", 100, -.05, .05);
      MC_vz = new TH1F("MC_vz", "MC Vertex Z", 100, -.05, .05);
      h_nmchits = new TH1F("h_nmchits", "N STS hits on MC track", 50, 0.0, 10.0);
 
      P_vs_P = new TH2F("P_vs_P", "Resolution P/Q vs P", 20, 0., 20.,100, -.05, .05);
      
      h2_vertex = new TH2F("h2_vertex", "2D vertex distribution", 105, -5., 100., 100, -50., 50.);
      h2_prim_vertex = new TH2F("h2_primvertex", "2D primary vertex distribution", 105, -5., 100., 100, -50., 50.);
      h2_sec_vertex = new TH2F("h2_sec_vertex", "2D secondary vertex distribution", 105, -5., 100., 100, -50., 50.);

      //h3_vertex = new TH3F("h3_vertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);
      //h3_prim_vertex = new TH3F("h3_primvertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);
      //h3_sec_vertex = new TH3F("h3_sec_vertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);

      h2_reg_nhits_vs_mom_prim = new TH2F("h2_reg_nhits_vs_mom_prim", "NHits vs. Momentum (Reg. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_reg_nhits_vs_mom_sec = new TH2F("h2_reg_nhits_vs_mom_sec", "NHits vs. Momentum (Reg. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_acc_nhits_vs_mom_prim = new TH2F("h2_acc_nhits_vs_mom_prim", "NHits vs. Momentum (Acc. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_acc_nhits_vs_mom_sec = new TH2F("h2_acc_nhits_vs_mom_sec", "NHits vs. Momentum (Acc. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_reco_nhits_vs_mom_prim = new TH2F("h2_reco_nhits_vs_mom_prim", "NHits vs. Momentum (Reco Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_reco_nhits_vs_mom_sec = new TH2F("h2_reco_nhits_vs_mom_sec", "NHits vs. Momentum (Reco Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_ghost_nhits_vs_mom = new TH2F("h2_ghost_nhits_vs_mom", "NHits vs. Momentum (Ghost Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_rest_nhits_vs_mom_prim = new TH2F("h2_rest_nhits_vs_mom_prim", "NHits vs. Momentum (!Found Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_rest_nhits_vs_mom_sec = new TH2F("h2_rest_nhits_vs_mom_sec", "NHits vs. Momentum (!Found Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);

      h2_reg_fstation_vs_mom_prim = new TH2F("h2_reg_fstation_vs_mom_prim", "First Station vs. Momentum (Reg. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_reg_fstation_vs_mom_sec = new TH2F("h2_reg_fstation_vs_mom_sec", "First Station vs. Momentum (Reg. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_acc_fstation_vs_mom_prim = new TH2F("h2_acc_fstation_vs_mom_prim", "First Station vs. Momentum (Acc. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_acc_fstation_vs_mom_sec = new TH2F("h2_acc_fstation_vs_mom_sec", "First Station vs. Momentum (Acc. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_reco_fstation_vs_mom_prim = new TH2F("h2_reco_fstation_vs_mom_prim", "First Station vs. Momentum (Reco Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_reco_fstation_vs_mom_sec = new TH2F("h2_reco_fstation_vs_mom_sec", "First Station vs. Momentum (Reco Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_ghost_fstation_vs_mom = new TH2F("h2_ghost_fstation_vs_mom", "First Station vs. Momentum (Ghost Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_rest_fstation_vs_mom_prim = new TH2F("h2_rest_fstation_vs_mom_prim", "First Station vs. Momentum (!Found Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
      h2_rest_fstation_vs_mom_sec = new TH2F("h2_rest_fstation_vs_mom_sec", "First Station vs. Momentum (!Found Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);

      h2_reg_lstation_vs_fstation_prim = new TH2F("h2_reg_lstation_vs_fstation_prim", "Last vs. First Station (Reg. Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_reg_lstation_vs_fstation_sec = new TH2F("h2_reg_lstation_vs_fstation_sec", "Last vs. First Station (Reg. Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_acc_lstation_vs_fstation_prim = new TH2F("h2_acc_lstation_vs_fstation_prim", "Last vs. First Station (Acc. Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_acc_lstation_vs_fstation_sec = new TH2F("h2_acc_lstation_vs_fstation_sec", "Last vs. First Station (Acc. Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_reco_lstation_vs_fstation_prim = new TH2F("h2_reco_lstation_vs_fstation_prim", "Last vs. First Station (Reco Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_reco_lstation_vs_fstation_sec = new TH2F("h2_reco_lstation_vs_fstation_sec", "Last vs. First Station (Reco Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_ghost_lstation_vs_fstation = new TH2F("h2_ghost_lstation_vs_fstation", "Last vs. First Station (Ghost Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_rest_lstation_vs_fstation_prim = new TH2F("h2_rest_lstation_vs_fstation_prim", "Last vs. First Station (!Found Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
      h2_rest_lstation_vs_fstation_sec = new TH2F("h2_rest_lstation_vs_fstation_sec", "Last vs. First Station (!Found Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);

      //maindir->cd(); 

      // ----- Create list of all histogram pointers
      
      curdir->cd();      
      
      for (int i = 0; i < algo->NStations; i++){
	L1_NHITS[i]  = 0;
	L1_NFAKES[i] = 0;
      }

    }// first_call 

  
  //track finding part 

  typedef  map<int, int >           Hitmap;   // <trackID, nhits>
  typedef  map<int, int >::iterator HitmapIt;
  Hitmap   hitmap;
    
  map <int, int> recostamap, mcstamap;

  map <CbmL1MCTrack*, int> pmtramap; // reco track + clones  
  map <CbmL1Track*, int> rtradistortmap; // reco track + clones  

  map <int, CbmL1MCTrack*> pmctrackmap; pmctrackmap.clear();


  //track finding performance

  pmtramap.clear();
  rtradistortmap.clear();
    
  mtrartramap.clear();
  rtramtramap.clear();
    
  mtrartradistortmap.clear();
  rtramtradistortmap.clear();
    
  bool vHitUsed[vHitStore.size()];
  for( unsigned int ih=0; ih<vHitStore.size(); ih++) vHitUsed[ih]=0;

  int ntracks = 0;
  int nghosts = 0;
  //cout<<"reco.."<<endl;
  for( vector<CbmL1MCTrack>::iterator i = vMCTracks.begin(); i != vMCTracks.end(); ++i){
    CbmL1MCTrack &MC = *i;
    
    if (pmctrackmap.find(MC.ID) == pmctrackmap.end()){
      pmctrackmap.insert(pair<int, CbmL1MCTrack*>(MC.ID, &MC));
    }
    else{
      cout << "*** L1: Track ID " << MC.ID << " encountered twice! ***" << endl;
    }
  }
  
  //hit density calculation: h_hit_density[10]
  /*
  for (vector<CbmL1HitStore>::iterator hIt = vHitStore.begin(); hIt != vHitStore.end(); ++hIt){
    float x = hIt->x;
    float y = hIt->y;
    float r = sqrt(x*x+y*y);    
    h_hit_density[hIt->iStation]->Fill(r, 1.0/(2.0*3.1415*r));
  }
  */
  //reconstructed tracks
  for (vector<CbmL1Track>::iterator rtraIt = vRTracks.begin(); rtraIt != vRTracks.end(); ++rtraIt){
      
    ntracks++;
    
    int hitsum  = 0;
    hitmap.clear();
    
    for (vector<int>::iterator ih = (rtraIt->StsHits).begin();
	 ih != (rtraIt->StsHits).end(); ++ih){
      //L1StsHit &h = algo->vStsHits[*ih];
      vHitUsed[*ih]=1;
      int ID = -1;
      int iMC = vHitMCRef[*ih];
      if (iMC >= 0)
	ID = vMCPoints[iMC].ID;
      if(hitmap.find(ID) == hitmap.end())
	hitmap.insert(pair<int, int>(ID, 1));
      else{
	hitmap[ID] += 1;
      }
      hitsum++;
    }

    {      
      CbmL1Track* prtra = &(*rtraIt);
      if( fabs(prtra->T[4])>1.e-10 ) h_reco_mom->Fill(fabs(1.0/prtra->T[4]));
      h_reco_nhits->Fill((prtra->StsHits).size());
      CbmL1HitStore &mh = vHitStore[prtra->StsHits[0]];
      h_reco_station->Fill(mh.iStation);
      if (prtra->NDF > 0){
	h_reco_chi2->Fill(prtra->chi2/prtra->NDF);
	h_reco_prob->Fill(TMath::Erf(sqrt(fabs(prtra->chi2/prtra->NDF))));
      }
    }
    
    // RTRA <-> MTRA identification 
    int ghost = 1;
    double momentum = 0.0;
    double max_percent = 0.0;
    for( HitmapIt posIt = hitmap.begin(); posIt != hitmap.end(); posIt++ ){ // for Hitmap
      
      if (posIt->first < 0) continue; //not a MC track - based on fake hits

      if (100.0*double(posIt->second) > max_percent*double(hitsum)) 
	max_percent = 100.0*double(posIt->second)/double(hitsum);
      
      if ( double(posIt->second) > 0.7*double(hitsum) ){ //MTRA reconstructed
	
	if (pmctrackmap.find(posIt->first) == pmctrackmap.end()) continue;
	CbmL1MCTrack* pmtra = pmctrackmap[posIt->first]; //pointer to MC track
	CbmL1Track* prtra = &(*rtraIt);
	
	mtrartramap.insert(pair<CbmL1MCTrack*, CbmL1Track*>(pmtra, prtra));
	rtramtramap.insert(pair<CbmL1Track*, CbmL1MCTrack*>(prtra, pmtra));
	
	momentum = pmtra->p;
	  
	if(pmtramap.find(pmtra) == pmtramap.end())
	  pmtramap.insert(pair<CbmL1MCTrack*, int>(pmtra, 1));
	else{
	  pmtramap[pmtra]++;
	}
	
	ghost = 0;

      }
      else{ // MTRA destorted
	if (pmctrackmap.find(posIt->first) == pmctrackmap.end()) continue;
	CbmL1MCTrack* pmtra = pmctrackmap[posIt->first]; //pointer to MC track
	CbmL1Track* prtra = &(*rtraIt);
	
	mtrartradistortmap.insert(pair<CbmL1MCTrack*, CbmL1Track*>(pmtra, prtra));
	rtramtradistortmap.insert(pair<CbmL1Track*, CbmL1MCTrack*>(prtra, pmtra));
	
	if(rtradistortmap.find(prtra) == rtradistortmap.end())
	  rtradistortmap.insert(pair<CbmL1Track*, int>(prtra, 1));
	else{
	  rtradistortmap[prtra]++;
	}
      }
    }

    h_reco_clean->Fill(max_percent);
    
    nghosts += ghost;
    
    if (ghost == 1){ //ghost
      CbmL1Track* prtra = &(*rtraIt);
      //cout << "Ghost T[4] " << prtra->T[4] << endl;
      if( fabs(prtra->T[4])>1.e-10) 
	h_ghost_mom->Fill(fabs(1.0/prtra->T[4]));
      h_ghost_nhits->Fill((prtra->StsHits).size());
      CbmL1HitStore &h1 = vHitStore[prtra->StsHits[0]];
      CbmL1HitStore &h2 = vHitStore[prtra->StsHits[1]];
      h_ghost_fstation->Fill(h1.iStation);
      if (prtra->NDF > 0){
	h_ghost_chi2->Fill(prtra->chi2/prtra->NDF);
	h_ghost_prob->Fill(TMath::Erf(sqrt(fabs(prtra->chi2/prtra->NDF))));
      }
      h_ghost_r->Fill(sqrt(fabs(h1.x*h1.x+h1.y*h1.y)));
      double z1 = algo->vStations[h1.iStation].z[0];
      double z2 = algo->vStations[h2.iStation].z[0];
      if( fabs(z2-z1)>1.e-4 ){
	h_ghost_tx->Fill((h2.x-h1.x)/(z2-z1));
	h_ghost_ty->Fill((h2.y-h1.y)/(z2-z1));
      }

      if( fabs(prtra->T[4])>1.e-10) 
	h2_ghost_nhits_vs_mom->Fill(fabs(1.0/prtra->T[4]), (prtra->StsHits).size());
      CbmL1HitStore &hf = vHitStore[prtra->StsHits[0]];
      CbmL1HitStore &hl = vHitStore[prtra->StsHits[(prtra->StsHits).size()-1]];
      if( fabs(prtra->T[4])>1.e-10) 
	h2_ghost_fstation_vs_mom->Fill(fabs(1.0/prtra->T[4]), hf.iStation+1);
      if (hl.iStation >= hf.iStation)
	h2_ghost_lstation_vs_fstation->Fill(hf.iStation+1, hl.iStation+1);

    }
    
  }//reco tracks

  // clone 
  /*
  for (vector<CbmL1Track>::iterator rtraIt = vRTracks.begin(); rtraIt != vRTracks.end(); ++rtraIt){
         	
    CbmL1Track* prtra = &(*rtraIt);
    
    if( rtramtramap.find(prtra) == rtramtramap.end() ) continue;
    CbmL1MCTrack *pmtra = rtramtramap[prtra];
    
    if(pmtramap.find(pmtra) == pmtramap.end()) continue;
    if( pmtramap[pmtra]<=1 ) continue;
    CbmL1HitStore &fh = vHitStore[prtra->StsHits[0]];
    CbmL1HitStore &lh = vHitStore[prtra->StsHits[prtra->StsHits.size()-1]];
    cout<<"clone "<<pmtra->ID<<" "<<fh.iStation<<" "<<lh.iStation<<endl;	
  }
  */


  int nmtra_d0         = 0;
  int nmtra_total      = 0;
  int nmtra_fast       = 0;
  int nmtra_fast_prim  = 0;
  int nmtra_fast_sec   = 0;
  int nmtra_slow       = 0;
  int nmtra_slow_prim  = 0;
  int nmtra_slow_sec   = 0;
  
  int nmtra_d0_reco    = 0;
  int nmtra_total_reco = 0;
  int nmtra_fast_reco  = 0;
  int nmtra_fast_prim_reco  = 0;
  int nmtra_fast_sec_reco   = 0;
  int nmtra_slow_reco  = 0;
  int nmtra_slow_prim_reco  = 0;
  int nmtra_slow_sec_reco   = 0;
  int nclones          = 0;

  int nmtra_d0_killed    = 0;
  int nmtra_total_killed = 0;
  int nmtra_fast_killed  = 0;
  int nmtra_fast_prim_killed  = 0;
  int nmtra_fast_sec_killed   = 0;
  int nmtra_slow_killed  = 0;
  int nmtra_slow_prim_killed  = 0;
  int nmtra_slow_sec_killed   = 0;



  int sta_nhits[algo->NStations], sta_nfakes[algo->NStations];

  for (int i = 0; i < algo->NStations; i++){
    sta_nhits[i]  = 0;
    sta_nfakes[i] = 0;
  }

  for ( map<int, CbmL1MCTrack*>::iterator mtraIt = pmctrackmap.begin();
	mtraIt != pmctrackmap.end(); mtraIt++ ) {
      
    // No Sts hits? 
    int nmchits = (mtraIt->second)->StsHits.size();
    //if (nmchits == 0) continue;
  
    double momentum = (mtraIt->second)->p;

    h_mcp->Fill(momentum);
    h_nmchits->Fill(nmchits);

    if (nmchits < 2) continue;// allset  3 or 4 or 5 stations ?
 
   int sta_count = 0;
    int ista = -1;
    for( vector<int>::iterator ih = (mtraIt->second)->StsHits.begin();
	 ih!= (mtraIt->second)->StsHits.end(); ++ih){
      CbmL1HitStore &mh = vHitStore[*ih];
      if (mh.iStation <= ista ) continue;
      sta_count++;
    }

    CbmL1HitStore &fh = vHitStore[*((mtraIt->second)->StsHits.begin())];
    CbmL1HitStore &lh = vHitStore[*((mtraIt->second)->StsHits.rbegin())];

    if ((mtraIt->second)->mother_ID < 0){ // primary
      h_reg_mom_prim->Fill(momentum);
      h_reg_nhits_prim->Fill(sta_count);
      h2_reg_nhits_vs_mom_prim->Fill(momentum, sta_count);
      h2_reg_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
      if (lh.iStation >= fh.iStation)
	h2_reg_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
    }else{
      h_reg_mom_sec->Fill(momentum);
      h_reg_nhits_sec->Fill(sta_count);
      if (momentum > 0.01){
	h2_reg_nhits_vs_mom_sec->Fill(momentum, sta_count);
	h2_reg_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
	if (lh.iStation >= fh.iStation)
	  h2_reg_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
      }
    }
      

    //cout << " => " << sta_count << endl;
    //if (sta_count < 4) continue;//detected in less than 4 stations
    if( mtraIt->second->nMCContStations <4 ) continue;

       
   if (momentum < 0.1) continue;// reject very slow tracks from analysis
      

    if ((mtraIt->second)->mother_ID < 0){ // primary
      h_acc_mom_prim->Fill(momentum);
      h_acc_nhits_prim->Fill(sta_count);
      h2_acc_nhits_vs_mom_prim->Fill(momentum, sta_count);
      h2_acc_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
      if (lh.iStation >= fh.iStation)
	h2_acc_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
    }else{
      h_acc_mom_sec->Fill(momentum);
      h_acc_nhits_sec->Fill(sta_count);
      if (momentum > 0.01){
	h2_acc_nhits_vs_mom_sec->Fill(momentum, sta_count);
	h2_acc_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
	if (lh.iStation >= fh.iStation)
	  h2_acc_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
      }
    }
      
      

    // vertex distribution of primary and secondary tracks   

    h2_vertex->Fill((mtraIt->second)->z, (mtraIt->second)->y);
    //h3_vertex->Fill((mtraIt->second)->z, (mtraIt->second)->x, (mtraIt->second)->y);
    if ((mtraIt->second)->mother_ID < 0){ // primary
      h2_prim_vertex->Fill((mtraIt->second)->z, (mtraIt->second)->y);
      //h3_prim_vertex->Fill((mtraIt->second)->z, (mtraIt->second)->x, (mtraIt->second)->y);
    }else{
      h2_sec_vertex->Fill((mtraIt->second)->z, (mtraIt->second)->y);
      //h3_sec_vertex->Fill((mtraIt->second)->z, (mtraIt->second)->x, (mtraIt->second)->y);
    }


    //continuous stations
    int iph = (mtraIt->second)->StsHits[0];
    CbmL1HitStore &ph = vHitStore[iph];
    
    h_sec_r->Fill(sqrt(fabs(ph.x*ph.x+ph.y*ph.y)));
    if( fabs( (mtraIt->second)->pz )>1.e-8 ){
      h_tx->Fill((mtraIt->second)->px/(mtraIt->second)->pz);
      h_ty->Fill((mtraIt->second)->py/(mtraIt->second)->pz);
    }
      

    bool reco = (pmtramap.find(mtraIt->second) != pmtramap.end());
       
    if (reco){
      p_eff_all_vs_mom->Fill(momentum, 100.0);
      if ((mtraIt->second)->mother_ID < 0){ // primary
	p_eff_prim_vs_mom->Fill(momentum, 100.0);
      }else{
	p_eff_sec_vs_mom->Fill(momentum, 100.0);
      }
      if ((mtraIt->second)->mother_ID < 0){ // primary
	h_reco_mom_prim->Fill(momentum);
	h_reco_nhits_prim->Fill(sta_count);
	h2_reco_nhits_vs_mom_prim->Fill(momentum, sta_count);
	h2_reco_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
	if (lh.iStation >= fh.iStation)
	  h2_reco_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
      }else{
	h_reco_mom_sec->Fill(momentum);
	h_reco_nhits_sec->Fill(sta_count);
	if (momentum > 0.01){
	  h2_reco_nhits_vs_mom_sec->Fill(momentum, sta_count);
	  h2_reco_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
	  if (lh.iStation >= fh.iStation)
	    h2_reco_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
	}
      }
    }else{
      h_notfound_mom->Fill(momentum);
      p_eff_all_vs_mom->Fill(momentum, 0.0);
      if ((mtraIt->second)->mother_ID < 0){ // primary
	p_eff_prim_vs_mom->Fill(momentum, 0.0);
      }else{
	p_eff_sec_vs_mom->Fill(momentum, 0.0);
      }
      if ((mtraIt->second)->mother_ID < 0){ // primary
	h_rest_mom_prim->Fill(momentum);
	h_rest_nhits_prim->Fill(sta_count);
	h2_rest_nhits_vs_mom_prim->Fill(momentum, sta_count);
	h2_rest_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
	if (lh.iStation >= fh.iStation)
	  h2_rest_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
      }else{
	h_rest_mom_sec->Fill(momentum);
	h_rest_nhits_sec->Fill(sta_count);
	if (momentum > 0.01){
	  h2_rest_nhits_vs_mom_sec->Fill(momentum, sta_count);
	  h2_rest_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
	  if (lh.iStation >= fh.iStation)
	    h2_rest_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
	}
      }
    }

      
    if (pmtramap.find(mtraIt->second) != pmtramap.end()){
      nclones += pmtramap[mtraIt->second]-1;
    }
 
    bool killed = 0;
    if(!reco){
      h_notfound_nhits->Fill(nmchits);
      h_notfound_station->Fill(ph.iStation);
      h_notfound_r->Fill(sqrt(fabs(ph.x*ph.x+ph.y*ph.y)));
      CbmL1HitStore &ph21 = vHitStore[mtraIt->second->StsHits[0]];
      CbmL1HitStore &ph22 = vHitStore[mtraIt->second->StsHits[1]];
      
      double z21 = algo->vStations[ph21.iStation].z[0];
      double z22 = algo->vStations[ph22.iStation].z[0];
      if( fabs(z22-z21)>1.e-4 ){
	h_notfound_tx->Fill((ph22.x-ph21.x)/(z22-z21));
	h_notfound_ty->Fill((ph22.y-ph21.y)/(z22-z21));
      }

      int nusedhits = 0;
      for( vector<int>::iterator ih = (mtraIt->second)->StsHits.begin();
	   ih != (mtraIt->second)->StsHits.end(); ih++ ){
	if( vHitUsed[*ih] ) nusedhits++;
      }

      if( nusedhits >0 ) killed = 1;      

    }
    
    if (((mtraIt->second)->mother_ID < 0)&&((mtraIt->second)->z > 0)){ // D0
      nmtra_d0++;
      if (reco) nmtra_d0_reco++;
      if (reco) p_eff_d0_vs_mom->Fill(momentum, 100.0);
      else      p_eff_d0_vs_mom->Fill(momentum,   0.0);
      
      if (killed) nmtra_d0_killed++;
      h_reco_d0_mom->Fill(momentum);
    }
      

    nmtra_total++;
    
    if (reco) nmtra_total_reco++;
    if (killed) nmtra_total_killed++;

    if (momentum>1.0 /*&&  mtraIt->second->nMCContStations >=6*/){ // reference tracks
      nmtra_fast++;
      if (reco) nmtra_fast_reco++;
      if (killed) nmtra_fast_killed++;
	
      if ((mtraIt->second)->mother_ID < 0){ // primary
	nmtra_fast_prim++;
	if (reco) nmtra_fast_prim_reco++;
	if (killed) nmtra_fast_prim_killed++;
     }
      else{
	nmtra_fast_sec++;
	if (reco) nmtra_fast_sec_reco++;
	if (killed) nmtra_fast_sec_killed++;
      }
    }
    else{   // extra set of tracks
      nmtra_slow++;
      if (reco) nmtra_slow_reco++;      
      if (killed) nmtra_slow_killed++;
       
      if ((mtraIt->second)->mother_ID < 0){ // primary
	nmtra_slow_prim++;
	if (reco) nmtra_slow_prim_reco++;
	if (killed) nmtra_slow_prim_killed++;
      }
      else{
	nmtra_slow_sec++;
	if (reco) nmtra_slow_sec_reco++;
	if (killed) nmtra_slow_sec_killed++;
      }
      
    }
        
  
  }


  h_reco_time->Fill(algo->CATime);
  h_reco_timeNtr->Fill(nmtra_total,algo->CATime);
  h_reco_timeNhit->Fill(algo->vStsHits.size(),algo->CATime);

  L1_CATIME += algo->CATime;

  int NFakes = 0;
  for( unsigned ih=0; ih<algo->vStsHits.size(); ih++){
    int iMC = vHitMCRef[ih];
    if (iMC < 0) NFakes++;
    //station
    int ista = vHitStore[ih].iStation;
    sta_nhits[ista]++;
    L1_NHITS[ista]++;
    if (iMC < 0){ 
      sta_nfakes[ista]++;
      L1_NFAKES[ista]++;
    }
  }

  h_reco_fakeNtr->Fill(nmtra_total,NFakes);
  h_reco_fakeNhit->Fill(algo->vStsHits.size()-NFakes,NFakes);
  

  L1_NMTRA_D0              += nmtra_d0;
  L1_NMTRA_TOTAL           += nmtra_total;
  L1_NMTRA_FAST            += nmtra_fast;
  L1_NMTRA_FAST_PRIM       += nmtra_fast_prim;
  L1_NMTRA_FAST_SEC        += nmtra_fast_sec;
  L1_NMTRA_SLOW            += nmtra_slow;
  L1_NMTRA_SLOW_PRIM       += nmtra_slow_prim;
  L1_NMTRA_SLOW_SEC        += nmtra_slow_sec;
  
  L1_NMTRA_D0_RECO         += nmtra_d0_reco;
  L1_NMTRA_TOTAL_RECO      += nmtra_total_reco;
  L1_NMTRA_FAST_RECO       += nmtra_fast_reco;
  L1_NMTRA_FAST_PRIM_RECO  += nmtra_fast_prim_reco;
  L1_NMTRA_FAST_SEC_RECO   += nmtra_fast_sec_reco;
  L1_NMTRA_SLOW_RECO       += nmtra_slow_reco;
  L1_NMTRA_SLOW_PRIM_RECO  += nmtra_slow_prim_reco;
  L1_NMTRA_SLOW_SEC_RECO   += nmtra_slow_sec_reco;
  L1_NCLONES               += nclones;
  L1_NGHOSTS               += nghosts;
  L1_NTRACKS               += ntracks;

  L1_NMTRA_D0_KILLED         += nmtra_d0_killed;
  L1_NMTRA_TOTAL_KILLED      += nmtra_total_killed;
  L1_NMTRA_FAST_KILLED       += nmtra_fast_killed;
  L1_NMTRA_FAST_PRIM_KILLED  += nmtra_fast_prim_killed;
  L1_NMTRA_FAST_SEC_KILLED   += nmtra_fast_sec_killed;
  L1_NMTRA_SLOW_KILLED       += nmtra_slow_killed;
  L1_NMTRA_SLOW_PRIM_KILLED  += nmtra_slow_prim_killed;
  L1_NMTRA_SLOW_SEC_KILLED   += nmtra_slow_sec_killed;
  
  L1_NEVENTS++;
  
  double p_total=0.0,p_fast=0.0,p_fast_prim=0.0,p_fast_sec=0.0,p_slow=0.0,p_slow_prim=0.0,p_slow_sec=0.0;
  double p_clone=0.0,p_ghost=0.0;
  if (nmtra_total    !=0) p_total = double(nmtra_total_reco)/double(nmtra_total);
  if (nmtra_fast     !=0) p_fast  = double(nmtra_fast_reco)/double(nmtra_fast);
  if (nmtra_fast_prim!=0) p_fast_prim  = double(nmtra_fast_prim_reco)/double(nmtra_fast_prim);
  if (nmtra_fast_sec !=0) p_fast_sec  = double(nmtra_fast_sec_reco)/double(nmtra_fast_sec);
  if (nmtra_slow     !=0) p_slow  = double(nmtra_slow_reco)/double(nmtra_slow);
  if (nmtra_slow_prim!=0) p_slow_prim  = double(nmtra_slow_prim_reco)/double(nmtra_slow_prim);
  if (nmtra_slow_sec !=0) p_slow_sec  = double(nmtra_slow_sec_reco)/double(nmtra_slow_sec);
  if (ntracks        !=0) p_clone = double(nclones)/double(ntracks);
  if (ntracks        !=0) p_ghost = double(nghosts)/double(ntracks);
  
  double P_D0=0.0,P_TOTAL=0.0,P_FAST=0.0,P_FAST_PRIM=0.0,P_FAST_SEC=0.0,P_SLOW=0.0,P_SLOW_PRIM=0.0,P_SLOW_SEC=0.0;
  double K_D0=0.0,K_TOTAL=0.0,K_FAST=0.0,K_FAST_PRIM=0.0,K_FAST_SEC=0.0,K_SLOW=0.0,K_SLOW_PRIM=0.0,K_SLOW_SEC=0.0;
  double P_CLONE=0.0,P_GHOST=0.0;
  if (L1_NMTRA_D0!=0){
    P_D0 = double(L1_NMTRA_D0_RECO)/double(L1_NMTRA_D0);
    K_D0 = double(L1_NMTRA_D0_KILLED)/double(L1_NMTRA_D0);
  }
  if (L1_NMTRA_TOTAL!=0){
    P_TOTAL = double(L1_NMTRA_TOTAL_RECO)/double(L1_NMTRA_TOTAL);
    K_TOTAL = double(L1_NMTRA_TOTAL_KILLED)/double(L1_NMTRA_TOTAL);
  }
  if (L1_NMTRA_FAST!=0){
    P_FAST  = double(L1_NMTRA_FAST_RECO)/double(L1_NMTRA_FAST);
    K_FAST  = double(L1_NMTRA_FAST_KILLED)/double(L1_NMTRA_FAST);
  }
  if (L1_NMTRA_FAST_PRIM!=0){
    P_FAST_PRIM  = double(L1_NMTRA_FAST_PRIM_RECO)/double(L1_NMTRA_FAST_PRIM);
    K_FAST_PRIM  = double(L1_NMTRA_FAST_PRIM_KILLED)/double(L1_NMTRA_FAST_PRIM);
  }
  if (L1_NMTRA_FAST_SEC!=0){
    P_FAST_SEC  = double(L1_NMTRA_FAST_SEC_RECO)/double(L1_NMTRA_FAST_SEC);
    K_FAST_SEC  = double(L1_NMTRA_FAST_SEC_KILLED)/double(L1_NMTRA_FAST_SEC);
  }
  if (L1_NMTRA_SLOW!=0){
    P_SLOW  = double(L1_NMTRA_SLOW_RECO)/double(L1_NMTRA_SLOW);
    K_SLOW  = double(L1_NMTRA_SLOW_KILLED)/double(L1_NMTRA_SLOW);
  }
  if (L1_NMTRA_SLOW_PRIM!=0){
    P_SLOW_PRIM  = double(L1_NMTRA_SLOW_PRIM_RECO)/double(L1_NMTRA_SLOW_PRIM);
    K_SLOW_PRIM  = double(L1_NMTRA_SLOW_PRIM_KILLED)/double(L1_NMTRA_SLOW_PRIM);
  }
  if (L1_NMTRA_SLOW_SEC!=0){
    P_SLOW_SEC  = double(L1_NMTRA_SLOW_SEC_RECO)/double(L1_NMTRA_SLOW_SEC);
    K_SLOW_SEC  = double(L1_NMTRA_SLOW_SEC_KILLED)/double(L1_NMTRA_SLOW_SEC);
  }
  if (L1_NMTRA_TOTAL_RECO!=0)
    //P_CLONE = double(L1_NCLONES)/double(L1_NTRACKS);
    P_CLONE = double(L1_NCLONES)/double(L1_NMTRA_TOTAL);
  if (L1_NMTRA_TOTAL_RECO!=0)
    //P_GHOST = double(L1_NGHOSTS)/double(L1_NTRACKS);
    P_GHOST = double(L1_NGHOSTS)/double(L1_NMTRA_TOTAL_RECO);
    
  
  if( fVerbose ){
    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(3);
#ifdef XXX
  cout                                                   << endl;
  cout << "L1 PER EVENT STAT      : "                    << endl;
  cout << "MC Refset              : " << nmtra_fast      << endl;
  cout << "MC Extras              : " << nmtra_slow      << endl;
  cout << "ALL SIMULATED          : " << nmtra_total     << endl;
  cout                                                   << endl;
  cout << "RC Refset              : " << nmtra_fast_reco << endl;
  cout << "RC Extras              : " << nmtra_slow_reco << endl;
  cout << "ghosts                 : " << nghosts         << endl;
  cout << "clones                 : " << nclones         << endl;
  cout << "ALL RECONSTRUCTED      : " << ntracks         << endl;
  cout                                                   << endl;
  cout << "Refset efficiency      : " << p_fast          << endl;
  cout << "Allset efficiency      : " << p_total         << endl;
  cout << "Extra  efficiency      : " << p_slow          << endl;
  cout << "clone  probability     : " << p_clone         << endl;
  cout << "ghost  probability     : " << p_ghost         << endl;

  cout                                                   << endl;


  cout << "Number of true and fake hits in stations: " << endl;
  for (int i = 0; i < algo->NStations; i++){
    cout << sta_nhits[i]-sta_nfakes[i] << "+" << sta_nfakes[i] << "   ";
  }
  cout << endl;

#endif//XXX
  cout                                                   << endl;
  cout << "L1 ACCUMULATED STAT    : " << L1_NEVENTS << " EVENTS "               << endl << endl;
  
  if (L1_NMTRA_D0!=0)
    cout << "D0        efficiency   : " << P_D0        <<" | "<<L1_NMTRA_D0_RECO   << endl;
  
  cout << "RefPrim   efficiency   : " << P_FAST_PRIM <<" / "<< K_FAST_PRIM <<" | "<<L1_NMTRA_FAST_PRIM_RECO   << endl;
  cout << "RefSec    efficiency   : " << P_FAST_SEC  <<" / "<< K_FAST_SEC  <<" | "<<L1_NMTRA_FAST_SEC_RECO    << endl;
  cout << "Refset    efficiency   : " << P_FAST      <<" / "<< K_FAST      <<" | "<<L1_NMTRA_FAST_RECO        << endl;
  cout << "Allset    efficiency   : " << P_TOTAL     <<" / "<< K_TOTAL     <<" | "<<L1_NMTRA_TOTAL_RECO       << endl;
  cout << "ExtraPrim efficiency   : " << P_SLOW_PRIM <<" / "<< K_SLOW_PRIM <<" | "<<L1_NMTRA_SLOW_PRIM_RECO   << endl;
  cout << "ExtraSec  efficiency   : " << P_SLOW_SEC  <<" / "<< K_SLOW_SEC  <<" | "<<L1_NMTRA_SLOW_SEC_RECO    << endl;
  cout << "Extra     efficiency   : " << P_SLOW      <<" / "<< K_SLOW      <<" | "<<L1_NMTRA_SLOW_RECO        << endl;
  cout << "Clone     probability  : " << P_CLONE     <<" | "<<L1_NCLONES                << endl;
  cout << "Ghost     probability  : " << P_GHOST     <<" | "<<L1_NGHOSTS                << endl;
  cout << "MC tracks/event found  : " << int(double(L1_NMTRA_TOTAL_RECO)/double(L1_NEVENTS)) << endl;
  
  cout<<endl;

  cout<<"CA Track Finder: " << L1_CATIME/L1_NEVENTS << " s/ev" << endl << endl;
  /*
  cout << "Mean number of all (true+fake) hits in stations: " << endl;
  for (int i = 0; i < algo->NStations; i++){
    cout << (int)((L1_NHITS[i]-L1_NFAKES[i])/L1_NEVENTS) + (int)(L1_NFAKES[i]/L1_NEVENTS)  << "(" << (int)((L1_NHITS[i]-L1_NFAKES[i])/L1_NEVENTS) << "+" << (int)(L1_NFAKES[i]/L1_NEVENTS) << ")   ";
  }
  cout << endl;

  cout.precision(1);
  cout << "Mean percentage (%) of true+fake hits in stations: " << endl;
  for (int i = 0; i < algo->NStations; i++){
    cout << 100.0-100.0*L1_NFAKES[i]/L1_NHITS[i] << "+" << 100.0*L1_NFAKES[i]/L1_NHITS[i] << "   ";
  }
  cout << endl;
  */
  cout.precision(3);
  
  //#endif//XXX
  }
  // fit quality
  if(0){

    for (vector<CbmL1Track>::iterator it = vRTracks.begin(); it != vRTracks.end(); ++it){

      if (rtramtramap.find(&(*it)) == rtramtramap.end()) continue;
      {
	int iMC = vHitMCRef[it->StsHits.front()];
	if (iMC < 0) continue;
	CbmL1MCPoint &mc = vMCPoints[iMC];
	h_fit[0]->Fill( (mc.x-it->T[0]) *1.e4);
	h_fit[1]->Fill( (mc.y-it->T[1]) *1.e4);
	h_fit[2]->Fill((mc.px/mc.pz-it->T[2])*1.e3);
	h_fit[3]->Fill((mc.py/mc.pz-it->T[3])*1.e3);
	h_fit[4]->Fill(it->T[4]/mc.q*mc.p-1);
	if( finite(it->C[0]) && it->C[0]>0 )h_fit[5]->Fill( (mc.x-it->T[0])/sqrt(it->C[0]));
	if( finite(it->C[2]) && it->C[2]>0 )h_fit[6]->Fill( (mc.y-it->T[1])/sqrt(it->C[2]));
	if( finite(it->C[5]) && it->C[5]>0 )h_fit[7]->Fill( (mc.px/mc.pz-it->T[2])/sqrt(it->C[5]));
	if( finite(it->C[9]) && it->C[9]>0 )h_fit[8]->Fill( (mc.py/mc.pz-it->T[3])/sqrt(it->C[9]));
	if( finite(it->C[14]) && it->C[14]>0 )h_fit[9]->Fill( (mc.q/mc.p-it->T[4])/sqrt(it->C[14]));
      }
      {
	int iMC = vHitMCRef[it->StsHits.back()];
	if (iMC < 0) continue;
	CbmL1MCPoint &mc = vMCPoints[iMC];
	h_fitL[0]->Fill( (mc.x-it->TLast[0]) *1.e4);
	h_fitL[1]->Fill( (mc.y-it->TLast[1]) *1.e4);
	h_fitL[2]->Fill((mc.px/mc.pz-it->TLast[2])*1.e3);
	h_fitL[3]->Fill((mc.py/mc.pz-it->TLast[3])*1.e3);
	h_fitL[4]->Fill(it->T[4]/mc.q*mc.p-1);
	if( finite(it->CLast[0]) && it->CLast[0]>0 ) h_fitL[5]->Fill( (mc.x-it->TLast[0])/sqrt(it->CLast[0]));
	if( finite(it->CLast[2]) && it->CLast[2]>0 ) h_fitL[6]->Fill( (mc.y-it->TLast[1])/sqrt(it->CLast[2]));
	if( finite(it->CLast[5]) && it->CLast[5]>0 ) h_fitL[7]->Fill( (mc.px/mc.pz-it->TLast[2])/sqrt(it->CLast[5]));
	if( finite(it->CLast[9]) && it->CLast[9]>0 ) h_fitL[8]->Fill( (mc.py/mc.pz-it->TLast[3])/sqrt(it->CLast[9]));
	if( finite(it->CLast[14]) && it->CLast[14]>0 ) h_fitL[9]->Fill( (mc.q/mc.p-it->TLast[4])/sqrt(it->CLast[14]));   
      }
      h_fit_chi2->Fill(it->chi2/it->NDF);
    }
  }

  bool write = false;

  if (L1_NEVENTS < 10){//write at every event
    write = true;
  }

  if (L1_NEVENTS < 100){//write at every 10th event
    if ( L1_NEVENTS % 10 == 0) 
      write = true;
  }

  if (L1_NEVENTS >= 100){//write at every 100th event
    if ( L1_NEVENTS % 100 == 0) 
      write = true;
  }
  //write = 0;
  if (write){
    TDirectory *curr = gDirectory;
    // Open output file and write histograms
    TFile* outfile = new TFile("L1_histo.root","RECREATE");
    outfile->cd();
    writedir2current(histodir);
    outfile->Close();
    curr->cd();
  }

}
