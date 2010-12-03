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
	Double_t mass, charge, energy, rapidity;
	Double_t chiPrimary;
	Int_t mcPdg, mcMotherID, mcMotherPdg;
	Int_t stsMCTrackId;
	Int_t richMCTrackId;
    Int_t trdMCTrackId;
    Int_t tofMCTrackId;
	Int_t stsInd;
	Int_t richInd;
	Int_t trdInd;
	Int_t tofInd;
	Bool_t isInSts;
	Bool_t isInRich;
	Bool_t isInTrd;
	Bool_t isInTof;
	Bool_t isRichElectron;
	Bool_t isTrdElectron;
	Bool_t isTofElectron;


	Int_t is_fake, is_wrong, is_pion, is_proton;

	Bool_t isMCSignalElectron;
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



    virtual void Finish();
    void WriteOutput();

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
    Bool_t IsRichElectron(CbmRichRing* ring, Double_t momentum);
    Bool_t IsTrdElectron(CbmTrdTrack* trdTrack);
    Bool_t IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum);
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
    Int_t fNofPtcutPairs;

    Int_t fNofRecBg;
    Int_t fNofRichIdBg;
    Int_t fNofTrdIdBg;
    Int_t fNofTofIdBg;
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
    Double_t fPtCut;
    Double_t fAngleCut;
    
    CbmRichElectronIdAnn * fElIdAnn;
    Bool_t fUseRichAnn;
    
    Int_t fNofGlobalTracks; // number of global tracks
    
    TH2D* fh_mc_signal_pty; // pt/y distribution for signal mc
    TH2D* fh_acc_signal_pty; // pt/y distribution for accepted signal
    TH2D* fh_acc_signal_pty_eff; //efficiency of accepted signal 

    TH1D* fh_mc_signal_mom; //momentum distribution for signal mc
    TH1D* fh_mc_bg_mom;
    TH1D* fh_acc_signal_mom; //momentum distribution of accepted signal
    TH1D* fh_acc_signal_mom_eff; // momentum efficiency
    TH1D* fh_mc_signal_minv; // invariant mass distribution for signal mc
    TH1D* fh_mc_bg_minv;
    TH1D* fh_acc_signal_minv; //invariant mass distribution for accepted signal
    TH1D* fh_mc_mother_pdg; //mother pdg code for e-/e+
    TH1D* fh_acc_mother_pdg; //mother pdg code for accepted e-/e+
    TH1D* fh_rec_mc_mom_signal;
    TH1D* fh_rec_signal_minv; //invariant mass distribution of reconstructed signal (ideal ID)

    TH1D* fh_chi2_prim_signal; // Chi2 primary
    TH1D* fh_chi2_prim_bg;
    
    TH2D* fh_mc_vertex_gamma_xz;
    TH2D* fh_mc_vertex_gamma_yz;
    TH2D* fh_mc_vertex_gamma_xy;

    TH1D* fh_rich_id_signal_minv;
    TH1D* fh_trd_id_signal_minv;
    TH1D* fh_tof_id_signal_minv;

    TH1D* fh_rich_id_bg_minv;
    TH1D* fh_trd_id_bg_minv;
    TH1D* fh_tof_id_bg_minv;
    TH1D* fh_rec_bg_minv;

    TH1D* fh_pt_signal;
    TH1D* fh_pt_bg;
    TH1D* fh_position_signal;
    TH1D* fh_position_bg;

    TH2D* fh_reco_signal_pty;
    TH2D* fh_rich_id_signal_pty;
    TH2D* fh_trd_id_signal_pty;
    TH2D* fh_tof_id_signal_pty;

    TH1D* fh_angle_signal;
    TH1D* fh_angle_bg;

    TH1D* fh_ptcut_signal_minv; //pt cut after identification for signal
    TH1D* fh_anglecut_signal_minv; // openning angle after pt cut for signal
    TH2D* fh_ptcut_signal_pty; 
    TH2D* fh_anglecut_signal_pty;

    TH1D* fh_ptcut_bg_minv; //pt cut after identification for BG
    TH1D* fh_anglecut_bg_minv; // openning angle after pt cut for BG
     
    
public:
    void SetUseRich(Bool_t use){fUseRich = use;};
    void SetUseTrd(Bool_t use){fUseTrd = use;};
    void SetUseTof(Bool_t use){fUseTof = use;};
};

#endif
