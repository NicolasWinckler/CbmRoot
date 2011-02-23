#ifndef CBM_ANA_DIELECTRON_TASK_H
#define CBM_ANA_DIELECTRON_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"

#include "TProfile.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
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

class DielectronCandidate{
public:
	TVector3 position, momentum;
	Double_t mass, energy, rapidity;
    Int_t charge;
	Double_t chiPrimary;
	Int_t MCMotherId;
	Int_t stsMCTrackId;
	Int_t richMCTrackId;
    Int_t trdMCTrackId;
    Int_t tofMCTrackId;
	Int_t stsInd;
	Int_t richInd;
	Int_t trdInd;
	Int_t tofInd;
	Bool_t isRichElectron;
	Bool_t isTrdElectron;
	Bool_t isTofElectron;
	Bool_t isMCSignalElectron;
	Bool_t isMCPi0Electron;
	Bool_t isMCGammaElectron;
	Bool_t isMCEtaElectron;

	Bool_t isGamma;
    Double_t dSts;
    Bool_t isTTCutElectron;
    Bool_t isSTCutElectron;
    Double_t richAnn;
    Double_t trdAnn;
    Double_t mass2;
};

class KinematicParams{
public:
    Double_t momentumMag;
    Double_t pt;
    Double_t rapidity;
    Double_t minv;
    Double_t angle; // opening angle
};

class CbmAnaDielectronTask : public FairTask {

public:
    CbmAnaDielectronTask();
    CbmAnaDielectronTask(const char *name, const char *title="CBM Task");
    virtual ~CbmAnaDielectronTask();
    virtual InitStatus Init();
    virtual void Exec(Option_t *option);
    
    KinematicParams CalculateKinematicParams(CbmMCTrack* mctrackP, CbmMCTrack* mctrackM);
    KinematicParams CalculateKinematicParams(DielectronCandidate* candP, DielectronCandidate* candM);
    void CalculateArmPodParams(DielectronCandidate* candP, DielectronCandidate* candM,
                       Double_t &alpha, Double_t &ptt);
    void SourcePairs(DielectronCandidate* candP, DielectronCandidate* candM, TH2D* h_source_pair);

    void SingleParticleAcceptance();
    void MC();
    void MCPairs();
    void MCPairsBg();
    void PairAcceptance();
    void FillSegmentCandidatesArray();
    void FillCandidateArray();
    void DifferenceSignalAndBg();
    void RecoQa();
    void PairsReco();
    void Reco();
    void SingleReco();
    void SetDefaultIdParameters();
    void BgReco();
    void Pi0Reco();
    void EtaReco();
    void CheckGammaConvAndPi0();
    void FindClosestMvdHit();
    void CheckTrackTopologyCut();
    void CheckTrackTopologyRecoCut();
    Bool_t CheckArmPod(Double_t alfa, Double_t pt);

    virtual void Finish();

    ClassDef(CbmAnaDielectronTask,1);

private:
    TClonesArray *fMCTracks;

    TClonesArray *fRichRings;
    TClonesArray *fRichProj;
    TClonesArray *fRichPoints;
    TClonesArray *fRichRingMatches;
    TClonesArray *fRichHits;

    TClonesArray *fGlobalTracks;

    TClonesArray *fStsTracks;
    TClonesArray *fStsTrackMatches;
    TClonesArray *fStsHits;
    TClonesArray *fMvdHits;

    TClonesArray *fTrdTracks;
    TClonesArray *fTrdHits;
    TClonesArray *fTrdTrackMatches;

    TClonesArray *fTofHits;
    TClonesArray *fTofPoints;

    CbmVertex *fPrimVertex;
    CbmStsKFTrackFitter fKFFitter;

    Int_t fEvents;

    Bool_t fUseRich;
    Bool_t fUseTrd;
    Bool_t fUseTof;

    void IsRichElectron(CbmRichRing* ring, Double_t momentum, DielectronCandidate* cand);
    void IsTrdElectron(CbmTrdTrack* trdTrack, DielectronCandidate* cand);
    void IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum, DielectronCandidate* cand);

    vector<DielectronCandidate> fCandidates;
    vector<DielectronCandidate> fSegmentCandidates;

    Double_t fWeight; //Multiplicity*BR

    Int_t fNofMcEp;  //number of MC e+
    Int_t fNofMcEm;  // number of MC e-
    Int_t fNofMcEpBg;  
    Int_t fNofMcEmBg; 

    Int_t fNofAccEp; //number of accepted e+
    Int_t fNofAccEm; //number of accepted e-
    Int_t fNofAccEpBg; //number of accepted e+ from BG 
    Int_t fNofAccEmBg; //number of accepted e- from BG 
    Int_t fNofAccPairs; //number of accepted pairs of e-/e+

    Int_t fNofRecPairs;
    Int_t fNofRichIdPairs; //number of rich id signal pairs
    Int_t fNofTrdIdPairs; //number of trd id signal pairs
    Int_t fNofTofIdPairs; //number of tof id signal pairs
    Int_t fNofChiPrimCutPairs;//number of signal pairs after chi primary cut
    Int_t fNofGammaCutPairs;
    Int_t fNofTTcutPairs;
    Int_t fNofSTcutPairs;
    Int_t fNofAngleCutPairs;
    Int_t fNofAPcutPairs;
    Int_t fNofPtcutPairs;

    Int_t fNofRecBg;
    Int_t fNofRichIdBg;
    Int_t fNofTrdIdBg;
    Int_t fNofTofIdBg;
    Int_t fNofChiPrimCutBg;
    Int_t fNofGammaCutBg;
    Int_t fNofTTcutBg;
    Int_t fNofSTcutBg;
    Int_t fNofAngleCutBg;
    Int_t fNofAPcutBg;
    Int_t fNofPtcutBg;

// ID cuts
    Double_t fTrdAnnCut;
    Double_t fRichAnnCut;
    Double_t fMeanA;
    Double_t fMeanB;
    Double_t fRmsA;
    Double_t fRmsB;
    Double_t fRmsCoeff;
    Double_t fDistCut;
// Analysis cuts
    Double_t fChiPrimCut;
    Double_t fPtCut;
    Double_t fAngleCut;
    Double_t fGammaCut;

    CbmRichElectronIdAnn * fElIdAnn;
    Bool_t fUseRichAnn;

    TH2D* fh_mc_signal_pty; // pt/y distribution for signal mc
    TH2D* fh_acc_signal_pty; // pt/y distribution for accepted signal

    TH1D* fh_mc_signal_mom; //momentum distribution for signal mc
    TH1D* fh_acc_signal_mom; //momentum distribution of accepted signal
    TH1D* fh_reco_signal_mom;
    TH1D* fh_sts_reco_signal_mom;
    TH1D* fh_rich_reco_signal_mom;
    TH1D* fh_rich_id_signal_mom;
    TH1D* fh_trd_id_signal_mom;
    TH1D* fh_tof_id_signal_mom;
    TH1D* fh_chi_prim_signal_mom; 
    TH1D* fh_ptcut_signal_mom;
    TH1D* fh_anglecut_signal_mom;
    TH1D* fh_gammacut_signal_mom;
    TH1D* fh_ttcut_signal_mom;
    TH1D* fh_stcut_signal_mom;
    TH1D* fh_apcut_signal_mom;

//////////
   TH1D* fh_sts_reco_mom;
   TH1D* fh_rich_reco_mom; 
   TH1D* fh_trd_reco_mom;
   TH1D* fh_tof_reco_mom;
   TH1D* fh_richID_mom;
   TH1D* fh_trdID_mom;
   TH1D* fh_tofID_mom;

    TH1D* fh_mc_mom;
    TH1D* fh_mc_gamma_mom;
    TH1D* fh_mc_pi0_mom;
    TH1D* fh_mc_eta_mom;

    TH1D* fh_acc_mom;
    TH1D* fh_acc_gamma_mom;
    TH1D* fh_acc_pi0_mom;
    TH1D* fh_acc_eta_mom;

    TH1D* fh_sts_reco_pi0_mom;
    TH1D* fh_sts_reco_eta_mom;
    TH1D* fh_sts_reco_gamma_mom;

    TH1D* fh_rich_reco_pi0_mom;
    TH1D* fh_rich_reco_eta_mom;
    TH1D* fh_rich_reco_gamma_mom;

    TH1D* fh_trd_reco_pi0_mom;
    TH1D* fh_trd_reco_eta_mom;
    TH1D* fh_trd_reco_gamma_mom;

    TH1D* fh_tof_reco_pi0_mom;
    TH1D* fh_tof_reco_gamma_mom;
    TH1D* fh_tof_reco_eta_mom;
//////////
    TH1D* fh_mc_signal_minv; // invariant mass distribution for signal mc
    TH1D* fh_acc_signal_minv; //invariant mass distribution for accepted signal
    TH1D* fh_mc_mother_pdg; //mother pdg code for e-/e+
    TH1D* fh_acc_mother_pdg; //mother pdg code for accepted e-/e+

    TH1D* fh_rec_mc_mom_signal;
    TH2D* fh_mom_res_vs_mom_signal;
    TH1D* fh_mean_mom_vs_mom_signal;
    TH1D* fh_count_mom_vs_mom_signal;
    
    TH2D* fh_mc_vertex_gamma_xz;
    TH2D* fh_mc_vertex_gamma_yz;
    TH2D* fh_mc_vertex_gamma_xy;

//signal minv
    TH1D* fh_rec_signal_minv; //invariant mass distribution of reconstructed signal (ideal ID)
    TH1D* fh_rich_id_signal_minv;
    TH1D* fh_trd_id_signal_minv;
    TH1D* fh_tof_id_signal_minv;
    TH1D* fh_chi_prim_signal_minv; //chi primary cut after identification for signal
    TH1D* fh_ptcut_signal_minv; //pt cut 
    TH1D* fh_anglecut_signal_minv; // openning angle after pt cut for signal   
    TH1D* fh_gammacut_signal_minv;
    TH1D* fh_ttcut_signal_minv;
    TH1D* fh_stcut_signal_minv;
    TH1D* fh_apcut_signal_minv;

//BG minv
    TH1D* fh_rec_bg_minv;
    TH1D* fh_rich_id_bg_minv;
    TH1D* fh_trd_id_bg_minv;
    TH1D* fh_tof_id_bg_minv;
    TH1D* fh_chi_prim_bg_minv; //chi primary cut 
    TH1D* fh_ptcut_bg_minv; //pt cut 
    TH1D* fh_anglecut_bg_minv; // openning angle after pt cut for BG
    TH1D* fh_gammacut_bg_minv;
    TH1D* fh_ttcut_bg_minv;
    TH1D* fh_stcut_bg_minv;
    TH1D* fh_apcut_bg_minv;
//pi0 minv
    TH1D* fh_rec_pi0_minv;
    TH1D* fh_rich_id_pi0_minv;
    TH1D* fh_trd_id_pi0_minv;
    TH1D* fh_tof_id_pi0_minv;
    TH1D* fh_chi_prim_pi0_minv; //chi primary cut
    TH1D* fh_ptcut_pi0_minv; //pt cut
    TH1D* fh_anglecut_pi0_minv; // openning angle after pt cut for pi0
    TH1D* fh_gammacut_pi0_minv;
    TH1D* fh_ttcut_pi0_minv;
    TH1D* fh_stcut_pi0_minv;
    TH1D* fh_apcut_pi0_minv;

//eta minv
    TH1D* fh_rec_eta_minv;
	TH1D* fh_rich_id_eta_minv;
	TH1D* fh_trd_id_eta_minv;
	TH1D* fh_tof_id_eta_minv;
	TH1D* fh_chi_prim_eta_minv; //chi primary cut
	TH1D* fh_ptcut_eta_minv; //pt cut
	TH1D* fh_anglecut_eta_minv; // openning angle after pt cut for eta
	TH1D* fh_gammacut_eta_minv;
	TH1D* fh_ttcut_eta_minv;
    TH1D* fh_stcut_eta_minv;
	TH1D* fh_apcut_eta_minv;

// cuts distribution
    TH1D* fh_pt_signal;
    TH1D* fh_pt_bg;
    TH1D* fh_position_signal;
    TH1D* fh_position_bg;
    TH1D* fh_chi2_prim_signal; // Chi2 primary
    TH1D* fh_chi2_prim_bg;
    TH1D* fh_mom_signal; // signal e+/e- momentum 
    TH1D* fh_mom_bg; // bg momentum
    TH1D* fh_angle_signal;
    TH1D* fh_angle_bg;
    TH2D* fh_ttcut_signal;
    TH2D* fh_stcut_signal;
    TH2D* fh_ttcut_bg;
    TH2D* fh_stcut_bg;
    TH2D* fh_apcut_signal;
    TH2D* fh_apcut_bg;
    TH2D* fh_ttcut_pi0;
    TH2D* fh_stcut_pi0;
    TH2D* fh_apcut_pi0;
    TH2D* fh_ttcut_gamma;
    TH2D* fh_stcut_gamma;
    TH2D* fh_apcut_gamma;
  
    TH2D*  fh_source_pair_reco;
    TH2D*  fh_source_pair_rich_id;
    TH2D* fh_source_pair_trd_id;
    TH2D*  fh_source_pair_tof_id;
    TH2D*  fh_source_pair_chi_prim;
    TH2D*  fh_source_pair_ptcut;
    TH2D*  fh_source_pair_anglecut;
    TH2D*  fh_source_pair_gammacut;
    TH2D*  fh_source_pair_ttcut;
    TH2D*  fh_source_pair_stcut;
    TH2D*  fh_source_pair_apcut;

    TH2D* fh_reco_signal_pty;
    TH2D* fh_rich_id_signal_pty;
    TH2D* fh_trd_id_signal_pty;
    TH2D* fh_tof_id_signal_pty;
    TH2D* fh_chi_prim_signal_pty; 
    TH2D* fh_ptcut_signal_pty; 
    TH2D* fh_anglecut_signal_pty;
    TH2D* fh_gammacut_signal_pty;
    TH2D* fh_ttcut_signal_pty;
    TH2D* fh_stcut_signal_pty;
    TH2D* fh_apcut_signal_pty;

    //ID cuts distributions
    TH1D* fh_rich_ann_signal;
    TH1D* fh_rich_ann_bg;
    TH2D* fh_rich_trd_ann_signal;
    TH2D* fh_rich_trd_ann_bg;
    TH1D* fh_trd_ann_signal;
    TH1D* fh_trd_ann_bg;
    TH2D* fh_tof_m2_signal;
    TH2D* fh_tof_m2_bg;

public:
    void SetUseRich(Bool_t use){fUseRich = use;};
    void SetUseTrd(Bool_t use){fUseTrd = use;};
    void SetUseTof(Bool_t use){fUseTof = use;};
    void SetWeight(Double_t weight){fWeight = weight;};
};

#endif
