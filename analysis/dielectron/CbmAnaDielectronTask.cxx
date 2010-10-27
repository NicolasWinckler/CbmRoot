#include "CbmAnaDielectronTask.h"

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
    fUseRich = true;
    fUseTrd = true;
    fUseTof = true;
    fNofMcEp = 0;
    fNofMcEm = 0;
    fNofAccEp = 0;
    fNofAccEm = 0;
    fNofAccPairs = 0;

    fh_mc_signal_pty = new TH2D("fh_mc_signal_pty","fh_mc_signal_pty", 40, 0., 4., 20, 0., 2.);
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

InitStatus CbmAnaDielectronTask::Init()
{
	cout << "InitStatus CbmRichElectronsQa::Init()"<<endl;

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

	return kSUCCESS;
}


void CbmAnaDielectronTask::Exec(Option_t *option)
{

    fEvents++;
    cout << "-I- CbmAnaDielectronTask event number " << fEvents << endl;
    MCPairs();
    SingleParticleAcceptance();
    PairAcceptance();

    cout << "fNofMcEp = " << fNofMcEp << ", per event " << fNofMcEp / (Double_t)fEvents << endl;
    cout << "fNofMcEm = " << fNofMcEm << ", per event " << fNofMcEm / (Double_t)fEvents << endl;
    cout << "fNofAccEp = " << fNofAccEp << ", per event " << fNofAccEp / (Double_t)fEvents << endl;
    cout << "fNofAccEm = " << fNofAccEm << ", per event " << fNofAccEm / (Double_t)fEvents << endl;
    cout << "fNofAccPairs = " << fNofAccPairs << ", per event " << fNofAccPairs / (Double_t)fEvents << endl;
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
    } // nmber of e-/e+

    for (Int_t iP = 0; iP < nMcTracks; iP++) {
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
        Int_t motherId  = mctrackP->GetMotherId();
        Int_t pdg = mctrackP->GetPdgCode();
        if ( !(motherId == -1 && pdg == -11)) continue;
        TVector3 momP;  //momentum e+
        mctrackP->GetMomentum(momP);
        Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E); 
        for (Int_t iM = 0; iM < nMcTracks; iM++) {
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
            motherId = mctrackM->GetMotherId();
            pdg = mctrackM->GetPdgCode();
            if ( !(motherId == -1 && pdg == 11)) continue;
            TVector3 momM;  //momentum e-
            mctrackM->GetMomentum(momM);
            Double_t energyM = TMath::Sqrt(momM.Mag2() + M2E);

            //Calculate kinematic parameters of the pair
            TVector3 momPair = momP + momM;
            Double_t energyPair = energyP + energyM;
            Double_t ptPair = momPair.Perp();
            Double_t pzPair = momPair.Pz();
            Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));

            fh_mc_signal_pty->Fill(yPair,ptPair,fWeight);
        } //iM
    } //iP

} //MC Pairs

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
        Bool_t isAcc = (nMvdPoints+nStsPoints >= 4);// && (nRichPoints > 0) ;
        if (motherId == -1 && pdg == -11 && isAcc) fNofAccEp++;
        if (motherId == -1 && pdg == 11 && isAcc) fNofAccEm++; 
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
                CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
                motherId = mctrackM->GetMotherId();
                pdg = mctrackM->GetPdgCode();
                if ( !(motherId == -1 && pdg == 11 )) continue;
                nMvdPoints = mctrackM->GetNPoints(kMVD);
                nStsPoints = mctrackM->GetNPoints(kSTS);
                nRichPoints = mctrackM->GetNPoints(kRICH);
                Bool_t isAccM = (nMvdPoints+nStsPoints >= 4);// && (nRichPoints > 0) ;
                if (isAccP && isAccM) fNofAccPairs++;
            }//iM
        }
    }//iP

} // PairsAcceptance

void CbmAnaDielectronTask::Finish()
{
    Double_t scale = 1./(Double_t)fEvents;
    fh_mc_signal_pty->Scale(scale);
    fh_mc_signal_pty->Write();
}

void CbmAnaDielectronTask::WriteOutput()
{
    // --- Normalize histogramms ---
    Double_t scale = 1./(Double_t)fEvents;

}
