/** CbmAnaDielectronTask.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 2.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_H
#define CBM_ANA_DIELECTRON_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"

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
#include <string>
using std::map;
using std::vector;
using std::string;

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
   Double_t chi2sts;
   Int_t MCMotherId;
   Int_t stsMCTrackId;
   Int_t richMCTrackId;
   Int_t trdMCTrackId;
   Int_t tofMCTrackId;
   Int_t stsInd;
   Int_t richInd;
   Int_t trdInd;
   Int_t tofInd;
   Bool_t isElectron;
   Bool_t isMCSignalElectron;
   Bool_t isMCPi0Electron;
   Bool_t isMCGammaElectron;
   Bool_t isMCEtaElectron;

   Bool_t isGamma;
   Double_t dSts;
   Bool_t isTTCutElectron;
   Bool_t isSTCutElectron;
   Bool_t isAPMCutElectron;
   Bool_t isDSTSCutElectron;
   Bool_t isDSTSCut2Electron;
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

    CbmAnaDielectronTask(
        const char *name,
        const char *title = "CbmAnaDielectronTask");

    virtual ~CbmAnaDielectronTask();


    virtual InitStatus Init();

    virtual void Exec(
          Option_t *option);
    
    TH1D* CreateMinvHisto(
          const string& name);

    TH1D* CreateSignalMomHisto(
          const string& name);

    TH2D* CreatePtYHisto(
          const string& name);

    KinematicParams CalculateKinematicParams(
        CbmMCTrack* mctrackP, 
        CbmMCTrack* mctrackM);

    KinematicParams CalculateKinematicParams(
        DielectronCandidate* candP, 
        DielectronCandidate* candM);

    void CalculateArmPodParams(
        DielectronCandidate* candP, 
        DielectronCandidate* candM,
        Double_t &alpha, 
        Double_t &ptt);

    void SourcePairs(
        DielectronCandidate* candP, 
        DielectronCandidate* candM, 
        TH2D* h_source_pair);

    void TrackSource(
    		DielectronCandidate* cand,
    		Double_t binNum,
    		Int_t pdg);

    void SingleParticleAcceptance();

    void FillRichRingNofHits();

    void MCPairs();

    void PairAcceptance();

    void FillSegmentCandidatesArray();

    void FillCandidateArray();

    void DifferenceSignalAndBg();

    void SetDefaultIdParameters();

    void SignalAndBgReco();

    void CheckGammaConvAndPi0();

    void CheckClosestMvdHit(
    		Int_t mvdStationNum,
    		TH2D* signal,
    		TH2D* bg,
    		TH2D* gamma,
    		TH2D* pi0,
    		TH2D* eta);

    void CheckTrackTopologyCut();

    void CheckTrackTopologyRecoCut();

    Bool_t CheckArmPod(
        Double_t alfa, 
        Double_t pt);

    void CheckArmPodModified();

    virtual void Finish();

    ClassDef(CbmAnaDielectronTask,1);

private:

    void IsElectron(
    	CbmRichRing * ring,
    	Double_t momentum,
    	CbmTrdTrack* trdTrack,
    	CbmGlobalTrack * gTrack,
    	DielectronCandidate* cand);

    Bool_t IsRichElectron(
        CbmRichRing* ring, 
        Double_t momentum, 
        DielectronCandidate* cand);

    Bool_t IsTrdElectron(
        CbmTrdTrack* trdTrack, 
        DielectronCandidate* cand);

    Bool_t IsTofElectron(
        CbmGlobalTrack* gTrack, 
        Double_t momentum, 
        DielectronCandidate* cand);

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

    Bool_t fUseMvd;
    Bool_t fUseRich;
    Bool_t fUseTrd;
    Bool_t fUseTof;

    vector<DielectronCandidate> fCandidates;
    vector<DielectronCandidate> fSegmentCandidates;

    Double_t fWeight; //Multiplicity*BR

    // ID cuts
    Double_t fTrdAnnCut;
    Double_t fRichAnnCut;
    Double_t fMeanA;
    Double_t fMeanB;
    Double_t fRmsA;
    Double_t fRmsB;
    Double_t fRmsCoeff;
    Double_t fDistCut;
    CbmRichElectronIdAnn * fElIdAnn;
    Bool_t fUseRichAnn;
    // Analysis cuts
    Double_t fChiPrimCut;
    Double_t fPtCut;
    Double_t fAngleCut;
    Double_t fGammaCut;
    Double_t fSTCutAngle;
    Double_t fSTCutPP;
    Double_t fTTCutAngle;
    Double_t fTTCutPP;
    Double_t fDSTSCutP;
    Double_t fDSTSCutD;
    Double_t fDSTSCut2P;
    Double_t fDSTSCut2D;

    vector<TH1*> fHistoList; //list of all histograms

    // Number of hits in the MC RICH ring
    std::map<Int_t, Int_t> fNofHitsInRingMap;

   //signal momentum distribution
   TH1D* fh_mc_signal_mom;
   TH1D* fh_acc_signal_mom;
   TH1D* fh_reco_signal_mom;
   TH1D* fh_el_id_signal_mom;
   TH1D* fh_chi_prim_signal_mom;
   TH1D* fh_ptcut_signal_mom;
   TH1D* fh_anglecut_signal_mom;
   TH1D* fh_gammacut_signal_mom;
   TH1D* fh_ttcut_signal_mom;
   TH1D* fh_stcut_signal_mom;
   TH1D* fh_apmcut_signal_mom;
   TH1D* fh_dstscut_signal_mom;
   TH1D* fh_dsts2cut_signal_mom;

   TH1D* fh_mc_mother_pdg; //mother pdg code for e-/e+
   TH1D* fh_acc_mother_pdg; //mother pdg code for accepted e-/e+

   TH2D* fh_mc_vertex_gamma_xz;
   TH2D* fh_mc_vertex_gamma_yz;
   TH2D* fh_mc_vertex_gamma_xy;

   // invariant mass distribution for signal
   TH1D* fh_mc_signal_minv;
   TH1D* fh_acc_signal_minv;
   TH1D* fh_rec_signal_minv; //minv of reconstructed signal (ideal ID)
   TH1D* fh_el_id_signal_minv;
   TH1D* fh_chi_prim_signal_minv; //chi primary cut after identification for signal
   TH1D* fh_ptcut_signal_minv; //pt cut
   TH1D* fh_anglecut_signal_minv; // openning angle after pt cut for signal
   TH1D* fh_gammacut_signal_minv;
   TH1D* fh_ttcut_signal_minv;
   TH1D* fh_stcut_signal_minv;
   TH1D* fh_apmcut_signal_minv;
   TH1D* fh_dstscut_signal_minv;
   TH1D* fh_dsts2cut_signal_minv;

   TH2D* fh_ttcut_signal_minv_pt;
   TH2D* fh_ptcut_signal_minv_pt;

   //BG minv
   TH1D* fh_rec_bg_minv;
   TH1D* fh_el_id_bg_minv;
   TH1D* fh_chi_prim_bg_minv; //chi primary cut
   TH1D* fh_ptcut_bg_minv; //pt cut
   TH1D* fh_anglecut_bg_minv; // openning angle after pt cut for BG
   TH1D* fh_gammacut_bg_minv;
   TH1D* fh_ttcut_bg_minv;
   TH1D* fh_stcut_bg_minv;
   TH1D* fh_apmcut_bg_minv;
   TH1D* fh_dstscut_bg_minv;
   TH1D* fh_dsts2cut_bg_minv;

   //pi0 minv
   TH1D* fh_rec_pi0_minv;
   TH1D* fh_el_id_pi0_minv;
   TH1D* fh_chi_prim_pi0_minv; //chi primary cut
   TH1D* fh_ptcut_pi0_minv; //pt cut
   TH1D* fh_anglecut_pi0_minv; // openning angle after pt cut for pi0
   TH1D* fh_gammacut_pi0_minv;
   TH1D* fh_ttcut_pi0_minv;
   TH1D* fh_stcut_pi0_minv;
   TH1D* fh_apmcut_pi0_minv;
   TH1D* fh_dstscut_pi0_minv;
   TH1D* fh_dsts2cut_pi0_minv;

   //eta minv
   TH1D* fh_rec_eta_minv;
	TH1D* fh_el_id_eta_minv;
	TH1D* fh_chi_prim_eta_minv; //chi primary cut
	TH1D* fh_ptcut_eta_minv; //pt cut
	TH1D* fh_anglecut_eta_minv; // openning angle after pt cut for eta
	TH1D* fh_gammacut_eta_minv;
	TH1D* fh_ttcut_eta_minv;
   TH1D* fh_stcut_eta_minv;
	TH1D* fh_apmcut_eta_minv;
	TH1D* fh_dstscut_eta_minv;
	TH1D* fh_dsts2cut_eta_minv;

	// cuts distribution
   TH1D* fh_pt_signal;
   TH1D* fh_pt_bg;
   TH1D* fh_chi2_prim_signal; // Chi2 primary
   TH1D* fh_chi2_prim_bg;
   TH1D* fh_mom_signal; // signal e+/e- momentum
   TH1D* fh_mom_bg; // bg momentum
   TH1D* fh_angle_signal;
   TH1D* fh_angle_bg;
   TH1D* fh_chi2sts_signal;
   TH1D* fh_chi2sts_bg;
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
   TH2D* fh_apmcut_signal;
   TH2D* fh_apmcut_bg;
   TH2D* fh_dsts_signal;
   TH2D* fh_dsts_bg;
   TH2D* fh_dsts_gamma;
   TH2D* fh_dsts_pi0;
   TH2D* fh_dsts_eta;
   //dsts cut at he second MVD station
   TH2D* fh_dsts2_signal;
   TH2D* fh_dsts2_bg;
   TH2D* fh_dsts2_gamma;
   TH2D* fh_dsts2_pi0;
   TH2D* fh_dsts2_eta;

   //source of BG pairs
   TH2D*  fh_source_pair_reco;
   TH2D*  fh_source_pair_el_id;
   TH2D*  fh_source_pair_chi_prim;
   TH2D*  fh_source_pair_ptcut;
   TH2D*  fh_source_pair_anglecut;
   TH2D*  fh_source_pair_gammacut;
   TH2D*  fh_source_pair_ttcut;
   TH2D*  fh_source_pair_stcut;
   TH2D*  fh_source_pair_apmcut;
   TH2D*  fh_source_pair_dstscut;
   TH2D*  fh_source_pair_dsts2cut;

   //pt/y distribution for signal
   TH2D* fh_mc_signal_pty;
   TH2D* fh_acc_signal_pty;
   TH2D* fh_reco_signal_pty;
   TH2D* fh_el_id_signal_pty;
   TH2D* fh_chi_prim_signal_pty;
   TH2D* fh_ptcut_signal_pty;
   TH2D* fh_anglecut_signal_pty;
   TH2D* fh_gammacut_signal_pty;
   TH2D* fh_ttcut_signal_pty;
   TH2D* fh_stcut_signal_pty;
   TH2D* fh_apmcut_signal_pty;
   TH2D* fh_dstscut_signal_pty;
   TH2D* fh_dsts2cut_signal_pty;

   //ID cuts distributions
   TH1D* fh_rich_ann_signal;
   TH1D* fh_rich_ann_bg;
   TH1D* fh_trd_ann_signal;
   TH1D* fh_trd_ann_bg;
   TH2D* fh_tof_m2_signal;
   TH2D* fh_tof_m2_bg;

   //store event number
   TH1D* fh_event_number;

   //nof signal and bg tracks after each cut
   TH1D* fh_nof_bg_tracks;
   TH1D* fh_nof_el_tracks;
   TH2D* fh_source_tracks;

   TH1D* fh_gamma_mom;
   TH1D* fh_pi0_mom;
   TH1D* fh_pions_mom;
   TH1D* fh_proton_mom;
   TH1D* fh_kaons_mom;
   TH1D* fh_other_mom;
   TH1D* fh_gamma_id_mom;
   TH1D* fh_pi0_id_mom;
   TH1D* fh_pions_id_mom;
   TH1D* fh_proton_id_mom;
   TH1D* fh_kaons_id_mom;
   TH1D* fh_other_id_mom;

public:
   void SetUseMvd(Bool_t use){fUseMvd = use;};
   void SetUseRich(Bool_t use){fUseRich = use;};
   void SetUseTrd(Bool_t use){fUseTrd = use;};
   void SetUseTof(Bool_t use){fUseTof = use;};
   void SetWeight(Double_t weight){fWeight = weight;};

   // ID cuts
   void SetTrdAnnCut(Double_t par){fTrdAnnCut = par;}
   void SetRichAnnCut(Double_t par){fRichAnnCut = par;}
   void SetUseRichAnn(Bool_t par){fUseRichAnn = par;}
   // Analysis cuts
   void SetChiPrimCut(Double_t par){fChiPrimCut = par;}
   void SetPtCut(Double_t par){fPtCut = par;}
   void SetAngleCut(Double_t par){fAngleCut = par;}
   void SetGammaCut(Double_t par){fGammaCut = par;}
   void SetSTCut(Double_t ang, Double_t pp){fSTCutAngle = ang; fSTCutPP = pp;}
   void SetTTCut(Double_t ang, Double_t pp){fTTCutAngle = ang; fTTCutPP = pp;}
};

#endif
