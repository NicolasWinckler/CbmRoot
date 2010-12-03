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

#define M2E 2.6112004954086e-7

using namespace std;

ClassImp(CbmAnaDielectronTask);

CbmAnaDielectronTask::CbmAnaDielectronTask(const char *name, const char *title)
: FairTask(name)
{
    fEvents = 0;

    // weight for rho0 = 0.001081; omega_ee = 0.0026866; omega_dalitz = 0.02242; phi = 0.00039552; pi0 = 4.38   ------ for 25 GeV
    fWeight = 0.001081;
    fUseRich = true;
    fUseTrd = true;
    fUseTof = true;
    fNofMcEp = 0;
    fNofMcEpBg = 0;
    fNofMcEm = 0;
    fNofMcEmBg = 0;
    fNofAccEp = 0;
    fNofAccEm = 0;
    fNofAccPairs = 0;
    fNofAccEmBg = 0;
    fNofAccEpBg = 0;
    fNofGlobalTracks = 0;
    fNofRecPairs = 0;
    fNofRichIdPairs = 0;
    fNofTrdIdPairs = 0;
    fNofTofIdPairs = 0;
    fNofRecBg = 0;
    fNofRichIdBg = 0;
    fNofTrdIdBg = 0;
    fNofTofIdBg = 0;
    fNofPtcutPairs = 0;
    fNofPtcutBg = 0;


    fh_mc_signal_pty = new TH2D("fh_mc_signal_pty","fh_mc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_acc_signal_pty = new TH2D("fh_acc_signal_pty","fh_acc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_acc_signal_pty_eff = new TH2D("fh_acc_signal_pty_eff","fh_acc_signal_pty_eff;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);

    fh_mc_signal_mom = new TH1D("fh_mc_signal_mom", "fh_mc_signal_mom;momentum [GeV/c];yeild",150, 0., 15.);
    fh_mc_bg_mom = new TH1D("fh_mc_bg_mom", "fh_mc_bg_mom;momentum [GeV/c];yeild",150, 0., 15.);
    fh_acc_signal_mom = new TH1D("fh_acc_signal_mom","fh_acc_signal_mom;momentum [GeV/c];yeild", 150, 0., 15.);
    fh_acc_signal_mom_eff = new TH1D("fh_acc_signal_mom_eff","fh_acc_signal_mom_eff;momentum [GeV/c];yeild", 150, 0., 15.);

    fh_mc_signal_minv = new TH1D("fh_mc_signal_minv","fh_mc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_mc_bg_minv = new TH1D("fh_mc_bg_minv","fh_mc_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_acc_signal_minv = new TH1D("fh_acc_signal_minv","fh_acc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
    fh_acc_mother_pdg = new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
    

    fh_chi2_prim_signal = new TH1D("fh_chi2_prim_signal", "fh_chi2_prim_signal", 100, 0., 10.);
    fh_chi2_prim_bg = new TH1D("fh_chi2_prim_bg","fh_chi2_prim_bg", 100, 0., 10.);

    fh_mc_vertex_gamma_xz = new TH2D("fh_mc_vertex_gamma_xz","fh_mc_vertex_gamma_xz;Z [cm];X [cm]",1000, -10., 110., 1000, -100.,100.);
    fh_mc_vertex_gamma_yz = new TH2D("fh_mc_vertex_gamma_yz", "fh_mc_vertex_gamma_yz;Z [cm];Y [cm]", 1000, -10., 110., 1000, -100., 100.);
    fh_mc_vertex_gamma_xy = new TH2D("fh_mc_vertex_gamma_xy","fh_mc_vertex_gamma_xy;X [cm];Y [cm]", 1000, -100.,100., 1000, -100., 100.); 

    fh_rec_mc_mom_signal = new TH1D("fh_rec_mc_mom_signal","fh_rec_mc_mom_signal;#Delta p/p [%];yeild",500, -10., 10.);

    fh_rec_signal_minv = new TH1D("fh_rec_signal_minv","fh_rec_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_rich_id_signal_minv = new TH1D("fh_rich_id_signal_minv","fh_rich_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_signal_minv = new TH1D("fh_trd_id_signal_minv", "fh_trd_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_signal_minv = new TH1D("fh_tof_id_signal_minv","fh_tof_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);

    fh_rec_bg_minv = new TH1D("fh_rec_bg_minv","fh_rec_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_rich_id_bg_minv = new TH1D("fh_rich_id_bg_minv","fh_rich_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_bg_minv = new TH1D("fh_trd_id_bg_minv","fh_trd_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_bg_minv = new TH1D("fh_tof_id_bg_minv","fh_tof_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);

    fh_pt_signal = new TH1D("fh_pt_signal","fh_pt_signal;p_{t} [GeV/c];yeild",200, 0., 2.);
    fh_pt_bg = new TH1D("fh_pt_bg","fh_pt_bg;p_{t} [GeV/c];yeild",200, 0., 2.);

    fh_position_signal = new TH1D("fh_position_signal","fh_position_signal", 100, 0., 0.02);
    fh_position_bg = new TH1D("fh_position_bg","fh_position_bg",100, 0., 0.02);
    
    fh_reco_signal_pty = new TH2D("fh_reco_signal_pty","fh_reco_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_signal_pty = new TH2D("fh_rich_id_signal_pty","fh_rich_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_trd_id_signal_pty = new TH2D("fh_trd_id_signal_pty","fh_trd_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_tof_id_signal_pty = new TH2D("fh_tof_id_signal_pty","fh_tof_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);

    fh_angle_signal = new TH1D("fh_angle_signal","fh_angle_signal;opening angle [deg];yeild", 360, 0., 180.);
    fh_angle_bg = new TH1D("fh_angle_bg","fh_angle_bg;opening angle [deg];yeild", 360, 0., 180.); 

    fh_ptcut_signal_minv = new TH1D("fh_ptcut_signal_minv","fh_ptcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ptcut_signal_pty = new TH2D("fh_ptcut_signal_pty","fh_ptcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_anglecut_signal_minv = new TH1D("fh_anglecut_signal_minv","fh_anglecut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_signal_pty = new TH2D("fh_anglecut_signal_pty","fh_anglecut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);

    fh_ptcut_bg_minv = new TH1D("fh_ptcut_bg_minv","fh_ptcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_bg_minv = new TH1D("fh_anglecut_bg_minv","fh_anglecut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    

    SetDefaultIdParameters();
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

void CbmAnaDielectronTask::SetDefaultIdParameters()
{
    fTrdAnnCut = 0.8;
    fRichAnnCut = -0.5;
    fUseRichAnn = true;
    fMeanA = -1.;
    fMeanB = -1.;
    fRmsA = -1.;
    fRmsB = -1.;
    fRmsCoeff = -1.;
    fDistCut = -1.;
    fPtCut = 0.2;
    fAngleCut = 6.;
}


InitStatus CbmAnaDielectronTask::Init()
{
	cout << "InitStatus CbmAnaDielectronTask::Init()"<<endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmAnaDielectronTask::Init: " << "RootManager not instantised!"<< endl;
		return kERROR;
	}

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( !fMCTracks) {
		cout << "-E- CbmAnaDielectronTask::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	if (fUseRich == true){
		fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
		if ( !fRichHits) {
			cout << "-W- CbmAnaDielectronTask::Init: No RichHit array!" << endl;
		}

		fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
		if ( !fRichRings) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichRing array!" << endl;
			return kERROR;
		}

		fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
		if ( !fRichPoints) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichPoint array!" << endl;
			return kERROR;
		}

		fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
		if ( !fRichRingMatches) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichRingMatch array!" << endl;
			return kERROR;
		}

		fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
		if ( !fRichProj) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichProjection array!" << endl;
			return kERROR;
		}
	}//fUseRich

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if ( !fStsTrackMatches) {
		cout << "-E- CbmAnaDielectronTask::Init: No track match array!"<< endl;
		return kERROR;
	}

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( !fStsTracks) {
		cout << "-E- CbmAnaDielectronTask::Init: No  STSTrack array!"<< endl;
		return kERROR;
	}

    fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
    if (! fStsHits) {
		cout << "-E- CbmAnaDielectronTask::Init: No STSHit array!" << endl;
		return kERROR;
    }

    fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
    if (! fMvdHits) {
		cout << "-E- CbmAnaDielectronTask::Init: No MvdHit array!" << endl;
		return kERROR;
    }

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( !fGlobalTracks) {
		cout << "-W- CbmAnaDielectronTask::Init: No global track array!" << endl;
		return kERROR;
	}

	if (fUseTrd == true){
		fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
		if ( !fTrdTracks) {
			cout << "-W- CbmAnaDielectronTask::Init: No TrdTrack array!" << endl;
			return kERROR;
		}
		fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
		if ( !fTrdTrackMatches) {
			cout << "-W- CbmAnaDielectronTask::Init: No TrdTrackMatch array!" << endl;
			return kERROR;
		}
	}//fUseTrd


	if (fUseTof == true){
		fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
		if ( !fTofPoints) {
			cout << "-W- CbmAnaDielectronTask::Init: No TOFPoint array!" << endl;
			return kERROR;
		}

		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if ( !fTofHits) {
			cout << "-W- CbmAnaDielectronTask::Init: No TOFHit array!" << endl;
			return kERROR;
		}
	}//fUseTof

    fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
    if (! fPrimVertex) {
		cout << "-E- CbmAnaDielectronTask::Init: No Primary Vertex!" << endl;
		return kERROR;
    }

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

    fEvents++;
    cout << "-I- CbmAnaDielectronTask event number " << fEvents << endl;
    MCPairs();
    MCPairsBg();
    SingleParticleAcceptance();
    PairAcceptance();
    FillCandidateArray();
    DifferenceSignalAndBg();
    RecoQa();
    PairsReco();
    BgReco();

    cout << "fNofMcEp = " << fNofMcEp << ", per event " << fNofMcEp / (Double_t)fEvents << endl;
    cout << "fNofMcEm = " << fNofMcEm << ", per event " << fNofMcEm / (Double_t)fEvents << endl;
    cout << "fNofAccEp = " << fNofAccEp << ", per event " << fNofAccEp / (Double_t)fEvents << endl;
    cout << "fNofAccEm = " << fNofAccEm << ", per event " << fNofAccEm / (Double_t)fEvents << endl;
    cout << "fNofAccPairs = " << fNofAccPairs << ", per event " << fNofAccPairs / (Double_t)fEvents << endl;
    cout << "fNofMcEpBg = " << fNofMcEpBg << ", per event " << fNofMcEpBg / (Double_t)fEvents << endl;
    cout << "fNofMcEmBg = " << fNofMcEmBg << ", per event " << fNofMcEmBg / (Double_t)fEvents << endl;
    cout << "fNofAccEmBg = " << fNofAccEmBg << ", per event " << fNofAccEmBg / (Double_t)fEvents <<endl;
    cout << "fNofAccEpBg = " << fNofAccEpBg << ", per event " << fNofAccEpBg / (Double_t)fEvents <<endl;
    cout << "fNofGlobalTracks = " << fNofGlobalTracks << ", per event " << fNofGlobalTracks / (Double_t)fEvents << endl;

    cout << "fNofRecPairs = " << fNofRecPairs << ", per event " << fNofRecPairs / (Double_t)fEvents << endl;
    cout << "fNofRichIdPairs = " << fNofRichIdPairs << ", per event " << fNofRichIdPairs / (Double_t)fEvents << endl;
    cout << "fNofTrdIdPairs = " << fNofTrdIdPairs << ", per event " << fNofTrdIdPairs / (Double_t)fEvents << endl;
    cout << "fNofTofIdPairs = " << fNofTofIdPairs << ", per event " << fNofTofIdPairs / (Double_t)fEvents << endl;
    cout << "fNofPtcutPairs = " << fNofPtcutPairs << ", per event " << fNofPtcutPairs /(Double_t)fEvents << endl;

    cout << "fNofRecBg = " << fNofRecBg << ", per event " << fNofRecBg /(Double_t)fEvents << endl;
    cout << "fNofRichIdBg = " << fNofRichIdBg << ", per event " << fNofRichIdBg /(Double_t)fEvents << endl;
    cout << "fNofTrdIdBg = " << fNofTrdIdBg << ", per event " << fNofTrdIdBg /(Double_t)fEvents << endl;
    cout << "fNofTofIdBg = " << fNofTofIdBg << ", per event " << fNofTofIdBg /(Double_t)fEvents << endl;
    cout << "fNofPtcutBg = " << fNofPtcutBg << ", per event " << fNofPtcutBg /(Double_t)fEvents << endl;

}// Exec

void CbmAnaDielectronTask::MCPairs()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++){
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = mctrack->GetPdgCode();
        if (motherId == -1 && pdg == -11) fNofMcEp++;
        if (motherId == -1 && pdg == 11) fNofMcEm++;
        
        
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
       
    } // nmber of e-/e+

        
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

            //Fill histogramms
            fh_mc_signal_pty->Fill(p.rapidity,p.pt,fWeight);
            fh_mc_signal_mom->Fill(p.momentumMag,fWeight);
            fh_mc_signal_minv->Fill(p.minv, fWeight);
        } //iM
    } //iP

} //MC Pairs

void CbmAnaDielectronTask::MCPairsBg()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++){
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t pdg = mctrack->GetPdgCode();
        Int_t motherId = mctrack->GetMotherId();
        if (pdg == -11 && motherId != -1) fNofMcEpBg++;
        if (pdg == 11 && motherId != -1) fNofMcEmBg++;
               
      
    } // nmber of e-/e+

        
    for (Int_t iP = 0; iP < nMcTracks; iP++) {
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
        Int_t pdg = mctrackP->GetPdgCode();
        if ( !(pdg == -11)) continue;
     
        for (Int_t iM = 0; iM < nMcTracks; iM++) {
            if (iP == iM) continue;
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
            pdg = mctrackM->GetPdgCode();
            if ( !(pdg == 11)) continue;

            KinematicParams p = CalculateKinematicParams(mctrackP,mctrackM);        

             //Fill histogramms
            fh_mc_bg_mom->Fill(p.momentumMag);
            fh_mc_bg_minv->Fill(p.minv);
        } //iM
    } //iP

} //MC BG Pairs

void CbmAnaDielectronTask::SingleParticleAcceptance()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++) {
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = mctrack->GetPdgCode();
        Int_t nMvdPoints = mctrack->GetNPoints(kMVD);
        Int_t nStsPoints = mctrack->GetNPoints(kSTS);
        Int_t nRichPoints = mctrack->GetNPoints(kRICH);

        Bool_t isAcc = ( nMvdPoints+nStsPoints >= 4);// && nRichPoints > 0) 
        if (motherId == -1 && pdg == -11 && isAcc) fNofAccEp++;
        if (motherId == -1 && pdg == 11 && isAcc) fNofAccEm++; 
        if (motherId != -1 && pdg == -11 && isAcc) fNofAccEpBg++;
        if (motherId != -1 && pdg == 11 && isAcc) fNofAccEmBg++;
 

        // mother PDG accepted e-/e+
        Int_t mcMotherPdg = 0;
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
        Int_t nRichPoints = mctrackP->GetNPoints(kRICH);
        Bool_t isAccP = (nMvdPoints+nStsPoints >= 4);// && (nRichPoints > 0) ;
        if (isAccP){
            for (Int_t iM = 0; iM < nMcTracks; iM++) {
                if (iP == iM) continue;
                CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
                motherId = mctrackM->GetMotherId();
                pdg = mctrackM->GetPdgCode();
                if ( !(motherId == -1 && pdg == 11 )) continue;

                nMvdPoints = mctrackM->GetNPoints(kMVD);
                nStsPoints = mctrackM->GetNPoints(kSTS);
                nRichPoints = mctrackM->GetNPoints(kRICH);
                Bool_t isAccM = (nMvdPoints+nStsPoints >= 4);// && (nRichPoints > 0) ;
                if (isAccP && isAccM) fNofAccPairs++;

                 KinematicParams p = CalculateKinematicParams(mctrackP,mctrackM);        
                
                //Fill histogramms
                if (isAccP && isAccM) {
                    fh_acc_signal_pty->Fill(p.rapidity, p.pt,fWeight);
                    fh_acc_signal_mom->Fill(p.momentumMag,fWeight);
                    fh_acc_signal_minv->Fill(p.minv,fWeight);
                }
            }//iM
        }
    }//iP

} // PairsAcceptance


void CbmAnaDielectronTask::FillCandidateArray()
{
    fCandidates.clear();
    Int_t nGTracks = fGlobalTracks->GetEntriesFast();
    fNofGlobalTracks = fNofGlobalTracks+nGTracks;
    fCandidates.reserve(nGTracks);

    for (Int_t i = 0; i < nGTracks; i++){
        DielectronCandidate cand;
        cand.isMCSignalElectron = false;
        cand.isRichElectron = false;
        cand.isTofElectron = false;
        cand.isTrdElectron = false;

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

	    cand.chiPrimary = fKFFitter.GetChiToVertex(stsTrack);
        // Fit tracks to the primary vertex
        FairTrackParam vtxTrack;  
        fKFFitter.FitToVertex(stsTrack, fPrimVertex, &vtxTrack);

        vtxTrack.Position(cand.position);
        vtxTrack.Momentum(cand.momentum);

        cand.mass     = TDatabasePDG::Instance()->GetParticle(11)->Mass();
        cand.charge   = (vtxTrack.GetQp() > 0) ?1 :-1;
        cand.energy   = sqrt(cand.momentum.Mag2() + cand.mass * cand.mass);
        cand.rapidity = 0.5*TMath::Log((cand.energy + cand.momentum.Z()) /
                    (cand.energy - cand.momentum.Z()));
       

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
            if (pdg == 11 && motherId == -1) cand.isMCSignalElectron = true;
	    }

        cand.isRichElectron = IsRichElectron(richRing, cand.momentum.Mag());

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

        cand.isTrdElectron = IsTrdElectron(trdTrack);
        // check RICH -STS -TRD matches
//    if (cand.stsMCTrackId == cand.richMCTrackId && 
//          cand.stsMCTrackId == cand.trdMCTrackId){}  
  
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

        cand.isTofElectron = IsTofElectron(gTrack, cand.momentum.Mag());
    //check RICH-STS-TRD-ToF matches
 /*   if (cand.stsMCTrackId == cand.richMCTrackId &&
        cand.stsMCTrackId == cand.trdMCTrackId &&
        cand.stsMCTrackId == cand.tofMCTrackId) {}    
*/
        fCandidates.push_back(cand);

    }// global tracks
}

void CbmAnaDielectronTask::PairsReco()
{
    Int_t ncand = fCandidates.size();
    for (Int_t iP = 0; iP < ncand; iP++){
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(fCandidates[iP].stsMCTrackId);
        if (fCandidates[iP].isMCSignalElectron && fCandidates[iP].charge > 0){
            for (Int_t iM = 0; iM < ncand; iM++){
                CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(fCandidates[iM].stsMCTrackId);
                if (fCandidates[iM].isMCSignalElectron && iM != iP && fCandidates[iM].charge < 0){
                    
                    KinematicParams pMC = CalculateKinematicParams(mctrackP,mctrackM); 
                    fh_reco_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);

                    KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 
                    fh_rec_signal_minv->Fill(pRec.minv, fWeight);
                    fNofRecPairs++;

                    if (fCandidates[iP].isRichElectron && fCandidates[iM].isRichElectron) {
                        fNofRichIdPairs++;
                        fh_rich_id_signal_minv->Fill(pRec.minv, fWeight);
                        fh_rich_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                    }
                    if ((fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron) && 
                        (fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron)) {
                        fNofTrdIdPairs++;
                        fh_trd_id_signal_minv->Fill(pRec.minv, fWeight);
                        fh_trd_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                    }

                    if ((fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron) &&
                        (fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron && fCandidates[iM].isTofElectron)) {
                        fNofTofIdPairs++;
                        fh_tof_id_signal_minv->Fill(pRec.minv, fWeight);
                        fh_tof_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                        fh_angle_signal->Fill(pRec.angle);
                    }
                    if ((fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron) &&
                        (fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron && fCandidates[iM].isTofElectron)) {
                        if (fCandidates[iP].momentum.Perp()>fPtCut && fCandidates[iM].momentum.Perp() > fPtCut){
                            fNofPtcutPairs++;
                            fh_ptcut_signal_minv->Fill(pRec.minv, fWeight);
                            fh_ptcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                            if (pRec.angle >fAngleCut) {
                                fh_anglecut_signal_minv->Fill(pRec.minv, fWeight);
                                fh_anglecut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                            } //opening angle cut

                        }//Pt cut
                    }
                    
                } // if
            }//iM
        }
    } //iP
}

void CbmAnaDielectronTask::BgReco()
{
    Int_t ncand = fCandidates.size();
    for (Int_t i = 0; i < ncand; i++){
        if(!fCandidates[i].isMCSignalElectron) {
            fNofRecBg++;
            if (fCandidates[i].isRichElectron) fNofRichIdBg++;
            if (fCandidates[i].isRichElectron && fCandidates[i].isTrdElectron) fNofTrdIdBg++;
            if (fCandidates[i].isRichElectron && fCandidates[i].isTrdElectron && fCandidates[i].isTofElectron) fNofTofIdBg++;                                              
        }
    }

    for (Int_t iP = 0; iP < ncand; iP++){
        if (fCandidates[iP].charge < 0) continue; 
        for (Int_t iM = 0; iM < ncand; iM++){
            if (fCandidates[iM].charge > 0) continue; 
            if (iM == iP ) continue;

            KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 

            if (!(fCandidates[iP].isMCSignalElectron && fCandidates[iM].isMCSignalElectron)) {
                fh_rec_bg_minv->Fill(pRec.minv);
        
                if (fCandidates[iP].isRichElectron && fCandidates[iM].isRichElectron){
                    fh_rich_id_bg_minv->Fill(pRec.minv);
                }
                if (fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && 
                    fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron) {
                    fh_trd_id_bg_minv->Fill(pRec.minv);
                } 
                if (fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron && 
                    fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron && fCandidates[iM].isTofElectron) {
                    fh_tof_id_bg_minv->Fill(pRec.minv);
                    fh_angle_bg->Fill(pRec.angle);   
                }
                  if ((fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron) &&
                     (fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron && fCandidates[iM].isTofElectron)) {
                        if (fCandidates[iP].momentum.Perp()>fPtCut && fCandidates[iM].momentum.Perp() > fPtCut){
                            fNofPtcutBg++;
                            fh_ptcut_bg_minv->Fill(pRec.minv);
                            if (pRec.angle >fAngleCut) {
                                fh_anglecut_bg_minv->Fill(pRec.minv);
                            } //opening angle cut

                        }//Pt cut
                    }
        
            } // if
        } // iM 
                                               
    } // iP
}

Bool_t CbmAnaDielectronTask::IsRichElectron(CbmRichRing * ring, Double_t momentum)
{   
    if (fUseRichAnn == false){
        Double_t axisA = ring->GetAaxis();
        Double_t axisB = ring->GetBaxis();
        Double_t dist = ring->GetDistance();
        if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&
           fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && dist < fDistCut) return true;
        return false;
    } else {
       if (fElIdAnn->DoSelect(ring, momentum) > fRichAnnCut) return true;
       return false;
       }
   return false;
}


Bool_t CbmAnaDielectronTask::IsTrdElectron(CbmTrdTrack* trdTrack)
{
    Double_t ann = trdTrack->GetPidANN();
    if (ann > fTrdAnnCut) return true;

    return false;
}

Bool_t CbmAnaDielectronTask::IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum)
{
    // Get the tracklength
    Double_t trackLength = gTrack->GetLength() / 100.;

    // Calculate time of flight from TOF hit
    Int_t tofIndex = gTrack->GetTofHitIndex();
    CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
    if (NULL == tofHit)
        return false;
    Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

    // Calculate mass squared
    Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time
            / trackLength, 2) - 1);
    if (momentum >= 1.) {
        if (mass2 < (0.01 + (momentum - 1.) * 0.09))
            return true;
    } else {
        if (mass2 < 0.)
            return true;//fTofM2
    }
    return false;
}

KinematicParams CbmAnaDielectronTask::CalculateKinematicParams(CbmMCTrack* mctrackP, CbmMCTrack* mctrackM)
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

KinematicParams CbmAnaDielectronTask::CalculateKinematicParams(DielectronCandidate* candP, DielectronCandidate* candM)
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
    Int_t nCand = fCandidates.size(); 
    for (Int_t i = 0; i < nCand; i++){
        if (!(fCandidates[i].isRichElectron && 
             fCandidates[i].isTrdElectron && 
             fCandidates[i].isTofElectron)) continue;

        if (fCandidates[i].isMCSignalElectron == true ){
            fh_pt_signal->Fill(fCandidates[i].momentum.Perp(), fWeight);
            fh_chi2_prim_signal->Fill(fCandidates[i].chiPrimary);
            fh_position_signal->Fill(fCandidates[i].position.Mag());
            
        } else {
            fh_pt_bg->Fill(fCandidates[i].momentum.Perp());
            fh_chi2_prim_bg->Fill(fCandidates[i].chiPrimary);
            fh_position_bg->Fill(fCandidates[i].position.Mag());
        }
    }
}

void CbmAnaDielectronTask::RecoQa()
{
    Int_t ncand = fCandidates.size();
    for (Int_t i = 0; i < ncand; i++){
        if (fCandidates[i].isMCSignalElectron == true){
            CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].stsMCTrackId);
            TVector3 momMC;
            mcTrack1->GetMomentum(momMC);
            fh_rec_mc_mom_signal->Fill(100.*(momMC.Mag()-fCandidates[i].momentum.Mag()) / momMC.Mag());
        }
            
    }
}



void CbmAnaDielectronTask::Finish()
{
    Double_t scale = 1./(Double_t)fEvents;
    fh_mc_signal_pty->Scale(scale);
    fh_acc_signal_pty->Scale(scale);
    fh_mc_signal_mom->Scale(scale);
    fh_mc_bg_mom->Scale(scale);
    fh_acc_signal_mom->Scale(scale);
    fh_mc_signal_minv->Scale(scale);
    fh_mc_bg_minv->Scale(scale);
    fh_acc_signal_minv->Scale(scale);
    fh_mc_mother_pdg->Scale(scale);
    fh_acc_mother_pdg->Scale(scale);
    fh_mc_vertex_gamma_xz->Scale(scale);
    fh_mc_vertex_gamma_yz->Scale(scale);
    fh_mc_vertex_gamma_xy->Scale(scale);
    fh_rec_mc_mom_signal->Scale(1./fh_rec_mc_mom_signal->Integral());
    fh_rec_signal_minv->Scale(scale);
    fh_rich_id_signal_minv->Scale(scale);
    fh_trd_id_signal_minv->Scale(scale);
    fh_tof_id_signal_minv->Scale(scale);
    fh_rec_bg_minv->Scale(scale);
    fh_rich_id_bg_minv->Scale(scale);
    fh_trd_id_bg_minv->Scale(scale);
    fh_tof_id_bg_minv->Scale(scale);
    fh_pt_signal->Scale(1./fh_pt_signal->Integral());
    fh_pt_bg->Scale(1./fh_pt_bg->Integral());
    fh_position_signal->Scale(1./fh_position_signal->Integral());
    fh_position_bg-> Scale(1./fh_position_bg->Integral());
    fh_reco_signal_pty->Scale(scale);
    fh_rich_id_signal_pty->Scale(scale); 
    fh_trd_id_signal_pty->Scale(scale); 
    fh_tof_id_signal_pty->Scale(scale);
    fh_angle_signal->Scale(1./ fh_angle_signal->Integral());
    fh_angle_bg->Scale(1./ fh_angle_bg->Integral());
    fh_ptcut_signal_minv->Scale(scale);
    fh_anglecut_signal_minv->Scale(scale);
    fh_ptcut_bg_minv->Scale(scale);
    fh_anglecut_bg_minv->Scale(scale);
    fh_ptcut_signal_pty->Scale(scale);
    fh_anglecut_signal_pty->Scale(scale);


    fh_chi2_prim_signal->Scale(1./fh_chi2_prim_signal->Integral());
    fh_chi2_prim_bg->Scale(1./fh_chi2_prim_bg->Integral());

    fh_acc_signal_pty_eff->Divide(fh_acc_signal_pty, fh_mc_signal_pty);
//    fh_acc_signal_mom->Sumw2();
//    fh_mc_signal_mom->Sumw2();
    fh_acc_signal_mom_eff->Divide(fh_acc_signal_mom, fh_mc_signal_mom,1,1,"B");

    fh_mc_signal_pty->Write();
    fh_acc_signal_pty->Write();
    fh_acc_signal_pty_eff->Write();
    fh_mc_signal_mom->Write();
    fh_mc_bg_mom->Write();
    fh_acc_signal_mom->Write();
    fh_acc_signal_mom_eff->Write();
    fh_mc_signal_minv->Write();
    fh_mc_bg_minv->Write();
    fh_acc_signal_minv->Write();
    fh_mc_mother_pdg->Write();
    fh_acc_mother_pdg->Write();
    fh_chi2_prim_signal->Write();
    fh_chi2_prim_bg->Write();
    fh_mc_vertex_gamma_xz->Write();
    fh_mc_vertex_gamma_yz->Write();
    fh_mc_vertex_gamma_xy->Write();
    fh_rec_mc_mom_signal->Write();
    fh_rec_signal_minv->Write();
    fh_rich_id_signal_minv->Write();
    fh_trd_id_signal_minv->Write();
    fh_tof_id_signal_minv->Write();
    fh_rec_bg_minv->Write();
    fh_rich_id_bg_minv->Write();
    fh_trd_id_bg_minv->Write();
    fh_tof_id_bg_minv->Write();
    fh_pt_signal->Write();
    fh_pt_bg->Write();
    fh_position_signal->Write();
    fh_position_bg->Write();
    fh_reco_signal_pty->Write();
    fh_rich_id_signal_pty->Write();
    fh_trd_id_signal_pty->Write();
    fh_tof_id_signal_pty->Write();
    fh_angle_signal->Write();
    fh_angle_bg->Write();
    fh_ptcut_signal_minv->Write();
    fh_anglecut_signal_minv->Write();
    fh_ptcut_bg_minv->Write();
    fh_anglecut_bg_minv->Write();
    fh_ptcut_signal_pty->Write();
    fh_anglecut_signal_pty->Write();
}

void CbmAnaDielectronTask::WriteOutput()
{
    // --- Normalize histogramms ---
    Double_t scale = 1./(Double_t)fEvents;

}
