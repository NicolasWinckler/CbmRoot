#include "CbmLitCheckField.h"
#include "CbmLitEnvironment.h"
#include "CbmLitFloat.h"
#include "CbmLitFieldFitter.h"
#include "CbmLitUtils.h"
#include "CbmLitDrawHist.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TH2D.h"
#include "TGraph2D.h"
#include "TGraph.h"
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
#include <limits>


CbmLitCheckField::CbmLitCheckField():
	fDrawBx(true),
	fDrawBy(true),
	fDrawBz(true),
	fDrawMod(true),
	fDrawFieldMap(true),
	fDrawPhd(true),
	fDrawPoly(true),
	fDrawSlices(true),
	fFixedBounds(true),
	fNofSlices(0),
	fXangle(25.),
	fYangle(25.),
	fNofBinsX(100),
	fNofBinsY(100),
	fUseEllipseAcc(true),
	fOutputDir("./field/"),
	fPolynomDegreeIndex(1),
	fNofPolynoms(4),
	fZMin(-10.),
	fZMax(300.),
	fZStep(5.)
{

}

CbmLitCheckField::~CbmLitCheckField()
{
}

InitStatus CbmLitCheckField::Init()
{
	// Set draw styles
	SetStyles();

//	fDegrees.push_back(3);
//	fDegrees.push_back(5);
	fDegrees.push_back(7);
//	fDegrees.push_back(9);
	fNofPolynoms = fDegrees.size();

	fZpos.push_back(30.);
	fZpos.push_back(50.);
	fZpos.push_back(100.);
	fZpos.push_back(105.);
	fZpos.push_back(115.);
	fZpos.push_back(125.);
//	fZpos.push_back(135.);
	fZpos.push_back(145.);
//	fZpos.push_back(155.);
	fZpos.push_back(165.);
	fZpos.push_back(200.);
	fZpos.push_back(300.);
	fZpos.push_back(400.);
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

	fAlongZAngles.push_back(0.);
	fAlongZAngles.push_back(5.);
	fAlongZAngles.push_back(10.);
	fAlongZAngles.push_back(15.);
	fAlongZAngles.push_back(20.);
	fAlongZAngles.push_back(25.);
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

	CreateFieldFitter();
	std::cout << "Field fitter created" << std::endl;
	CreateHistos();
	std::cout << "Histograms created" << std::endl;
	FillBHistos();
	std::cout << "B histograms filled" << std::endl;
	FillErrHistos();
	std::cout << "Error histograms filled" << std::endl;
	FillHistosAlongZ();
	std::cout << "B histograms along Z filled" << std::endl;

	if (fDrawSlices) {
		if (IsDrawBx()) DrawSlices(BX);
		if (IsDrawBy()) DrawSlices(BY);
		if (IsDrawBz()) DrawSlices(BZ);
		if (IsDrawBz()) DrawSlices(MOD);
	}

	if (fDrawPoly) {
		DrawPoly("rel");
		DrawPoly("abs");
	}

	if (fDrawPhd) {
		if (IsDrawBx()) DrawPhd(BX);
		if (IsDrawBy()) DrawPhd(BY);
		if (IsDrawBz()) DrawPhd(BZ);
		if (IsDrawBz()) DrawPhd(MOD);
	}

	if (fDrawFieldMap) {
		DrawField();
		DrawFieldAlongZ();
	}
}

void CbmLitCheckField::Finish()
{
}

void CbmLitCheckField::CreateFieldFitter()
{
	// Fit the magnetic field
	fFitter.resize(fNofPolynoms);
	for (unsigned int i = 0; i < fNofPolynoms; i++) {
		std::cout << "Fit with " << fDegrees[i] << " degree polynom" << std::endl;
		fFitter[i] = new CbmLitFieldFitter(fDegrees[i]);
		fFitter[i]->SetXangle(fXangle);
		fFitter[i]->SetYangle(fYangle);
		fFitter[i]->SetNofBinsX(100);//fNofBinsX);
		fFitter[i]->SetNofBinsY(100);//fNofBinsY);
		fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
		for(Int_t j = 0; j < fNofSlices; j++) {
//			std::cout << "Fit slice at " << fZpos[j] << std::endl;
			fFitter[i]->FitSliceMy(fZpos[j], fCx[j][i], fCy[j][i], fCz[j][i]);
		}
	}
}

void CbmLitCheckField::CreateHistos()
{
	// Histogram list
	fHistoList = new TList();

	// resize histogram vectors
	fhBGraph.resize(4);
	fhBAprGraph.resize(4);
	fhBErrH1D.resize(4);
	fhBErrH2D.resize(4);
	fhBRelErrH1D.resize(4);
	fhBRelErrH2D.resize(4);
	for (Int_t i = 0; i < 4; i++) {
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

	std::string names[] = {"Bx", "By", "Bz", "Mod"};
	std::string namesErrH1D[] = {"BxErrH1D", "ByErrH1D", "BzErrH1D", "ModErrH1D"};
	std::string namesErrH2D[] = {"BxErrH2D", "ByErrH2D", "BzErrH2D", "ModErrH2D"};
	std::string namesRelErrH1D[] = {"BxRelErrH1D", "ByRelErrH1D", "BzRelErrH1D", "ModRelErrH1D"};
	std::string namesRelErrH2D[] = {"BxRelErrH2D", "ByRelErrH2D", "BzRelErrH2D", "ModRelErrH2D"};

	Int_t nofBinsX = fNofBinsX;
	Int_t nofBinsY = fNofBinsY;
	Int_t nofBinsErrB = 100;
	Int_t nofBinsRelErrB = 100;
	Int_t nofBinsErrX = 100;
	Int_t nofBinsErrY = 100;
	Double_t minErrB = 0., maxErrB = 0., minRelErrB = 0., maxRelErrB = 0.;
	if (fFixedBounds) {
		minErrB = -0.5;
		maxErrB = 0.5;
		minRelErrB = -10.;
		maxRelErrB = 10.;
	}

	// Create histograms
	for (Int_t v = 0; v < 4; v++) {
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

	fhBAlongZGraph.resize(3);
	for (Int_t i = 0; i < 3; i++) {
		fhBAlongZGraph[i].resize(fAlongZAngles.size());
	}
	for (Int_t v = 0; v < 3; v++) {
		for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
			fhBAlongZGraph[v][i] = new TGraph();
		}
	}

	std::string namesErrAlongZH1D[] = {"BxErrAlongZH1D", "ByErrAlongZH1D", "BzErrAlongZH1D"};
	std::string namesRelErrAlongZH1D[] = {"BxRelErrAlongZH1D", "ByRelErrAlongZH1D", "BzRelErrAlongZH1D"};
	Int_t nofZHistos = Int_t((fZMax - fZMin) / fZStep);
	fhBErrAlongZH1D.resize(3);
	fhBRelErrAlongZH1D.resize(3);
	for (Int_t v = 0; v < 3; v++) {
		fhBErrAlongZH1D[v].resize(fNofPolynoms);
		fhBRelErrAlongZH1D[v].resize(fNofPolynoms);
	}
	for (Int_t v = 0; v < 3; v++) {
		for (Int_t i = 0; i < fNofPolynoms; i++) {
			std::stringstream histName2, histTitle2;
			histName2 << "h" << namesErrAlongZH1D[v] << i;
			histTitle2 << namesErrAlongZH1D[v] << " degree " << fDegrees[i];
			fhBErrAlongZH1D[v][i] = new TH1D(histName2.str().c_str(), histTitle2.str().c_str(), nofBinsErrB, minErrB, maxErrB);
			fHistoList->Add(fhBErrAlongZH1D[v][i]);

			std::stringstream histName3, histTitle3;
			histName3 << "h" << namesRelErrAlongZH1D[v] << i;
			histTitle3 << namesRelErrAlongZH1D[v] << " degree " << fDegrees[i];
			fhBErrAlongZH1D[v][i] = new TH1D(histName3.str().c_str(), histTitle3.str().c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
			fHistoList->Add(fhBErrAlongZH1D[v][i]);
		}
	}
//	std::vector<std::vector<TH1D*> > fhBErrAlongZH1D;
}

void CbmLitCheckField::FillBHistos()
{
	for (unsigned int iSlice = 0; iSlice < fNofSlices; iSlice++) { // loop over z positions
		std::cout << "Fill B histograms for slice " << iSlice << std::endl;
		double max[4] = {
				std::numeric_limits<double>::min(),
				std::numeric_limits<double>::min(),
				std::numeric_limits<double>::min(),
				std::numeric_limits<double>::min()};
		double min[4] = {
				std::numeric_limits<double>::max(),
				std::numeric_limits<double>::max(),
				std::numeric_limits<double>::max(),
				std::numeric_limits<double>::max()};

		double Z = fZpos[iSlice];

		int cnt = 0;

		double HX = 2 * fXpos[iSlice] / fNofBinsX; // step size for X position
		double HY = 2 * fYpos[iSlice] / fNofBinsY; // step size for Y position
		for (int iX = 0; iX < fNofBinsX; iX++) { // loop over x position
			double X = -fXpos[iSlice] + (iX + 0.5) * HX;
			for (int iY = 0; iY < fNofBinsY; iY++) { // loop over y position
				double Y = -fYpos[iSlice] + (iY + 0.5)  * HY;

				//check the acceptance
				double el = (X*X)/(fXpos[iSlice]*fXpos[iSlice]) + (Y*Y)/(fYpos[iSlice]*fYpos[iSlice]);
				if (fUseEllipseAcc && el > 1.) continue;

				// get field value
				double pos[3] = {X, Y, Z};
				double B[3];
				fField->GetFieldValue(pos, B);

				Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

				// find maximum entry in the histogram
				if (max[BX] < B[0]) max[BX] = B[0];
				if (max[BY] < B[1]) max[BY] = B[1];
				if (max[BZ] < B[2]) max[BZ] = B[2];
				if (max[MOD] < Bmod) max[MOD] = Bmod;
				//find minimum entry in the histogram
				if (min[BX] > B[0]) min[BX] = B[0];
				if (min[BY] > B[1]) min[BY] = B[1];
				if (min[BZ] > B[2]) min[BZ] = B[2];
				if (min[MOD] > Bmod) min[MOD] = Bmod;

				fhBGraph[BX][iSlice]->SetPoint(cnt, X, Y, B[BX]);
				fhBGraph[BY][iSlice]->SetPoint(cnt, X, Y, B[BY]);
				fhBGraph[BZ][iSlice]->SetPoint(cnt, X, Y, B[BZ]);
				fhBGraph[MOD][iSlice]->SetPoint(cnt, X, Y, Bmod);

				for (int p = 0; p < fNofPolynoms; p++) {
					Double_t Bx = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCx[iSlice][p][0]);
					Double_t By = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCy[iSlice][p][0]);
					Double_t Bz = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCz[iSlice][p][0]);
					Double_t mod = std::sqrt(Bx*Bx + By*By + Bz*Bz);
					fhBAprGraph[BX][iSlice][p]->SetPoint(cnt, X, Y, Bx);
					fhBAprGraph[BY][iSlice][p]->SetPoint(cnt, X, Y, By);
					fhBAprGraph[BZ][iSlice][p]->SetPoint(cnt, X, Y, Bz);
					fhBAprGraph[MOD][iSlice][p]->SetPoint(cnt, X, Y, mod);
				}
				cnt++;
			}
		}
		fhBGraph[BX][iSlice]->SetMinimum(min[BX] - 0.1 * std::abs(min[BX]));
		fhBGraph[BY][iSlice]->SetMinimum(min[BY] - 0.1 * std::abs(min[BY]));
		fhBGraph[BZ][iSlice]->SetMinimum(min[BZ] - 0.1 * std::abs(min[BZ]));
		fhBGraph[MOD][iSlice]->SetMinimum(min[MOD] - 0.1 * std::abs(min[MOD]));

		fhBGraph[BX][iSlice]->SetMaximum(max[BX] + 0.1 * std::abs(max[BX]));
		fhBGraph[BY][iSlice]->SetMaximum(max[BY] + 0.1 * std::abs(max[BY]));
		fhBGraph[BZ][iSlice]->SetMaximum(max[BZ] + 0.1 * std::abs(max[BZ]));
		fhBGraph[MOD][iSlice]->SetMaximum(max[MOD] + 0.1 * std::abs(max[MOD]));

		for (int p = 0; p < fNofPolynoms; p++) {
			fhBAprGraph[BX][iSlice][p]->SetMinimum(min[BX] - 0.1 * std::abs(min[BX]));
			fhBAprGraph[BY][iSlice][p]->SetMinimum(min[BY] - 0.1 * std::abs(min[BY]));
			fhBAprGraph[BZ][iSlice][p]->SetMinimum(min[BZ] - 0.1 * std::abs(min[BZ]));
			fhBAprGraph[MOD][iSlice][p]->SetMinimum(min[MOD] - 0.1 * std::abs(min[MOD]));

			fhBAprGraph[BX][iSlice][p]->SetMaximum(max[BX] + 0.1 * std::abs(max[BX]));
			fhBAprGraph[BY][iSlice][p]->SetMaximum(max[BY] + 0.1 * std::abs(max[BY]));
			fhBAprGraph[BZ][iSlice][p]->SetMaximum(max[BZ] + 0.1 * std::abs(max[BZ]));
			fhBAprGraph[MOD][iSlice][p]->SetMaximum(max[MOD] + 0.1 * std::abs(max[MOD]));
		}
	}
}

void CbmLitCheckField::FillErrHistos()
{
	Int_t nofBinsX = 100;
	Int_t nofBinsY = 100;
	for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
		std::cout << "Fill error histograms for slice " << iSlice << std::endl;
		Double_t Z = fZpos[iSlice];
		Double_t HX = 2 * fXpos[iSlice] / nofBinsX; // step size for X position
		Double_t HY = 2 * fYpos[iSlice] / nofBinsY; // step size for Y position
		for (int iX = 0; iX < nofBinsX; iX++) { // loop over x position
			Double_t X = -fXpos[iSlice] + (iX+0.5) * HX;
			for (int iY = 0; iY < nofBinsY; iY++) { // loop over y position
				Double_t Y = -fYpos[iSlice] + (iY+0.5) * HY;

				//check the acceptance
				if (fUseEllipseAcc) {
					double el = (X*X)/(fXpos[iSlice]*fXpos[iSlice]) + (Y*Y)/(fYpos[iSlice]*fYpos[iSlice]);
					if (el > 1.) continue;
				}

				// get field value
				Double_t pos[3] = {X, Y, Z};
				Double_t B[3];
				fField->GetFieldValue(pos, B);

				double Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

				for (int p = 0; p < fNofPolynoms; p++) {
					double aprBx = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCx[iSlice][p][0]);
					double aprBy = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCy[iSlice][p][0]);
					double aprBz = fFitter[p]->GetPolynom()->Calculate(X, Y, &fCz[iSlice][p][0]);
					double aprMod = std::sqrt(aprBx*aprBx + aprBy*aprBy + aprBz*aprBz);

					Double_t errBx = B[0] - aprBx;
					Double_t errBy = B[1] - aprBy;
					Double_t errBz = B[2] - aprBz;
					Double_t errMod = Bmod - aprMod;
					Double_t relErrBx = 0., relErrBy = 0., relErrBz = 0., relErrMod = 0.;
					if (B[0] != 0.) relErrBx = (errBx / B[0]) * 100.; else relErrBx = 0.;
					if (B[1] != 0.) relErrBy = (errBy / B[1]) * 100.; else relErrBy = 0.;
					if (B[2] != 0.) relErrBz = (errBz / B[2]) * 100.; else relErrBz = 0.;
					if (Bmod != 0.) relErrMod = (errMod / Bmod) * 100.; else relErrMod = 0.;

					fhBErrH2D[BX][iSlice][p]->Fill(X, Y, errBx);
					fhBErrH1D[BX][iSlice][p]->Fill(errBx);
					fhBRelErrH1D[BX][iSlice][p]->Fill(relErrBx);
					fhBRelErrH2D[BX][iSlice][p]->Fill(X, Y, relErrBx);
					fhBErrH2D[BY][iSlice][p]->Fill(X, Y, errBy);
					fhBErrH1D[BY][iSlice][p]->Fill(errBy);
					fhBRelErrH1D[BY][iSlice][p]->Fill(relErrBy);
					fhBRelErrH2D[BY][iSlice][p]->Fill(X, Y, relErrBy);
					fhBErrH2D[BZ][iSlice][p]->Fill(X, Y, errBz);
					fhBErrH1D[BZ][iSlice][p]->Fill(errBz);
					fhBRelErrH1D[BZ][iSlice][p]->Fill(relErrBz);
					fhBRelErrH2D[BZ][iSlice][p]->Fill(X, Y, relErrBz);
					fhBErrH2D[MOD][iSlice][p]->Fill(X, Y, errMod);
					fhBErrH1D[MOD][iSlice][p]->Fill(errMod);
					fhBRelErrH1D[MOD][iSlice][p]->Fill(relErrMod);
					fhBRelErrH2D[MOD][iSlice][p]->Fill(X, Y, relErrMod);
				}
			}
		}
	}
}

void CbmLitCheckField::FillErrHistosAlongZ()
{
	for (unsigned int i = 0; i < fNofPolynoms; i++) {

	}
}

void CbmLitCheckField::FillHistosAlongZ()
{
	for (int i = 0; i < fAlongZAngles.size(); i++) {
		int nofSteps = int((fZMax - fZMin) / fZStep);
		for (int istep = 0; istep < nofSteps; istep++){
			double Z = fZMin + istep * fZStep;
			double tanXangle = std::tan(fAlongZAngles[i]*3.14159265/180); //
			double tanYangle = std::tan(fAlongZAngles[i]*3.14159265/180); //
			double X = Z * tanXangle;
			double Y = Z * tanYangle;

			// get field value
			double pos[3] = {X, Y, Z};
			double B[3];
			fField->GetFieldValue(pos, B);

			fhBAlongZGraph[BX][i]->SetPoint(istep, Z, B[0]);
			fhBAlongZGraph[BY][i]->SetPoint(istep, Z, B[1]);
			fhBAlongZGraph[BZ][i]->SetPoint(istep, Z, B[2]);
		}
	}
}

void CbmLitCheckField::DrawSlices(
		Int_t v)
{
	std::string names[] = {"field_slice_Bx_", "field_slice_By_", "field_slice_Bz_", "field_slice_Mod_"};
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::stringstream ss;
		ss << names[v] << "z_" << fZpos[s];
		canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1200,800);
		canvas[s]->Divide(3, 2);
	}

	std::string title = "B_{x}";
	if (v == 1) title = "B_{y}";
	if (v == 2) title = "B_{z}";
	if (v == 3) title = "|B|";
	for (int i = 0; i < fNofSlices; i++) {
		canvas[i]->cd(1);
		TGraph2D* graph1 = fhBGraph[v][i];
		DrawGraph2D(graph1, "X [cm]", "Y [cm]", std::string(title + " [kGauss]"),
										false, false, false, "TRI1");

		canvas[i]->cd(2);
		TH1D* hist2 = fhBErrH1D[v][i][fPolynomDegreeIndex];
		DrawHist1D(hist2, std::string(title + " [kGauss]"), "Counter",
			   		LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
			   		LIT_MARKER_STYLE1, false, true, "");

		canvas[i]->cd(3);
		TH2D* hist3 = fhBErrH2D[v][i][fPolynomDegreeIndex];
		DrawHist2D(hist3, "X [cm]", "Y [cm]", std::string(title + " [kGauss]"),
				false, false, false, "colz");

		canvas[i]->cd(4);
		TGraph2D* graph2 = fhBAprGraph[v][i][fPolynomDegreeIndex];
		DrawGraph2D(graph2, "X [cm]", "Y [cm]", std::string(title + " [kGauss]"),
								false, false, false, "TRI1");

		canvas[i]->cd(5);
		TH1D* hist4 = fhBRelErrH1D[v][i][fPolynomDegreeIndex];
		DrawHist1D(hist4, std::string(title + " relative error [%]"), "Counter",
				LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
				LIT_MARKER_STYLE1, false, true, "");

		canvas[i]->cd(6);
		TH2D* hist5 = fhBRelErrH2D[v][i][fPolynomDegreeIndex];
		DrawHist2D(hist5, "X [cm]", "Y [cm]", std::string(title + " relative error [%]"),
						false, false, false, "colz");

		SaveCanvasAsImage(canvas[i], fOutputDir);
	}
}

void CbmLitCheckField::DrawPoly(
		const std::string& opt)
{
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::stringstream ss;
		ss << "field_" + opt + "_degree_z_" << fZpos[s];
		canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1300,800);
		canvas[s]->Divide(3, 2);
	}

	for (int i = 0; i < fNofSlices; i++) {
		TLegend* l1 = new TLegend(0.1,0.1,0.9,0.9);
		l1->SetFillColor(kWhite);
		l1->SetTextSize(0.1);
		l1->SetLineWidth(1);
		l1->SetHeader("Polynom degree");
		for (int v = 0; v < 4; v++) {
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
				std::string title;
				if (opt == "rel") {
					if (v == 0) title = "B_{x} relative error [%]";
					if (v == 1) title = "B_{y} relative error [%]";
					if (v == 2) title = "B_{z} relative error [%]";
					if (v == 3) title = "|B| relative error [%]";
				} else {
					if (v == 0) title = "B_{x} [kGauss]";
					if (v == 1) title = "B_{y} [kGauss]";
					if (v == 2) title = "B_{z} [kGauss]";
					if (v == 3) title = "|B| [kGauss]";
				}
				std::string draw_opt;
				if (j == 0) draw_opt = ""; else draw_opt = "SAME";
				DrawHist1D(hist1, title, "Counter",
						1+j, LIT_LINE_WIDTH, 1+j, LIT_MARKER_SIZE,
						kDot, false, true, draw_opt.c_str());

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

		SaveCanvasAsImage(canvas[i], fOutputDir);
	}
}

void CbmLitCheckField::DrawPhd(
		Int_t v)
{
	std::string names[] = {"field_phd_Bx_", "field_phd_By_", "field_phd_Bz_", "field_phd_Mod_"};
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::stringstream ss;
		ss << names[v] << "z_" << fZpos[s];
		canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 800,400);
		canvas[s]->Divide(2, 1);
	}

	std::string title = "B_{x} [kGauss]";
	if (v == 1) title = "B_{y} [kGauss]";
	if (v == 2) title = "B_{z} [kGauss]";
	if (v == 3) title = "|B| [kGauss]";
	for (int i = 0; i < fNofSlices; i++) {
		canvas[i]->cd(1);
		TGraph2D* graph2 = fhBAprGraph[v][i][fPolynomDegreeIndex];
		DrawGraph2D(graph2, "X [cm]", "Y [cm]", title,
								false, false, false, "TRI1");

		canvas[i]->cd(2);
		TH2D* hist3 = fhBErrH2D[v][i][fPolynomDegreeIndex];
		DrawHist2D(hist3, "X [cm]", "Y [cm]", title,
						false, false, false, "colz");

		SaveCanvasAsImage(canvas[i], fOutputDir);
	}
}

void CbmLitCheckField::DrawField()
{
	TCanvas* canvas[fNofSlices];
	for (Int_t s = 0; s < fNofSlices; s++) {
		std::string ss = "field_map_at_z_" +ToString<Double_t>(fZpos[s]);
		canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 1200,400);
		canvas[s]->Divide(2, 2);
	}

	for (int i = 0; i < fNofSlices; i++) {
		canvas[i]->cd(1);
		TGraph2D* graphBx = fhBGraph[BX][i];
		DrawGraph2D(graphBx, "X [cm]", "Y [cm]", "B_{x} [kGauss]",
					false, false, false, "TRI1");

		canvas[i]->cd(2);
		TGraph2D* graphBy = fhBGraph[BY][i];
		DrawGraph2D(graphBy, "X [cm]", "Y [cm]", "B_{y} [kGauss]",
					false, false, false, "TRI1");

		canvas[i]->cd(3);
		TGraph2D* graphBz = fhBGraph[BZ][i];
		DrawGraph2D(graphBz, "X [cm]", "Y [cm]", "B_{z} [kGauss]",
					false, false, false, "TRI1");

		canvas[i]->cd(4);
		TGraph2D* graphMod = fhBGraph[MOD][i];
		DrawGraph2D(graphMod, "X [cm]", "Y [cm]", "|B| [kGauss]",
					false, false, false, "TRI1");

		SaveCanvasAsImage(canvas[i], fOutputDir);
	}
}

void CbmLitCheckField::DrawFieldAlongZ()
{
	TCanvas* canvas = new TCanvas("field_map_along_z", "field_along_z", 1200, 800);
	canvas->Divide(3, 2);

	for (int i = 0; i < fAlongZAngles.size(); i++) {
		canvas->cd(i+1);
		TGraph* graphBx = fhBAlongZGraph[BX][i];
		TGraph* graphBy = fhBAlongZGraph[BY][i];
		TGraph* graphBz = fhBAlongZGraph[BZ][i];

		DrawGraph(graphBx, graphBy, graphBz,
				std::string(ToString<double>(fAlongZAngles[i]) + "#circ"),
				"Z [cm]", "B [kGauss]",
				"B_{x}", "B_{y}", "B_{z}", false, false, true,
				0.7, 0.5, 0.9, 0.3);
	}
	SaveCanvasAsImage(canvas, fOutputDir);
}
ClassImp(CbmLitCheckField);
