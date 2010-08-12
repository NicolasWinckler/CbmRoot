/** CbmTrdHitsQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/

#include "CbmTrdHitsQa.h"
#include "CbmLitUtils.h"
#include "CbmLitDrawHist.h"

#include "FairRootManager.h"
#include "CbmTrdHit.h"
#include "CbmTrdPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"

#include <iostream>


CbmTrdHitsQa::CbmTrdHitsQa():
	fNofPlanes(12),
	fOutputDir(""),
	fEventNo(0)
{
}

CbmTrdHitsQa::~CbmTrdHitsQa()
{
	fHistoList->Delete();
	delete fHistoList;
}

InitStatus CbmTrdHitsQa::Init()
{
	ReadDataBranches();
	CreateHistos();
}

void CbmTrdHitsQa::SetParContainers()
{
}

void CbmTrdHitsQa::Exec(
		Option_t* opt)
{
	ProcessHits();
	std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmTrdHitsQa::Finish()
{
	WriteToFile();
	Draw();
}

void CbmTrdHitsQa::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init","CbmRootManager is not instantiated");

   	fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
   	if (NULL == fTrdHits) Fatal("Init","No TrdHit array!");

   	fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   	if (NULL == fTrdPoints) Fatal("Init","No TrdPoint array!");
}

void CbmTrdHitsQa::CreateHistos()
{
	// Histogram list
	fHistoList = new TList();

	// Resize to number of planes
	fhX.resize(fNofPlanes);
	fhY.resize(fNofPlanes);
	fhDx.resize(fNofPlanes);
	fhDy.resize(fNofPlanes);
	fhXY.resize(fNofPlanes);

	fhResX.resize(fNofPlanes);
	fhResY.resize(fNofPlanes);
	fhPullX.resize(fNofPlanes);
	fhPullY.resize(fNofPlanes);

	Int_t nBins = 100;
	Double_t max = 0.;
	Double_t min = 0.;


	for (Int_t i = 0; i < fNofPlanes; i++) {
		std::string histName = "hX" + ToString<Int_t>(i);
		std::string histTitle = "X distribution on " + ToString<Int_t>(i) + " plane";
		fhX[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhX[i]);

		histName = "hY" + ToString<Int_t>(i);
		histTitle = "Y distribution on " + ToString<Int_t>(i) + " plane";
		fhY[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhY[i]);

		histName = "hDx" + ToString<Int_t>(i);
		histTitle = "Dx distribution on " + ToString<Int_t>(i) + " plane";
		fhDx[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhDx[i]);

		histName = "hDy" + ToString<Int_t>(i);
		histTitle = "Dy distribution on " + ToString<Int_t>(i) + " plane";
		fhDy[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhDy[i]);

		histName = "hXY" + ToString<Int_t>(i);
		histTitle = "XY distribution on " + ToString<Int_t>(i) + " plane";
		fhXY[i] = new TH2F(histName.c_str(), histTitle.c_str(), nBins, -500, 500, nBins, -500, 500);
		fHistoList->Add(fhXY[i]);

		histName = "hResX" + ToString<Int_t>(i);
		histTitle = "Residual X distribution on " + ToString<Int_t>(i) + " plane";
		fhResX[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhResX[i]);

		histName = "hResY" + ToString<Int_t>(i);
		histTitle = "Residual Y distribution on " + ToString<Int_t>(i) + " plane";
		fhResY[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhResY[i]);

		histName = "hPullX" + ToString<Int_t>(i);
		histTitle = "Pull X distribution on " + ToString<Int_t>(i) + " plane";
		fhPullX[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhPullX[i]);

		histName = "hPullY" + ToString<Int_t>(i);
		histTitle = "Pull Y distribution on " + ToString<Int_t>(i) + " plane";
		fhPullY[i] = new TH1F(histName.c_str(), histTitle.c_str(), nBins, min, max);
		fHistoList->Add(fhPullY[i]);
	}
}

void CbmTrdHitsQa::ProcessHits()
{
	Int_t nofHits = fTrdHits->GetEntriesFast();
	for (UInt_t iHit = 0; iHit < nofHits; iHit++){
		CbmTrdHit* hit = (CbmTrdHit*) fTrdHits->At(iHit);
		CbmTrdPoint* point = (CbmTrdPoint*) fTrdPoints->At(hit->GetRefId());
		Int_t plane = hit->GetPlaneId() - 1;

//		std::cout << "iHit=" << iHit << " plane=" << plane << std::endl;
		fhX[plane]->Fill(hit->GetX());
		fhY[plane]->Fill(hit->GetY());
		fhDx[plane]->Fill(hit->GetDx());
		fhDy[plane]->Fill(hit->GetDy());
		fhXY[plane]->Fill(hit->GetX(), hit->GetY());

		Double_t resX = hit->GetX() - point->GetX();
		Double_t resY = hit->GetY() - point->GetY();
		fhResX[plane]->Fill(resX);
		fhResY[plane]->Fill(resY);

		if (hit->GetDx() != 0.) fhPullX[plane]->Fill(resX / hit->GetDx());
		if (hit->GetDy() != 0.) fhPullY[plane]->Fill(resY / hit->GetDy());
	}
}

void CbmTrdHitsQa::WriteToFile()
{
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
}

void CbmTrdHitsQa::Draw()
{
	SetStyles();

	TCanvas* chX = new TCanvas("trd_hits_qa_x","trd_hits_qa_x", 1200, 800);
	chX->Divide(4,3);

	TCanvas* chY = new TCanvas("trd_hits_qa_y","trd_hits_qa_y", 1200, 800);
	chY->Divide(4,3);

	TCanvas* chDx = new TCanvas("trd_hits_qa_dx","trd_hits_qa_dx", 1200, 800);
	chDx->Divide(4,3);

	TCanvas* chDy = new TCanvas("trd_hits_qa_dy","trd_hits_qa_dy", 1200, 800);
	chDy->Divide(4,3);

	TCanvas* chXY = new TCanvas("trd_hits_qa_xy","trd_hits_qa_xy", 1200, 800);
	chXY->Divide(4,3);


	TCanvas* chResX = new TCanvas("trd_hits_qa_res_x","trd_hits_qa_res_x", 1200, 800);
	chResX->Divide(4,3);

	TCanvas* chResY = new TCanvas("trd_hits_qa_res_y","trd_hits_qa_res_y", 1200, 800);
	chResY->Divide(4,3);

	TCanvas* chPullX = new TCanvas("trd_hits_qa_pull_x","trd_hits_qa_pull_x", 1200, 800);
	chPullX->Divide(4,3);

	TCanvas* chPullY = new TCanvas("trd_hits_qa_pull_y","trd_hits_qa_pull_y", 1200, 800);
	chPullY->Divide(4,3);

	for (Int_t i = 0; i < fNofPlanes; i++) {
		chX->cd(i + 1);
		DrawHist1D(fhX[i], "X [cm]", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			   		LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chX, fOutputDir);

		chY->cd(i + 1);
		DrawHist1D(fhY[i], "Y [cm]", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
					LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chY, fOutputDir);

		chDx->cd(i + 1);
		DrawHist1D(fhDx[i], "Dx [cm]", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			   		LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chDx, fOutputDir);

		chDy->cd(i + 1);
		DrawHist1D(fhDy[i], "Dy [cm]", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
					LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chDy, fOutputDir);

		chXY->cd(i + 1);
		DrawHist2D(fhXY[i], "X [cm]", "Y [cm]", "Counter", false, false, true, "colz");
		SaveCanvasAsImage(chXY, fOutputDir);




		chResX->cd(i + 1);
		DrawHist1D(fhResX[i], "Residual X [cm]", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			   		LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chResX, fOutputDir);

		chResY->cd(i + 1);
		DrawHist1D(fhResY[i], "Residual Y [cm]", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
					LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chResY, fOutputDir);

		chPullX->cd(i + 1);
		DrawHist1D(fhPullX[i], "Pull X", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			   		LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chPullX, fOutputDir);

		TH1F* hist = fhPullX[i];
		hist->Fit("gaus");
		TF1 *fit = hist->GetFunction("gaus");
		Double_t sigma = 0., rms = 0.;
		if (fit){
			sigma = fit->GetParameter(2);
		    rms = hist->GetRMS();
		}
		DrawHistSigmaRMS(i, sigma, rms);

		chPullY->cd(i + 1);
		DrawHist1D(fhPullY[i], "Pull Y", "Counter",
					LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
					LIT_MARKER_STYLE1, false, true, "");
		SaveCanvasAsImage(chPullY, fOutputDir);
		hist = fhPullY[i];
		hist->Fit("gaus");
		fit = hist->GetFunction("gaus");
		if (fit){
			sigma = fit->GetParameter(2);
			rms = hist->GetRMS();
		}
		DrawHistSigmaRMS(i, sigma, rms);
	}
}

ClassImp(CbmTrdHitsQa);

