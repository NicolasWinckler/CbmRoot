/** CbmAnaDielectronTask.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 3.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_H
#define CBM_ANA_DIELECTRON_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"
#include "CbmAnaLmvmNames.h"
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
   Double_t chi2Prim;
   Double_t chi2sts;
   Int_t McMotherId;
   Int_t stsMcTrackId;
   Int_t richMcTrackId;
   Int_t trdMcTrackId;
   Int_t tofMcTrackId;
   Int_t stsInd;
   Int_t richInd;
   Int_t trdInd;
   Int_t tofInd;
   Bool_t isElectron;
   Bool_t isMcSignalElectron;
   Bool_t isMcPi0Electron;
   Bool_t isMcGammaElectron;
   Bool_t isMcEtaElectron;

   Bool_t isGamma;
   Double_t dSts;
   Bool_t isTtCutElectron;
   Bool_t isStCutElectron;
   Bool_t isApmCutElectron;
   Bool_t isMvd1CutElectron;
   Bool_t isMvd2CutElectron;
   Double_t richAnn;
   Double_t trdAnn;
   Double_t mass2;
};

class KinematicParams{
public:
   Double_t momentumMag; // Absolute value of momentum
   Double_t pt; // Transverse momentum
   Double_t rapidity; // Rapidity
   Double_t minv; // Invariant mass
   Double_t angle; // Opening angle
};

class CbmAnaDielectronTask : public FairTask {

public:
   /*
    * \brief Standard constructor.
    */
    CbmAnaDielectronTask();

    /*
     * \brief Standard destructor.
     */
    virtual ~CbmAnaDielectronTask();

    /*
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

    /*
     * \brief Inherited from FairTask.
     */
    virtual void Exec(
          Option_t *option);
    
    /*
     * \brief Creates 1D histograms for each analysis step.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] nBins Number of bins in the histogram.
     * \param[in] min Minimum value.
     * \param[in] max Maximum value.
     */
    void CreateAnalysisStepsH1(
          vector<TH1D*>& hist,
          const string& name,
          const string& axisX,
          const string& axisY,
          double nBins,
          double min,
          double max);


    /*
     * \brief Creates 2D histograms for each analysis step.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] axisZ Z axis title.
     * \param[in] nBinsX Number of bins for X axis in the histogram.
     * \param[in] minX Minimum value for X axis.
     * \param[in] maxX Maximum value for X axis.
     * \param[in] nBinsY Number of bins for Y axis in the histogram.
     * \param[in] minY Minimum value for Y axis.
     * \param[in] maxY Maximum value for Y axis.
     */
    void CreateAnalysisStepsH2(
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
          double maxY);

    /*
     * \brief Creates 1D histograms for different track source types.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] nBins Number of bins in the histogram.
     * \param[in] min Minimum value.
     * \param[in] max Maximum value.
     */
    void CreateSourceTypesH1(
          vector<TH1D*>& hist,
          const string& name,
          const string& axisX,
          const string& axisY,
          double nBins,
          double min,
          double max);


    /*
     * \brief Creates 2D histograms for different track source types.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] axisZ Z axis title.
     * \param[in] nBinsX Number of bins for X axis in the histogram.
     * \param[in] minX Minimum value for X axis.
     * \param[in] maxX Maximum value for X axis.
     * \param[in] nBinsY Number of bins for Y axis in the histogram.
     * \param[in] minY Minimum value for Y axis.
     * \param[in] maxY Maximum value for Y axis.
     */
    void CreateSourceTypesH2(
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
          double maxY);

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

    /*
     * \brief Fills histograms for pairs.
     * \param[in] candP Positive candidate.
     * \param[in] candM Negative candidate.
     * \param[in] step Enumeration AnalysisSteps, specify analysis step.
     * \param[in] angle Opening angle of the pair.
     */
    void PairSource(
        DielectronCandidate* candP, 
        DielectronCandidate* candM, 
        AnalysisSteps step,
        Double_t angle);

    /*
     * \brief Fills minv, pty, mom histograms for specified analysis step.
     * \param[in] candP Positive candidate.
     * \param[in] candM Negative candidate.
     * \param[in] parMc MC kinematic parameters.
     * \param[in] parRec Reconstructed kinematic parameters.
     * \param[in] step Enumeration AnalysisSteps, specify analysis step.
     */
    void FillPairHists(
          DielectronCandidate* candP,
          DielectronCandidate* candM,
          KinematicParams* parMc,
          KinematicParams* parRec,
          AnalysisSteps step);

    void TrackSource(
    		DielectronCandidate* cand,
    		AnalysisSteps step,
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

    /*
     * \brief
     * \param[in] mvdStationNum MVD station number.
     * \param[in, out] hist Vector of histograms for different source types.
     */
    void CheckClosestMvdHit(
    		Int_t mvdStationNum,
    		vector<TH2D*>& hist);

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
    CbmRichElectronIdAnn* fElIdAnn;
    Bool_t fUseRichAnn;
    // Analysis cuts
    Double_t fChiPrimCut;
    Double_t fPtCut;
    Double_t fAngleCut;
    Double_t fGammaCut;
    Double_t fStCutAngle;
    Double_t fStCutPP;
    Double_t fTtCutAngle;
    Double_t fTtCutPP;
    Double_t fMvd1CutP;
    Double_t fMvd1CutD;
    Double_t fMvd2CutP;
    Double_t fMvd2CutD;

    vector<TH1*> fHistoList; //list of all histograms

    // Number of hits in the MC RICH ring
    std::map<Int_t, Int_t> fNofHitsInRingMap;

   TH1D* fh_mc_mother_pdg; //mother pdg code for e-/e+
   TH1D* fh_acc_mother_pdg; //mother pdg code for accepted e-/e+

   TH2D* fh_mc_vertex_gamma_xz;
   TH2D* fh_mc_vertex_gamma_yz;
   TH2D* fh_mc_vertex_gamma_xy;

   //Index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   //Use AnalysisSteps enumeration for access.
   //MC and ACC histograms are not filled sometimes.
   vector<TH1D*> fh_signal_minv; // Invariant mass for Signal
   vector<TH1D*> fh_bg_minv; // Invariant mass for BG
   vector<TH1D*> fh_pi0_minv; // Invariant mass for Pi0
   vector<TH1D*> fh_eta_minv; // Invariant mass for Eta
   vector<TH1D*> fh_signal_mom; // Signal momentum distribution
   vector<TH2D*> fh_signal_pty; // Pt/y distribution for signal
   vector<TH2D*> fh_signal_minv_pt; // Invariant mass vs. MC Pt

   //Index is the source type: [0]-signal, [1]-bg, [2]-pi0, [3]-gamma
   //Use SourceTypes enumeration for access.
	vector<TH1D*> fh_pt; // Transverse momentum of single track distribution
   vector<TH1D*> fh_mom; //Momentum of the single track
   vector<TH1D*> fh_chi2sts; // Chi2 of the STS tracks
   vector<TH1D*> fh_chi2prim; // Chi2 of the primary vertex
   vector <TH2D*> fh_ttcut; // TT cut
   vector <TH2D*> fh_stcut; // ST cut
   vector<TH2D*> fh_apcut; // Armanteros-Podalyanski
   vector<TH2D*> fh_apmcut; // AP modified
   vector<TH2D*> fh_mvd1cut; // MVD cut at the first station
   vector<TH2D*> fh_mvd2cut; // MVD cut at the second station
   vector<TH1D*> fh_richann; // RICH ANN
   vector<TH1D*> fh_trdann; // TRD ANN
   vector<TH2D*> fh_tofm2; // TOF m2

   //source of BG pairs 2D.
   //second index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   //Use AnalysisSteps enumeration for access.
   vector<TH2D*> fh_source_pair;

   //store event number
   TH1D* fh_event_number;

   //nof signal and bg tracks after each cut
   TH1D* fh_nof_bg_tracks;
   TH1D* fh_nof_el_tracks;
   TH2D* fh_source_tracks;

   //First index is the source type: [0]-signal, [1]-bg, [2]-pi0, [3]-gamma
   //Use SourceTypes enumeration for access.
   //second index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   //Use AnalysisSteps enumeration for access.

   //Track momentum distribution for different sources after each cut.
   vector<vector<TH1D*> > fh_source_mom;
   //Opening angle distribution for different sources after each cut.
   vector<vector<TH1D*> > fh_opening_angle;

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
   void SetStCut(Double_t ang, Double_t pp){fStCutAngle = ang; fStCutPP = pp;}
   void SetTtCut(Double_t ang, Double_t pp){fTtCutAngle = ang; fTtCutPP = pp;}
};

#endif
