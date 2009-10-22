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
#include <string>

double my2Dfunc(double *p, double *par)
{
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
	fDrawBz(true),
	fNofSlices(0)
{
	// Some style settings for drawing
	gStyle->SetCanvasColor(kWhite);
	gStyle->SetFrameFillColor(kWhite);
	gStyle->SetPadColor(kWhite);
	gStyle->SetStatColor(kWhite);
	gStyle->SetTitleFillColor(kWhite);
	gStyle->SetPalette(1);
	gStyle->SetOptStat(0);

	fZpos.push_back(25.);
	fZpos.push_back(50.);
	fZpos.push_back(75.);
	fZpos.push_back(100.);
	fZpos.push_back(125.);
	fZpos.push_back(150.);

	fNofSlices = fZpos.size();

	// Fill maximum X and Y positions
	fXpos.resize(fNofSlices);
	fYpos.resize(fNofSlices);
	for (Int_t i = 0; i < fNofSlices; i++){
		double tan25 = std::tan(25*3.14159265/180); // tan of 25 degrees. CBM acceptance
		fXpos[i] = fZpos[i] * tan25;
		fYpos[i] = fZpos[i] * tan25;
	}

	// Vectors with coefficients
	fCx.resize(fNofSlices);
	fCy.resize(fNofSlices);
	fCz.resize(fNofSlices);
}

CbmLitCheckField::~CbmLitCheckField()
{
}

InitStatus CbmLitCheckField::Init()
{

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
	fNofSlices = fZpos.size();

	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	fField = env->GetField();

	// Fit the magnetic field
	CbmLitFieldFitter ff;
	for(Int_t i = 0; i < fNofSlices; i++) {
		std::cout << "Fit slice at " << fZpos[i] << std::endl;
		ff.FitSlice(fZpos[i], fCx[i], fCy[i], fCz[i]);
	}

	CreateHistos();
	FillBHistos();
	FillErrHistos();

	if (IsDrawBx()) DrawHistos(BX);
	if (IsDrawBy()) DrawHistos(BY);
	if (IsDrawBz()) DrawHistos(BZ);
}

void CbmLitCheckField::Finish()
{
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
	fhBRelErrH2D.resize(3);
	for (Int_t i = 0; i < 3; i++) {
		fhB[i].resize(fNofSlices);
		fhBErrH1D[i].resize(fNofSlices);
		fhBErrH2D[i].resize(fNofSlices);
		fhBRelErrH1D[i].resize(fNofSlices);
		fhBRelErrH2D[i].resize(fNofSlices);
	}

	std::string names[] = {"Bx", "By", "Bz"};
	std::string namesErrH1D[] = {"BxErrH1D", "ByErrH1D", "BzErrH1D"};
	std::string namesErrH2D[] = {"BxErrH2D", "ByErrH2D", "BzErrH2D"};
	std::string namesRelErrH1D[] = {"BxRelErrH1D", "ByRelErrH1D", "BzRelErrH1D"};
	std::string namesRelErrH2D[] = {"BxRelErrH2D", "ByRelErrH2D", "BzRelErrH2D"};

	Int_t nofBinsX = 30;
	Int_t nofBinsY = 30;
	Int_t nofBinsErrB = 100;
	Double_t minErrB = -0.5;
	Double_t maxErrB = 0.5;
	Int_t nofBinsRelErrB = 100;
	Double_t minRelErrB = -10.;
	Double_t maxRelErrB = 10.;

	// Create histograms
	for (Int_t v = 0; v < 3; v++) {
		for (Int_t i = 0; i < fNofSlices; i++) {
			std::stringstream histName, histTitle;
			histName << "h" << names[v] << i;
			histTitle << names[v] << " at z=" << fZpos[i];
			fhB[v][i] = new TH2D(histName.str().c_str(), histTitle.str().c_str(), nofBinsX, -fXpos[i], fXpos[i], nofBinsY, -fYpos[i], fYpos[i]);
			fHistoList->Add(fhB[v][i]);

			std::stringstream histName2, histTitle2;
			histName2 << "h" << namesErrH1D[v] << i;
			histTitle2 << namesErrH1D[v] << " at z=" << fZpos[i];
			fhBErrH1D[v][i] = new TH1D(histName2.str().c_str(), histTitle2.str().c_str(), nofBinsErrB, minErrB, maxErrB);
			fHistoList->Add(fhBErrH1D[v][i]);

			std::stringstream histName3, histTitle3;
			histName3 << "h" << namesErrH2D[v] << i;
			histTitle3 << namesErrH2D[v] << " at z=" << fZpos[i];
			fhBErrH2D[v][i] = new TH2D(histName3.str().c_str(), histTitle3.str().c_str(), nofBinsX, -fXpos[i], fXpos[i], nofBinsY, -fYpos[i], fYpos[i]);
			fHistoList->Add(fhBErrH2D[v][i]);

			std::stringstream histName4, histTitle4;
			histName4 << "h" << namesRelErrH1D[v] << i;
			histTitle4 << namesRelErrH1D[v] << " at z=" << fZpos[i];
			fhBRelErrH1D[v][i] = new TH1D(histName4.str().c_str(), histTitle4.str().c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
			fHistoList->Add(fhBRelErrH1D[v][i]);

			std::stringstream histName5, histTitle5;
			histName5 << "h" << namesRelErrH2D[v] << i;
			histTitle5 << namesRelErrH2D[v] << " at z=" << fZpos[i];
			fhBRelErrH2D[v][i] = new TH2D(histName5.str().c_str(), histTitle5.str().c_str(), nofBinsX, -fXpos[i], fXpos[i], nofBinsY, -fYpos[i], fYpos[i]);
			fHistoList->Add(fhBRelErrH2D[v][i]);
		}
	}
}

void CbmLitCheckField::FillBHistos()
{
	Int_t nofBinsX = 30;
	Int_t nofBinsY = 30;
	for (unsigned int i = 0; i < fNofSlices; i++) { // loop over z positions
		double Z = fZpos[i];

		double HX = 2 * fXpos[i] / nofBinsX; // step size for X position
		double HY = 2 * fYpos[i] / nofBinsY; // step size for Y position
		for (int j = 0; j < nofBinsX; j++) { // loop over x position
			double X = -fXpos[i] + (j+0.5) * HX;
			for (int k = 0; k < nofBinsY; k++) { // loop over y position
				double Y = -fYpos[i] + (k+0.5)  * HY;

				// get field value
				double pos[3] = {X, Y, Z};
				double B[3];
				fField->GetFieldValue(pos, B);

				// fill histograms
				fhB[BX][i]->Fill(X, Y, B[0]);
				fhB[BY][i]->Fill(X, Y, B[1]);
				fhB[BZ][i]->Fill(X, Y, B[2]);
			}
		}
	}
}

void CbmLitCheckField::FillErrHistos()
{
	Int_t nofBinsX = 100;
	Int_t nofBinsY = 100;
	for (Int_t i = 0; i < fNofSlices; i++) {
		Double_t Z = fZpos[i];
		Double_t HX = 2 * fXpos[i] / nofBinsX; // step size for X position
		Double_t HY = 2 * fYpos[i] / nofBinsY; // step size for Y position
		for (int j = 0; j < nofBinsX; j++) { // loop over x position
			Double_t X = -fXpos[i] + (j+0.5) * HX;
			for (int k = 0; k < nofBinsY; k++) { // loop over y position
				Double_t Y = -fYpos[i] + (k+0.5) * HY;

				// get field value
				Double_t pos[3] = {X, Y, Z};
				Double_t B[3];
				fField->GetFieldValue(pos, B);

				double p[2] = {X, Y};
				double aprBx = my2Dfunc(p, &fCx[i][0]);
				double aprBy = my2Dfunc(p, &fCy[i][0]);
				double aprBz = my2Dfunc(p, &fCz[i][0]);

				Double_t errBx = B[0] - aprBx;
				Double_t errBy = B[1] - aprBy;
				Double_t errBz = B[2] - aprBz;
				Double_t relErrBx = 0., relErrBy = 0., relErrBz = 0.;
				if (B[0] != 0.) relErrBx = (errBx / B[0]) * 100.;
				if (B[1] != 0.) relErrBy = (errBy / B[1]) * 100.;
				if (B[2] != 0.) relErrBz = (errBz / B[2]) * 100.;

				fhBErrH2D[BX][i]->Fill(X, Y, errBx);
				fhBErrH1D[BX][i]->Fill(errBx);
				fhBRelErrH1D[BX][i]->Fill(relErrBx);
				fhBRelErrH2D[BX][i]->Fill(X, Y, relErrBx);
				fhBErrH2D[BY][i]->Fill(X, Y, errBy);
				fhBErrH1D[BY][i]->Fill(errBy);
				fhBRelErrH1D[BY][i]->Fill(relErrBy);
				fhBRelErrH2D[BY][i]->Fill(X, Y, relErrBy);
				fhBErrH2D[BZ][i]->Fill(X, Y, errBz);
				fhBErrH1D[BZ][i]->Fill(errBz);
				fhBRelErrH1D[BZ][i]->Fill(relErrBz);
				fhBRelErrH2D[BZ][i]->Fill(X, Y, relErrBz);
			}
		}
	}
}

void CbmLitCheckField::DrawHistos(
		Int_t v)
{
	std::string names[] = {"Bx", "By", "Bz"};
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::stringstream ss;
		ss << names[v] << " at z=" << fZpos[s];
		canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1200,800);
		canvas[s]->Divide(3, 2);
	}

	for (int i = 0; i < fNofSlices; i++) {
		canvas[i]->cd(1);
		TH2D* hist1 = fhB[v][i];
		hist1->GetXaxis()->SetTitle("X [cm]");
		hist1->GetYaxis()->SetTitle("Y [cm]");
		hist1->GetZaxis()->SetTitle("B [kGauss]");
		gPad->SetRightMargin(0.2);
		hist1->Draw("colz");

		canvas[i]->cd(2);
		TH1D* hist2 = fhBErrH1D[v][i];
		hist2->GetZaxis()->SetTitle("B [kGauss]");
		hist2->Draw();

		canvas[i]->cd(3);
		TH2D* hist3 = fhBErrH2D[v][i];
		hist3->GetXaxis()->SetTitle("X [cm]");
		hist3->GetYaxis()->SetTitle("Y [cm]");
		hist3->GetZaxis()->SetTitle("B [kGauss]");
		gPad->SetRightMargin(0.2);
		hist3->Draw("colz");

		canvas[i]->cd(4);
		TH1D* hist4 = fhBRelErrH1D[v][i];
		hist4->GetZaxis()->SetTitle("Relative error [%]");
		hist4->Draw();

		canvas[i]->cd(5);
		TH2D* hist5 = fhBRelErrH2D[v][i];
		hist5->GetXaxis()->SetTitle("X [cm]");
		hist5->GetYaxis()->SetTitle("Y [cm]");
		hist5->GetZaxis()->SetTitle("B [kGauss]");
		gPad->SetRightMargin(0.2);
		hist5->Draw("colz");

		canvas[i]->cd(6);
		std::stringstream funcName;
		funcName << names[v] << "Func" << i;
		TF2 * func = new TF2(funcName.str().c_str(), my2Dfunc, -fXpos[i], fXpos[i], -fYpos[i], fYpos[i], 10);
		if (v == BX) func->SetParameters(&fCx[i][0]); else
		if (v == BY) func->SetParameters(&fCy[i][0]); else
		if (v == BZ) func->SetParameters(&fCz[i][0]);
		func->GetXaxis()->SetTitle("X [cm]");
		func->GetYaxis()->SetTitle("Y [cm]");
		func->GetZaxis()->SetTitle("B [kGauss]");
		func->Draw("surf1");

		std::string name(canvas[i]->GetName());
		canvas[i]->SaveAs(std::string("field/" + name + ".gif").c_str());
		canvas[i]->SaveAs(std::string("field/" + name + ".eps").c_str());
		canvas[i]->SaveAs(std::string("field/" + name + ".svg").c_str());

	}
}

ClassImp(CbmLitCheckField);
