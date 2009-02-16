#include "CbmLitRobustAna.h"

#include "CbmLitPixelHit.h"
#include "CbmLitTrack.h"
#include "CbmLitEnvironment.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TList.h"

#include <iostream>

CbmLitRobustAna* CbmLitRobustAna::fInstance = NULL;

CbmLitRobustAna::CbmLitRobustAna()
{
	CreateHistos();
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	fLayout = env->GetTrdLayout();
}

CbmLitRobustAna::~CbmLitRobustAna()
{
	WriteToFile();

	if (fInstance != NULL) delete fInstance;
	fHistoList->Delete();
	delete fHistoList;
}

CbmLitRobustAna* CbmLitRobustAna::Instance()
{
	if (fInstance == NULL) {
	   fInstance = new CbmLitRobustAna();
	}
	return fInstance;
}

void CbmLitRobustAna::Fill(
		CbmLitTrack* track)
{
	for (Int_t i = 0; i < fLayout.GetNofPlanes(); i++){
		fhStationNofHits->Fill(i, track->GetNofHits(i));
	}
}

void CbmLitRobustAna::Fill(
		CbmLitTrack* track,
		Int_t iter)
{
	fhIterChi2->Fill(iter, track->GetChi2()/track->GetNDF());
}

void CbmLitRobustAna::FillWeight(
		CbmLitTrack* track,
		Int_t iter)
{
	for (Int_t i = 0; i < track->GetNofHits(); i++){
		fhIterWeight->Fill(iter, track->GetHit(i)->GetW());
	}
}

void CbmLitRobustAna::FillErrFit(
		CbmLitTrack* track,
		Int_t iter)
{
	fhEFIter->Fill(iter);
	fhEFNofHits->Fill(track->GetNofHits());
	fhEFIterNofHits->Fill(iter, track->GetNofHits());
}

void CbmLitRobustAna::CreateHistos()
{
	fHistoList = new TList();

	Int_t nofBinsStation = 20;
	Int_t minStation = 0;
	Int_t maxStation = 20;
	Int_t nofBinsNofHits = 50;
	Int_t minNofHits = 0;
	Int_t maxNofHits = 50;
	Int_t nofBinsIter = 5;
	Int_t minIter = 0;
	Int_t maxIter = 5;
	Int_t nofBinsChi2 = 100;
	Double_t minChi2 = 0;
	Double_t maxChi2 = 100;
	Int_t nofBinsWeight = 12;
	Double_t minWeight = -0.1;
	Double_t maxWeight = 1.1;
	fhStationNofHits = new TH2D("hStationNofHits", "nof hits vs. station",
			nofBinsStation, minStation, maxStation, nofBinsNofHits, minNofHits, maxNofHits);
	fhIterChi2 = new TH2D("hIterChi2", "iteration vs. chi2",
			nofBinsIter, minIter, maxIter, nofBinsChi2, minChi2, maxChi2);
	fhIterWeight = new TH2D("hIterWeight", "iteration vs. weight",
			nofBinsIter, minIter, maxIter, nofBinsWeight, minWeight, maxWeight);
	fHistoList->Add(fhStationNofHits);
	fHistoList->Add(fhIterChi2);
	fHistoList->Add(fhIterWeight);

	fhEFIter = new TH1D("hEFIter", "iteration (error fit)",
			nofBinsIter, minIter, maxIter);
	fhEFNofHits = new TH1D("hEFNofHits", "nof hits (error fit)",
			nofBinsNofHits, minNofHits, maxNofHits);
	fhEFIterNofHits = new TH2D("hEFIterNofHits", "iteration vs. nof hits (error fit)",
			nofBinsIter, minIter, maxIter, nofBinsNofHits, minNofHits, maxNofHits);
	fHistoList->Add(fhEFIter);
	fHistoList->Add(fhEFNofHits);
	fHistoList->Add(fhEFIterNofHits);
}

void CbmLitRobustAna::WriteToFile()
{
	//  TDirectory *olddir = gDirectory;
	//  TDirectory *hdir = new TDirectory("MuchFinderQa",
	//				    "Performance of the track finding in Much");
	//  hdir->cd();
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
	//  olddir->cd();
	std::cout << "CbmLitRobustAna::WriteToFile histograms have been written to file" << std::endl;
}
ClassImp(CbmLitRobustAna);
