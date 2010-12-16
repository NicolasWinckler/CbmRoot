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

	Bool_t isPi0;
    Double_t dSts;
    Bool_t isTTCutElectron;
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

    void SingleParticleAcceptance();
    void MCPairs();
    void MCPairsBg();
    void PairAcceptance();
    void FillCandidateArray();
    void DifferenceSignalAndBg();
    void RecoQa();
    void PairsReco();
    void SetDefaultIdParameters();
    void BgReco();
    void Pi0Reco();
    void CheckGammaConvAndPi0();
    void FindClosestMvdHit();
    void CheckTrackTopologyCut();

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
    Int_t fNofPtcutPairs;
    Int_t fNofTTcutPairs;

    Int_t fNofRecBg;
    Int_t fNofRichIdBg;
    Int_t fNofTrdIdBg;
    Int_t fNofTofIdBg;
    Int_t fNofChiPrimCutBg;
    Int_t fNofPtcutBg;
    Int_t fNofTTcutBg;


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
    Double_t fPi0Cut;

    CbmRichElectronIdAnn * fElIdAnn;
    Bool_t fUseRichAnn;
    
    Int_t fNofGlobalTracks; // number of global tracks
    
    TH2D* fh_mc_signal_pty; // pt/y distribution for signal mc
    TH2D* fh_acc_signal_pty; // pt/y distribution for accepted signal

    TH1D* fh_mc_signal_mom; //momentum distribution for signal mc
    TH1D* fh_acc_signal_mom; //momentum distribution of accepted signal
    TH1D* fh_reco_signal_mom;
    
    TH1D* fh_rich_id_signal_mom;
    TH1D* fh_trd_id_signal_mom;
    TH1D* fh_tof_id_signal_mom;
    TH1D* fh_chi_prim_signal_mom; 
    TH1D* fh_ptcut_signal_mom;
    TH1D* fh_anglecut_signal_mom;
    TH1D* fh_pi0cut_signal_mom;
    TH1D* fh_ttcut_signal_mom;

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
    TH1D* fh_pi0cut_signal_minv;
    TH1D* fh_ttcut_signal_minv;
//BG minv
    TH1D* fh_rec_bg_minv;
    TH1D* fh_rich_id_bg_minv;
    TH1D* fh_trd_id_bg_minv;
    TH1D* fh_tof_id_bg_minv;
    TH1D* fh_chi_prim_bg_minv; //chi primary cut 
    TH1D* fh_ptcut_bg_minv; //pt cut 
    TH1D* fh_anglecut_bg_minv; // openning angle after pt cut for BG
    TH1D* fh_pi0cut_bg_minv;
    TH1D* fh_ttcut_bg_minv;
//pi0 minv
    TH1D* fh_rec_pi0_minv;
    TH1D* fh_rich_id_pi0_minv;
    TH1D* fh_trd_id_pi0_minv;
    TH1D* fh_tof_id_pi0_minv;
    TH1D* fh_chi_prim_pi0_minv; //chi primary cut
    TH1D* fh_ptcut_pi0_minv; //pt cut
    TH1D* fh_anglecut_pi0_minv; // openning angle after pt cut for BG
    TH1D* fh_pi0cut_pi0_minv;
    TH1D* fh_ttcut_pi0_minv;

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
    TH2D* fh_ttcut_bg;
    TH2D* fh_apcut_signal;
    TH2D* fh_apcut_bg;
    TH2D* fh_ttcut_pi0;
    TH2D* fh_apcut_pi0;
    TH2D* fh_ttcut_gamma;
    TH2D* fh_apcut_gamma;

    TH2D* fh_reco_signal_pty;
    TH2D* fh_rich_id_signal_pty;
    TH2D* fh_trd_id_signal_pty;
    TH2D* fh_tof_id_signal_pty;
    TH2D* fh_chi_prim_signal_pty; 
    TH2D* fh_ptcut_signal_pty; 
    TH2D* fh_anglecut_signal_pty;
    TH2D* fh_pi0cut_signal_pty;
    TH2D* fh_ttcut_signal_pty;

    //ID cuts distributions
    TH1D* fh_rich_ann_signal;
    TH1D* fh_rich_ann_bg;
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
