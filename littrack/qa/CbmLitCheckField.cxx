#include "CbmLitCheckField.h"

#include "CbmLitEnvironment.h"
#include "CbmLitFloat.h"
#include "CbmLitFieldFitter.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TPaveText.h"

#include <sstream>
#include <cmath>

double my2Dfunc(double *p, double *par) {
    float x = p[0];
    float y = p[1];
	float x2 = x*x;
    float y2 = y*y;
    float xy = x*y;
    float x3 = x2*x;
    float y3 = y2*y;
    float xy2 = x*y2;
    float x2y = x2*y;

    return par[0] + par[1]*x + par[2]*y + par[3]*x2 + par[4]*xy + par[5]*y2 + par[6]*x3 + par[7]*x2y + par[8]*xy2 + par[9]*y3;
}


CbmLitCheckField::CbmLitCheckField():
	BX(0),
	BY(1),
	BZ(2),
	fDrawBx(true),
	fDrawBy(true),
	fDrawBz(true)
{
	gStyle->SetCanvasColor(kWhite);
	gStyle->SetFrameFillColor(kWhite);
	gStyle->SetPadColor(kWhite);
	gStyle->SetStatColor(kWhite);
	gStyle->SetTitleFillColor(kWhite);
	gStyle->SetPalette(1);
}

CbmLitCheckField::~CbmLitCheckField()
{
}

InitStatus CbmLitCheckField::Init()
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	fField = env->GetField();

	// Fit the magnetic field
	CbmLitFieldFitter ff;
	ff.Fit();
	ff.GetFieldSlices(fSlices);
	ff.GetInputInfo(fZpos, fXpos, fYpos, fNofBinsX, fNofBinsY);

	CreateHistos();
	FillHistos();
	if (IsDrawBx()) {
//		FitHistos(BX);
		DrawHistos(BX);
		DrawFunc(BX);
	}
	if (IsDrawBy()) {
//		FitHistos(BY);
		DrawHistos(BY);
		DrawFunc(BY);
	}
	if (IsDrawBz()) {
//		FitHistos(BZ);
		DrawHistos(BZ);
		DrawFunc(BZ);
	}
	FindErrors();
}

void CbmLitCheckField::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    rtdb->getContainer("FairFieldPar");
}

void CbmLitCheckField::Exec(
		Option_t* opt)
{
}

void CbmLitCheckField::Finish()
{
	WriteToFile();
}

void CbmLitCheckField::CreateHistos()
{
	// Histogram list
	fHistoList = new TList();

	// resize histogram vectors
	fhB.resize(3);
	fhBErrH1D.resize(3);
	fhBErrH2D.resize(3);
	fhBRelErrH1D.resize(3);
	for (Int_t i = 0; i < 3; i++) {
		fhB[i].resize(fSlices.size());
		fhBErrH1D[i].resize(fSlices.size());
		fhBErrH2D[i].resize(fSlices.size());
		fhBRelErrH1D[i].resize(fSlices.size());
	}

	std::string names[] = {"Bx", "By", "Bz"};
	std::string namesErrH1D[] = {"BxErrH1D", "ByErrH1D", "BzErrH1D"};
	std::string namesErrH2D[] = {"BxErrH2D", "ByErrH2D", "BzErrH2D"};
	std::string namesRelErrH1D[] = {"BxRelErrH1D", "ByRelErrH1D", "BzRelErrH1D"};

	Int_t nofBinsErrB = 100;
	Double_t minErrB = 2.;
	Double_t maxErrB = 2.;
	Int_t nofBinsRelErrB = 100;
	Double_t minRelErrB = 10.;
	Double_t maxRelErrB = 10.;

	// Create histograms
	for (Int_t v = 0; v < 3; v++) {
		for (Int_t i = 0; i < fSlices.size(); i++) {
			std::stringstream histName, histTitle;
			histName << "h" << names[v] << i;
			histTitle << names[v] << " at z=" << fZpos[i];
			fhB[v][i] = new TH2D(histName.str().c_str(), histTitle.str().c_str(), fNofBinsX[i], -fXpos[i], fXpos[i], fNofBinsY[i], -fYpos[i], fYpos[i]);
	//		fhBx[i] = new TGraph2D();
	//		fhBx[i]->SetNameTitle(histNameBx.str().c_str(), histTitleBx.str().c_str());
			fHistoList->Add(fhB[v][i]);

			std::stringstream histName2, histTitle2;
			histName2 << "h" << namesErrH1D[v] << i;
			histTitle2 << namesErrH1D[v] << " at z=" << fZpos[i];
			fhBErrH1D[v][i] = new TH1D(histName2.str().c_str(), histTitle2.str().c_str(), nofBinsErrB, minErrB, maxErrB);
			fHistoList->Add(fhBErrH1D[v][i]);

			std::stringstream histName3, histTitle3;
			histName3 << "h" << namesErrH2D[v] << i;
			histTitle3 << namesErrH2D[v] << " at z=" << fZpos[i];
			fhBErrH2D[v][i] = new TH2D(histName3.str().c_str(), histTitle3.str().c_str(), fNofBinsX[i], -fXpos[i], fXpos[i], fNofBinsY[i], -fYpos[i], fYpos[i]);
			fHistoList->Add(fhBErrH2D[v][i]);

			std::stringstream histName4, histTitle4;
			histName4 << "h" << namesRelErrH1D[v] << i;
			histTitle4 << namesRelErrH1D[v] << " at z=" << fZpos[i];
			fhBRelErrH1D[v][i] = new TH1D(histName4.str().c_str(), histTitle4.str().c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
			fHistoList->Add(fhBRelErrH1D[v][i]);
		}
	}
}

void CbmLitCheckField::FillHistos()
{
	for (unsigned int i = 0; i < fSlices.size(); i++) { // loop over z positions
		double Z = fSlices[i].GetZ();
//		int cnt = 0;
		double HX = 2 * fXpos[i] / fNofBinsX[i]; // step size for X position
		double HY = 2 * fYpos[i] / fNofBinsY[i]; // step size for Y position
		for (int j = 0; j < fNofBinsX[i]; j++) { // loop over x position
			double X = -fXpos[i] + j * HX;
			for (int k = 0; k < fNofBinsY[i]; k++) { // loop over y position
				double Y = -fYpos[i] + k * HY;

				// get field value
				double pos[3] = {X, Y, Z};
				double B[3];
				fField->GetFieldValue(pos, B);

				// fill histograms
//				fhBx[i]->SetPoint(cnt, X, Y, B[0]);
//				fhBy[i]->SetPoint(cnt, X, Y, B[1]);
//				fhBz[i]->SetPoint(cnt, X, Y, B[2]);
//				cnt++;
				fhB[BX][i]->Fill(X, Y, B[0]);
				fhB[BY][i]->Fill(X, Y, B[1]);
				fhB[BZ][i]->Fill(X, Y, B[2]);
			}
		}
	}
}

void CbmLitCheckField::WriteToFile()
{
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
}

//void CbmLitCheckField::FitHistos(
//		Int_t v)
//{
//	for (Int_t i = 0; i < fNZ; i++) {
//		TF2 * func = new TF2("func",my2Dfunc, -fXpos[i], fXpos[i], -fYpos[i], fYpos[i], 10);
////		func->SetParameters(iniParams);
//		fhB[v][i]->Fit(func);
//	}
//}

void CbmLitCheckField::FindErrors()
{
//	for (Int_t i = 0; i < fNZ; i++) {
//		myf cx[10], cy[10], cz[10];
//		for (Int_t v = 0; v < 3; v++) {
//			TF1* func = fhB[v][i]->GetFunction("func");
//			for (Int_t j = 0; j < 10; j++) {
//				if (v == 0) cx[j] = func->GetParameter(j);
//				if (v == 1) cy[j] = func->GetParameter(j);
//				if (v == 2) cz[j] = func->GetParameter(j);
//			}
//		}
//		CbmLitFieldSlice slice(cx, cy, cz, fZpos[i]);
//		fhSlices.push_back(slice);
//	}

	for (Int_t i = 0; i < fSlices.size(); i++) {
		Double_t Z = fSlices[i].GetZ();
		Double_t HX = 2 * fXpos[i] / fNofBinsX[i]; // step size for X position
		Double_t HY = 2 * fYpos[i] / fNofBinsY[i]; // step size for Y position
		for (int j = 0; j < fNofBinsX[i]; j++) { // loop over x position
			Double_t X = -fXpos[i] + j * HX;
			for (int k = 0; k < fNofBinsY[i]; k++) { // loop over y position
				Double_t Y = -fYpos[i] + k * HY;

				// get field value
				Double_t pos[3] = {X, Y, Z};
				Double_t B[3];
				fField->GetFieldValue(pos, B);

				CbmLitFieldValue value;
				fSlices[i].GetFieldValue(X, Y, value);

				Double_t errBx = B[0] - value.GetBx();
				Double_t errBy = B[1] - value.GetBy();
				Double_t errBz = B[2] - value.GetBz();
				Double_t relErrBx = 0., relErrBy = 0., relErrBz = 0.;
				if (B[0] != 0.) relErrBx = (errBx / B[0]) * 100.;
				if (B[1] != 0.) relErrBy = (errBy / B[1]) * 100.;
				if (B[2] != 0.) relErrBz = (errBz / B[2]) * 100.;

				fhBErrH2D[BX][i]->Fill(X, Y, errBx);
				fhBErrH1D[BX][i]->Fill(errBx);
				fhBRelErrH1D[BX][i]->Fill(relErrBx);
				fhBErrH2D[BY][i]->Fill(X, Y, errBy);
				fhBErrH1D[BY][i]->Fill(errBy);
				fhBRelErrH1D[BY][i]->Fill(relErrBy);
				fhBErrH2D[BZ][i]->Fill(X, Y, errBz);
				fhBErrH1D[BZ][i]->Fill(errBz);
				fhBRelErrH1D[BZ][i]->Fill(relErrBz);
			}
		}
	}

	std::string namesH1D[] = {"BxErrH1D", "ByErrH1D", "BzErrH1D"};
	std::string namesH2D[] = {"BxErrH2D", "ByErrH2D", "BzErrH2D"};
	std::string namesRelH1D[] = {"BxRelErrH1D", "ByRelErrH1D", "BzRelErrH1D"};
	for (Int_t v = 0; v < 3; v++) {
		TCanvas* canvasH1D = new TCanvas(namesH1D[v].c_str(), namesH1D[v].c_str(), 1200,1000);
		canvasH1D->Divide(4, 3);
		TCanvas* canvasH2D = new TCanvas(namesH2D[v].c_str(), namesH2D[v].c_str(), 1200,1000);
		canvasH2D->Divide(4, 3);
		TCanvas* canvasRelH1D = new TCanvas(namesRelH1D[v].c_str(), namesRelH1D[v].c_str(), 1200,1000);
		canvasRelH1D->Divide(4, 3);
		for (Int_t i = 0; i < fSlices.size(); i++) {
			canvasH1D->cd(i+1);
			TH1D* hist1 = fhBErrH1D[v][i];
			hist1->GetZaxis()->SetTitle("B [kGauss]");
			hist1->Draw();

			canvasH2D->cd(i+1);
			TH2D* hist2 = fhBErrH2D[v][i];
			hist2->GetXaxis()->SetTitle("X [cm]");
			hist2->GetYaxis()->SetTitle("Y [cm]");
			hist2->GetZaxis()->SetTitle("B [kGauss]");
			hist2->Draw("surf1");

			canvasRelH1D->cd(i+1);
			TH1D* hist3 = fhBRelErrH1D[v][i];
			hist3->GetZaxis()->SetTitle("Relative error [%]");
			hist3->Draw();
		}
	}
}

void CbmLitCheckField::DrawHistos(
		Int_t v)
{
	std::string names[] = {"Bx", "By", "Bz"};
	TCanvas* canvas = new TCanvas(names[v].c_str(), names[v].c_str(), 1200,1000);
	canvas->Divide(4, 3);
	for (int i = 0; i < fSlices.size(); i++) {
		canvas->cd(i+1);
		TH2D* hist1 = fhB[v][i];
		hist1->GetXaxis()->SetTitle("X [cm]");
		hist1->GetYaxis()->SetTitle("Y [cm]");
		hist1->GetZaxis()->SetTitle("B [kGauss]");
		hist1->Draw("surf1");
	}
}

void CbmLitCheckField::DrawFunc(
		Int_t v)
{
	std::string names[] = {"FitBx", "FitBy", "FitBz"};
	TCanvas* canvas = new TCanvas(names[v].c_str(), names[v].c_str(), 1200,1000);
	canvas->Divide(4, 3);
	gStyle->SetPalette(1, 0);
	for (int i = 0; i < fSlices.size(); i++) {
		canvas->cd(i+1);
		std::stringstream funcName;
		funcName << names[v] << "Func" << i;
		TF2 * func = new TF2(funcName.str().c_str(), my2Dfunc, -fXpos[i], fXpos[i], -fYpos[i], fYpos[i], 10);
		if (v == BX) func->SetParameters(fSlices[i].GetCx()); else
		if (v == BY) func->SetParameters(fSlices[i].GetCy()); else
		if (v == BZ) func->SetParameters(fSlices[i].GetCz());
		func->GetXaxis()->SetTitle("X [cm]");
		func->GetYaxis()->SetTitle("Y [cm]");
		func->GetZaxis()->SetTitle("B [kGauss]");
		func->Draw("surf1");
	}
}

ClassImp(CbmLitCheckField);
