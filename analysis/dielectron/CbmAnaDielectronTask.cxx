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
    fWeight = 0.0026866 ;
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

    fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; yeild", 7000, -3500., 3500.);
    fh_mc_mother_pdg_bg = new TH1D("fh_mc_mother_pdg_bg", "fh_mc_mother_pdg; Pdg code; yeild", 7000, -3500., 3500.);
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

    fKFFitter.Init();

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

    cout << "fNofMcEp = " << fNofMcEp << ", per event " << fNofMcEp / (Double_t)fEvents << endl;
    cout << "fNofMcEm = " << fNofMcEm << ", per event " << fNofMcEm / (Double_t)fEvents << endl;
    cout << "fNofAccEp = " << fNofAccEp << ", per event " << fNofAccEp / (Double_t)fEvents << endl;
    cout << "fNofAccEm = " << fNofAccEm << ", per event " << fNofAccEm / (Double_t)fEvents << endl;
    cout << "fNofAccPairs = " << fNofAccPairs << ", per event " << fNofAccPairs / (Double_t)fEvents << endl;
    cout << "fNofMcEpBg = " << fNofMcEpBg << ", per event " << fNofMcEpBg / (Double_t)fEvents << endl;
    cout << "fNofMcEmBg = " << fNofMcEmBg << ", per event " << fNofMcEmBg / (Double_t)fEvents << endl;

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
        
        
        Int_t mcMotherPdg = 0;
        if (pdg == -11 || pdg == 11) {
            CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
            if (mother) mcMotherPdg = mother->GetPdgCode();
            fh_mc_mother_pdg->Fill(mcMotherPdg);
        }
    } // nmber of e-/e+

        
    for (Int_t iP = 0; iP < nMcTracks; iP++) {
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
        Int_t motherId  = mctrackP->GetMotherId();
        Int_t pdg = mctrackP->GetPdgCode();
        if ( !(motherId == -1 && pdg == -11)) continue;

        TVector3 momP;  //momentum e+
        mctrackP->GetMomentum(momP);
        Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E);
        TLorentzVector lorVecP(momP, energyP); 

        for (Int_t iM = 0; iM < nMcTracks; iM++) {
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
            motherId = mctrackM->GetMotherId();
            pdg = mctrackM->GetPdgCode();
            if ( !(motherId == -1 && pdg == 11)) continue;

            TVector3 momM;  //momentum e-
            mctrackM->GetMomentum(momM);
            Double_t energyM = TMath::Sqrt(momM.Mag2() + M2E);
            TLorentzVector lorVecM(momM, energyM); 

            //Calculate kinematic parameters of the pair
            TVector3 momPair = momP + momM;
            Double_t energyPair = energyP + energyM;
            Double_t ptPair = momPair.Perp();
            Double_t pzPair = momPair.Pz();
            Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
            Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
            Double_t minv = 2*TMath::Sin(anglePair / 2)*TMath::Sqrt(momM.Mag()*momP.Mag());
            //Fill histogramms
            fh_mc_signal_pty->Fill(yPair,ptPair,fWeight);
            fh_mc_signal_mom->Fill(momPair.Mag(),fWeight);
            fh_mc_signal_minv->Fill(minv, fWeight);
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
        if (pdg == -11) fNofMcEpBg++;
        if (pdg == 11) fNofMcEmBg++;
        
        
        Int_t mcMotherPdg = 0;
        if (pdg == -11 || pdg == 11) {
            CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
            if (mother) mcMotherPdg = mother->GetPdgCode();
            fh_mc_mother_pdg_bg->Fill(mcMotherPdg);
        }
    } // nmber of e-/e+

        
    for (Int_t iP = 0; iP < nMcTracks; iP++) {
        CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
        Int_t pdg = mctrackP->GetPdgCode();
        if ( !(pdg == -11)) continue;

        TVector3 momP;  //momentum e+
        mctrackP->GetMomentum(momP);
        Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E);
        TLorentzVector lorVecP(momP, energyP); 

        for (Int_t iM = 0; iM < nMcTracks; iM++) {
            CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
            pdg = mctrackM->GetPdgCode();
            if ( !(pdg == 11)) continue;

            TVector3 momM;  //momentum e-
            mctrackM->GetMomentum(momM);
            Double_t energyM = TMath::Sqrt(momM.Mag2() + M2E);
            TLorentzVector lorVecM(momM, energyM); 

            //Calculate kinematic parameters of the pair
            TVector3 momPair = momP + momM;
            Double_t energyPair = energyP + energyM;
            Double_t ptPair = momPair.Perp();
            Double_t pzPair = momPair.Pz();
            Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
            Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
            Double_t minv = 2*TMath::Sin(anglePair / 2)*TMath::Sqrt(momM.Mag()*momP.Mag());
            //Fill histogramms
          //  fh_mc_signal_pty->Fill(yPair,ptPair,fWeight);
            fh_mc_bg_mom->Fill(momPair.Mag());
            fh_mc_bg_minv->Fill(minv);
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

        TVector3 momP;
        mctrackP->GetMomentum(momP);
        Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E);
        TLorentzVector lorVecP(momP, energyP); 

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

                TVector3 momM;
                mctrackM->GetMomentum(momM);
                Double_t energyM = TMath::Sqrt(momM.Mag2() +M2E);
                TLorentzVector lorVecM(momM, energyM); 

                nMvdPoints = mctrackM->GetNPoints(kMVD);
                nStsPoints = mctrackM->GetNPoints(kSTS);
                nRichPoints = mctrackM->GetNPoints(kRICH);
                Bool_t isAccM = (nMvdPoints+nStsPoints >= 4);// && (nRichPoints > 0) ;
                if (isAccP && isAccM) fNofAccPairs++;

                //Calculate kinematic parameters of the pair
                TVector3 momPair = momP + momM;
                Double_t energyPair = energyP + energyM;
                Double_t ptPair = momPair.Perp();
                Double_t pzPair = momPair.Pz();
                Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
                Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
                Double_t minv = 2*TMath::Sin(anglePair / 2)*TMath::Sqrt(momM.Mag()*momP.Mag());
                
                //Fill histogramms
                if (isAccP && isAccM) {
                    fh_acc_signal_pty->Fill(yPair, ptPair,fWeight);
                    fh_acc_signal_mom->Fill(momPair.Mag(),fWeight);
                    fh_acc_signal_minv->Fill(minv,fWeight);
                }
            }//iM
        }
    }//iP

} // PairsAcceptance


void CbmAnaDielectronTask::FillCandidateArray()
{
    fCandidates.clear();
    Int_t nGTracks = fGlobalTracks->GetEntriesFast();
    fCandidates.reserve(nGTracks);

    for (Int_t i = 0; i < nGTracks; i++){
	DielectronCandidate cand;

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
             if (pdg == 11 && motherId == -1) cand.isMCSignalElectron;
	 }

         fCandidates.push_back(cand);

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

    //check RICH-STS-TRD-ToF matches
 /*   if (cand.stsMCTrackId == cand.richMCTrackId &&
        cand.stsMCTrackId == cand.trdMCTrackId &&
        cand.stsMCTrackId == cand.tofMCTrackId) {}    
*/

    }// global tracks

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
    fh_mc_mother_pdg_bg->Scale(scale);
    
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
    fh_mc_mother_pdg_bg->Write();
}

void CbmAnaDielectronTask::WriteOutput()
{
    // --- Normalize histogramms ---
    Double_t scale = 1./(Double_t)fEvents;

}
