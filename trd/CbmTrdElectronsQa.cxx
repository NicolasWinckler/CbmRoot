
#include "CbmTrdElectronsQa.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdPoint.h"
#include "CbmMCTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <vector>
using std::cout;
using std::endl;

CbmTrdElectronsQa::CbmTrdElectronsQa()
{
	fVerbose = 1;
	fEventNum = 0;
	fOutFileNamePi = "";
	fOutFileNameEl = "";
	fGeoType = "st";
}

CbmTrdElectronsQa::~CbmTrdElectronsQa()
{

}

void CbmTrdElectronsQa::InitHistos()
{
	Int_t nofELossBins = 500;
    fhPiELoss = new TH1D("fhPiELoss","Energy loss for pions in the first layer;energy loss, keV;entries",nofELossBins, 0, 50);
    fhPiELossSum = new TH1D("fhPiELossSum","Sum of energy losses for pions in all layers;energy loss, GeV;entries",nofELossBins, 0, 3.e-4);
    fhEldEdX = new TH1D("fhEldEdX","dEdX for electrons in the first layer;dEdX, GeV;entries",nofELossBins, 0, 1.5e-5);
    fhElTR = new TH1D("fhElTR","TR for electrons in the first layer;TR, GeV;entries",nofELossBins, 0, 1.5e-5);
    fhElELoss = new TH1D("fhElELoss","Energy loss (dEdX+TR) for electrons in the first layer;energy loss, GeV;entries",nofELossBins, 0, 5.0e-5);
    fhElElossSum = new TH1D("fhElELossSum","Sum of energy losses (dEdX+TR) for electrons in all layers;energy loss, GeV;entries",nofELossBins, 0, 3.e-4);
    fhElNofZeroTR = new TH1D("fhElNofZeroTR","Number of Zero TR layers;number of layers; entries",13, 0., 13.);
    fhElNofClusters = new TH1D("fhElNofClusters","Number of clusters;number of clusters; entries",13, 0, 13);
    fhPiNofClusters = new TH1D("fhPiNofClusters","Number of clusters;number of clusters; entries",13, 0., 13.);
    fhElElossRMS= new TH1D("fhElElossRMS","Eloss RMS;Eloss RMS, GeV; entries",nofELossBins, 0, 50e-6);
    fhPiElossRMS = new TH1D("fhPiElossRMS","Eloss RMS;Eloss RMS, GeV; entries",nofELossBins, 0., 50e-6);
    fhElElossMediana= new TH1D("fhElElossMediana","Eloss mediana;mediana, GeV; entries",nofELossBins, 0, 50e-6);
    fhPiElossMediana = new TH1D("fhPiElossMediana","Eloss mediana;mediana, GeV; entries",nofELossBins, 0., 50e-6);

	fhNofTrdHitsEl = new TH1D("fhNofTrdHitsEl", "Number of hits in TRD track for electrons;Nof hits;Entries", 13, 0, 13);
	fhNofTrdHitsPi = new TH1D("fhNofTrdHitsPi", "Number of hits in TRD track for pions;Nof hits;Entries", 13, 0, 13);
	char histTitle[50], histName[50];
	for (Int_t i = 0; i < 7; i++){
		sprintf(histName, "fhPidANNEl%d", i);
		sprintf(histTitle, "%d hits in track;ANN output;Entries", 12 - i);
		fhPidANNEl[i] = new TH1D(histName, histTitle, 100, -1.2, 1.2);
		sprintf(histName, "fhPidANNPi%d", i);
		sprintf(histTitle, "%d hits in track;ANN output;Entries", 12 - i);
		fhPidANNPi[i] = new TH1D(histName, histTitle, 100, -1.2, 1.2);
	}

	Double_t *histMax;
	Double_t histMaxSt[]={6e-6, 8e-6, 12e-6, 14e-6, 16e-6, 16e-6, 20e-6, 26e-6, 30e-6, 40e-6, 50e-6, 90e-6};
	Double_t histMaxMB[]={10e-6, 14e-6, 20e-6, 24e-6, 28e-6, 30e-6, 36e-6, 44e-6, 50e-6, 60e-6, 70e-6, 130e-6};
	if (fGeoType == "st" || fGeoType == "ST"){
		histMax = histMaxSt;
	}else if (fGeoType == "mb" || fGeoType == "MB" ){
		histMax = histMaxMB;
	}else{
		cout << "-E- CbmTrdElectronsQa::InitHistos wrong geo type " << fGeoType << endl;
		histMax = histMaxSt;
	}

	Int_t nofSortBins = 10000;

	for (Int_t i = 0; i < 12; i++){
		sprintf(histName,"fhElossSortEl%d",i);
		sprintf(histTitle, "Energy loss in %d hit;Energy loss, GeV;Entries", i);
		fhElossSortEl[i] = new TH1D(histName, histTitle, nofSortBins, 0, histMax[i]);
		sprintf(histName,"fhElossSortPi%d",i);
		fhElossSortPi[i] = new TH1D(histName, histTitle, nofSortBins, 0, histMax[i]);

		sprintf(histName,"fhCumProbSortEl%d",i);
		sprintf(histTitle, "Cumulative prob. in %d hit;Energy loss, GeV;cumulative probability", i);
		fhCumProbSortEl[i] = new TH1D(histName, histTitle, nofSortBins, 0, histMax[i]);
		sprintf(histName,"fhCumProbSortPi%d",i);
		fhCumProbSortPi[i] = new TH1D(histName, histTitle, nofSortBins, 0, histMax[i]);
	}

	fEnergyCutForClusters = 5e-6; //GeV

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

	fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( ! fMCTracks) {
		cout << "-E- CbmTrdElectronsQa::Init: No MCTrack array!" << endl;
		return kFATAL;
	}

	fTrdPoints  = (TClonesArray*) ioman->GetObject("TrdPoint");
	if ( ! fTrdPoints) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDPoint array!" << endl;
		return kFATAL;
	}

	fTrdTracks  = (TClonesArray*) ioman->GetObject("TrdTrack");
	if ( ! fTrdTracks) {
		cout << "-E- CbmTrdElectronsQa::Init: No TrdTrack array!" << endl;
		return kFATAL;
	}

	fTrdTrackMatches  = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
	if ( ! fTrdTrackMatches) {
		cout << "-E- CbmTrdElectronsQa::Init: No TrdTrackMatch array!" << endl;
		return kFATAL;
	}

	fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
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

	FillEnergyLossesAnaHistos();
	FillTestHistos();
	ElIdAnalysis();

}

Double_t CbmTrdElectronsQa::GetMomAtFirstTrdLayer(CbmTrdTrack* trdtrack)
{
	Int_t hitIndex = trdtrack->GetHitIndex(0);
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
		Int_t hitIndex = trdtrack->GetHitIndex(iHit);
		CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);

		eLoss[iHit] = trdhit->GetELoss();
		*sumELoss += trdhit->GetELoss();
		eLossTR[iHit] = trdhit->GetELossTR();
		eLossdEdX[iHit] = trdhit->GetELossdEdX();
	} //iHit
}

Int_t CbmTrdElectronsQa::GetNofClusters(CbmTrdTrack* trdtrack)
{
	Int_t nofClusters = 0;
	for (Int_t iHit = 0; iHit < 12; iHit++) {
		Int_t hitIndex = trdtrack->GetHitIndex(iHit);
		CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);

		if ( trdhit->GetELoss() > fEnergyCutForClusters){
			nofClusters++;
		}
	} //iHit
	return nofClusters;
}

Double_t CbmTrdElectronsQa::GetElossRMS(CbmTrdTrack* trdtrack)
{
	Double_t rms = 0.;
	Double_t mean = 0.;
	Double_t nHits = 12;
	for (Int_t iHit = 0; iHit < 12; iHit++) {
		Int_t hitIndex = trdtrack->GetHitIndex(iHit);
		CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);
		mean += trdhit->GetELoss();
	} //iHit
	mean = mean / nHits;

	for (Int_t iHit = 0; iHit < 12; iHit++) {
		Int_t hitIndex = trdtrack->GetHitIndex(iHit);
		CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);
		Double_t eLoss = trdhit->GetELoss();

		rms += (mean - eLoss) * (mean - eLoss) / nHits;
	} //iHit

	return sqrt(rms);
}

void CbmTrdElectronsQa::MakeTxtFile()
{
	cout << "-I- make txt file" << endl;
	Int_t nofTrdTracks = fTrdTracks->GetEntries();

	for(Int_t iTrdTrack=0; iTrdTrack < nofTrdTracks; iTrdTrack++){
		CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
		Int_t nHits = trdtrack->GetNofHits();
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

void CbmTrdElectronsQa::FillEnergyLossesAnaHistos()
{
	cout << "-I- fill energy losses analysis histograms" << endl;
	Int_t nofTrdTracks = fTrdTracks->GetEntries();

	for(Int_t iTrdTrack=0; iTrdTrack < nofTrdTracks; iTrdTrack++){
		CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
		Int_t nHits = trdtrack->GetNofHits();
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
	    Int_t nofClusters = GetNofClusters(trdtrack);

		///pions
		if (TMath::Abs(partPdg) == 211){
			fhPiELossSum->Fill(sumELoss);
			fhPiELoss->Fill(eLossdEdX[0] * 1e6);
			fhPiNofClusters->Fill(nofClusters);
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
			fhElNofClusters->Fill(nofClusters);
		}

	}//iTrdTrack
}

void CbmTrdElectronsQa::FillTestHistos()
{
	cout << "-I- fill test histograms" << endl;
	Int_t nofTrdTracks = fTrdTracks->GetEntries();

	for (Int_t iTrdTrack = 0; iTrdTrack < nofTrdTracks; iTrdTrack++) {

		CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
		Int_t nHits = trdtrack->GetNofHits();

		if (nHits < 0) continue;

		CbmTrackMatch* match = (CbmTrackMatch*) fTrdTrackMatches->At(iTrdTrack);
		Int_t iMC = match->GetMCTrackId();
		if (iMC == -1)	continue;
		if (iMC > fMCTracks->GetEntriesFast())	continue;

		CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(iMC);
		Int_t partPdg = TMath::Abs(mctrack->GetPdgCode());
		Double_t motherId = mctrack->GetMotherId();

		if (partPdg == 50000050)continue;

		///only primary tracks
		if (motherId != -1)	continue;

		Double_t pidANN = trdtrack->GetPidANN();

		if (partPdg == 11) {
			fhNofTrdHitsEl->Fill(nHits);
		}

		if (partPdg == 211) {
			fhNofTrdHitsPi->Fill(nHits);
		}

		if (nHits < 6)	continue;
		if (nHits > 12) {
			cout << "-E- nHits = " << nHits << endl;
			continue;
		}
		Int_t indexAr = 12 - nHits;
		if (partPdg == 11)
			fhPidANNEl[indexAr]->Fill(pidANN);
		if (partPdg == 211)
			fhPidANNPi[indexAr]->Fill(pidANN);

	}//iTrdTrack

}

void CbmTrdElectronsQa::ElIdAnalysis()
{
	cout << "-I- electron identification analysis" << endl;

	Int_t nofTrdTracks = fTrdTracks->GetEntries();
	std::vector<Double_t> vec;
	vec.resize(12);
	for (Int_t iTrdTrack = 0; iTrdTrack < nofTrdTracks; iTrdTrack++) {

		CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
		Int_t nHits = trdtrack->GetNofHits();

		if (nHits != 12) continue;

		CbmTrackMatch* match = (CbmTrackMatch*) fTrdTrackMatches->At(iTrdTrack);
		Int_t iMC = match->GetMCTrackId();
		if (iMC == -1)	continue;
		if (iMC > fMCTracks->GetEntriesFast())	continue;

		CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(iMC);
		Int_t partPdg = TMath::Abs(mctrack->GetPdgCode());
		Double_t motherId = mctrack->GetMotherId();

		if (partPdg == 50000050)continue;

		///only primary tracks
		if (motherId != -1)	continue;
		Double_t sumELoss = 0;
	    Double_t eLossdEdX[12], eLossTR[12], eLoss[12];
	    GetELossInfo(trdtrack, &sumELoss, eLossdEdX, eLossTR, eLoss);
	    for (int i = 0; i < 12; i++){
	    	vec[i] = eLoss[i];
	    }
	    std::sort(vec.begin(), vec.end());

	    Double_t rms = GetElossRMS(trdtrack);
	    Double_t mediana = (vec[5] + vec[6])/2.;

		if (partPdg == 11) {
			fhElElossRMS->Fill(rms);
			fhElElossMediana->Fill(mediana);
		}
	    if (partPdg == 211) {
			fhPiElossRMS->Fill(rms);
			fhPiElossMediana->Fill(mediana);
		}
	    for (int i = 0; i < 12; i++){
			if (partPdg == 11) {
				fhElossSortEl[i]->Fill(vec[i]);
			}//electrons

			if (partPdg == 211) {
				fhElossSortPi[i]->Fill(vec[i]);
			}//pions
	    }
	}//iTrdTrack
}



void CbmTrdElectronsQa::Finish()
{
//fill cumulative probability histograms
	for (Int_t iL=0; iL < 12; iL++){
		Double_t cumProbPi = 0.;
		Double_t cumProbEl = 0.;
		Double_t nofPi = fhElossSortPi[iL]->GetEntries();
		Double_t nofEl = fhElossSortEl[iL]->GetEntries();
		for (Int_t i = 1; i <= fhElossSortEl[iL]->GetNbinsX(); i++) {
			cumProbPi += fhElossSortPi[iL]->GetBinContent(i);
			fhCumProbSortPi[iL]->SetBinContent(i, cumProbPi/nofPi);

			cumProbEl += fhElossSortEl[iL]->GetBinContent(i);
			fhCumProbSortEl[iL]->SetBinContent(i, cumProbEl / nofEl);
		}
	}

	fhPiELossSum->Write();
	fhPiELoss->Write();
	fhElElossSum->Write();
	fhElNofZeroTR->Write();
	fhElTR->Write();
	fhEldEdX->Write();
	fhElELoss->Write();
	fhElNofClusters->Write();
	fhPiNofClusters->Write();
    fhElElossRMS->Write();
    fhPiElossRMS->Write();
    fhElElossMediana->Write();
    fhPiElossMediana->Write();

	fhNofTrdHitsEl->Write();
	fhNofTrdHitsPi->Write();

	for (Int_t i = 0; i < 7; i++){
		fhPidANNEl[i]->Write();
		fhPidANNPi[i]->Write();
	}

	for (Int_t i = 0; i < 12; i++){
		fhElossSortEl[i]->Write();
		fhElossSortPi[i]->Write();
		fhCumProbSortEl[i]->Write();
		fhCumProbSortPi[i]->Write();
	}
}


ClassImp(CbmTrdElectronsQa)




