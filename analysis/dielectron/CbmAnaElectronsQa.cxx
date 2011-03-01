#include "CbmAnaElectronsQa.h"

#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatch.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"

#include "TString.h"
#include "TSystem.h"
#include "TCanvas.h"

#include <iostream>
#include <vector>
#include <map>
#include <cmath>

class CbmStsTrack;

using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::fabs;

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmAnaElectronsQa::CbmAnaElectronsQa(const char *name, const char *title)
  :FairTask(name)
{
	fEventNum = 1;

	Int_t nofBinsMom = 100;
//signal electron acceptance
	fh_mc_el = new TH1D("fh_mc_el", "fh_mc_el;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_acc_sts_el = new TH1D("fh_acc_sts_el", "fh_acc_sts_el;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_acc_rich_el = new TH1D("fh_acc_rich_el", "fh_acc_rich_el;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_acc_trd_el = new TH1D("fh_acc_trd_el", "fh_acc_trd_el;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_acc_tof_el = new TH1D("fh_acc_tof_el", "fh_acc_tof_el;momentum, GeV/c;Entries", nofBinsMom,0,15);

//signal electron matching
	fh_match_rich = new TH1D("fh_match_rich", "fh_match_rich;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_match_trd = new TH1D("fh_match_trd", "fh_match_trd;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_match_tof = new TH1D("fh_match_tof", "fh_match_tof;momentum, GeV/c;Entries", nofBinsMom,0,15);

//ELECTRON IDENTIFICATION AND PION SUPPRESSION
	fh_elid_rich = new TH1D("fh_elid_rich", "fh_elid_rich;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_elid_trd = new TH1D("fh_elid_trd", "fh_elid_trd;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_elid_tof = new TH1D("fh_elid_tof", "fh_elid_tof;momentum, GeV/c;Entries", nofBinsMom,0,15);

	fh_acc_pi = new TH1D("fh_acc_pi", "fh_acc_pi;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_pi_as_el_rich = new TH1D("fh_pi_as_el_rich", "fh_pi_as_el_rich;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_pi_as_el_trd = new TH1D("fh_pi_as_el_trd", "fh_pi_as_el_trd;momentum, GeV/c;Entries", nofBinsMom,0,15);
	fh_pi_as_el_tof = new TH1D("fh_pi_as_el_tof", "fh_pi_as_el_tof;momentum, GeV/c;Entries", nofBinsMom,0,15);

//test distributions
	fh_nof_global = new TH1D("fh_nof_global", "fh_nof_global;nof global tracks/event;Entries", nofBinsMom,0,1000);
	fh_nof_sts_tracks = new TH1D("fh_nof_sts_tracks", "fh_nof_sts_tracks;nof STS tracks/event;Entries", nofBinsMom,0,1000);
	fh_nof_rich_rings = new TH1D("fh_nof_rich_rings", "fh_nof_rich_rings;nof RICH rings/event;Entries", nofBinsMom,0,100);
	fh_nof_trd_tracks = new TH1D("fh_nof_trd_tracks", "fh_nof_trd_tracks;nof TRD tracks/event;Entries", nofBinsMom,0,1000);

//STS Qa
    fh_rec_mc_mom_signal = new TH1D("fh_rec_mc_mom_signal","fh_rec_mc_mom_signal;#Delta p/p [%];yield",100, -10., 10.);
    fh_mom_res_vs_mom_signal = new TH2D("fh_mom_res_vs_mom_signal", "fh_mom_res_vs_mom_signal;p [GeV/c];#Delta p/p [%]",100, 0., 15., 100, -10., 10.);
    fh_mean_mom_vs_mom_signal = new TH1D("fh_mean_mom_vs_mom_signal","fh_mean_mom_vs_mom_signal",100, 0., 15.);
    fh_count_mom_vs_mom_signal = new TH1D("fh_count_mom_vs_mom_signal","fh_count_mom_vs_mom_signal", 100, 0., 15.);
    fh_chiprim_signal = new TH1D("fh_chiprim_signal", "fh_chiprim_signal;chi2,yield", 200, 0., 20.);
    fh_chiprim_signal2 = new TH1D("fh_chiprim_signal2", "fh_chiprim_signal2;chi2,yield", 200, 0., 20.);

	SetDefaultParameters();
}


CbmAnaElectronsQa::~CbmAnaElectronsQa()
{

}

void CbmAnaElectronsQa::SetDefaultParameters()
{
	fMinNofStsPoints = 4;
	fMinNofRichRingHits = 5;
	fMinNofTrdPoints = 8;

	fTrdAnnCut = -1.;
	fRichAnnCut = -1.;
	fUseRichAnn = false;
	fMeanA = -1.;
	fMeanB = -1.;
	fRmsA = -1.;
	fRmsB = -1.;
	fRmsCoeff = -1.;
	fDistCut = -1.;
}

Bool_t CbmAnaElectronsQa::SetParameters()
{
	if (fTrdAnnCut == -1.){
		cout << "-E- You must set TrdAnnCut, current value is " << fTrdAnnCut<<endl;
		return false;
	}
	if (fUseRichAnn){
		TString richANNFile = gSystem->Getenv("VMCWORKDIR");
		richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";

		fElIdAnn = new CbmRichElectronIdAnn(0, richANNFile);
		fElIdAnn->Init();

		if (fRichAnnCut == -1.){
			cout << "-E- You must set RichAnnCut, because you are using RICH ANN, current value is " << fRichAnnCut<<endl;
			return false;
		}
	} else {
		if (fMeanA == -1.){
			cout << "-E- You must set MeanA, current value is " << fMeanA<<endl;
			return false;
		}
		if (fMeanB == -1.){
			cout << "-E- You must set MeanB, current value is " << fMeanB<<endl;
			return false;
		}
		if (fRmsA == -1.){
			cout << "-E- You must set RmsA, current value is " << fRmsA<<endl;
			return false;
		}
		if (fRmsB == -1.){
			cout << "-E- You must set RmsB, current value is " << fRmsB<<endl;
			return false;
		}
		if (fRmsCoeff == -1.){
			cout << "-E- You must set RmsCoeff, current value is " << fRmsCoeff<<endl;
			return false;
		}
		if (fDistCut == -1.){
			cout << "-E- You must set fDistCut, current value is " << fDistCut<<endl;
			return false;
		}
	}
	return true;
}


InitStatus CbmAnaElectronsQa::Init()
{
	cout << "InitStatus CbmAnaElectronsQa::Init()"<<endl;
	// Get and check CbmRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmAnaElectronsQa::Init: " << "RootManager not instantised!"
				<< endl;
		return kERROR;
	}

	fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	if ( !fRichHits) {
		cout << "-W- CbmAnaElectronsQa::Init: No RichHit array!" << endl;
	}

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( !fRichRings) {
		cout << "-E- CbmAnaElectronsQa::Init: No RichRing array!" << endl;
		return kERROR;
	}

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( !fRichPoints) {
		cout << "-E- CbmAnaElectronsQa::Init: No RichPoint array!" << endl;
		return kERROR;
	}

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( !fMCTracks) {
		cout << "-E- CbmAnaElectronsQa::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( !fRichRingMatches) {
		cout << "-E- CbmAnaElectronsQa::Init: No RichRingMatch array!" << endl;
		return kERROR;
	}

	fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
	if ( !fRichProj) {
		cout << "-E- CbmAnaElectronsQa::Init: No RichProjection array!" << endl;
		return kERROR;
	}

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if ( !fStsTrackMatches) {
		cout << "-E- CbmAnaElectronsQa::Init: No track match array!"<< endl;
		return kERROR;
	}

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( !fGlobalTracks) {
		cout << "-W- CbmAnaElectronsQa::Init: No global track array!" << endl;
		return kERROR;
	}

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( !fStsTracks) {
		cout << "-E- CbmAnaElectronsQa::Init: No  STSTrack array!"<< endl;
		return kERROR;
	}

	fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	if ( !fTrdTracks) {
		cout << "-W- CbmAnaElectronsQa::Init: No TrdTrack array!" << endl;
		return kERROR;
	}

	fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
	if ( !fTrdTrackMatches) {
		cout << "-W- CbmAnaElectronsQa::Init: No TrdTrackMatch array!" << endl;
		return kERROR;
	}

	fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
	if ( !fTofPoints) {
		cout << "-W- CbmAnaElectronsQa::Init: No TOFPoint array!" << endl;
		return kERROR;
	}

	fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
	if ( !fTofHits) {
		cout << "-W- CbmAnaElectronsQa::Init: No TOFHit array!" << endl;
		return kERROR;
	}

    fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
    if (! fPrimVertex) {
		cout << "-E- CbmAnaElectronsQa::Init: No Primary Vertex!" << endl;
		return kERROR;
    }

    fKFFitter.Init();

	Bool_t isSetOk = SetParameters();

	if (!isSetOk) return kFATAL;

	return kSUCCESS;
}

void CbmAnaElectronsQa::Exec(Option_t* option)
{
	SignalElectronAcceptance();
	SignalElectronMatching();
	ElectronIdentificationEff();
	TestDistributions();
    StsQa();

    Double_t eventNum = (Double_t) fEventNum;

    cout << endl <<"----CbmAnaElectronsQa---- event # " << fEventNum << endl;
    cout <<"You set the following cuts:"<< endl;
	cout << "RichAnnCut="<<fRichAnnCut << ";  ";
	cout << "UseRichAnn=" <<fUseRichAnn <<";  ";
	cout << "TrdAnnCut=" <<fTrdAnnCut <<";  ";
	cout << "MeanA=" <<fMeanA <<";  ";
	cout << "MeanB=" <<fMeanB <<";  ";
	cout << "RmsA=" <<fRmsA <<";  ";
	cout << "RmsB=" <<fRmsB <<";  ";
	cout << "RmsCoeff=" <<fRmsCoeff <<";  ";
	cout << "DistCut=" <<fDistCut <<endl;

	fEventNum++;
}

void CbmAnaElectronsQa::SignalElectronAcceptance()
{
// STS acceptance
	Int_t nofMCTracks = fMCTracks->GetEntriesFast();
	for (Int_t iMC=0; iMC < nofMCTracks; iMC++) {
		CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMC);
		if ( !mcTrack)	continue;

		Double_t momentum = mcTrack->GetP();
		Int_t gcode = TMath::Abs(mcTrack->GetPdgCode());
		Int_t motherId = mcTrack->GetMotherId();

		if (gcode == 11 && motherId == -1){//signal electrons
			fh_mc_el->Fill(momentum);
			if (mcTrack->GetNPoints(kMVD)+ mcTrack->GetNPoints(kSTS) >= fMinNofStsPoints){//STS acceptance
				fh_acc_sts_el->Fill(momentum);
			}
		}
	}//iMC

//RICH acceptance
    map<Int_t, Int_t> ringMap;
    ringMap.clear();
	Int_t nRichHits = fRichHits->GetEntriesFast();
	//Loop over Rich hits and
	//calculate number of RICH hits for each MC track
	for (Int_t iHit=0; iHit < nRichHits; iHit++) {
		CbmRichHit* hit = (CbmRichHit*) fRichHits->At(iHit);
		if ( !hit)	continue;
		Int_t iPoint = hit->GetRefId();
		if (iPoint < 0)	continue; // Fake or background hit
		FairMCPoint* point = (FairMCPoint*) fRichPoints->At(iPoint);
		if ( !point) continue;
		Int_t iMCTrack = point->GetTrackID();
        if (iMCTrack < 0) continue;
		CbmMCTrack* track = (CbmMCTrack*)fMCTracks->At(iMCTrack);
		if ( !track) continue;
		Int_t iMother = track->GetMotherId();
		if (iMother == -1) continue;
		ringMap[iMother]++;
	}//iHit

    map<Int_t, Int_t>::iterator it;
	//Loop for all MC rings and
    //fill RICH, TRD and TOF acceptance histograms
	for (it=ringMap.begin(); it!=ringMap.end(); it++) {
        if (it->first < 0) continue; 
		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(it->first);
		if (!track) continue;

		Double_t momentum = track->GetP();
		Int_t trackID = it->first;
		Bool_t isProj = DoesRingHaveProjection(trackID);
		Int_t gcode = TMath::Abs(track->GetPdgCode());
		Int_t motherId = track->GetMotherId();

		if (gcode == 11 && motherId == -1){//signal electrons
			if (track->GetNPoints(kMVD)+ track->GetNPoints(kSTS) >= fMinNofStsPoints){
				if (isProj && it->second >= fMinNofRichRingHits) {//RICH acceptance
					fh_acc_rich_el->Fill(momentum);
					if (track->GetNPoints(kTRD) >= fMinNofTrdPoints){//TRD acceptance
						fh_acc_trd_el->Fill(momentum);
						if (track->GetNPoints(kTOF) >= 1){//TOF acceptance
							fh_acc_tof_el->Fill(momentum);
						}//TOF
					}//TRD
				}//RICH
			}//STS
		}//signal electrons
	}//Loop for all MC rings

// pions acceptance
	for (Int_t iMC=0; iMC < nofMCTracks; iMC++) {
		CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMC);
		if ( !mcTrack)	continue;
		Double_t momentum = mcTrack->GetP();
		Bool_t isProj = DoesRingHaveProjection(iMC);
		Int_t gcode = TMath::Abs(mcTrack->GetPdgCode());
		if (gcode == 211 && isProj){
			fh_acc_pi->Fill(momentum);
		}
	}// iMC
}

//search for projection with certain TrackID
Bool_t CbmAnaElectronsQa::DoesRingHaveProjection(Int_t trackId)
{
	Bool_t isProj = false;
	Int_t nProj = fRichProj->GetEntriesFast();

	for (Int_t iProj = 0; iProj < nProj; iProj++) {
		FairTrackParam* proj = (FairTrackParam*)fRichProj->At(iProj);
		if (!proj)continue;

		CbmGlobalTrack* gtrack = (CbmGlobalTrack*)fGlobalTracks->At(iProj);
		if (gtrack->GetStsTrackIndex() < 0) continue;
		CbmTrackMatch* trackMatch = (CbmTrackMatch*)fStsTrackMatches->At(gtrack->GetStsTrackIndex());
		if (!trackMatch) continue;
		if (trackMatch->GetMCTrackId() == trackId && proj->GetX()!= 0 &&  proj->GetY()!= 0){
			isProj = true;
			break;
		}
	}
	return isProj;
}

void CbmAnaElectronsQa::SignalElectronMatching()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack<nGlTracks; iTrack++) {
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);
//STS
		Int_t stsIndex = gTrack->GetStsTrackIndex();
		if (stsIndex == -1) continue;
		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsIndex);
		if (!stsTrack) continue;
		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(stsIndex);
		if (!stsTrackMatch) continue;
		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();

//MC track
        if (mcIdSts < 0) continue;
		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(mcIdSts);
        if (!track) continue;
        Int_t pdg = TMath::Abs(track->GetPdgCode());
        Int_t motherId = track->GetMotherId();
        Double_t momentum = track->GetP();
//select only signal electrons
        if (pdg != 11 || motherId != -1) continue;

//RICH
		Int_t richIndex = gTrack->GetRichRingIndex();
		if (richIndex == -1) continue;
		CbmRichRing* richRing = (CbmRichRing*)fRichRings->At(richIndex);
		if (!richRing) continue;
		CbmRichRingMatch* richRingMatch = (CbmRichRingMatch*)fRichRingMatches->At(richIndex);
		if (!richRingMatch) continue;
		Int_t mcIdRich = richRingMatch->GetMCTrackID();
		if (mcIdSts == mcIdRich && mcIdRich != -1){
			fh_match_rich->Fill(momentum);
		}

//TRD
		Int_t trdIndex = gTrack->GetTrdTrackIndex();
		if (trdIndex == -1)continue;
		CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTracks->At(trdIndex);
		if (!trdTrack)continue;
		CbmTrackMatch* trdTrackMatch = (CbmTrackMatch*)fTrdTrackMatches->At(trdIndex);
		if (!trdTrackMatch)continue;
		Int_t mcIdTrd = trdTrackMatch->GetMCTrackId();
		if (mcIdSts == mcIdRich &&mcIdSts == mcIdTrd &&	mcIdRich != -1){
			fh_match_trd->Fill(momentum);
		}

//TOF
		Int_t tofIndex = gTrack->GetTofHitIndex();
		if (tofIndex == -1) continue;
		CbmTofHit* tofHit = (CbmTofHit*)fTofHits->At(tofIndex);
		if (!tofHit)continue;
		Int_t tofPointIndex = tofHit->GetRefId();
		if (tofPointIndex < 0) continue;
		FairMCPoint* tofPoint = (FairMCPoint*)fTofPoints->At(tofPointIndex);
		if (!tofPoint)continue;
		Int_t mcIdTof = tofPoint->GetTrackID();
		if (mcIdSts == mcIdRich &&mcIdSts == mcIdTrd &&	mcIdSts == mcIdTof && mcIdRich != -1){
			fh_match_tof->Fill(momentum);
		}
	}//global tracks
}

void CbmAnaElectronsQa::ElectronIdentificationEff()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack<nGlTracks; iTrack++) {

		CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);
//STS
		Int_t stsIndex = gTrack->GetStsTrackIndex();
		if (stsIndex == -1)continue;
		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsIndex);
		if (!stsTrack) continue;
		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(stsIndex);
		if (!stsTrackMatch) continue;
		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();

//MC Track
        if (mcIdSts < 0) continue;
		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(mcIdSts);
        if (!track) continue;
        Int_t pdg = TMath::Abs(track->GetPdgCode());
        Int_t motherId = track->GetMotherId();
        Double_t momentum = track->GetP();

//RICH
		Int_t richIndex = gTrack->GetRichRingIndex();
		if (richIndex == -1) continue;
		CbmRichRing* richRing = (CbmRichRing*)fRichRings->At(richIndex);
		if (!richRing) continue;
		CbmRichRingMatch* richRingMatch = (CbmRichRingMatch*)fRichRingMatches->At(richIndex);
		if (!richRingMatch) continue;
		Int_t mcIdRich = richRingMatch->GetMCTrackID();

		if (pdg == 11 && motherId == -1 && mcIdSts == mcIdRich && mcIdRich != -1 &&
				IsRichElectron(richRing, momentum)){
			fh_elid_rich->Fill(momentum);
		}
		//pion suppression
		if ( pdg == 211 &&  mcIdRich != -1 && IsRichElectron(richRing, momentum)){
			fh_pi_as_el_rich->Fill(momentum);
		}

///TRD
		Int_t trdIndex = gTrack->GetTrdTrackIndex();
		if (trdIndex == -1)	continue;
		CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTracks->At(trdIndex);
		if (!trdTrack)continue;
		CbmTrackMatch* trdTrackMatch = (CbmTrackMatch*)fTrdTrackMatches->At(trdIndex);
		if (!trdTrackMatch)continue;
		Int_t mcIdTrd = trdTrackMatch->GetMCTrackId();

		if (pdg == 11 && motherId == -1 && mcIdSts == mcIdRich && mcIdRich != -1 &&
				IsRichElectron(richRing, momentum)&& IsTrdElectron(trdTrack)){
			fh_elid_trd->Fill(momentum);
		}
		//pion suppression
		if ( pdg == 211 &&  mcIdRich != -1 && IsRichElectron(richRing, momentum) &&
				IsTrdElectron(trdTrack)){
        	fh_pi_as_el_trd->Fill(momentum);
		}

///TOF
		Int_t tofIndex = gTrack->GetTofHitIndex();
		if (tofIndex == -1)	continue;
		CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
		if (!tofHit)continue;
		Int_t iPoint = tofHit->GetRefId();
		CbmTofPoint* tofPoint = (CbmTofPoint*) fTofPoints->At(iPoint);
		Int_t mcIdTof = tofPoint->GetTrackID();

		if (pdg == 11 && motherId == -1 && mcIdSts == mcIdRich && mcIdRich != -1 &&
				IsRichElectron(richRing, momentum) && IsTrdElectron(trdTrack)
				&& IsTofElectron(gTrack, momentum)) {
			fh_elid_tof->Fill(momentum);
		}
		//pion suppression
		if (pdg == 211 && mcIdRich != -1 && IsRichElectron(richRing, momentum)
				&& IsTrdElectron(trdTrack) && IsTofElectron(gTrack, momentum)) {
			fh_pi_as_el_tof->Fill(momentum);
		}
	}// global tracks
}


Bool_t CbmAnaElectronsQa::IsRichElectron(CbmRichRing * ring, Double_t momentum)
{
    if (fUseRichAnn == false){
        Double_t axisA = ring->GetAaxis();
        Double_t axisB = ring->GetBaxis();
        Double_t dist = ring->GetDistance();
        if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && dist < fDistCut){
            return true;
        } else {
            return false;
        }
    } else {
        Double_t ann = fElIdAnn->DoSelect(ring, momentum);
        if (ann > fRichAnnCut) return true;
        else return false;
    }
}


Bool_t CbmAnaElectronsQa::IsTrdElectron(CbmTrdTrack* trdTrack)
{
    Double_t ann = trdTrack->GetPidANN();
    if (ann > fTrdAnnCut) return true;
    else return false;
}

Bool_t CbmAnaElectronsQa::IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum)
{
    // Get the tracklength
    Double_t trackLength = gTrack->GetLength() / 100.;

    // Calculate time of flight from TOF hit
    Int_t tofIndex = gTrack->GetTofHitIndex();
    CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
    if (NULL == tofHit){
        return false;
    }
    Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

    // Calculate mass squared
    Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);

    if (momentum >= 1.) {
        if (mass2 < (0.013*momentum - 0.003)){
            return true;
        }
    } else {
        if (mass2 < 0.01){
            return true;
        }
    }
    return false;
}

void CbmAnaElectronsQa::TestDistributions()
{
	fh_nof_global->Fill(fGlobalTracks->GetEntriesFast());
	fh_nof_sts_tracks->Fill(fStsTracks->GetEntriesFast());
	fh_nof_rich_rings->Fill(fRichRings->GetEntriesFast());
	fh_nof_trd_tracks->Fill(fTrdTracks->GetEntriesFast());
}

void CbmAnaElectronsQa::StsQa()
{
    Int_t nSts = fStsTracks->GetEntriesFast();
    for (Int_t i = 0; i < nSts; i++){
		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(i);
		if (!stsTrack) continue;
		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(i);
		if (!stsTrackMatch) continue;
		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
        CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdSts);

        Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
        Int_t motherId = mcTrack->GetMotherId();
		//select only signal electrons
        if (pdg != 11 || motherId != -1) continue;

		TVector3 momMC;
		mcTrack->GetMomentum(momMC);

        Double_t chiPrimary = fKFFitter.GetChiToVertex(stsTrack, fPrimVertex);
        fh_chiprim_signal->Fill(chiPrimary);
        Double_t chiPrimary2 = fKFFitter.GetChiToVertex(stsTrack);
        fh_chiprim_signal2->Fill(chiPrimary2);

        // Fit tracks to the primary vertex
        FairTrackParam vtxTrack;
        fKFFitter.FitToVertex(stsTrack, fPrimVertex, &vtxTrack);
		TVector3 momRec;
        vtxTrack.Momentum(momRec);

		Double_t dpp = 100.*(momMC.Mag()-momRec.Mag()) / momMC.Mag();
		fh_rec_mc_mom_signal->Fill(dpp);
		fh_mom_res_vs_mom_signal->Fill(momMC.Mag(),dpp);
		fh_count_mom_vs_mom_signal->Fill(momMC.Mag());
		fh_mean_mom_vs_mom_signal->Fill(momMC.Mag(),dpp);
    }
}//StsQa

// -----   Finish Task   ---------------------------------------------------
void CbmAnaElectronsQa::FinishTask()
{
    fh_rec_mc_mom_signal->Scale(1./fh_rec_mc_mom_signal->Integral());
    fh_mom_res_vs_mom_signal->Scale(1./fh_mom_res_vs_mom_signal->Integral());
    //fh_mean_mom_vs_mom_signal->Sumw2();
    //fh_count_mom_vs_mom_signal->Sumw2();
    fh_mean_mom_vs_mom_signal->Divide(fh_count_mom_vs_mom_signal);

//signal electron acceptance
	fh_mc_el->Write();
	fh_acc_sts_el->Write();
	fh_acc_rich_el->Write();
	fh_acc_trd_el->Write();
	fh_acc_tof_el->Write();

//signal electron matching
	fh_match_rich->Write();
	fh_match_trd->Write();
	fh_match_tof->Write();

//ELECTRON IDENTIFICATION AND PION SUPPRESSION
	fh_elid_rich->Write();
	fh_elid_trd->Write();
	fh_elid_tof->Write();

	fh_acc_pi->Write();
	fh_pi_as_el_rich->Write();
	fh_pi_as_el_trd->Write();
	fh_pi_as_el_tof->Write();

//test distributions
	fh_nof_global->Write();
	fh_nof_sts_tracks->Write();
	fh_nof_rich_rings->Write();
	fh_nof_trd_tracks->Write();

//STS Qa
    fh_rec_mc_mom_signal->Write();
    fh_mom_res_vs_mom_signal->Write();
    fh_mean_mom_vs_mom_signal->Write();
    fh_count_mom_vs_mom_signal->Write();
    fh_chiprim_signal->Write();
    fh_chiprim_signal2->Write();
}

ClassImp(CbmAnaElectronsQa)

