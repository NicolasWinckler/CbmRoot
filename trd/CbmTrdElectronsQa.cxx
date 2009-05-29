
#include "CbmTrdElectronsQa.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdPoint.h"
#include "CbmMCTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

CbmTrdElectronsQa::CbmTrdElectronsQa()
{
	fVerbose = 1;
	fEventNum = 0;
	fOutFileNamePi = "";
	fOutFileNameEl = "";
}

CbmTrdElectronsQa::~CbmTrdElectronsQa()
{

}

void CbmTrdElectronsQa::InitHistos()
{

    fhPiELoss = new TH1F("fhPiELoss","Energy loss for pions in one layer;energy loss, keV;entries",100, 0, 50);
    fhPiELossSum = new TH1F("fhPiELossSum","Sum of energy losses for pions in all layers;energy loss, GeV;entries",100, 0, 3.e-4);
    fhEldEdX = new TH1F("fhEldEdX","dEdX for electrons in one layer;dEdX, GeV;entries",100, 0, 1.5e-5);
    fhElTR = new TH1F("fhElTR","TR for electrons in one layer;TR, GeV;entries",100, 0, 1.5e-5);
    fhElELoss = new TH1F("fhElELoss","Energy loss (dEdX+TR) for electrons in one layer;energy loss, GeV;entries",100, 0, 5.0e-5);
    fhElElossSum = new TH1F("fhElELossSum","Sum of energy losses (dEdX+TR) for electrons in all layers;energy loss, GeV;entries",100, 0, 3.e-4);
    fhElNofZeroTR = new TH1F("fhElNofZeroTR","Number of Zero TR layers;number of layers; entries",13, 0., 13.);

	fOutPi.open((const char*)fOutFileNamePi);
	fOutEl.open((const char*)fOutFileNameEl);
}

InitStatus CbmTrdElectronsQa::Init()
{

	FairRootManager* ioman = FairRootManager::Instance();
	if (! ioman) {
		cout << "-E- CbmTrdElectronsQa::Init: RootManager not instantised!" << endl;
		return kFATAL;
	}

	fMCTracks  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
	if ( ! fMCTracks) {
		cout << "-E- CbmTrdElectronsQa::Init: No MCTrack array!" << endl;
		return kFATAL;
	}

	fTrdPoints  = (TClonesArray*) ioman->ActivateBranch("TRDPoint");
	if ( ! fTrdPoints) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDPoint array!" << endl;
		return kFATAL;
	}

	fTrdTracks  = (TClonesArray*) ioman->ActivateBranch("TRDTrack");
	if ( ! fTrdTracks) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDTrack array!" << endl;
		return kFATAL;
	}

	fTrdTrackMatches  = (TClonesArray*) ioman->ActivateBranch("TRDTrackMatch");
	if ( ! fTrdTrackMatches) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDTrackMatch array!" << endl;
		return kFATAL;
	}

	fTrdHits  = (TClonesArray*) ioman->ActivateBranch("TRDHit");
	if ( ! fTrdHits) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDHit array!" << endl;
		return kFATAL;
	}

	InitHistos();

	return kSUCCESS;
}

void CbmTrdElectronsQa::Exec(Option_t* option)
{
	fEventNum++;
	if (!fMCTracks) return;
	if (!fTrdPoints) return;
	if (!fTrdTracks) return;
	if (!fTrdTrackMatches) return;
	if (!fTrdHits) return;

	cout << "Electrons Qa event # " << fEventNum << endl;

	if (fOutFileNameEl != "" && fOutFileNamePi != ""){
		MakeTxtFile();
	}

	BuildEnergyLossesAnaHistos();
}

Double_t CbmTrdElectronsQa::GetMomAtFirstTrdLayer(CbmTrdTrack* trdtrack)
{
	Int_t hitIndex = trdtrack->GetTrdHitIndex(0);
	CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);
	Int_t iPoint = trdhit->GetRefId();
	if (iPoint < 0)	return 0.0;
	FairMCPoint* point = (FairMCPoint*) fTrdPoints->At(iPoint);
	TVector3 mom;
	point->Momentum(mom);
	return mom.Mag();
}

void CbmTrdElectronsQa::GetELossInfo(CbmTrdTrack* trdtrack, Double_t *sumELoss, Double_t  eLossdEdX[],
		  Double_t  eLossTR[], Double_t  eLoss[])
{
	for (Int_t iHit = 0; iHit < 12; iHit++) {
		Int_t hitIndex = trdtrack->GetTrdHitIndex(iHit);
		CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);

		eLoss[iHit] = trdhit->GetELoss();
		*sumELoss += trdhit->GetELoss();
		eLossTR[iHit] = trdhit->GetELossTR();
		eLossdEdX[iHit] = trdhit->GetELossdEdX();
	} //iHit
}

void CbmTrdElectronsQa::MakeTxtFile()
{
	cout << "-I- make txt file" << endl;
	Int_t nofTrdTracks = fTrdTracks->GetEntries();

	for(Int_t iTrdTrack=0; iTrdTrack < nofTrdTracks; iTrdTrack++){
		CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
		Int_t nHits = trdtrack->GetNofTrdHits();
		if (nHits < 12) continue;

		CbmTrackMatch* trdmatch = (CbmTrackMatch*) fTrdTrackMatches->At(iTrdTrack);
		Int_t iMC = trdmatch->GetMCTrackId();
		if (iMC == -1)	continue;
		if (iMC > fMCTracks->GetEntriesFast())continue;

		CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(iMC);
		Int_t partPdg = TMath::Abs(mctrack->GetPdgCode());
		Double_t momMC = mctrack->GetP();
		Double_t motherId = mctrack->GetMotherId();

		if (partPdg == 50000050)continue;
		///only primary tracks
		if (motherId != -1)	continue;

		if ( GetMomAtFirstTrdLayer(trdtrack) < .5) continue;

		Double_t sumELoss = 0;
	    Double_t eLossdEdX[12], eLossTR[12], eLoss[12];
	    GetELossInfo(trdtrack, &sumELoss, eLossdEdX, eLossTR, eLoss);

		if (abs(partPdg) == 211) {
			for (Int_t iHit = 0; iHit < 12; iHit++) {
				fOutPi << eLossdEdX[iHit] << " " << eLossTR[iHit] << " "
						<< eLoss[iHit] << " ";
			}
			fOutPi << momMC << endl;
		}

		if (abs(partPdg) == 11) {
			for (Int_t iHit = 0; iHit < 12; iHit++) {
				fOutEl << eLossdEdX[iHit] << " " << eLossTR[iHit] << " "
						<< eLoss[iHit] << " ";
			}
			fOutEl << momMC << endl;
		}
	}//iTrdTrack
}

void CbmTrdElectronsQa::BuildEnergyLossesAnaHistos()
{
	Int_t nofTrdTracks = fTrdTracks->GetEntries();

	for(Int_t iTrdTrack=0; iTrdTrack < nofTrdTracks; iTrdTrack++){
		CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
		Int_t nHits = trdtrack->GetNofTrdHits();
		if (nHits < 12) continue;

		CbmTrackMatch* trdmatch = (CbmTrackMatch*) fTrdTrackMatches->At(iTrdTrack);
		Int_t iMC = trdmatch->GetMCTrackId();
		if (iMC == -1)	continue;
		if (iMC > fMCTracks->GetEntriesFast())continue;

		CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(iMC);
		Int_t partPdg = TMath::Abs(mctrack->GetPdgCode());
		Double_t momMC = mctrack->GetP();
		Double_t motherId = mctrack->GetMotherId();

		if (partPdg == 50000050)continue;
		///only primary tracks
		if (motherId != -1)	continue;
		if ( GetMomAtFirstTrdLayer(trdtrack) < .5) continue;

		Double_t sumELoss = 0;
	    Double_t eLossdEdX[12], eLossTR[12], eLoss[12];
	    GetELossInfo(trdtrack, &sumELoss, eLossdEdX, eLossTR, eLoss);

		///pions
		if (TMath::Abs(partPdg) == 211){
			fhPiELossSum->Fill(sumELoss);
			fhPiELoss->Fill(eLossdEdX[0] * 1e6);
		}

		Int_t nofZeroTRLayers = 0;
		for (Int_t iHit = 0; iHit < 12; iHit++) {
			if (eLossTR[iHit] < 1e-9) nofZeroTRLayers++;
		}

		///electrons
		if (TMath::Abs(partPdg) == 11){
			fhElElossSum->Fill(sumELoss);
			fhElNofZeroTR->Fill(nofZeroTRLayers);
			fhElTR->Fill(eLossTR[0]);
			fhEldEdX->Fill(eLossdEdX[0]);
			fhElELoss->Fill(eLoss[0]);
		}

	}//iTrdTrack
}

void CbmTrdElectronsQa::Finish()
{
	fhPiELossSum->Write();
	fhPiELoss->Write();
	fhElElossSum->Write();
	fhElNofZeroTR->Write();
	fhElTR->Write();
	fhEldEdX->Write();
	fhElELoss->Write();
}


ClassImp(CbmTrdElectronsQa)




