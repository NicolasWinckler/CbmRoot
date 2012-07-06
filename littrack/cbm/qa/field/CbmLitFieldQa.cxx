/**
 * \file CbmLitFieldQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "qa/field/CbmLitFieldQa.h"

#include "base/CbmLitEnvironment.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitFieldFitter.h"
#include "base/CbmLitFieldGridCreator.h"
#include "utils/CbmLitUtils.h"
#include "cbm/qa/draw/CbmLitDrawHist.h"
#include "../field/CbmLitFieldQaReport.h"

#include "../../../parallel/LitField.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <cmath>
#include <string>
#include <limits>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>

using lit::ToString;
using lit::SaveCanvasAsImage;
using lit::parallel::LitFieldSlice;
using lit::parallel::LitFieldValue;
using lit::parallel::LitFieldGrid;
using boost::assign::list_of;

CbmLitFieldQa::CbmLitFieldQa():
   fField(NULL),
   fHistoList(NULL),
   fNofSlices(0),
   fZpos(),
   fXpos(),
   fYpos(),
   fhB(),
   fhBPolynomial(),
   fhBPolynomialErrH2(),
   fhBPolynomialErrH1(),
   fhBPolynomialRelErrH1(),
   fhBPolynomialRelErrH2(),
   fhBAlongZGraph(),
   fAlongZAngles(),
   fZMin(-10.),
   fZMax(300.),
   fZStep(10.),
   fCheckFieldMap(true),
   fCheckFieldApproximation(true),
   fCheckGridCreator(true),
   fDrawFieldMap(true),
   fDrawFieldApproximation(true),
   fDrawGridCreator(true),
   fDrawBx(true),
   fDrawBy(true),
   fDrawBz(true),
   fDrawMod(true),
   fFixedBounds(true),
   fXangle(25.),
   fYangle(25.),
   fNofBinsX(100),
   fNofBinsY(100),
   fUseEllipseAcc(true),
   fOutputDir("./field/"),
   fFitter(),
   fPolynomDegreeIndex(1),
   fNofPolynoms(4),
   fPolynomDegrees()
{

}

CbmLitFieldQa::~CbmLitFieldQa()
{
}

InitStatus CbmLitFieldQa::Init()
{
   // Set draw styles
   SetStyles();

   fNofSlices = fZpos.size();
   fNofPolynoms = fPolynomDegrees.size();

   // Calculate (X, Y) window for each slice
   fXpos.resize(fNofSlices);
   fYpos.resize(fNofSlices);
   for (Int_t i = 0; i < fNofSlices; i++) {
      Double_t tanXangle = std::tan(fXangle * 3.14159265 / 180); //
      Double_t tanYangle = std::tan(fYangle * 3.14159265 / 180); //
      fXpos[i] = fZpos[i] * tanXangle;
      fYpos[i] = fZpos[i] * tanYangle;
   }

   //
   fAlongZAngles.push_back(0.);
   fAlongZAngles.push_back(5.);
   fAlongZAngles.push_back(10.);
   fAlongZAngles.push_back(15.);
   fAlongZAngles.push_back(20.);
   fAlongZAngles.push_back(25.);

   // Create field approximation tool for each polynom degree
   if (fCheckFieldApproximation) {
      fFitter.resize(fNofPolynoms);
      for (UInt_t i = 0; i < fNofPolynoms; i++) {
         fFitter[i] = new CbmLitFieldFitter(fPolynomDegrees[i]);
         fFitter[i]->SetXangle(fXangle);
         fFitter[i]->SetYangle(fYangle);
         fFitter[i]->SetNofBinsX(fNofBinsX);
         fFitter[i]->SetNofBinsY(fNofBinsY);
         fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
      }
   }

   // Create grid creator tool
   if (fCheckGridCreator) {
      fGridCreator = new CbmLitFieldGridCreator();
   }

   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fField = env->GetField();
   fField->Print();
   return kSUCCESS;
}

void CbmLitFieldQa::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("FairFieldPar");
}

void CbmLitFieldQa::Exec(
   Option_t* opt)
{

}

void CbmLitFieldQa::Finish()
{
   // Fill and create graphs and histograms
   CreateHistos();

   // This is always needed since field map is always drawn for comparison
   FillBHistos();

   // Check and draw field map
   if (fCheckFieldMap) {
	  if (fDrawFieldMap) {
		 DrawFieldSlices();
		 DrawFieldAlongZ();
	  }
   }

   // Check and draw field approximation
   if (fCheckFieldApproximation) {
	  FillFieldApproximationHistos();
	  if (fDrawFieldApproximation) {
		 if (fDrawBx) { DrawSlices(BX, "apr"); }
		 if (fDrawBy) { DrawSlices(BY, "apr"); }
		 if (fDrawBz) { DrawSlices(BZ, "apr"); }
		 if (fDrawMod) { DrawSlices(MOD, "apr"); }

		 DrawPoly("rel");
		 DrawPoly("abs");
	  }
   }

   // Check and draw histograms for grid creator tool
   if (fCheckGridCreator) {
	  FillGridCreatorHistos();
	  if (fDrawGridCreator) {
		 if (fDrawBx) { DrawSlices(BX, "grid"); }
		 if (fDrawBy) { DrawSlices(BY, "grid"); }
		 if (fDrawBz) { DrawSlices(BZ, "grid"); }
		 if (fDrawMod) { DrawSlices(MOD, "grid"); }
	  }
   }

   CreatePropertyTree();

   TIter next(fHistoList);
   while ( TObject* histo = ((TObject*)next()) ) { histo->Write(); }
}

void CbmLitFieldQa::CreateHistos()
{
   fHistoList = new TList();

   CreateFieldHistos();
   CreateFitterHistos();
   CreateGridHistos();
}

void CbmLitFieldQa::CreateFieldHistos()
{
   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};

   // [BX, BY, BZ, MOD] components
   fhB.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhB[i].resize(fNofSlices);
   }
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
    	  string histName = "hb_component_" + lit::ToString<Int_t>(v) + "_slice_" + lit::ToString<Int_t>(i);
    	  string histTitle = histName + ";X [cm];Y [cm];" + zTitle[v];
          fhB[v][i] = new TH2D(histName.c_str(), histTitle.c_str(), fNofBinsX, -fXpos[i], fXpos[i], fNofBinsY, -fYpos[i], fYpos[i]);
          fHistoList->Add(fhB[v][i]);
      }
   }

   // [BX, BY, BZ]
   fhBAlongZGraph.resize(3);
   for (Int_t i = 0; i < 3; i++) {
      fhBAlongZGraph[i].resize(fAlongZAngles.size());
   }
   for (Int_t v = 0; v < 3; v++) {
      for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
    	  string histName = "halongz_component_" + lit::ToString<Int_t>(v) + "_angle_" + lit::ToString<Int_t>(i);
    	  string histTitle = histName + ";Z [cm];" + zTitle[v];
    	  Int_t nbins = (fZMax - fZMin) / fZStep;
    	  std::cout << "nbins=" << nbins << " fZMax=" << fZMax << " fZMin=" << fZMin << " fZStep=" << fZStep << std::endl;
    	  fhBAlongZGraph[v][i] = new TH1D(histName.c_str(), histTitle.c_str(), nbins, fZMin, fZMax);
    	  fHistoList->Add(fhBAlongZGraph[v][i] );
      }
   }
}

void CbmLitFieldQa::CreateFitterHistos()
{
   // Resize histogram vectors
   fhBPolynomial.resize(4);
   fhBPolynomialErrH1.resize(4);
   fhBPolynomialErrH2.resize(4);
   fhBPolynomialRelErrH1.resize(4);
   fhBPolynomialRelErrH2.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBPolynomial[i].resize(fNofSlices);
      fhBPolynomialErrH1[i].resize(fNofSlices);
      fhBPolynomialErrH2[i].resize(fNofSlices);
      fhBPolynomialRelErrH1[i].resize(fNofSlices);
      fhBPolynomialRelErrH2[i].resize(fNofSlices);
      for (Int_t j = 0; j < fNofSlices; j++) {
         fhBPolynomial[i][j].resize(fNofPolynoms);
         fhBPolynomialErrH1[i][j].resize(fNofPolynoms);
         fhBPolynomialErrH2[i][j].resize(fNofPolynoms);
         fhBPolynomialRelErrH1[i][j].resize(fNofPolynoms);
         fhBPolynomialRelErrH2[i][j].resize(fNofPolynoms);
      }
   }

   string names[] = {"Bx", "By", "Bz", "Mod"};

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

   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};
   string errTitle[] = {"B_{x} error [%]", "B_{y} error [%]", "B_{z} error [%]", "|B| error [%]"};
   string relErrTitle[] = {"B_{x} relative error [%]", "B_{y} relative error [%]", "B_{z} relative error [%]", "|B| relative error [%]"};

   // Create histograms
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         for(Int_t j = 0; j < fNofPolynoms; j++) {
       	    string histName = "hbapr_component_" + lit::ToString<Int_t>(v) + "_slice_" + lit::ToString<Int_t>(i);
       	    string histTitle = histName + ";X [cm];Y [cm];" + zTitle[v];
            fhBPolynomial[v][i][j] = new TH2D(histName.c_str(), histTitle.c_str(), fNofBinsX, -fXpos[i], fXpos[i], fNofBinsY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBPolynomial[v][i][j]);

            //string
            histName = "hBErrH1D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialErrH1[v][i][j] = new TH1D(histName.c_str(), string(histName + ";" + errTitle[v] + ";Counter").c_str(), nofBinsErrB, minErrB, maxErrB);
            fHistoList->Add(fhBPolynomialErrH1[v][i][j]);

            histName = "hBErrH2D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialErrH2[v][i][j] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + errTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBPolynomialErrH2[v][i][j]);

            histName = "hBRelErrH1D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialRelErrH1[v][i][j] = new TH1D(histName.c_str(), string(histName + ";" + relErrTitle[v] + ";Counter").c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
            fHistoList->Add(fhBPolynomialRelErrH1[v][i][j]);

            histName = "hBRelErrH2D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialRelErrH2[v][i][j] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + relErrTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBPolynomialRelErrH2[v][i][j]);
         }
      }
   }
   std::cout << "-I- CbmLitFieldQa::CreateFitterErrHistos: Field fitter error histograms created" << std::endl;
}

void CbmLitFieldQa::CreateGridHistos()
{
   // Resize histogram vectors
   fhBGrid.resize(4);
   fhBGridErrH1.resize(4);
   fhBGridErrH2.resize(4);
   fhBGridRelErrH1.resize(4);
   fhBGridRelErrH2.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBGrid[i].resize(fNofSlices);
      fhBGridErrH1[i].resize(fNofSlices);
      fhBGridErrH2[i].resize(fNofSlices);
      fhBGridRelErrH1[i].resize(fNofSlices);
      fhBGridRelErrH2[i].resize(fNofSlices);
   }

   string names[] = {"Bx", "By", "Bz", "Mod"};

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

   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};
   string errTitle[] = {"B_{x} error [%]", "B_{y} error [%]", "B_{z} error [%]", "|B| error [%]"};
   string relErrTitle[] = {"B_{x} relative error [%]", "B_{y} relative error [%]", "B_{z} relative error [%]", "|B| relative error [%]"};

   // Create histograms
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
    	 string histName = "hbgrid_component_" + lit::ToString<Int_t>(v) + "_slice_" + lit::ToString<Int_t>(i);
    	 string histTitle = histName + ";X [cm];Y [cm];" + zTitle[v];
         fhBGrid[v][i] = new TH2D(histName.c_str(), histTitle.c_str(), fNofBinsX, -fXpos[i], fXpos[i], fNofBinsY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhBGrid[v][i]);

         //string
         histName = "hGridBErrH1D" + names[v] + ToString<Int_t>(i);
         fhBGridErrH1[v][i] = new TH1D(histName.c_str(), string(histName + ";" + errTitle[v] + ";Counter").c_str(), nofBinsErrB, minErrB, maxErrB);
         fHistoList->Add(fhBGridErrH1[v][i]);

         histName = "hGridBErrH2D" + names[v] + ToString<Int_t>(i);
         fhBGridErrH2[v][i] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + errTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhBGridErrH2[v][i]);

         histName = "hGridBRelErrH1D" + names[v] + ToString<Int_t>(i);
         fhBGridRelErrH1[v][i] = new TH1D(histName.c_str(), string(histName + ";" + relErrTitle[v] + ";Counter").c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
         fHistoList->Add(fhBGridRelErrH1[v][i]);

         histName = "hGridBRelErrH2D" + names[v] + ToString<Int_t>(i);
         fhBGridRelErrH2[v][i] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + relErrTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhBGridRelErrH2[v][i]);
      }
   }
   std::cout << "-I- CbmLitFieldQa::CreateGridErrHistos(): Grid creator error histograms created" << std::endl;
}

void CbmLitFieldQa::FillBHistos()
{
   // Fill graphs for magnetic field for each (X, Y) slice
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // loop over slices
      Double_t Z = fZpos[iSlice];
      Double_t HX = 2 * fXpos[iSlice] / fNofBinsX; // step size for X position
      Double_t HY = 2 * fYpos[iSlice] / fNofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // loop over x position
         Double_t X = -fXpos[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // loop over y position
            Double_t Y = -fYpos[iSlice] + (iY + 0.5)  * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXpos[iSlice]*fXpos[iSlice]) + (Y*Y)/(fYpos[iSlice]*fYpos[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            TH1* hbx = fhB[BX][iSlice];
            hbx->SetBinContent(hbx->FindBin(X, Y), B[BX]);
            TH1* hby = fhB[BY][iSlice];
            hby->SetBinContent(hby->FindBin(X, Y), B[BY]);
            TH1* hbz = fhB[BZ][iSlice];
            hbz->SetBinContent(hbz->FindBin(X, Y), B[BZ]);
            TH1* hmod = fhB[MOD][iSlice];
            hmod->SetBinContent(hmod->FindBin(X, Y), Bmod);
         }
      }
   }

   // Fill histograms for magnetic field along Z
   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      Int_t nofSteps = Int_t((fZMax - fZMin) / fZStep);
      for (Int_t istep = 0; istep < nofSteps; istep++) {
         Double_t Z = fZMin + (0.5 + istep) * fZStep;
         Double_t tanXangle = std::tan(fAlongZAngles[i]*3.14159265/180); //
         Double_t tanYangle = std::tan(fAlongZAngles[i]*3.14159265/180); //
         Double_t X = Z * tanXangle;
         Double_t Y = Z * tanYangle;

         // Get field value
         Double_t pos[3] = {X, Y, Z};
         Double_t B[3];
         fField->GetFieldValue(pos, B);

         TH1* hbx = fhBAlongZGraph[BX][i];
         hbx->SetBinContent(hbx->FindBin(Z), B[BX]);
         TH1* hby = fhBAlongZGraph[BY][i];
		 hby->SetBinContent(hby->FindBin(Z), B[BY]);
		 TH1* hbz = fhBAlongZGraph[BZ][i];
	     hbz->SetBinContent(hbz->FindBin(Z), B[BZ]);
      }
   }
}

void CbmLitFieldQa::FillFieldApproximationHistos()
{
   vector<vector<LitFieldSlice<float> > > slices;
   slices.resize(fNofPolynoms);
   for (UInt_t i = 0; i < fNofPolynoms; i++) {slices[i].resize(fNofSlices);}

   // Approximate field in each slice for each polynomial degree
   for (UInt_t i = 0; i < fNofPolynoms; i++) {
      for(Int_t j = 0; j < fNofSlices; j++) {
         fFitter[i]->FitSlice(fZpos[j], slices[i][j]);
         std::cout << "-I- CbmLitFieldQa::FillFieldApproximationHistos: "
               << " field approximation (degree=" << fPolynomDegrees[i]
               << ", Z=" << fZpos[j] << ")" << std::endl;
      }
   }

   // Fill graph for approximated field map
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // Loop over slices
      Double_t Z = fZpos[iSlice];
      Double_t HX = 2 * fXpos[iSlice] / fNofBinsX; // Step size for X position
      Double_t HY = 2 * fYpos[iSlice] / fNofBinsY; // Step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // Loop over x position
         Double_t X = -fXpos[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // Loop over y position
            Double_t Y = -fYpos[iSlice] + (iY + 0.5)  * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXpos[iSlice]*fXpos[iSlice]) + (Y*Y)/(fYpos[iSlice]*fYpos[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            for (Int_t p = 0; p < fNofPolynoms; p++) {
               LitFieldValue<float> v;
               slices[p][iSlice].GetFieldValue(X, Y, v);
               Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
               TH1* hbx = fhBPolynomial[BX][iSlice][p];
               hbx->SetBinContent(hbx->FindBin(X, Y), v.Bx);
               TH1* hby = fhBPolynomial[BY][iSlice][p];
               hby->SetBinContent(hby->FindBin(X, Y), v.By);
               TH1* hbz = fhBPolynomial[BZ][iSlice][p];
               hbz->SetBinContent(hbz->FindBin(X, Y), v.Bz);
               TH1* hmod = fhBPolynomial[MOD][iSlice][p];
               hmod->SetBinContent(hmod->FindBin(X, Y), mod);
            }
         } // End loop over y position
      } // End loop over x position
   } // End loop over slices

	for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
		for (Int_t p = 0; p < fNofPolynoms; p++) {
			SubtructHistos(fhB[BX][iSlice], fhBPolynomial[BX][iSlice][p], fhBPolynomialErrH2[BX][iSlice][p]);
			DivideHistos(fhBPolynomialErrH2[BX][iSlice][p], fhB[BX][iSlice], fhBPolynomialRelErrH2[BX][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialErrH2[BX][iSlice][p], fhBPolynomialErrH1[BX][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialRelErrH2[BX][iSlice][p], fhBPolynomialRelErrH1[BX][iSlice][p]);
			SubtructHistos(fhB[BY][iSlice], fhBPolynomial[BY][iSlice][p], fhBPolynomialErrH2[BY][iSlice][p]);
			DivideHistos(fhBPolynomialErrH2[BY][iSlice][p], fhB[BY][iSlice], fhBPolynomialRelErrH2[BY][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialErrH2[BY][iSlice][p], fhBPolynomialErrH1[BY][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialRelErrH2[BY][iSlice][p], fhBPolynomialRelErrH1[BY][iSlice][p]);
			SubtructHistos(fhB[BZ][iSlice], fhBPolynomial[BZ][iSlice][p], fhBPolynomialErrH2[BZ][iSlice][p]);
			DivideHistos(fhBPolynomialErrH2[BZ][iSlice][p], fhB[BZ][iSlice], fhBPolynomialRelErrH2[BZ][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialErrH2[BZ][iSlice][p], fhBPolynomialErrH1[BZ][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialRelErrH2[BZ][iSlice][p], fhBPolynomialRelErrH1[BZ][iSlice][p]);
			SubtructHistos(fhB[MOD][iSlice], fhBPolynomial[MOD][iSlice][p], fhBPolynomialErrH2[MOD][iSlice][p]);
			DivideHistos(fhBPolynomialErrH2[MOD][iSlice][p], fhB[MOD][iSlice], fhBPolynomialRelErrH2[MOD][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialErrH2[MOD][iSlice][p], fhBPolynomialErrH1[MOD][iSlice][p]);
			ConvertH2ToH1(fhBPolynomialRelErrH2[MOD][iSlice][p], fhBPolynomialRelErrH1[MOD][iSlice][p]);
		}
	}
}

void CbmLitFieldQa::FillGridCreatorHistos()
{
   vector<LitFieldGrid> grids;
   grids.resize(fNofSlices);
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      fGridCreator->CreateGrid(fZpos[iSlice], grids[iSlice]);
   }

   // Fill graph
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Double_t Z = fZpos[iSlice];
      Double_t HX = 2 * fXpos[iSlice] / fNofBinsX; // step size for X position
      Double_t HY = 2 * fYpos[iSlice] / fNofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // loop over x position
         Double_t X = -fXpos[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // loop over y position
            Double_t Y = -fYpos[iSlice] + (iY + 0.5)  * HY;
            LitFieldValue<float> v;
            grids[iSlice].GetFieldValue(X, Y, v);
            Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
            TH1* hbx = fhBGrid[BX][iSlice];
            hbx->SetBinContent(hbx->FindBin(X, Y), v.Bx);
            TH1* hby = fhBGrid[BY][iSlice];
            hby->SetBinContent(hby->FindBin(X, Y), v.By);
            TH1* hbz = fhBGrid[BZ][iSlice];
            hbz->SetBinContent(hbz->FindBin(X, Y), v.Bz);
            TH1* hmod = fhBGrid[MOD][iSlice];
            hmod->SetBinContent(hmod->FindBin(X, Y), mod);
         }
      }
   }

   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
		SubtructHistos(fhB[BX][iSlice], fhBGrid[BX][iSlice], fhBGridErrH2[BX][iSlice]);
		DivideHistos(fhBGridErrH2[BX][iSlice], fhB[BX][iSlice], fhBGridRelErrH2[BX][iSlice]);
		ConvertH2ToH1(fhBGridErrH2[BX][iSlice], fhBGridErrH1[BX][iSlice]);
		ConvertH2ToH1(fhBGridRelErrH2[BX][iSlice], fhBGridRelErrH1[BX][iSlice]);
		SubtructHistos(fhB[BY][iSlice], fhBGrid[BY][iSlice], fhBGridErrH2[BY][iSlice]);
		DivideHistos(fhBGridErrH2[BY][iSlice], fhB[BY][iSlice], fhBGridRelErrH2[BY][iSlice]);
		ConvertH2ToH1(fhBGridErrH2[BY][iSlice], fhBGridErrH1[BY][iSlice]);
		ConvertH2ToH1(fhBGridRelErrH2[BY][iSlice], fhBGridRelErrH1[BY][iSlice]);
		SubtructHistos(fhB[BZ][iSlice], fhBGrid[BZ][iSlice], fhBGridErrH2[BZ][iSlice]);
		DivideHistos(fhBGridErrH2[BZ][iSlice], fhB[BZ][iSlice], fhBGridRelErrH2[BZ][iSlice]);
		ConvertH2ToH1(fhBGridErrH2[BZ][iSlice], fhBGridErrH1[BZ][iSlice]);
		ConvertH2ToH1(fhBGridRelErrH2[BZ][iSlice], fhBGridRelErrH1[BZ][iSlice]);
		SubtructHistos(fhB[MOD][iSlice], fhBGrid[MOD][iSlice], fhBGridErrH2[MOD][iSlice]);
		DivideHistos(fhBGridErrH2[MOD][iSlice], fhB[MOD][iSlice], fhBGridRelErrH2[MOD][iSlice]);
		ConvertH2ToH1(fhBGridErrH2[MOD][iSlice], fhBGridErrH1[MOD][iSlice]);
		ConvertH2ToH1(fhBGridRelErrH2[MOD][iSlice], fhBGridRelErrH1[MOD][iSlice]);
	}
}

void CbmLitFieldQa::CreatePropertyTree()
{
   // Create and serialize property tree
   boost::property_tree::ptree qa;
   qa.put("NofPolynoms", fNofPolynoms);
   qa.put("NofSlices", fNofSlices);
   string vnames[4] = {"BX", "BY", "BZ", "MOD"};

   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      string slice = "slice" + ToString<Int_t>(iSlice);
      qa.put(slice + ".Z", fZpos[iSlice]);
      for (Int_t iPolynom = 0; iPolynom < fNofPolynoms; iPolynom++) {
         for (Int_t v = 0; v < 4; v++) {
            string name = slice + ".polynomial" + ToString<Int_t>(iPolynom);
            qa.put(name + ".degree", fPolynomDegrees[iPolynom]);
            qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBPolynomialErrH1[v][iSlice][iPolynom]->GetMean());
            qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBPolynomialErrH1[v][iSlice][iPolynom]->GetRMS());
            qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBPolynomialRelErrH1[v][iSlice][iPolynom]->GetMean());
            qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBPolynomialRelErrH1[v][iSlice][iPolynom]->GetRMS());
         }
      }
      string name = slice + ".grid";
      for (Int_t v = 0; v < 4; v++) {
         qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBGridErrH1[v][iSlice]->GetMean());
         qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBGridErrH1[v][iSlice]->GetRMS());
         qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBGridRelErrH1[v][iSlice]->GetMean());
         qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBGridRelErrH1[v][iSlice]->GetRMS());
      }
   }

   write_json(string(fOutputDir + "field_qa.json").c_str(), qa);

   // Create report
   CbmLitSimulationReport* report = new CbmLitFieldQaReport();
   ofstream foutHtml(string(fOutputDir + "field_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "field_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "field_qa.txt").c_str());
   report->Create(kLitHtml, foutHtml, fOutputDir);
   report->Create(kLitLatex, foutLatex, fOutputDir);
   report->Create(kLitText, foutText, fOutputDir);
   delete report;
}


void CbmLitFieldQa::DrawSlices(
   Int_t v,
   const string& opt)
{
   string names[] = {"field_qa_slice_Bx_", "field_qa_slice_By_", "field_qa_slice_Bz_", "field_qa_slice_Mod_"};
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      string ss = names[v] + "z_" + ToString<float>(fZpos[s]) + "_" + opt;
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      TH2* graph1 = fhB[v][i];

      DrawH2(graph1, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(2);
      TH1* hist2 = (opt != "grid") ? fhBPolynomialErrH1[v][i][fPolynomDegreeIndex] : fhBGridErrH1[v][i];
      DrawH1(hist2, kLitLinear, kLitLog);

      canvas[i]->cd(3);
      TH2* hist3 = (opt != "grid") ? fhBPolynomialErrH2[v][i][fPolynomDegreeIndex] : fhBGridErrH2[v][i];
      DrawH2(hist3, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(4);
      TH2* graph2 = (opt != "grid") ? fhBPolynomial[v][i][fPolynomDegreeIndex] : fhBGrid[v][i];
      DrawH2(graph2, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(5);
      TH1* hist4 = (opt != "grid") ? fhBPolynomialRelErrH1[v][i][fPolynomDegreeIndex] : fhBGridRelErrH1[v][i];
      DrawH1(hist4, kLitLinear, kLitLog);

      canvas[i]->cd(6);
      TH2* hist5 = (opt != "grid") ? fhBPolynomialRelErrH2[v][i][fPolynomDegreeIndex] : fhBGridRelErrH2[v][i];
      DrawH2(hist5, kLitLinear, kLitLinear, kLitLinear, "colz");

      SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitFieldQa::DrawPoly(
   const string& opt)
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      string canvasName = "field_qa_" + opt + "_degree_z_" + lit::ToString<Double_t>(fZpos[s]);
      canvas[s] = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      TLegend* l1 = new TLegend(0.1,0.1,0.9,0.9);
      l1->SetFillColor(kWhite);
      l1->SetTextSize(0.1);
      l1->SetLineWidth(1);
      l1->SetHeader("Polynom degree");
      for (Int_t v = 0; v < 4; v++) {
         TH1* firsthist = (opt == "rel") ? fhBPolynomialRelErrH1[v][i][0] : fhBPolynomialErrH1[v][i][0];
         Double_t max = firsthist->GetMaximum();
         for (Int_t j = 0; j < fNofPolynoms; j++) {
            canvas[i]->cd(v+2);
            TH1* hist1 = (opt == "rel") ? fhBPolynomialRelErrH1[v][i][j] : fhBPolynomialErrH1[v][i][j];
            if (max < hist1->GetMaximum()) { max = hist1->GetMaximum(); }
            string draw_opt = (j == 0) ? "" : "SAME";
            DrawH1(hist1, kLitLinear, kLitLog, draw_opt.c_str(),
                       1 + j, LitDrawingOptions::LineWidth(), 1 + j, LitDrawingOptions::MarkerSize(), kDot);

            if (v == 0) {
               string poly = lit::ToString<Int_t>(fPolynomDegrees[j]);
               l1->AddEntry(hist1, poly.c_str(),"lp");
            }
         }
         firsthist->SetMaximum(1.2 * max);
      }

      canvas[i]->cd(1);
      l1->Draw();

      SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitFieldQa::DrawFieldSlices()
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      string ss = "field_qa_map_at_z_" + ToString<Double_t>(fZpos[s]);
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 800, 800);
      canvas[s]->Divide(2, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      DrawH2(fhB[BX][i], kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(2);
      TH2* graphBy = fhB[BY][i];
      DrawH2(fhB[BY][i], kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(3);
      TH2* graphBz = fhB[BZ][i];
      DrawH2(fhB[BZ][i], kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(4);
      TH2* graphMod = fhB[MOD][i];
      DrawH2(fhB[MOD][i], kLitLinear, kLitLinear, kLitLinear, "colz");

      SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitFieldQa::DrawFieldAlongZ()
{
   TCanvas* canvas = new TCanvas("field_qa_map_along_z", "field_qa_map_along_z", 1200, 800);
   canvas->Divide(3, 2);

   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      canvas->cd(i+1);
      DrawH1(list_of(fhBAlongZGraph[BX][i])(fhBAlongZGraph[BY][i])(fhBAlongZGraph[BZ][i]),
            list_of("B_{x}")("B_{y}")("B_{z}"), kLitLinear, kLitLinear, true, 0.7, 0.5, 0.9, 0.3);
   }
   SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitFieldQa::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3)
{
   //histo1->Sumw2();
   //histo2->Sumw2();
   histo3->Sumw2();
   histo3->Divide(histo1, histo2, 1., 1., "B");
}

void CbmLitFieldQa::SubtructHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3)
{
	histo3->Add(histo1, 1);
	histo3->Add(histo2, -1);
}

void CbmLitFieldQa::ConvertH2ToH1(
   const TH2* h2,
   TH1* h1)
{
	Int_t nofBinsX = h2->GetNbinsX();
	Int_t nofBinsY = h2->GetNbinsY();
	for (Int_t iBinX = 1; iBinX <= nofBinsX; iBinX++) {
		for (Int_t iBinY = 1; iBinY <= nofBinsY; iBinY++) {
			h1->Fill(h2->GetBinContent(iBinX, iBinY));
		}
	}
}
ClassImp(CbmLitFieldQa);
