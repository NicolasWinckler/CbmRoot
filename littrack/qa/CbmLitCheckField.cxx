#include "CbmLitCheckField.h"

#include "CbmLitEnvironment.h"
#include "CbmLitFloat.h"
#include "CbmLitFieldFitter.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TH2D.h"
#include "TGraph2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TLegend.h"

#include <sstream>
#include <cmath>
#include <string>


CbmLitCheckField::CbmLitCheckField():
	BX(0),
	BY(1),
	BZ(2),
	fDrawBx(true),
	fDrawBy(true),
	fDrawBz(true),
	fNofSlices(0),
	fXangle(25.),
	fYangle(25.),
	fNofBinsX(30),
	fNofBinsY(30),
	fUseEllipseAcc(true),
	fOutputDir("field/"),
	fPolynomDegree(1),
	fNofPolynoms(4)
{

}

CbmLitCheckField::~CbmLitCheckField()
{
}

InitStatus CbmLitCheckField::Init()
{
	// Some style settings for drawing
	gStyle->SetCanvasColor(kWhite);
	gStyle->SetFrameFillColor(kWhite);
	gStyle->SetPadColor(kWhite);
	gStyle->SetStatColor(kWhite);
	gStyle->SetTitleFillColor(kWhite);
	gStyle->SetPalette(1);
	gStyle->SetOptStat(0);

	fDegrees.push_back(3);
	fDegrees.push_back(5);
	fDegrees.push_back(7);
	fDegrees.push_back(9);
	fNofPolynoms = fDegrees.size();

	fZpos.push_back(30.);
	fZpos.push_back(50.);
	fZpos.push_back(100.);
	fZpos.push_back(125.);
	fNofSlices = fZpos.size();

	fXpos.resize(fNofSlices);
	fYpos.resize(fNofSlices);
	for (Int_t i = 0; i < fNofSlices; i++){
		double tanXangle = std::tan(fXangle*3.14159265/180); //
		double tanYangle = std::tan(fYangle*3.14159265/180); //
		fXpos[i] = fZpos[i] * tanXangle;
		fYpos[i] = fZpos[i] * tanYangle;
	}

	fCx.resize(fNofSlices);
	fCy.resize(fNofSlices);
	fCz.resize(fNofSlices);
	for (int i = 0; i < fNofSlices; i++) {
		fCx[i].resize(fNofPolynoms);
		fCy[i].resize(fNofPolynoms);
		fCz[i].resize(fNofPolynoms);
	}
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
	fFitter.resize(fNofPolynoms);
	for (unsigned int i = 0; i < fNofPolynoms; i++) {
		std::cout << "Fit with " << fDegrees[i] << " degree polynom" << std::endl;
		fFitter[i] = new CbmLitFieldFitter(fDegrees[i]);
		fFitter[i]->SetXangle(fXangle);
		fFitter[i]->SetYangle(fYangle);
		fFitter[i]->SetNofBinsX(fNofBinsX);
		fFitter[i]->SetNofBinsY(fNofBinsY);
		fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
		for(Int_t j = 0; j < fNofSlices; j++) {
			std::cout << "Fit slice at " << fZpos[j] << std::endl;
			fFitter[i]->FitSlice(fZpos[j], fCx[j][i], fCy[j][i], fCz[j][i]);
		}
	}

	CreateHistos();
	std::cout << "Histograms created" << std::endl;
	FillBHistos();
	std::cout << "B histograms filled" << std::endl;
	FillErrHistos();
	std::cout << "Error histograms filled" << std::endl;

	if (IsDrawBx()) DrawHistos(BX);
	if (IsDrawBy()) DrawHistos(BY);
	if (IsDrawBz()) DrawHistos(BZ);

	DrawHistosPoly("rel");
	DrawHistosPoly("abs");

	if (IsDrawBx()) DrawHistosPhd(BX);
	if (IsDrawBy()) DrawHistosPhd(BY);
	if (IsDrawBz()) DrawHistosPhd(BZ);
}

void CbmLitCheckField::Finish()
{
}

void CbmLitCheckField::CreateHistos()
{
	// Histogram list
	fHistoList = new TList();

	// resize histogram vectors
	fhBGraph.resize(3);
	fhBAprGraph.resize(3);
	fhBErrH1D.resize(3);
	fhBErrH2D.resize(3);
	fhBRelErrH1D.resize(3);
	fhBRelErrH2D.resize(3);
	for (Int_t i = 0; i < 3; i++) {
		fhBGraph[i].resize(fNofSlices);
		fhBAprGraph[i].resize(fNofSlices);
		fhBErrH1D[i].resize(fNofSlices);
		fhBErrH2D[i].resize(fNofSlices);
		fhBRelErrH1D[i].resize(fNofSlices);
		fhBRelErrH2D[i].resize(fNofSlices);
		for (int j = 0; j < fNofSlices; j++) {
			fhBAprGraph[i][j].resize(fNofPolynoms);
			fhBErrH1D[i][j].resize(fNofPolynoms);
			fhBErrH2D[i][j].resize(fNofPolynoms);
			fhBRelErrH1D[i][j].resize(fNofPolynoms);
			fhBRelErrH2D[i][j].resize(fNofPolynoms);
		}
	}

	std::string names[] = {"Bx", "By", "Bz"};
	std::string namesErrH1D[] = {"BxErrH1D", "ByErrH1D", "BzErrH1D"};
	std::string namesErrH2D[] = {"BxErrH2D", "ByErrH2D", "BzErrH2D"};
	std::string namesRelErrH1D[] = {"BxRelErrH1D", "ByRelErrH1D", "BzRelErrH1D"};
	std::string namesRelErrH2D[] = {"BxRelErrH2D", "ByRelErrH2D", "BzRelErrH2D"};

	Int_t nofBinsX = fNofBinsX;
	Int_t nofBinsY = fNofBinsY;
	Int_t nofBinsErrB = 100;
	Double_t minErrB = -0.5;
	Double_t maxErrB = 0.5;
	Int_t nofBinsRelErrB = 100;
	Double_t minRelErrB = -10.;
	Double_t maxRelErrB = 10.;

	Int_t nofBinsErrX = 100;
	Int_t nofBinsErrY = 100;

	// Create histograms
	for (Int_t v = 0; v < 3; v++) {
		for (Int_t i = 0; i < fNofSlices; i++) {
			fhBGraph[v][i] = new TGraph2D();

			for(int j = 0; j < fNofPolynoms; j++) {
				fhBAprGraph[v][i][j] = new TGraph2D();

				std::stringstream histName2, histTitle2;
				histName2 << "h" << namesErrH1D[v] << i << "_" << j;
				histTitle2 << namesErrH1D[v] << " at z=" << fZpos[i] <<  " degree " << fDegrees[j];
				fhBErrH1D[v][i][j] = new TH1D(histName2.str().c_str(), histTitle2.str().c_str(), nofBinsErrB, minErrB, maxErrB);
				fHistoList->Add(fhBErrH1D[v][i][j]);

				std::stringstream histName3, histTitle3;
				histName3 << "h" << namesErrH2D[v] << i << "_" << j;
				histTitle3 << namesErrH2D[v] << " at z=" << fZpos[i] <<  " degree " << fDegrees[j];
				fhBErrH2D[v][i][j] = new TH2D(histName3.str().c_str(), histTitle3.str().c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
				fHistoList->Add(fhBErrH2D[v][i][j]);

				std::stringstream histName4, histTitle4;
				histName4 << "h" << namesRelErrH1D[v] << i << "_" << j;
				histTitle4 << namesRelErrH1D[v] << " at z=" << fZpos[i] <<  " degree " << fDegrees[j];
				fhBRelErrH1D[v][i][j] = new TH1D(histName4.str().c_str(), histTitle4.str().c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
				fHistoList->Add(fhBRelErrH1D[v][i][j]);

				std::stringstream histName5, histTitle5;
				histName5 << "h" << namesRelErrH2D[v] << i << "_" << j;
				histTitle5 << namesRelErrH2D[v] << " at z=" << fZpos[i] <<  " degree " << fDegrees[j];
				fhBRelErrH2D[v][i][j] = new TH2D(histName5.str().c_str(), histTitle5.str().c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
				fHistoList->Add(fhBRelErrH2D[v][i][j]);
			}
		}
	}
}

void CbmLitCheckField::FillBHistos()
{
	const double LARGE = 1e5;
	for (unsigned int i = 0; i < fNofSlices; i++) { // loop over z positions
		double max[3] = {-LARGE, -LARGE, -LARGE};
		double min[3] = {LARGE, LARGE, LARGE};

		double Z = fZpos[i];

		int cnt = 0;

		double HX = 2 * fXpos[i] / fNofBinsX; // step size for X position
		double HY = 2 * fYpos[i] / fNofBinsY; // step size for Y position
		for (int j = 0; j < fNofBinsX; j++) { // loop over x position
			double X = -fXpos[i] + (j+0.5) * HX;
			for (int k = 0; k < fNofBinsY; k++) { // loop over y position
				double Y = -fYpos[i] + (k+0.5)  * HY;

				//check the acceptance
				double el = (X*X)/(fXpos[i]*fXpos[i]) + (Y*Y)/(fYpos[i]*fYpos[i]);
				if (fUseEllipseAcc && el > 1.) continue;

				// get field value
				double pos[3] = {X, Y, Z};
				double B[3];
				fField->GetFieldValue(pos, B);

				// find maximum entry in the histogram
				if (max[BX] < B[0]) max[BX] = B[0];
				if (max[BY] < B[1]) max[BY] = B[1];
				if (max[BZ] < B[2]) max[BZ] = B[2];
				//find minimum entry in the histogram
				if (min[BX] > B[0]) min[BX] = B[0];
				if (min[BY] > B[1]) min[BY] = B[1];
				if (min[BZ] > B[2]) min[BZ] = B[2];

				fhBGraph[BX][i]->SetPoint(cnt, X, Y, B[0]);
				fhBGraph[BY][i]->SetPoint(cnt, X, Y, B[1]);
				fhBGraph[BZ][i]->SetPoint(cnt, X, Y, B[2]);

				for (int p = 0; p < fNofPolynoms; p++) {
					fhBAprGraph[BX][i][p]->SetPoint(cnt, X, Y, fFitter[p]->GetPolynom()->Calculate(X, Y, &fCx[i][p][0]));
					fhBAprGraph[BY][i][p]->SetPoint(cnt, X, Y, fFitter[p]->GetPolynom()->Calculate(X, Y, &fCy[i][p][0]));
					fhBAprGraph[BZ][i][p]->SetPoint(cnt, X, Y, fFitter[p]->GetPolynom()->Calculate(X, Y, &fCz[i][p][0]));
				}
				cnt++;
			}
		}
		fhBGraph[BX][i]->SetMinimum(min[BX] - 0.1 * std::abs(min[BX]));
		fhBGraph[BY][i]->SetMinimum(min[BY] - 0.1 * std::abs(min[BY]));
		fhBGraph[BZ][i]->SetMinimum(min[BZ] - 0.1 * std::abs(min[BZ]));

		fhBGraph[BX][i]->SetMaximum(max[BX] + 0.1 * std::abs(max[BX]));
		fhBGraph[BY][i]->SetMaximum(max[BY] + 0.1 * std::abs(max[BY]));
		fhBGraph[BZ][i]->SetMaximum(max[BZ] + 0.1 * std::abs(max[BZ]));

		for (int p = 0; p < fNofPolynoms; p++) {
			fhBAprGraph[BX][i][p]->SetMinimum(min[BX] - 0.1 * std::abs(min[BX]));
			fhBAprGraph[BY][i][p]->SetMinimum(min[BY] - 0.1 * std::abs(min[BY]));
			fhBAprGraph[BZ][i][p]->SetMinimum(min[BZ] - 0.1 * std::abs(min[BZ]));

			fhBAprGraph[BX][i][p]->SetMaximum(max[BX] + 0.1 * std::abs(max[BX]));
			fhBAprGraph[BY][i][p]->SetMaximum(max[BY] + 0.1 * std::abs(max[BY]));
			fhBAprGraph[BZ][i][p]->SetMaximum(max[BZ] + 0.1 * std::abs(max[BZ]));
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

				//check the acceptance
				if (fUseEllipseAcc) {
					double el = (X*X)/(fXpos[i]*fXpos[i]) + (Y*Y)/(fYpos[i]*fYpos[i]);
					if (el > 1.) continue;
				}

				// get field value
				Double_t pos[3] = {X, Y, Z};
				Double_t B[3];
				fField->GetFieldValue(pos, B);

				for (int p = 0; p < fNofPolynoms; p++) {
					double aprBx = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCx[i][p][0]);
					double aprBy = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCy[i][p][0]);
					double aprBz = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCz[i][p][0]);

					Double_t errBx = B[0] - aprBx;
					Double_t errBy = B[1] - aprBy;
					Double_t errBz = B[2] - aprBz;
					Double_t relErrBx = 0., relErrBy = 0., relErrBz = 0.;
					if (B[0] != 0.) relErrBx = (errBx / B[0]) * 100.; else relErrBx = 0.;
					if (B[1] != 0.) relErrBy = (errBy / B[1]) * 100.; else relErrBy = 0.;
					if (B[2] != 0.) relErrBz = (errBz / B[2]) * 100.; else relErrBz = 0.;

					fhBErrH2D[BX][i][p]->Fill(X, Y, errBx);
					fhBErrH1D[BX][i][p]->Fill(errBx);
					fhBRelErrH1D[BX][i][p]->Fill(relErrBx);
					fhBRelErrH2D[BX][i][p]->Fill(X, Y, relErrBx);
					fhBErrH2D[BY][i][p]->Fill(X, Y, errBy);
					fhBErrH1D[BY][i][p]->Fill(errBy);
					fhBRelErrH1D[BY][i][p]->Fill(relErrBy);
					fhBRelErrH2D[BY][i][p]->Fill(X, Y, relErrBy);
					fhBErrH2D[BZ][i][p]->Fill(X, Y, errBz);
					fhBErrH1D[BZ][i][p]->Fill(errBz);
					fhBRelErrH1D[BZ][i][p]->Fill(relErrBz);
					fhBRelErrH2D[BZ][i][p]->Fill(X, Y, relErrBz);
				}
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
		TGraph2D* graph1 = fhBGraph[v][i];
		graph1->GetXaxis()->SetTitle("X [cm]");
		graph1->GetYaxis()->SetTitle("Y [cm]");
		graph1->GetZaxis()->SetTitle("B [kGauss]");
		graph1->GetXaxis()->SetTitleOffset(1.7);
		graph1->GetYaxis()->SetTitleOffset(2);
		graph1->GetZaxis()->SetTitleOffset(1.8);
		gPad->SetLeftMargin(0.2);
		graph1->Draw("TRI1");

		canvas[i]->cd(2);
		TH1D* hist2 = fhBErrH1D[v][i][fPolynomDegree];
		hist2->GetXaxis()->SetTitle("B [kGauss]");
		gPad->SetLogy();
		hist2->Draw();

		canvas[i]->cd(3);
		TH2D* hist3 = fhBErrH2D[v][i][fPolynomDegree];
		hist3->GetXaxis()->SetTitle("X [cm]");
		hist3->GetYaxis()->SetTitle("Y [cm]");
		hist3->GetZaxis()->SetTitle("B [kGauss]");
		hist3->GetZaxis()->SetTitleOffset(1.7);
		gPad->SetRightMargin(0.2);
		hist3->Draw("colz");

		canvas[i]->cd(4);
		TGraph2D* graph2 = fhBAprGraph[v][i][fPolynomDegree];
		graph2->GetXaxis()->SetTitle("X [cm]");
		graph2->GetYaxis()->SetTitle("Y [cm]");
		graph2->GetZaxis()->SetTitle("B [kGauss]");
		graph2->GetXaxis()->SetTitleOffset(1.7);
		graph2->GetYaxis()->SetTitleOffset(2);
		graph2->GetZaxis()->SetTitleOffset(1.8);
		gPad->SetLeftMargin(0.2);
		graph2->Draw("TRI1");

		canvas[i]->cd(5);
		TH1D* hist4 = fhBRelErrH1D[v][i][fPolynomDegree];
		hist4->GetXaxis()->SetTitle("Relative error [%]");
		gPad->SetLogy();
		hist4->Draw();

		canvas[i]->cd(6);
		TH2D* hist5 = fhBRelErrH2D[v][i][fPolynomDegree];
		hist5->GetXaxis()->SetTitle("X [cm]");
		hist5->GetYaxis()->SetTitle("Y [cm]");
		hist5->GetZaxis()->SetTitle("B [kGauss]");
		hist5->GetZaxis()->SetTitleOffset(1.7);
		gPad->SetRightMargin(0.2);
		hist5->Draw("colz");


		std::string name(canvas[i]->GetName());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".gif").c_str());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".eps").c_str());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".svg").c_str());
	}
}

void CbmLitCheckField::DrawHistosPoly(
		const std::string& opt)
{
	std::string names[] = {"Bx", "By", "Bz"};
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::stringstream ss;
		ss << opt + " degree at z=" << fZpos[s];
		canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1500,500);
		canvas[s]->Divide(4, 1);
	}

	for (int i = 0; i < fNofSlices; i++) {
		TLegend* l1 = new TLegend(0.1,0.1,0.9,0.9);
		l1->SetFillColor(kWhite);
		l1->SetTextSize(0.1);
		l1->SetLineWidth(7);
		l1->SetHeader("");
		for (int v = 0; v < 3; v++) {
			TH1D* firsthist;
			if (opt == "rel") firsthist = fhBRelErrH1D[v][i][0];
			else firsthist = fhBErrH1D[v][i][0];
			double max = firsthist->GetMaximum();
			for (int j = 0; j < fNofPolynoms; j++) {
				canvas[i]->cd(v+2);
				TH1D* hist1;
				if (opt == "rel") hist1 = fhBRelErrH1D[v][i][j];
				else hist1 = fhBErrH1D[v][i][j];
				if (max < hist1->GetMaximum()) max = hist1->GetMaximum();
				if (opt == "rel") hist1->GetXaxis()->SetTitle("Relative error [%]");
				else hist1->GetXaxis()->SetTitle("B [kGauss]");
				hist1->SetLineColor(1+j);
				hist1->SetLineWidth(3);
				hist1->SetLineStyle(1+j);
				gPad->SetLogy();
				if (j == 0) hist1->Draw(); else hist1->Draw("SAME");

				if (v == 0) {
					std::stringstream ss;
					ss << fDegrees[j];
					l1->AddEntry(hist1, ss.str().c_str(),"lp");
				}
			}
			firsthist->SetMaximum(1.2*max);
		}

		canvas[i]->cd(1);
		l1->Draw();

		std::string name(canvas[i]->GetName());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".gif").c_str());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".eps").c_str());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".svg").c_str());
	}
}


void CbmLitCheckField::DrawHistosPhd(
		Int_t v)
{
	gStyle->SetOptTitle(0);
	gStyle->SetPalette(0);
	gStyle->SetOptStat(0);
	std::string names[] = {"phd_Bx", "phd_By", "phd_Bz"};
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::stringstream ss;
		ss << names[v] << " at z=" << fZpos[s];
		canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 800,400);
		canvas[s]->Divide(2, 1);
	}

	for (int i = 0; i < fNofSlices; i++) {
		canvas[i]->cd(1);
		TGraph2D* graph2 = fhBAprGraph[v][i][fPolynomDegree];
//		graph2->Scale(0.01);
		graph2->GetXaxis()->SetTitle("X [cm]");
		graph2->GetYaxis()->SetTitle("Y [cm]");
		graph2->GetZaxis()->SetTitle("B [kGauss]");
		graph2->GetXaxis()->SetTitleOffset(1.7);
		graph2->GetYaxis()->SetTitleOffset(2);
		graph2->GetZaxis()->SetTitleOffset(1.8);
		gPad->SetLeftMargin(0.2);
		graph2->Draw("TRI1");

		canvas[i]->cd(2);
		TH2D* hist3 = fhBErrH2D[v][i][fPolynomDegree];
//		hist3->Scale(0.01);
		hist3->GetXaxis()->SetTitle("X [cm]");
		hist3->GetYaxis()->SetTitle("Y [cm]");
		hist3->GetZaxis()->SetTitle("B [kGauss]");
		hist3->GetZaxis()->SetTitleOffset(1.7);
		gPad->SetRightMargin(0.2);
		hist3->Draw("colz");

		std::string name(canvas[i]->GetName());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".gif").c_str());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".eps").c_str());
		canvas[i]->SaveAs(std::string(fOutputDir + name + ".svg").c_str());
	}
}

ClassImp(CbmLitCheckField);
