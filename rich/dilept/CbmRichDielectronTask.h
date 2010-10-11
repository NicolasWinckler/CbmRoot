#ifndef CBM_RICH_DIELECTRON_TASK_H
#define CBM_RICH_DIELECTRON_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"

#include "TProfile.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmRichElectronIdAnn.h"

#include <map>
#include <fstream>
#include <vector>
using std::map;
using std::vector;

class TClonesArray;
class TH2D;
class TH1D;
class TH2F;

class FairRootManager;

class DiGlobalTrack{
public:
	CbmGlobalTrack *fGlobalTrack;
	CbmMCTrack *fMCTrack;
	TVector3 position, momentum;
	Double_t mass, charge, energy, rapidity;
	Double_t axisA, axisB, distance;
	Double_t chiPrimary;
	Int_t mcPdg, mcMotherID, mcMotherPdg;
	Int_t stsMCid, richMCid;
	Int_t stsInd, richInd, trdInd, tofInd;
	Int_t is_sts, is_rich, is_trd, is_tof;
	Int_t is_fake, is_wrong, is_pion, is_proton;
	Bool_t is_e;
};

class CbmRichDielectronTask : public FairTask {
    Bool_t SetParameters();
    void FillDiGlobalTracks();
public:
    CbmRichDielectronTask();
    CbmRichDielectronTask(const char *name, const char *title="CBM Task");
    virtual ~CbmRichDielectronTask();
    virtual InitStatus Init(); //*MENU
    virtual void Exec(Option_t *option);
    void MCPairs();
    void SingleParticleAcceptance();
    void PairAcceptance();
    void DStsMomCut();
    virtual void Finish(); //*MENU
    void WriteOutput();

    virtual void SetParContainers();

    ClassDef(CbmRichDielectronTask,1);


private:

    TClonesArray *fArrayMCTrack;        //!
    TClonesArray *fArrayGlobalTrack;    //!
    TClonesArray *fArrayStsTrack;       //!
    TClonesArray *fArrayStsMatch;       //!
    TClonesArray *fArrayTrdTrack;       //!
    TClonesArray *fArrayTrdHit;         //!
    TClonesArray *fArrayRichRing;       //!
    TClonesArray *fArrayRichProj;       //!
    TClonesArray *fArrayRichPoint;      //!
    TClonesArray *fArrayRichMatch;      //!
    TClonesArray *fArrayTofHit;         //!
    TClonesArray *fArrayStsHit;         //!
    TClonesArray *fArrayMvdHit;         //!

    FairBaseParSet       *fBasePar;      //!
    FairPrimaryGenerator *fPrimGen;      //!
    FairRootManager      *fRootManager;  //!
    CbmVertex            *fPrimVertex;   //!
    CbmVertex            fMCPrimVertex;  //!
    CbmStsKFTrackFitter  fFitter;        //!

    Int_t fEvents;                 //!

    // chi2 primary vertex
    TH1D* fh_chi2_prim_all;        //!
    TH1D* fh_chi2_prim_e;          //!

    // radial vs z
    TH2D* fh_mc_all_zr;            //!

    // single track momentum
    TH1D* fh_mc_all_lept_mom;      //!
    TH1D* fh_mc_rec_lept_mom;      //!
    TH1D* fh_rich_id_lept_mom;     //!
    TH1D* fh_rich_id_ep_mom;       //!
    TH1D* fh_rich_id_em_mom;       //!

    TH1D* fh_rich_id_no_cut_mom;       //!
    TH1D* fh_rich_id_mom_cut_mom;      //!
    TH1D* fh_rich_id_dist_cut_mom;     //!
    TH1D* fh_rich_id_ann_cut_mom;      //!
    TH1D* fh_rich_id_axisAB_cut_mom;   //!
    TH1D* fh_rich_tof_id_cut_mom;

    // pair momentum
    TH1D* fh_mc_all_mom;           //!
    TH1D* fh_mc_rec_mom;           //!
    TH1D* fh_rich_id_mom;          //!
    // omega
    TH1D* fh_mc_all_omega_mom;     //!
    TH1D* fh_mc_rec_omega_mom;     //!
    TH1D* fh_rich_id_omega_mom;    //!
    // eta
    TH1D* fh_mc_all_eta_mom;       //!
    TH1D* fh_mc_rec_eta_mom;       //!
    TH1D* fh_rich_id_eta_mom;      //!
    // pi0
    TH1D* fh_mc_all_pi0_mom;       //!
    TH1D* fh_mc_rec_pi0_mom;       //!
    TH1D* fh_rich_id_pi0_mom;      //!
    // gamma
    TH1D* fh_mc_all_gamma_mom;     //!
    TH1D* fh_mc_rec_gamma_mom;     //!
    TH1D* fh_rich_id_gamma_mom;    //!

    // opening angle
    TH1D* fh_mc_all_theta;         //!
    TH1D* fh_mc_rec_theta;         //!
    TH1D* fh_rich_id_theta;        //!
    // omega
    TH1D* fh_mc_all_omega_theta;   //!
    TH1D* fh_mc_rec_omega_theta;   //!
    TH1D* fh_rich_id_omega_theta;  //!
    // eta
    TH1D* fh_mc_all_eta_theta;     //!
    TH1D* fh_mc_rec_eta_theta;     //!
    TH1D* fh_rich_id_eta_theta;    //!
    // pi0
    TH1D* fh_mc_all_pi0_theta;     //!
    TH1D* fh_mc_rec_pi0_theta;     //!
    TH1D* fh_rich_id_pi0_theta;    //!
    // gamma
    TH1D* fh_mc_all_gamma_theta;   //!
    TH1D* fh_mc_rec_gamma_theta;   //!
    TH1D* fh_rich_id_gamma_theta;  //!

    // rich identification
    TH1D* fh_rich_ring_dist;       //!
    TH1D* fh_rich_ring_dist_e;     //!
    TH2D* fh_rich_dist_mom;        //!
    TH2D* fh_rich_dist_mom_e;      //!
    TH2D* fh_rich_dist_mom_cut;    //!

    TH2D* fh_rich_ring_axisA_mom_all;  //!
    TH2D* fh_rich_ring_axisB_mom_all;  //!
    TH2D* fh_rich_ring_axisA_mom;      //!
    TH2D* fh_rich_ring_axisB_mom;      //!
    TH1D* fh_rich_ring_axisA;          //!
    TH1D* fh_rich_ring_axisB;          //!
    TH2D* fh_rich_ring_axisA_mom_e;    //!
    TH2D* fh_rich_ring_axisB_mom_e;    //!
    TH1D* fh_rich_ring_axisA_e;        //!
    TH1D* fh_rich_ring_axisB_e;        //!

    // trd  and tof information
    TH1D* fh_trd_eloss;            //!
    TH1D* fh_trd_eloss_e;          //!
    TH1D* fh_trd_ann;              //!
    TH1D* fh_trd_ann_e;            //!
    TH1D* fh_trd_ann_pi;           //!
    TH2D* fh_trd_ann_mom;          //!
    TH2D* fh_trd_ann_mom_e;        //!

    TH2D* fh_tof_m2_mom;           //!
    TH2D* fh_tof_m2_mom_zoom;      //!
    TH2D* fh_tof_m2_mom_zoom_cut;  //!

    // d to the closest neighbour
    TH1D* fh_dsts_bg;              //!
    TH1D* fh_dsts_omega;           //!
    TH1D* fh_dsts_pi0;             //!
    TH1D* fh_dsts_gamma;           //!

    TH2D* fh_dsts_mom_bg;          //!
    TH2D* fh_dsts_mom_omega;       //!
    TH2D* fh_dsts_mom_pi0;         //!
    TH2D* fh_dsts_mom_gamma;       //!

    // (theta + mom) cut
    TH2D* fh_dcut_theta_mom_omega; //!
    TH2D* fh_dcut_theta_mom_omega_true; //!
    TH2D* fh_dcut_theta_mom_pi0;   //!
    TH2D* fh_dcut_theta_mom_gamma; //!
    TH2D* fh_dcut_theta_mom_gamma_true; //!
    TH2D* fh_dcut_theta_mom_bg;    //!

    // pt cut
    TH1D* fh_tpcut_pt_lept_omega;  //!
    TH1D* fh_tpcut_pt_lept_bg;     //!

    // opening angle
    TH1D* fh_ptcut_theta_omega;      //!
    TH1D* fh_ptcut_theta_omega_true; //!
    TH1D* fh_ptcut_theta_pi0;        //!
    TH1D* fh_ptcut_theta_gamma;      //!
    TH1D* fh_ptcut_theta_bg;         //!

    // invariant mass
    TH1D* fh_mc_all_minv;          //!
    TH1D* fh_mc_rec_minv;          //!
    TH1D* fh_rich_id_minv;         //!
    TH1D* fh_gcut_minv;            //!
    TH1D* fh_dcut_minv;            //!
    TH1D* fh_tpcut_minv;           //!
    TH1D* fh_ptcut_minv;           //!
    TH1D* fh_tcut_minv;            //!
    TH1D* fh_pi0cut_minv;          //!
    TH1D* fh_rich_id_mc_minv;      //!
    TH1D* fh_gcut_mc_minv;         //!
    TH1D* fh_dcut_mc_minv;         //!
    TH1D* fh_tpcut_mc_minv;        //!
    TH1D* fh_ptcut_mc_minv;        //!
    TH1D* fh_tcut_mc_minv;         //!
    TH1D* fh_pi0cut_mc_minv;       //!

    TH1D* fh_pi0cut_epep_minv;     //!
    TH1D* fh_pi0cut_emem_minv;     //!

    TH2D* fh_mc_all_pt_minv_omega; //!
    TH2D* fh_mc_all_pt_minv_pairs; //!
    TH2D* fh_mc_rec_pt_minv_omega; //!
    TH2D* fh_mc_rec_pt_minv_pairs; //!
    TH2D* fh_rich_id_pt_minv_omega;//!
    TH2D* fh_rich_id_pt_minv_pairs;//!
    TH2D* fh_tpcut_pt_minv_omega; //!
    TH2D* fh_tpcut_pt_minv_pairs; //!
    TH2D* fh_pi0cut_pt_minv_omega; //!
    TH2D* fh_pi0cut_pt_minv_pairs; //!

    TH1D* fh_mc_all_omega_bg_minv;     //!
    TH1D* fh_mc_rec_omega_bg_minv;     //!
    TH1D* fh_rich_id_omega_bg_minv;    //!
    TH1D* fh_gcut_omega_bg_minv;       //!
    TH1D* fh_dcut_omega_bg_minv;       //!
    TH1D* fh_tpcut_omega_bg_minv;      //!
    TH1D* fh_ptcut_omega_bg_minv;      //!
    TH1D* fh_tcut_omega_bg_minv;       //!
    TH1D* fh_pi0cut_omega_bg_minv;     //!
    TH1D* fh_rich_id_mc_omega_bg_minv; //!
    TH1D* fh_gcut_mc_omega_bg_minv;    //!
    TH1D* fh_dcut_mc_omega_bg_minv;    //!
    TH1D* fh_tpcut_mc_omega_bg_minv;   //!
    TH1D* fh_ptcut_mc_omega_bg_minv;   //!
    TH1D* fh_tcut_mc_omega_bg_minv;    //!
    TH1D* fh_pi0cut_mc_omega_bg_minv;  //!

    // omega
    TH1D* fh_mc_all_omega_minv;    //!
    TH1D* fh_mc_rec_omega_minv;    //!
    TH1D* fh_rich_id_omega_minv;   //!
    TH1D* fh_gcut_omega_minv;      //!
    TH1D* fh_dcut_omega_minv;      //!
    TH1D* fh_tpcut_omega_minv;     //!
    TH1D* fh_ptcut_omega_minv;     //!
    TH1D* fh_tcut_omega_minv;      //!
    TH1D* fh_pi0cut_omega_minv;    //!
    // eta
    TH1D* fh_mc_all_eta_minv;      //!
    TH1D* fh_mc_rec_eta_minv;      //!
    TH1D* fh_rich_id_eta_minv;     //!
    TH1D* fh_gcut_eta_minv;        //!
    TH1D* fh_dcut_eta_minv;        //!
    TH1D* fh_tpcut_eta_minv;       //!
    TH1D* fh_ptcut_eta_minv;       //!
    TH1D* fh_tcut_eta_minv;        //!
    TH1D* fh_pi0cut_eta_minv;      //!
    // pi0
    TH1D* fh_mc_all_pi0_minv;      //!
    TH1D* fh_mc_rec_pi0_minv;      //!
    TH1D* fh_rich_id_pi0_minv;     //!
    TH1D* fh_gcut_pi0_minv;        //!
    TH1D* fh_dcut_pi0_minv;        //!
    TH1D* fh_tpcut_pi0_minv;       //!
    TH1D* fh_ptcut_pi0_minv;       //!
    TH1D* fh_tcut_pi0_minv;        //!
    TH1D* fh_pi0cut_pi0_minv;      //!
    TH1D* fh_pi0cut_fake_pi0_minv; //!
    TH1D* fh_pi0cut_true_pi0_minv; //!
    // gamma
    TH1D* fh_mc_all_gamma_minv;    //!
    TH1D* fh_mc_rec_gamma_minv;    //!
    TH1D* fh_rich_id_gamma_minv;   //!
    TH1D* fh_gcut_gamma_minv;      //!
    TH1D* fh_dcut_gamma_minv;      //!
    TH1D* fh_tpcut_gamma_minv;     //!
    TH1D* fh_ptcut_gamma_minv;     //!
    TH1D* fh_tcut_gamma_minv;      //!
    TH1D* fh_pi0cut_gamma_minv;    //!

    // pair pt-y
    TH2D* fh_mc_all_pty;           //!
    TH2D* fh_mc_rec_pty;           //!
    TH2D* fh_rich_id_pty;          //!
    TH2D* fh_gcut_pty;             //!
    TH2D* fh_dcut_pty;             //!
    TH2D* fh_tpcut_pty;            //!
    TH2D* fh_ptcut_pty;            //!
    TH2D* fh_tcut_pty;             //!
    TH2D* fh_pi0cut_pty;           //!
    // omega
    TH2D* fh_mc_all_omega_pty;     //!
    TH2D* fh_mc_rec_omega_pty;     //!
    TH2D* fh_rich_id_omega_pty;    //!
    TH2D* fh_gcut_omega_pty;       //!
    TH2D* fh_dcut_omega_pty;       //!
    TH2D* fh_tpcut_omega_pty;      //!
    TH2D* fh_ptcut_omega_pty;      //!
    TH2D* fh_tcut_omega_pty;       //!
    TH2D* fh_pi0cut_omega_pty;     //!
    // eta
    TH2D* fh_mc_all_eta_pty;       //!
    TH2D* fh_mc_rec_eta_pty;       //!
    TH2D* fh_rich_id_eta_pty;      //!
    TH2D* fh_gcut_eta_pty;         //!
    TH2D* fh_dcut_eta_pty;         //!
    TH2D* fh_tpcut_eta_pty;        //!
    TH2D* fh_ptcut_eta_pty;        //!
    TH2D* fh_tcut_eta_pty;         //!
    TH2D* fh_pi0cut_eta_pty;       //!
    // pi0
    TH2D* fh_mc_all_pi0_pty;       //!
    TH2D* fh_mc_rec_pi0_pty;       //!
    TH2D* fh_rich_id_pi0_pty;      //!
    TH2D* fh_gcut_pi0_pty;         //!
    TH2D* fh_dcut_pi0_pty;         //!
    TH2D* fh_tpcut_pi0_pty;        //!
    TH2D* fh_ptcut_pi0_pty;        //!
    TH2D* fh_tcut_pi0_pty;         //!
    TH2D* fh_pi0cut_pi0_pty;       //!
    // gamma
    TH2D* fh_mc_all_gamma_pty;     //!
    TH2D* fh_mc_rec_gamma_pty;     //!
    TH2D* fh_rich_id_gamma_pty;    //!
    TH2D* fh_gcut_gamma_pty;       //!
    TH2D* fh_dcut_gamma_pty;       //!
    TH2D* fh_tpcut_gamma_pty;      //!
    TH2D* fh_ptcut_gamma_pty;      //!
    TH2D* fh_tcut_gamma_pty;       //!
    TH2D* fh_pi0cut_gamma_pty;     //!

    Double_t fn_rich_id_ep;
    Double_t fn_rich_id_em;
    Double_t fn_gcut_ep;
    Double_t fn_gcut_em;
    Double_t fn_dcut_ep;
    Double_t fn_dcut_em;
    Double_t fn_tpcut_ep;
    Double_t fn_tpcut_em;
    Double_t fn_ptcut_ep;
    Double_t fn_ptcut_em;
    Double_t fn_tcut_ep;
    Double_t fn_tcut_em;
    Double_t fn_pi0cut_ep;
    Double_t fn_pi0cut_em;

    TH1D* fh_mom_rich;
    TH1D* fh_mom_trd;
    TH1D* fh_mom_tof;

    TH1D* fh_mom_w_sts;
    TH1D* fh_mom_w_rich;
    TH1D* fh_mom_w_trd;
    TH1D* fh_mom_w_tof;

    TH1D* fh_mom_e_sts;
    TH1D* fh_mom_e_rich;
    TH1D* fh_mom_e_trd;
    TH1D* fh_mom_e_tof;

    TH1D* fh_rich_id_pi0_pi0_minv;
    TH1D* fh_rich_id_pi0_gamma_minv;
    TH1D* fh_rich_id_gamma_gamma_minv;
    TH1D* fh_rich_id_eta_gamma_minv;
    TH1D* fh_rich_id_eta_eta_minv;
    TH1D* fh_rich_id_eta_pi0_minv;

    TH1D* fh_trd_id_pi0_pi0_minv;
    TH1D* fh_trd_id_pi0_gamma_minv;
    TH1D* fh_trd_id_gamma_gamma_minv;
    TH1D* fh_trd_id_eta_gamma_minv;
    TH1D* fh_trd_id_eta_eta_minv;
    TH1D* fh_trd_id_eta_pi0_minv;

    TH1D* fh_tof_id_pi0_pi0_minv;
    TH1D* fh_tof_id_pi0_gamma_minv;
    TH1D* fh_tof_id_gamma_gamma_minv;
    TH1D* fh_tof_id_eta_gamma_minv;
    TH1D* fh_tof_id_eta_eta_minv;
    TH1D* fh_tof_id_eta_pi0_minv;

    TH1D* fh_pi0cut_pi0_pi0_minv;
    TH1D* fh_pi0cut_pi0_gamma_minv;
    TH1D* fh_pi0cut_gamma_gamma_minv;
    TH1D* fh_pi0cut_eta_gamma_minv;
    TH1D* fh_pi0cut_eta_eta_minv;
    TH1D* fh_pi0cut_eta_pi0_minv;

    Int_t fCoutBunch;

    // cut values
    Double_t fRichMom;
    Double_t fRichDist;
    Double_t fRichAxisAmean;
    Double_t fRichAxisBmean;
    Double_t fRichAxisAsigma;
    Double_t fRichAxisBsigma;
    Double_t fRichCoeff;

    Double_t fRichAnnCut;
    Bool_t fUseRichAnn;
    CbmRichElectronIdAnn *fElIdAnn;

    Double_t fTrdEloss;
    Double_t fTrdAnn;
    Double_t fTofM2;
    Double_t fDcut;
    Double_t fGcut;
    Double_t f2Dtcut;
    Double_t f2Dpcut;
    Double_t fPtcut;
    Double_t fTcut;
    Double_t fPi0cut;

    Double_t fWeight;

    std::vector<DiGlobalTrack> fDiGlobalTracks;

    void CreateHistogramms();

public:

    inline void SetCoutBunch(Int_t coutBunch) {fCoutBunch = coutBunch;};

    // cut for rich id
    inline void SetRichMom    (Double_t richmom)    {fRichMom  = richmom;};
    inline void SetRichDist   (Double_t richdist)   {fRichDist = richdist;};
    inline void SetRichAxisAmean  (Double_t richaxisamean)  {fRichAxisAmean  = richaxisamean;};
    inline void SetRichAxisAsigma (Double_t richaxisasigma) {fRichAxisAsigma = richaxisasigma;};
    inline void SetRichAxisBmean  (Double_t richaxisbmean)  {fRichAxisBmean  = richaxisbmean;};
    inline void SetRichAxisBsigma (Double_t richaxisbsigma) {fRichAxisBsigma = richaxisbsigma;};
    inline void SetRichCoeff      (Double_t richcoeff)      {fRichCoeff      = richcoeff;};

    inline void SetRichAnnCut(Double_t richAnnCut) {fRichAnnCut = richAnnCut;};
    inline void SetUseRichAnn(Bool_t useRichAnn) {fUseRichAnn = useRichAnn;}; 
    
    inline void SetTrdEloss   (Double_t trdeloss) {fTrdEloss = trdeloss;};
    inline void SetTrdAnn     (Double_t trdann)   {fTrdAnn   = trdann;};
    inline void SetTofM2      (Double_t tofm2)    {fTofM2    = tofm2;};

    // cut values
    inline void SetDcut   (Double_t dcut)   {fDcut   = dcut;};
    inline void SetGcut   (Double_t gcut)   {fGcut   = gcut;};
    inline void Set2Dtcut (Double_t dtcut)  {f2Dtcut = dtcut;};
    inline void Set2Dpcut (Double_t dpcut)  {f2Dpcut = dpcut;};
    inline void SetPtcut  (Double_t ptcut)  {fPtcut  = ptcut;};
    inline void SetTcut   (Double_t tcut)   {fTcut   = tcut;};
    inline void SetPi0cut (Double_t pi0cut) {fPi0cut = pi0cut;};
    inline void SetWeight (Double_t weight) {fWeight = weight;};
};

#endif
