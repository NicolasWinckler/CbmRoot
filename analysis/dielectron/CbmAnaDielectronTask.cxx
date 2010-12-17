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
    fWeight = 0.0;
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
    fNofChiPrimCutPairs = 0;
    fNofChiPrimCutBg = 0;
    fNofPtcutPairs = 0;
    fNofPtcutBg = 0;
    fNofTTcutPairs = 0;
    fNofTTcutBg = 0;

    fh_mc_signal_pty = new TH2D("fh_mc_signal_pty","fh_mc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_acc_signal_pty = new TH2D("fh_acc_signal_pty","fh_acc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);

    fh_mc_signal_mom = new TH1D("fh_mc_signal_mom", "fh_mc_signal_mom;momentum [GeV/c];yeild",100, 0., 15.);
    fh_acc_signal_mom = new TH1D("fh_acc_signal_mom","fh_acc_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_reco_signal_mom = new TH1D("fh_reco_signal_mom","fh_reco_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_rich_id_signal_mom = new TH1D("fh_rich_id_signal_mom","fh_rich_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_trd_id_signal_mom = new TH1D("fh_trd_id_signal_mom","fh_trd_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_tof_id_signal_mom = new TH1D("fh_tof_id_signal_mom","fh_tof_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_chi_prim_signal_mom = new TH1D("fh_chi_prim_signal_mom","fh_chi_prim_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_ptcut_signal_mom = new TH1D("fh_ptcut_signal_mom","fh_ptcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_anglecut_signal_mom = new TH1D("fh_anglecut_signal_mom","fh_anglecut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_pi0cut_signal_mom = new TH1D("fh_pi0cut_signal_mom","fh_pi0cut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_ttcut_signal_mom = new TH1D("fh_ttcut_signal_mom","fh_ttcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_apcut_signal_mom = new TH1D("fh_apcut_signal_mom","fh_apcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    
    fh_mc_signal_minv = new TH1D("fh_mc_signal_minv","fh_mc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_acc_signal_minv = new TH1D("fh_acc_signal_minv","fh_acc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
    fh_acc_mother_pdg = new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
    
    fh_mc_vertex_gamma_xz = new TH2D("fh_mc_vertex_gamma_xz","fh_mc_vertex_gamma_xz;Z [cm];X [cm]",200, -10., 110., 200, -100.,100.);
    fh_mc_vertex_gamma_yz = new TH2D("fh_mc_vertex_gamma_yz", "fh_mc_vertex_gamma_yz;Z [cm];Y [cm]", 200, -10., 110., 200, -100., 100.);
    fh_mc_vertex_gamma_xy = new TH2D("fh_mc_vertex_gamma_xy","fh_mc_vertex_gamma_xy;X [cm];Y [cm]", 200, -100.,100., 200, -100., 100.); 

    fh_rec_mc_mom_signal = new TH1D("fh_rec_mc_mom_signal","fh_rec_mc_mom_signal;#Delta p/p [%];yeild",100, -10., 10.);
    fh_mom_res_vs_mom_signal = new TH2D("fh_mom_res_vs_mom_signal", "fh_mom_res_vs_mom_signal;p [GeV/c];#Delta p/p [%]",100, 0., 15., 100, -10., 10.);
    fh_mean_mom_vs_mom_signal = new TH1D("fh_mean_mom_vs_mom_signal","fh_mean_mom_vs_mom_signal",100, 0., 15.);
    fh_count_mom_vs_mom_signal = new TH1D("fh_count_mom_vs_mom_signal","fh_count_mom_vs_mom_signal", 100, 0., 15.);

//signal minv
    fh_rec_signal_minv = new TH1D("fh_rec_signal_minv","fh_rec_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_rich_id_signal_minv = new TH1D("fh_rich_id_signal_minv","fh_rich_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_signal_minv = new TH1D("fh_trd_id_signal_minv", "fh_trd_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_signal_minv = new TH1D("fh_tof_id_signal_minv","fh_tof_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_chi_prim_signal_minv = new TH1D("fh_chi_prim_signal_minv","fh_chi_prim_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);    
    fh_ptcut_signal_minv = new TH1D("fh_ptcut_signal_minv","fh_ptcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_signal_minv = new TH1D("fh_anglecut_signal_minv","fh_anglecut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_pi0cut_signal_minv = new TH1D("fh_pi0cut_signal_minv","fh_pi0cut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ttcut_signal_minv = new TH1D("fh_ttcut_signal_minv","fh_ttcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_apcut_signal_minv = new TH1D("fh_apcut_signal_minv","fh_apcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
//BG minv
    fh_rec_bg_minv = new TH1D("fh_rec_bg_minv","fh_rec_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_rich_id_bg_minv = new TH1D("fh_rich_id_bg_minv","fh_rich_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_bg_minv = new TH1D("fh_trd_id_bg_minv","fh_trd_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_bg_minv = new TH1D("fh_tof_id_bg_minv","fh_tof_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_chi_prim_bg_minv = new TH1D("fh_chi_prim_bg_minv","fh_chi_prim_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);    
    fh_ptcut_bg_minv = new TH1D("fh_ptcut_bg_minv","fh_ptcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_bg_minv = new TH1D("fh_anglecut_bg_minv","fh_anglecut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_pi0cut_bg_minv = new TH1D("fh_pi0cut_bg_minv","fh_pi0cut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ttcut_bg_minv = new TH1D("fh_ttcut_bg_minv","fh_ttcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_apcut_bg_minv = new TH1D("fh_apcut_bg_minv","fh_apcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
//pi0 minv
    fh_rec_pi0_minv = new TH1D("fh_rec_pi0_minv","fh_rec_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_rich_id_pi0_minv = new TH1D("fh_rich_id_pi0_minv","fh_rich_id_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_pi0_minv = new TH1D("fh_trd_id_pi0_minv","fh_trd_id_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_pi0_minv = new TH1D("fh_tof_id_pi0_minv","fh_tof_id_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_chi_prim_pi0_minv = new TH1D("fh_chi_prim_pi0_minv","fh_chi_prim_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ptcut_pi0_minv = new TH1D("fh_ptcut_pi0_minv","fh_ptcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_pi0_minv = new TH1D("fh_anglecut_pi0_minv","fh_anglecut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_pi0cut_pi0_minv = new TH1D("fh_pi0cut_pi0_minv","fh_pi0cut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ttcut_pi0_minv = new TH1D("fh_ttcut_pi0_minv","fh_ttcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_apcut_pi0_minv = new TH1D("fh_apcut_pi0_minv","fh_apcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_reco_signal_pty = new TH2D("fh_reco_signal_pty","fh_reco_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_signal_pty = new TH2D("fh_rich_id_signal_pty","fh_rich_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_trd_id_signal_pty = new TH2D("fh_trd_id_signal_pty","fh_trd_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_tof_id_signal_pty = new TH2D("fh_tof_id_signal_pty","fh_tof_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_chi_prim_signal_pty = new TH2D("fh_chi_prim_signal_pty","fh_chi_prim_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);    
    fh_ptcut_signal_pty = new TH2D("fh_ptcut_signal_pty","fh_ptcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_anglecut_signal_pty = new TH2D("fh_anglecut_signal_pty","fh_anglecut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_signal_pty = new TH2D("fh_pi0cut_signal_pty","fh_pi0cut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_ttcut_signal_pty = new TH2D("fh_ttcut_signal_pty","fh_ttcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_apcut_signal_pty = new TH2D("fh_apcut_signal_pty","fh_apcut_signal_pty;Rapidity;p_{t} [GeV/c]",40, 0., 4., 20, 0., 2.);

// cut distribution
    fh_angle_signal = new TH1D("fh_angle_signal","fh_angle_signal;opening angle [deg];yeild", 360, 0., 180.);
    fh_angle_bg = new TH1D("fh_angle_bg","fh_angle_bg;opening angle [deg];yeild", 360, 0., 180.); 
    fh_position_signal = new TH1D("fh_position_signal","fh_position_signal", 100, 0., 0.02);
    fh_position_bg = new TH1D("fh_position_bg","fh_position_bg",100, 0., 0.02);
    fh_pt_signal = new TH1D("fh_pt_signal","fh_pt_signal;p_{t} [GeV/c];yeild",200, 0., 2.);
    fh_pt_bg = new TH1D("fh_pt_bg","fh_pt_bg;p_{t} [GeV/c];yeild",200, 0., 2.);
    fh_mom_signal = new TH1D("fh_mom_signal","fh_mom_signal;p [GeV/c];yeild",200, 0., 10.);
    fh_mom_bg = new TH1D("fh_mom_bg","fh_mom_bg;p [GeV/c];yeild",200, 0., 10.);
    fh_chi2_prim_signal = new TH1D("fh_chi2_prim_signal", "fh_chi2_prim_signal;chi2,yeild", 200, 0., 20.);
    fh_chi2_prim_bg = new TH1D("fh_chi2_prim_bg","fh_chi2_prim_bg;chi2,yeild", 200, 0., 20.);
    fh_ttcut_signal = new TH2D("fh_ttcut_signal", "fh_ttcut_signal;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
    fh_ttcut_bg = new TH2D("fh_ttcut_bg","fh_ttcut_bg;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
    fh_apcut_signal = new TH2D("fh_apcut_signal", "fh_apcut_signal;#alpha;p_{t} [GeV/c]", 100, -1., 1., 200, 0., 1.);
    fh_apcut_bg = new TH2D("fh_apcut_bg","fh_apcut_bg;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);
    fh_ttcut_pi0 = new TH2D("fh_ttcut_pi0","fh_ttcut_pi0;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
    fh_apcut_pi0 = new TH2D("fh_apcut_pi0","fh_apcut_pi0;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);
    fh_ttcut_gamma = new TH2D("fh_ttcut_gamma","fh_ttcut_gamma;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
    fh_apcut_gamma = new TH2D("fh_apcut_gamma","fh_apcut_gamma;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.);

    //ID cut distributions
    fh_rich_ann_signal = new TH1D("fh_rich_ann_signal", "fh_rich_ann_signal;ann output;yeild", 100, -1.1, 1.1);
    fh_rich_ann_bg = new TH1D("fh_rich_ann_bg", "fh_rich_ann_bg;ann output;yeild", 100, -1.1, 1.1);
    fh_trd_ann_signal = new TH1D("fh_trd_ann_signal", "fh_trd_ann_signal;ann output;yeild", 100, -1.1, 1.1);
    fh_trd_ann_bg = new TH1D("fh_trd_ann_bg", "fh_trd_ann_bg;ann output;yeild", 100, -1.1, 1.1);
    fh_tof_m2_signal = new TH2D("fh_tof_m2_signal","fh_tof_m2_signal;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2);
    fh_tof_m2_bg = new TH2D("fh_tof_m2_bg","fh_tof_m2_bg;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2);

    SetDefaultIdParameters();
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

void CbmAnaDielectronTask::SetDefaultIdParameters()
{
    fTrdAnnCut = 0.85;
    fRichAnnCut = 0.1;
    fUseRichAnn = true;
    fMeanA = -1.;
    fMeanB = -1.;
    fRmsA = -1.;
    fRmsB = -1.;
    fRmsCoeff = -1.;
    fDistCut = -1.;
    fPtCut = 0.2;
    fAngleCut = 12.;
    fChiPrimCut = 3.;
    fPi0Cut = 0.025;
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
    CheckGammaConvAndPi0();
    CheckTrackTopologyCut();
    PairsReco();
    BgReco();
    Pi0Reco();

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
    cout << "fNofChiPrimCutPairs = " << fNofChiPrimCutPairs << ", per event " << fNofChiPrimCutPairs /(Double_t)fEvents << endl;    
    cout << "fNofPtcutPairs = " << fNofPtcutPairs << ", per event " << fNofPtcutPairs /(Double_t)fEvents << endl;
    cout << "fNofTTcutPairs = " << fNofTTcutPairs << ", per event " << fNofTTcutPairs /(Double_t)fEvents << endl;

    cout << "fNofRecBg = " << fNofRecBg << ", per event " << fNofRecBg /(Double_t)fEvents << endl;
    cout << "fNofRichIdBg = " << fNofRichIdBg << ", per event " << fNofRichIdBg /(Double_t)fEvents << endl;
    cout << "fNofTrdIdBg = " << fNofTrdIdBg << ", per event " << fNofTrdIdBg /(Double_t)fEvents << endl;
    cout << "fNofTofIdBg = " << fNofTofIdBg << ", per event " << fNofTofIdBg /(Double_t)fEvents << endl;
    cout << "fNofChiPrimCutBg = " << fNofChiPrimCutBg << ", per event " << fNofChiPrimCutBg /(Double_t)fEvents << endl;
    cout << "fNofPtcutBg = " << fNofPtcutBg << ", per event " << fNofPtcutBg /(Double_t)fEvents << endl;
    cout << "fNofTTcutBg = " << fNofTTcutBg << ", per event " << fNofTTcutBg /(Double_t)fEvents << endl;

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
       
    } // number of e-/e+

        
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
    } // number of e-/e+
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
        cand.isMCPi0Electron = false;
        cand.isMCGammaElectron = false;
        cand.MCMotherId = -1;
        cand.isRichElectron = false;
        cand.isTofElectron = false;
        cand.isTrdElectron = false;
        cand.isPi0 = false;
        cand.isTTCutElectron = false;

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

			}
	    }

        IsRichElectron(richRing, cand.momentum.Mag(), &cand);

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

        IsTrdElectron(trdTrack, &cand);
  
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

        IsTofElectron(gTrack, cand.momentum.Mag(), &cand);  

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
                    fh_reco_signal_mom->Fill(pMC.momentumMag,fWeight);

                    KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 
                    fh_rec_signal_minv->Fill(pRec.minv, fWeight);
                    fNofRecPairs++;

                    if (fCandidates[iP].isRichElectron && fCandidates[iM].isRichElectron) {
                        fNofRichIdPairs++;
                        fh_rich_id_signal_minv->Fill(pRec.minv, fWeight);
                        fh_rich_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
                        fh_rich_id_signal_mom->Fill(pMC.momentumMag, fWeight);

						if (fCandidates[iP].isTrdElectron && fCandidates[iM].isTrdElectron) {
							fNofTrdIdPairs++;
							fh_trd_id_signal_minv->Fill(pRec.minv, fWeight);
							fh_trd_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
							fh_trd_id_signal_mom->Fill(pMC.momentumMag, fWeight);

							if (fCandidates[iP].isTofElectron && fCandidates[iM].isTofElectron) {
								fNofTofIdPairs++;
								fh_tof_id_signal_minv->Fill(pRec.minv, fWeight);
								fh_tof_id_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
								fh_angle_signal->Fill(pRec.angle);
								fh_tof_id_signal_mom->Fill(pMC.momentumMag,fWeight);

								if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iM].chiPrimary < fChiPrimCut){
									fNofChiPrimCutPairs++;
									fh_chi_prim_signal_minv->Fill(pRec.minv, fWeight);
									fh_chi_prim_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
									fh_chi_prim_signal_mom->Fill(pMC.momentumMag, fWeight);

									if (fCandidates[iP].momentum.Perp() > fPtCut && fCandidates[iM].momentum.Perp() > fPtCut){
										fNofPtcutPairs++;
										fh_ptcut_signal_minv->Fill(pRec.minv, fWeight);
										fh_ptcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
										fh_ptcut_signal_mom->Fill(pMC.momentumMag, fWeight);

										if (pRec.angle >fAngleCut) {
											fh_anglecut_signal_minv->Fill(pRec.minv, fWeight);
											fh_anglecut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
											fh_anglecut_signal_mom->Fill(pMC.momentumMag, fWeight);

											if (!fCandidates[iP].isPi0 && !fCandidates[iM].isPi0){
												fh_pi0cut_signal_minv->Fill(pRec.minv, fWeight);
												fh_pi0cut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
												fh_pi0cut_signal_mom->Fill(pMC.momentumMag, fWeight);

												if (fCandidates[iP].isTTCutElectron && fCandidates[iM].isTTCutElectron){
													fNofTTcutPairs++;
													fh_ttcut_signal_minv->Fill(pRec.minv, fWeight);
													fh_ttcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
													fh_ttcut_signal_mom->Fill(pMC.momentumMag, fWeight);

													Double_t pt = 0;
													Double_t alfa = 0;
													CalculateArmPodParams(&fCandidates[iP], &fCandidates[iM], alfa, pt);
													if (pt > 0.25) {
														fh_apcut_signal_minv->Fill(pRec.minv, fWeight);
														fh_apcut_signal_pty->Fill(pMC.rapidity, pMC.pt, fWeight);
														fh_apcut_signal_mom->Fill(pMC.momentumMag, fWeight);

													}//ap cut
												}//tt cut
											}//pi0 cut
										}//opening angle cut
									}//Pt cut
								}// Chi prim cut
							}//isTofElectron
						}//isTrdElectron
                    }//isRichElectron
                }//isMCSignalElectron
            }//iM
        }//isMCSignalElectron
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

					if (fCandidates[iP].isTrdElectron && fCandidates[iM].isTrdElectron) {
						fh_trd_id_bg_minv->Fill(pRec.minv);

						if (fCandidates[iP].isTofElectron && fCandidates[iM].isTofElectron) {
							fh_tof_id_bg_minv->Fill(pRec.minv);
							fh_angle_bg->Fill(pRec.angle);
							if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iM].chiPrimary < fChiPrimCut){
								fNofChiPrimCutBg++;
								fh_chi_prim_bg_minv->Fill(pRec.minv);
								if (fCandidates[iP].momentum.Perp() > fPtCut && fCandidates[iM].momentum.Perp() > fPtCut){
									fNofPtcutBg++;
									fh_ptcut_bg_minv->Fill(pRec.minv);
									if (pRec.angle > fAngleCut) {
										fh_anglecut_bg_minv->Fill(pRec.minv);
										 if (!fCandidates[iP].isPi0 && !fCandidates[iM].isPi0){
											 fh_pi0cut_bg_minv->Fill(pRec.minv);
											 if (fCandidates[iP].isTTCutElectron && fCandidates[iM].isTTCutElectron){
												 fNofTTcutBg++;
												 fh_ttcut_bg_minv->Fill(pRec.minv);

												Double_t pt = 0;
												Double_t alfa = 0;
												CalculateArmPodParams(&fCandidates[iP], &fCandidates[iM], alfa, pt);
												if (pt > 0.25) {
													fh_apcut_bg_minv->Fill(pRec.minv);
												} //ap cut
											 }// tt cut
										 } // pi0 cut
									} //opening angle cut
								}//Pt cut
							}//Chi primary cut
						}//isTofElectron
					}//isTrdElectron
                } //isRichElectron
            } // MCSignalElectron
        } // iM                                   
    } // iP
}

void CbmAnaDielectronTask::Pi0Reco()
{
    Int_t ncand = fCandidates.size();

    for (Int_t iP = 0; iP < ncand; iP++){
        if (fCandidates[iP].charge < 0) continue;
        for (Int_t iM = 0; iM < ncand; iM++){
            if (fCandidates[iM].charge > 0) continue;
            if (iM == iP ) continue;
        
            KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]);

			// select only electrons from the same pi0
            if (fCandidates[iP].isMCPi0Electron && fCandidates[iM].isMCPi0Electron
            		&& fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId) {

                fh_rec_pi0_minv->Fill(pRec.minv);

                if (fCandidates[iP].isRichElectron && fCandidates[iM].isRichElectron){
                    fh_rich_id_pi0_minv->Fill(pRec.minv);

					if (fCandidates[iP].isTrdElectron && fCandidates[iM].isTrdElectron) {
						fh_trd_id_pi0_minv->Fill(pRec.minv);

						if (fCandidates[iP].isTofElectron && fCandidates[iM].isTofElectron) {
							fh_tof_id_pi0_minv->Fill(pRec.minv);

							if (fCandidates[iP].chiPrimary < fChiPrimCut && fCandidates[iM].chiPrimary < fChiPrimCut){
								fh_chi_prim_pi0_minv->Fill(pRec.minv);
								if (fCandidates[iP].momentum.Perp() > fPtCut && fCandidates[iM].momentum.Perp() > fPtCut){
									fh_ptcut_pi0_minv->Fill(pRec.minv);
									if (pRec.angle > fAngleCut) {
										fh_anglecut_pi0_minv->Fill(pRec.minv);
										 if (!fCandidates[iP].isPi0 && !fCandidates[iM].isPi0){
											 fh_pi0cut_pi0_minv->Fill(pRec.minv);
											 if (fCandidates[iP].isTTCutElectron && fCandidates[iM].isTTCutElectron){
												 fh_ttcut_pi0_minv->Fill(pRec.minv);

												Double_t pt = 0;
												Double_t alfa = 0;
												CalculateArmPodParams(&fCandidates[iP], &fCandidates[iM], alfa, pt);
												if (pt > 0.25) {
													fh_apcut_pi0_minv->Fill(pRec.minv);
												}//apcut
											 }// tt cut
										 } // pi0 cut
									}//opening angle cut
								}//Pt cut
							}//Chi primary cut
						}//isTofElectron
					}//isTrdElectron
				}//isRichElectron
            }//isMCPi0Electron
        }//iM
    }//iP
}

void CbmAnaDielectronTask::CheckGammaConvAndPi0()
{
    Int_t ncand = fCandidates.size();
    for (Int_t iP = 0; iP < ncand; iP++){
        if (fCandidates[iP].charge < 0) continue; 
        for (Int_t iM = 0; iM < ncand; iM++){
            if (fCandidates[iM].charge > 0) continue; 
            if (iM == iP ) continue;
            if ((fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron) &&
               (fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron && fCandidates[iM].isTofElectron)) {
                
                KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 
                if (pRec.minv < fPi0Cut) {
                    fCandidates[iM].isPi0 = true;
                    fCandidates[iP].isPi0 = true;
                }
            }   
        }
    }
}

void CbmAnaDielectronTask::CheckTrackTopologyCut()
{
    vector<Float_t> angles, mom;
    angles.reserve(50);
    mom.reserve(50);
    Int_t nCand = fCandidates.size();
    Double_t angCut = 3.5;
    Double_t ppCut = 3.5;
    for (Int_t iP = 0; iP < nCand; iP++){
        //if (fCandidates[iP].charge > 0){
            if (fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron){
                angles.clear();
                mom.clear();
                for (Int_t iM = 0; iM < nCand; iM++){
                    // different charges, charge Im != charge iP
                    if ( iM != iP && fCandidates[iM].charge != fCandidates[iP].charge){
                        KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 
                        angles.push_back(pRec.angle);
                        mom.push_back(fCandidates[iM].momentum.Mag());
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
                Double_t val = -1.*(angCut/ppCut)*sqrt_mom + angCut;
                if ( !(sqrt_mom < ppCut && val > minAng) ) fCandidates[iP].isTTCutElectron = true;
            }//if electron
        //}// charge >0
    } //iP
}

void CbmAnaDielectronTask::IsRichElectron(CbmRichRing * ring, Double_t momentum, DielectronCandidate* cand)
{   
    if (fUseRichAnn == false){
        Double_t axisA = ring->GetAaxis();
        Double_t axisB = ring->GetBaxis();
        Double_t dist = ring->GetDistance();
        if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && dist < fDistCut){
            cand->isRichElectron = true;
        } else {
            cand->isRichElectron = false;
        }
    } else {
        Double_t ann = fElIdAnn->DoSelect(ring, momentum);
        if (ann > fRichAnnCut) cand->isRichElectron = true;
        else  cand->isRichElectron = false;
        cand->richAnn = ann;
    }
}


void CbmAnaDielectronTask::IsTrdElectron(CbmTrdTrack* trdTrack, DielectronCandidate* cand)
{
    Double_t ann = trdTrack->GetPidANN();
    if (ann > fTrdAnnCut) cand->isTrdElectron = true;
    else cand->isTrdElectron = false;
    cand->trdAnn = ann;
}

void CbmAnaDielectronTask::IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum, DielectronCandidate* cand)
{
    // Get the tracklength
    Double_t trackLength = gTrack->GetLength() / 100.;

    // Calculate time of flight from TOF hit
    Int_t tofIndex = gTrack->GetTofHitIndex();
    CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
    if (NULL == tofHit){
        cand->isTofElectron = false;
        cand->mass2 = 100.;   
        return;
    }
    Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

    // Calculate mass squared
    Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);
    cand->mass2 =mass2;

    cand->isTofElectron = false;
    if (momentum >= 1.) {
        if (mass2 < (0.013*momentum - 0.003)){
            cand->isTofElectron = true;
            return;
        }
    } else {
        if (mass2 < 0.01){
            cand->isTofElectron = true;
            return;//fTofM2
        }
    }
    cand->isTofElectron = false;
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
//ID cuts    
    Int_t nCand = fCandidates.size(); 
    for (Int_t i = 0; i < nCand; i++){
        if (fCandidates[i].isMCSignalElectron){
            fh_rich_ann_signal->Fill(fCandidates[i].richAnn, fWeight);
            fh_trd_ann_signal->Fill(fCandidates[i].trdAnn, fWeight);
            fh_tof_m2_signal->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2, fWeight);
        } else {
            fh_rich_ann_bg->Fill(fCandidates[i].richAnn);
            fh_trd_ann_bg->Fill(fCandidates[i].trdAnn);
            fh_tof_m2_bg->Fill(fCandidates[i].momentum.Mag(), fCandidates[i].mass2);
        }
    }


    for (Int_t i = 0; i < nCand; i++){
        if (!(fCandidates[i].isRichElectron && 
             fCandidates[i].isTrdElectron && 
             fCandidates[i].isTofElectron)) continue;

        if (fCandidates[i].isMCSignalElectron){
            fh_pt_signal->Fill(fCandidates[i].momentum.Perp(), fWeight);
            fh_chi2_prim_signal->Fill(fCandidates[i].chiPrimary, fWeight);
            fh_position_signal->Fill(fCandidates[i].position.Mag(), fWeight);
            fh_mom_signal->Fill(fCandidates[i].momentum.Mag(), fWeight);
        } else {
            fh_pt_bg->Fill(fCandidates[i].momentum.Perp());
            fh_chi2_prim_bg->Fill(fCandidates[i].chiPrimary);
            fh_position_bg->Fill(fCandidates[i].position.Mag());
            fh_mom_bg->Fill(fCandidates[i].momentum.Mag());
        }
    }

//track topology cut
    vector<Float_t> angles, mom;
    angles.reserve(50);
    mom.reserve(50);
    for (Int_t iP = 0; iP < nCand; iP++){
        //if (fCandidates[iP].charge > 0){
            if (fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron){
                angles.clear();
                mom.clear();
                for (Int_t iM = 0; iM < nCand; iM++){
                    // different charges, charge Im != charge iP
                    if ( iM != iP && fCandidates[iM].charge != fCandidates[iP].charge){
                        KinematicParams pRec = CalculateKinematicParams(&fCandidates[iP],&fCandidates[iM]); 
                        angles.push_back(pRec.angle);
                        mom.push_back(fCandidates[iM].momentum.Mag());
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
                if (fCandidates[iP].isMCSignalElectron) fh_ttcut_signal->Fill(sqrt_mom, minAng, fWeight);
                else fh_ttcut_bg->Fill(sqrt_mom, minAng);
                if (fCandidates[iP].isMCPi0Electron) fh_ttcut_pi0->Fill(sqrt_mom, minAng);
                if (fCandidates[iP].isMCGammaElectron) fh_ttcut_gamma->Fill(sqrt_mom, minAng);
            }//if electron
        //}// charge >0
    } //iP

// Armenteros - Podolansky plot
    for (Int_t iP = 0; iP < nCand; iP++){
        if (fCandidates[iP].charge > 0){
            if (fCandidates[iP].isRichElectron && fCandidates[iP].isTrdElectron && fCandidates[iP].isTofElectron){
                for (Int_t iM = 0; iM < nCand; iM++){
                    if ( iM != iP && fCandidates[iM].charge < 0){
                        if (fCandidates[iM].isRichElectron && fCandidates[iM].isTrdElectron && fCandidates[iM].isTofElectron){
                            Double_t alfa = 0.;
                            Double_t ptt = 0.;
                            CalculateArmPodParams(&fCandidates[iP], &fCandidates[iM], alfa, ptt);
                            if (fCandidates[iP].isMCSignalElectron && fCandidates[iM].isMCSignalElectron){ 
                                fh_apcut_signal->Fill(alfa, ptt, fWeight);
                            } else { 
                                fh_apcut_bg->Fill(alfa, ptt);
                            }
                            if (fCandidates[iP].isMCPi0Electron && fCandidates[iM].isMCPi0Electron &&
                            	fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId)
                            	fh_apcut_pi0->Fill(alfa, ptt);
                            if (fCandidates[iP].isMCGammaElectron && fCandidates[iM].isMCGammaElectron &&
                            	fCandidates[iP].MCMotherId == fCandidates[iM].MCMotherId)
                            	fh_apcut_gamma->Fill(alfa, ptt);
                        }
                    } // if
                }// iM
            }//if electron
        }// charge >0
    } //iP

    //FindClosestMvdHit();
}

// Armenteros - Podolansky plot
void CbmAnaDielectronTask::CalculateArmPodParams(DielectronCandidate* cand1, DielectronCandidate* cand2,
                       Double_t &alpha, Double_t &ptt)
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

void CbmAnaDielectronTask::FindClosestMvdHit()
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
        if (KF->StsStationIDMap[pmh->GetDetectorID()] == 0){ // only first station
            mvdX.push_back(pmh->GetX());
            mvdY.push_back(pmh->GetY());
        }
    }

    Int_t nCand = fCandidates.size(); 
    for (Int_t i = 0; i < nCand; i++){
        if (fCandidates[i].isRichElectron && fCandidates[i].isTrdElectron && fCandidates[i].isTofElectron){
            CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].stsInd);
            if(!track) continue;
            Int_t nthits = track->GetNMvdHits();
            cout <<  nthits << endl;
            for(Int_t ith = 0; ith < nthits; ith++) {
                Int_t iHit = track->GetMvdHitIndex(ith);
                CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
                if(!pmh) continue;
                if (KF->StsStationIDMap[pmh->GetDetectorID()] == 0){ // only first station
                    candX.push_back(pmh->GetX());
                    candY.push_back(pmh->GetY());
                    candInd.push_back(i);
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
        fCandidates[iTrack].dSts = sqrt( mind );
    } // iTrack
} // FindClosestMvdHit

void CbmAnaDielectronTask::RecoQa()
{
    Int_t ncand = fCandidates.size();
    for (Int_t i = 0; i < ncand; i++){
        if (fCandidates[i].isMCSignalElectron == true){
            CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].stsMCTrackId);
            TVector3 momMC;
            mcTrack1->GetMomentum(momMC);
            Double_t dpp = 100.*(momMC.Mag()-fCandidates[i].momentum.Mag()) / momMC.Mag();
            fh_rec_mc_mom_signal->Fill(dpp);
            fh_mom_res_vs_mom_signal->Fill(momMC.Mag(),dpp);
            fh_count_mom_vs_mom_signal->Fill(momMC.Mag());
            fh_mean_mom_vs_mom_signal->Fill(momMC.Mag(),dpp);
  
        }
    }
}

void CbmAnaDielectronTask::Finish()
{
    Double_t scale = 1./(Double_t)fEvents;

    fh_mc_signal_pty->Scale(scale);
    fh_acc_signal_pty->Scale(scale);

    fh_mc_signal_mom->Scale(scale);
    fh_acc_signal_mom->Scale(scale);
    fh_reco_signal_mom->Scale(scale);
    fh_rich_id_signal_mom->Scale(scale);
    fh_trd_id_signal_mom->Scale(scale);
    fh_tof_id_signal_mom->Scale(scale);
    fh_chi_prim_signal_mom->Scale(scale);
    fh_ptcut_signal_mom->Scale(scale);
    fh_anglecut_signal_mom->Scale(scale);
    fh_pi0cut_signal_mom->Scale(scale);
    fh_ttcut_signal_mom->Scale(scale);
    fh_apcut_signal_mom->Scale(scale);

    fh_mc_signal_minv->Scale(scale);
    fh_acc_signal_minv->Scale(scale);

    fh_mc_mother_pdg->Scale(scale);
    fh_acc_mother_pdg->Scale(scale);

    fh_mc_vertex_gamma_xz->Scale(scale);
    fh_mc_vertex_gamma_yz->Scale(scale);
    fh_mc_vertex_gamma_xy->Scale(scale);

    fh_rec_mc_mom_signal->Scale(1./fh_rec_mc_mom_signal->Integral());
    fh_mom_res_vs_mom_signal->Scale(1./fh_mom_res_vs_mom_signal->Integral());
    //fh_mean_mom_vs_mom_signal->Sumw2();
    //fh_count_mom_vs_mom_signal->Sumw2();
    fh_mean_mom_vs_mom_signal->Divide(fh_count_mom_vs_mom_signal);
//Signal minv
    fh_rec_signal_minv->Scale(scale);
    fh_rich_id_signal_minv->Scale(scale);
    fh_trd_id_signal_minv->Scale(scale);
    fh_tof_id_signal_minv->Scale(scale);
    fh_chi_prim_signal_minv->Scale(scale);
    fh_ptcut_signal_minv->Scale(scale);
    fh_anglecut_signal_minv->Scale(scale);    
    fh_pi0cut_signal_minv->Scale(scale);
    fh_ttcut_signal_minv->Scale(scale);
    fh_apcut_signal_minv->Scale(scale);
//BG minv
    fh_rec_bg_minv->Scale(scale);
    fh_rich_id_bg_minv->Scale(scale);
    fh_trd_id_bg_minv->Scale(scale);
    fh_tof_id_bg_minv->Scale(scale);
    fh_chi_prim_bg_minv->Scale(scale);
    fh_ptcut_bg_minv->Scale(scale);
    fh_anglecut_bg_minv->Scale(scale);
    fh_pi0cut_bg_minv->Scale(scale);
    fh_ttcut_bg_minv->Scale(scale);
    fh_apcut_bg_minv->Scale(scale);
//pi0 minv
    fh_rec_pi0_minv->Scale(scale);
    fh_rich_id_pi0_minv->Scale(scale);
    fh_trd_id_pi0_minv->Scale(scale);
    fh_tof_id_pi0_minv->Scale(scale);
    fh_chi_prim_pi0_minv->Scale(scale);
    fh_ptcut_pi0_minv->Scale(scale);
    fh_anglecut_pi0_minv->Scale(scale);
    fh_pi0cut_pi0_minv->Scale(scale);
    fh_ttcut_pi0_minv->Scale(scale);
    fh_apcut_pi0_minv->Scale(scale);

    fh_reco_signal_pty->Scale(scale);
    fh_rich_id_signal_pty->Scale(scale); 
    fh_trd_id_signal_pty->Scale(scale); 
    fh_tof_id_signal_pty->Scale(scale);
    fh_chi_prim_signal_pty->Scale(scale);
    fh_ptcut_signal_pty->Scale(scale);
    fh_anglecut_signal_pty->Scale(scale);
    fh_pi0cut_signal_pty->Scale(scale);
    fh_ttcut_signal_pty->Scale(scale);
    fh_apcut_signal_pty->Scale(scale);

// cuts distribution
    fh_angle_signal->Scale(scale);
    fh_angle_bg->Scale(scale);
    fh_mom_signal->Scale(scale);
    fh_mom_bg->Scale(scale);
    fh_position_signal->Scale(scale);
    fh_position_bg-> Scale(scale);
    fh_pt_signal->Scale(scale);
    fh_pt_bg->Scale(scale);
    fh_chi2_prim_signal->Scale(scale);
    fh_chi2_prim_bg->Scale(scale);
    fh_ttcut_signal->Scale(scale);
    fh_ttcut_bg->Scale(scale);
    fh_apcut_signal->Scale(scale);
    fh_apcut_bg->Scale(scale);
    fh_ttcut_pi0->Scale(scale);
    fh_apcut_pi0->Scale(scale);
    fh_ttcut_gamma->Scale(scale);
    fh_apcut_gamma->Scale(scale);

//ID cuts
    fh_rich_ann_signal->Scale(scale);
    fh_rich_ann_bg->Scale(scale);
    fh_trd_ann_signal->Scale(scale);
    fh_trd_ann_bg->Scale(scale);
    fh_tof_m2_signal->Scale(scale);
    fh_tof_m2_bg->Scale(scale);
   

///WRITE HISTOGRAMS TO THE FILE
    fh_mc_signal_pty->Write();
    fh_acc_signal_pty->Write();

    fh_mc_signal_mom->Write();
    fh_acc_signal_mom->Write();
    fh_reco_signal_mom->Write();
    fh_rich_id_signal_mom->Write();
    fh_trd_id_signal_mom->Write();
    fh_tof_id_signal_mom->Write();
    fh_chi_prim_signal_mom->Write();
    fh_ptcut_signal_mom->Write();
    fh_anglecut_signal_mom->Write();
    fh_pi0cut_signal_mom->Write();
    fh_ttcut_signal_mom->Write();
    fh_apcut_signal_mom->Write();

    fh_mc_signal_minv->Write();
    fh_acc_signal_minv->Write();

    fh_mc_mother_pdg->Write();
    fh_acc_mother_pdg->Write();

    fh_mc_vertex_gamma_xz->Write();
    fh_mc_vertex_gamma_yz->Write();
    fh_mc_vertex_gamma_xy->Write();

    fh_rec_mc_mom_signal->Write();
    fh_mom_res_vs_mom_signal->Write();
    fh_mean_mom_vs_mom_signal->Write();
//signal minv
    fh_rec_signal_minv->Write();
    fh_rich_id_signal_minv->Write();
    fh_trd_id_signal_minv->Write();
    fh_tof_id_signal_minv->Write();
    fh_chi_prim_signal_minv->Write();
    fh_ptcut_signal_minv->Write();
    fh_anglecut_signal_minv->Write();
    fh_pi0cut_signal_minv->Write();
    fh_ttcut_signal_minv->Write();
    fh_apcut_signal_minv->Write();
//BG minv
    fh_rec_bg_minv->Write();
    fh_rich_id_bg_minv->Write();
    fh_trd_id_bg_minv->Write();
    fh_tof_id_bg_minv->Write();
    fh_chi_prim_bg_minv->Write();
    fh_ptcut_bg_minv->Write();
    fh_anglecut_bg_minv->Write();
    fh_pi0cut_bg_minv->Write();
    fh_ttcut_bg_minv->Write();
    fh_apcut_bg_minv->Write();
//pi0 minv
    fh_rec_pi0_minv->Write();
    fh_rich_id_pi0_minv->Write();
    fh_trd_id_pi0_minv->Write();
    fh_tof_id_pi0_minv->Write();
    fh_chi_prim_pi0_minv->Write();
    fh_ptcut_pi0_minv->Write();
    fh_anglecut_pi0_minv->Write();
    fh_pi0cut_pi0_minv->Write();
    fh_ttcut_pi0_minv->Write();
    fh_apcut_pi0_minv->Write();

// cuts distribution
    fh_pt_signal->Write();
    fh_pt_bg->Write();
    fh_position_signal->Write();
    fh_position_bg->Write();
    fh_chi2_prim_signal->Write();
    fh_chi2_prim_bg->Write();
    fh_angle_signal->Write();
    fh_angle_bg->Write();
    fh_mom_signal->Write();
    fh_mom_bg->Write();
    fh_ttcut_signal->Write();
    fh_ttcut_bg->Write();
    fh_apcut_signal->Write();
    fh_apcut_bg->Write();
    fh_ttcut_pi0->Write();
    fh_apcut_pi0->Write();
    fh_ttcut_gamma->Write();
    fh_apcut_gamma->Write();


    fh_reco_signal_pty->Write();
    fh_rich_id_signal_pty->Write();
    fh_trd_id_signal_pty->Write();
    fh_tof_id_signal_pty->Write();
    fh_chi_prim_signal_pty->Write();
    fh_ptcut_signal_pty->Write();
    fh_anglecut_signal_pty->Write();
    fh_pi0cut_signal_pty->Write();
    fh_ttcut_signal_pty->Write();
    fh_apcut_signal_pty->Write();

    //ID cuts
    fh_rich_ann_signal->Write();
    fh_rich_ann_bg->Write();
    fh_trd_ann_signal->Write();
    fh_trd_ann_bg->Write();
    fh_tof_m2_signal->Write();
    fh_tof_m2_bg->Write();
}
