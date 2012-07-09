/**
 * \file CbmLitFieldApproximationQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmLitFieldApproximationQa.h"
#include "CbmLitFieldApproximationQaReport.h"
#include "base/CbmLitEnvironment.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitFieldFitter.h"
#include "base/CbmLitFieldGridCreator.h"
#include "utils/CbmLitUtils.h"
#include "cbm/qa/draw/CbmLitDrawHist.h"

#include "../../../parallel/LitField.h"

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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>

using lit::ToString;
using lit::SaveCanvasAsImage;
using lit::parallel::LitFieldSlice;
using lit::parallel::LitFieldValue;
using lit::parallel::LitFieldGrid;
using boost::assign::list_of;

CbmLitFieldApproximationQa::CbmLitFieldApproximationQa():
   fField(NULL),
   fHistoList(NULL),
   fNofSlices(0),
   fZSlicePosition(),
   fXSlicePosition(),
   fYSlicePosition(),
   fhBGraph(),
   fhBAprGraph(),
   fhBGridGraph(),
   fhBPolynomialErrH2(),
   fhBPolynomialErrH1(),
   fhBPolynomialRelErrH1(),
   fhBPolynomialRelErrH2(),
   fhBGridErrH2(),
   fhBGridErrH1(),
   fhBGridRelErrH1(),
   fhBGridRelErrH2(),
   fFixedBounds(true),
   fAcceptanceAngleX(25.),
   fAcceptanceAngleY(25.),
   fNofBinsX(30),
   fNofBinsY(30),
   fUseEllipseAcc(true),
   fOutputDir("./field/"),
   fFitter(),
   fGridCreator(),
   fPolynomDegreeIndex(1),
   fNofPolynoms(4),
   fPolynomDegrees()
{

}

CbmLitFieldApproximationQa::~CbmLitFieldApproximationQa()
{
}

InitStatus CbmLitFieldApproximationQa::Init()
{
   // Set draw styles
   SetStyles();

   fNofSlices = fZSlicePosition.size();
   fNofPolynoms = fPolynomDegrees.size();

   // Calculate (X, Y) window for each slice
   fXSlicePosition.resize(fNofSlices);
   fYSlicePosition.resize(fNofSlices);
   for (Int_t i = 0; i < fNofSlices; i++) {
      Double_t tanXangle = std::tan(fAcceptanceAngleX * 3.14159265 / 180); //
      Double_t tanYangle = std::tan(fAcceptanceAngleY * 3.14159265 / 180); //
      fXSlicePosition[i] = fZSlicePosition[i] * tanXangle;
      fYSlicePosition[i] = fZSlicePosition[i] * tanYangle;
   }

   // Create field approximation tool for each polynom degree
   fFitter.resize(fNofPolynoms);
   for (UInt_t i = 0; i < fNofPolynoms; i++) {
      fFitter[i] = new CbmLitFieldFitter(fPolynomDegrees[i]);
      fFitter[i]->SetXangle(fAcceptanceAngleX);
      fFitter[i]->SetYangle(fAcceptanceAngleY);
      fFitter[i]->SetNofBinsX(100);//fNofBinsX);
      fFitter[i]->SetNofBinsY(100);//fNofBinsY);
      fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
   }

   // Create grid creator tool
   fGridCreator = new CbmLitFieldGridCreator();

   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fField = env->GetField();

   return kSUCCESS;
}

void CbmLitFieldApproximationQa::Exec(
   Option_t* opt)
{

}

void CbmLitFieldApproximationQa::Finish()
{
  // Fill and create graphs and histograms
   CreateHistos();

   // This is always needed since field map is always drawn for comparison
   FillBHistos();

   // Check and draw polynomial field approximation
   FillFieldApproximationHistos();
	DrawSlices(BX, "apr");
	DrawSlices(BY, "apr");
	DrawSlices(BZ, "apr");
	DrawSlices(MOD, "apr");
   DrawPoly("rel");
	DrawPoly("abs");

   // Check and draw histograms for grid creator tool
	FillGridCreatorHistos();
	DrawSlices(BX, "grid");
	DrawSlices(BY, "grid");
	DrawSlices(BZ, "grid");
	DrawSlices(MOD, "grid");

   CreatePropertyTree();

   TIter next(fHistoList);
   while ( TObject* histo = ((TObject*)next()) ) { histo->Write(); }
}

void CbmLitFieldApproximationQa::CreateHistos()
{
   fHistoList = new TList();

   CreateFieldHistos();
   CreateFitterHistos();
   CreateGridHistos();
}

void CbmLitFieldApproximationQa::CreateFieldHistos()
{
   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};

   // [BX, BY, BZ, MOD] components
   fhBGraph.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBGraph[i].resize(fNofSlices);
   }
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         fhBGraph[v][i] = new TGraph2D();
         string graphName = "fhBGraph";
         string graphTitle = string("fhBGraph;X [cm]; Y [cm];") + zTitle[v];
         fhBGraph[v][i]->SetNameTitle(graphName.c_str(), graphTitle.c_str());
         fHistoList->Add(fhBGraph[v][i]);
      }
   }
}

void CbmLitFieldApproximationQa::CreateFitterHistos()
{
   // Resize histogram vectors
   fhBAprGraph.resize(4);
   fhBPolynomialErrH1.resize(4);
   fhBPolynomialErrH2.resize(4);
   fhBPolynomialRelErrH1.resize(4);
   fhBPolynomialRelErrH2.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBAprGraph[i].resize(fNofSlices);
      fhBPolynomialErrH1[i].resize(fNofSlices);
      fhBPolynomialErrH2[i].resize(fNofSlices);
      fhBPolynomialRelErrH1[i].resize(fNofSlices);
      fhBPolynomialRelErrH2[i].resize(fNofSlices);
      for (Int_t j = 0; j < fNofSlices; j++) {
         fhBAprGraph[i][j].resize(fNofPolynoms);
         fhBPolynomialErrH1[i][j].resize(fNofPolynoms);
         fhBPolynomialErrH2[i][j].resize(fNofPolynoms);
         fhBPolynomialRelErrH1[i][j].resize(fNofPolynoms);
         fhBPolynomialRelErrH2[i][j].resize(fNofPolynoms);
      }
   }

   std::string names[] = {"Bx", "By", "Bz", "Mod"};

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
            fhBAprGraph[v][i][j] = new TGraph2D();
            string graphName = "fhBAprGraph";
            string graphTitle = string("fhBAprGraph;X [cm]; Y [cm];") + zTitle[v];
            fhBAprGraph[v][i][j]->SetNameTitle(graphName.c_str(), graphTitle.c_str());
            fHistoList->Add(fhBAprGraph[v][i][j]);

            std::string histName = "hBErrH1" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialErrH1[v][i][j] = new TH1D(histName.c_str(), string(histName + ";" + errTitle[v] + ";Counter").c_str(), nofBinsErrB, minErrB, maxErrB);
            fHistoList->Add(fhBPolynomialErrH1[v][i][j]);

            histName = "hBErrH2" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialErrH2[v][i][j] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + errTitle[v]).c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]);
            fHistoList->Add(fhBPolynomialErrH2[v][i][j]);

            histName = "hBRelErrH1" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialRelErrH1[v][i][j] = new TH1D(histName.c_str(), string(histName + ";" + relErrTitle[v] + ";Counter").c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
            fHistoList->Add(fhBPolynomialRelErrH1[v][i][j]);

            histName = "hBRelErrH2" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBPolynomialRelErrH2[v][i][j] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + relErrTitle[v]).c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]);
            fHistoList->Add(fhBPolynomialRelErrH2[v][i][j]);
         }
      }
   }
   std::cout << "-I- CbmLitFieldApproximationQa::CreateFitterErrHistos: Field fitter error histograms created" << std::endl;
}

void CbmLitFieldApproximationQa::CreateGridHistos()
{
   // Resize histogram vectors
   fhBGridGraph.resize(4);
   fhBGridErrH1.resize(4);
   fhBGridErrH2.resize(4);
   fhBGridRelErrH1.resize(4);
   fhBGridRelErrH2.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBGridGraph[i].resize(fNofSlices);
      fhBGridErrH1[i].resize(fNofSlices);
      fhBGridErrH2[i].resize(fNofSlices);
      fhBGridRelErrH1[i].resize(fNofSlices);
      fhBGridRelErrH2[i].resize(fNofSlices);
   }

   std::string names[] = {"Bx", "By", "Bz", "Mod"};

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
         fhBGridGraph[v][i] = new TGraph2D();
         string graphName = "fhBGridGraph";
         string graphTitle = string("fhBGridGraph;X [cm]; Y [cm];") + zTitle[v];
         fhBGridGraph[v][i]->SetNameTitle(graphName.c_str(), graphTitle.c_str());
         fHistoList->Add(fhBGridGraph[v][i]);

         std::string histName = "hGridBErrH1" + names[v] + ToString<Int_t>(i);
         fhBGridErrH1[v][i] = new TH1D(histName.c_str(), string(histName + ";" + errTitle[v] + ";Counter").c_str(), nofBinsErrB, minErrB, maxErrB);
         fHistoList->Add(fhBGridErrH1[v][i]);

         histName = "hGridBErrH2" + names[v] + ToString<Int_t>(i);
         fhBGridErrH2[v][i] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + errTitle[v]).c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]);
         fHistoList->Add(fhBGridErrH2[v][i]);

         histName = "hGridBRelErrH1" + names[v] + ToString<Int_t>(i);
         fhBGridRelErrH1[v][i] = new TH1D(histName.c_str(), string(histName + ";" + relErrTitle[v] + ";Counter").c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
         fHistoList->Add(fhBGridRelErrH1[v][i]);

         histName = "hGridBRelErrH2" + names[v] + ToString<Int_t>(i);
         fhBGridRelErrH2[v][i] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + relErrTitle[v]).c_str(), nofBinsErrX, -fXSlicePosition[i], fXSlicePosition[i], nofBinsErrY, -fYSlicePosition[i], fYSlicePosition[i]);
         fHistoList->Add(fhBGridRelErrH2[v][i]);
      }
   }
   std::cout << "-I- CbmLitFieldApproximationQa::CreateGridErrHistos(): Grid creator error histograms created" << std::endl;
}

void CbmLitFieldApproximationQa::FillBHistos()
{
   // Fill graphs for magnetic field for each (X, Y) slice
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // loop over slices
      Double_t Z = fZSlicePosition[iSlice];

      Int_t cnt = 0;
      Double_t HX = 2 * fXSlicePosition[iSlice] / fNofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / fNofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY + 0.5)  * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXSlicePosition[iSlice]*fXSlicePosition[iSlice]) + (Y*Y)/(fYSlicePosition[iSlice]*fYSlicePosition[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            fhBGraph[BX][iSlice]->SetPoint(cnt, X, Y, B[BX]);
            fhBGraph[BY][iSlice]->SetPoint(cnt, X, Y, B[BY]);
            fhBGraph[BZ][iSlice]->SetPoint(cnt, X, Y, B[BZ]);
            fhBGraph[MOD][iSlice]->SetPoint(cnt, X, Y, Bmod);
            cnt++;
         }
      }
   }
}

void CbmLitFieldApproximationQa::FillFieldApproximationHistos()
{
   std::vector<std::vector<LitFieldSlice<float> > > slices;
   slices.resize(fNofPolynoms);
   for (UInt_t i = 0; i < fNofPolynoms; i++) {slices[i].resize(fNofSlices);}

   // Approximate field in each slice for each polynomial degree
   for (UInt_t i = 0; i < fNofPolynoms; i++) {
      for(Int_t j = 0; j < fNofSlices; j++) {
         fFitter[i]->FitSlice(fZSlicePosition[j], slices[i][j]);
         std::cout << "-I- CbmLitFieldApproximationQa::FillFieldApproximationHistos: "
               << " field approximation (degree=" << fPolynomDegrees[i]
               << ", Z=" << fZSlicePosition[j] << ")" << std::endl;
      }
   }

   // Fill graph for approximated field map
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // Loop over slices
      Double_t Z = fZSlicePosition[iSlice];
      Int_t cnt = 0;

      Double_t HX = 2 * fXSlicePosition[iSlice] / fNofBinsX; // Step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / fNofBinsY; // Step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // Loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // Loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY + 0.5)  * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXSlicePosition[iSlice]*fXSlicePosition[iSlice]) + (Y*Y)/(fYSlicePosition[iSlice]*fYSlicePosition[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            for (Int_t p = 0; p < fNofPolynoms; p++) {
               LitFieldValue<float> v;
               slices[p][iSlice].GetFieldValue(X, Y, v);
               Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
               fhBAprGraph[BX][iSlice][p]->SetPoint(cnt, X, Y, v.Bx);
               fhBAprGraph[BY][iSlice][p]->SetPoint(cnt, X, Y, v.By);
               fhBAprGraph[BZ][iSlice][p]->SetPoint(cnt, X, Y, v.Bz);
               fhBAprGraph[MOD][iSlice][p]->SetPoint(cnt, X, Y, mod);
            }
            cnt++;
         } // End loop over y position
      } // End loop over x position
   } // End loop over slices

   // Fill error histograms
   Int_t nofBinsX = 100;
   Int_t nofBinsY = 100;
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Double_t Z = fZSlicePosition[iSlice];
      Double_t HX = 2 * fXSlicePosition[iSlice] / nofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / nofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < nofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX+0.5) * HX;
         for (Int_t iY = 0; iY < nofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY+0.5) * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXSlicePosition[iSlice]*fXSlicePosition[iSlice]) + (Y*Y)/(fYSlicePosition[iSlice]*fYSlicePosition[iSlice]);
            if (fUseEllipseAcc && el > 1.) { continue; }

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            for (Int_t p = 0; p < fNofPolynoms; p++) {
               LitFieldValue<float> v;
               slices[p][iSlice].GetFieldValue(X, Y, v);
               Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);

               Double_t errBx = B[0] - v.Bx;
               Double_t errBy = B[1] - v.By;
               Double_t errBz = B[2] - v.Bz;
               Double_t errMod = Bmod - mod;
               Double_t relErrBx = (B[0] != 0.) ? (errBx / B[0]) * 100. : 0.;
               Double_t relErrBy = (B[1] != 0.) ? (errBy / B[1]) * 100. : 0.;
               Double_t relErrBz = (B[2] != 0.) ? (errBz / B[2]) * 100. : 0.;
               Double_t relErrMod = (Bmod != 0.) ? (errMod / Bmod) * 100. : 0;

               fhBPolynomialErrH2[BX][iSlice][p]->Fill(X, Y, errBx);
               fhBPolynomialErrH1[BX][iSlice][p]->Fill(errBx);
               fhBPolynomialRelErrH1[BX][iSlice][p]->Fill(relErrBx);
               fhBPolynomialRelErrH2[BX][iSlice][p]->Fill(X, Y, relErrBx);
               fhBPolynomialErrH2[BY][iSlice][p]->Fill(X, Y, errBy);
               fhBPolynomialErrH1[BY][iSlice][p]->Fill(errBy);
               fhBPolynomialRelErrH1[BY][iSlice][p]->Fill(relErrBy);
               fhBPolynomialRelErrH2[BY][iSlice][p]->Fill(X, Y, relErrBy);
               fhBPolynomialErrH2[BZ][iSlice][p]->Fill(X, Y, errBz);
               fhBPolynomialErrH1[BZ][iSlice][p]->Fill(errBz);
               fhBPolynomialRelErrH1[BZ][iSlice][p]->Fill(relErrBz);
               fhBPolynomialRelErrH2[BZ][iSlice][p]->Fill(X, Y, relErrBz);
               fhBPolynomialErrH2[MOD][iSlice][p]->Fill(X, Y, errMod);
               fhBPolynomialErrH1[MOD][iSlice][p]->Fill(errMod);
               fhBPolynomialRelErrH1[MOD][iSlice][p]->Fill(relErrMod);
               fhBPolynomialRelErrH2[MOD][iSlice][p]->Fill(X, Y, relErrMod);
            }
         }
      }
   }
}

void CbmLitFieldApproximationQa::FillGridCreatorHistos()
{
   std::vector<LitFieldGrid> grids;
   grids.resize(fNofSlices);
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      fGridCreator->CreateGrid(fZSlicePosition[iSlice], grids[iSlice]);
   }

   // Fill graph
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Int_t cnt = 0;
      Double_t Z = fZSlicePosition[iSlice];
      Double_t HX = 2 * fXSlicePosition[iSlice] / fNofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / fNofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < fNofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX + 0.5) * HX;
         for (Int_t iY = 0; iY < fNofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY + 0.5)  * HY;
            LitFieldValue<float> v;
            grids[iSlice].GetFieldValue(X, Y, v);
            Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
            fhBGridGraph[BX][iSlice]->SetPoint(cnt, X, Y, v.Bx);
            fhBGridGraph[BY][iSlice]->SetPoint(cnt, X, Y, v.By);
            fhBGridGraph[BZ][iSlice]->SetPoint(cnt, X, Y, v.Bz);
            fhBGridGraph[MOD][iSlice]->SetPoint(cnt, X, Y, mod);
            cnt++;
         }
      }
   }

   // Fill error histograms
   Int_t nofBinsX = 100;
   Int_t nofBinsY = 100;
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Double_t Z = fZSlicePosition[iSlice];
      Double_t HX = 2 * fXSlicePosition[iSlice] / nofBinsX; // step size for X position
      Double_t HY = 2 * fYSlicePosition[iSlice] / nofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < nofBinsX; iX++) { // loop over x position
         Double_t X = -fXSlicePosition[iSlice] + (iX+0.5) * HX;
         for (Int_t iY = 0; iY < nofBinsY; iY++) { // loop over y position
            Double_t Y = -fYSlicePosition[iSlice] + (iY+0.5) * HY;

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            LitFieldValue<float> v;
            grids[iSlice].GetFieldValue(X, Y, v);
            Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);

            Double_t errBx = B[0] - v.Bx;
            Double_t errBy = B[1] - v.By;
            Double_t errBz = B[2] - v.Bz;
            Double_t errMod = Bmod - mod;
            Double_t relErrBx = (B[0] != 0.) ? (errBx / B[0]) * 100. : 0.;
            Double_t relErrBy = (B[1] != 0.) ? (errBy / B[1]) * 100. : 0.;
            Double_t relErrBz = (B[2] != 0.) ? (errBz / B[2]) * 100. : 0.;
            Double_t relErrMod = (Bmod != 0.) ? (errMod / Bmod) * 100. : 0;

            fhBGridErrH2[BX][iSlice]->Fill(X, Y, errBx);
            fhBGridErrH1[BX][iSlice]->Fill(errBx);
            fhBGridRelErrH1[BX][iSlice]->Fill(relErrBx);
            fhBGridRelErrH2[BX][iSlice]->Fill(X, Y, relErrBx);
            fhBGridErrH2[BY][iSlice]->Fill(X, Y, errBy);
            fhBGridErrH1[BY][iSlice]->Fill(errBy);
            fhBGridRelErrH1[BY][iSlice]->Fill(relErrBy);
            fhBGridRelErrH2[BY][iSlice]->Fill(X, Y, relErrBy);
            fhBGridErrH2[BZ][iSlice]->Fill(X, Y, errBz);
            fhBGridErrH1[BZ][iSlice]->Fill(errBz);
            fhBGridRelErrH1[BZ][iSlice]->Fill(relErrBz);
            fhBGridRelErrH2[BZ][iSlice]->Fill(X, Y, relErrBz);
            fhBGridErrH2[MOD][iSlice]->Fill(X, Y, errMod);
            fhBGridErrH1[MOD][iSlice]->Fill(errMod);
            fhBGridRelErrH1[MOD][iSlice]->Fill(relErrMod);
            fhBGridRelErrH2[MOD][iSlice]->Fill(X, Y, relErrMod);

         }
      }
   }
}

void CbmLitFieldApproximationQa::CreatePropertyTree()
{
   // Create and serialize property tree
   boost::property_tree::ptree qa;
   qa.put("NofPolynoms", fNofPolynoms);
   qa.put("NofSlices", fNofSlices);
   std::string vnames[4] = {"BX", "BY", "BZ", "MOD"};

   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      std::string slice = "slice" + ToString<Int_t>(iSlice);
      qa.put(slice + ".Z", fZSlicePosition[iSlice]);
      for (Int_t iPolynom = 0; iPolynom < fNofPolynoms; iPolynom++) {
         for (Int_t v = 0; v < 4; v++) {
            std::string name = slice + ".polynomial" + ToString<Int_t>(iPolynom);
            qa.put(name + ".degree", fPolynomDegrees[iPolynom]);
            qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBPolynomialErrH1[v][iSlice][iPolynom]->GetMean());
            qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBPolynomialErrH1[v][iSlice][iPolynom]->GetRMS());
            qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBPolynomialRelErrH1[v][iSlice][iPolynom]->GetMean());
            qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBPolynomialRelErrH1[v][iSlice][iPolynom]->GetRMS());
         }
      }
      std::string name = slice + ".grid";
      for (Int_t v = 0; v < 4; v++) {
         qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBGridErrH1[v][iSlice]->GetMean());
         qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBGridErrH1[v][iSlice]->GetRMS());
         qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBGridRelErrH1[v][iSlice]->GetMean());
         qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBGridRelErrH1[v][iSlice]->GetRMS());
      }
   }

   write_json(std::string(fOutputDir + "fieldapr_qa.json").c_str(), qa);

   // Create report
   CbmSimulationReport* report = new CbmLitFieldApproximationQaReport();
   ofstream foutHtml(string(fOutputDir + "fieldapr_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "fieldapr_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "fieldapr_qa.txt").c_str());
   report->Create(kHtmlReport, foutHtml, fOutputDir);
   report->Create(kLatexReport, foutLatex, fOutputDir);
   report->Create(kTextReport, foutText, fOutputDir);
   delete report;
}


void CbmLitFieldApproximationQa::DrawSlices(
   Int_t v,
   const std::string& opt)
{
   std::string names[] = {"fieldapr_qa_slice_Bx_", "fieldapr_qa_slice_By_", "fieldapr_qa_slice_Bz_", "fieldapr_qa_slice_Mod_"};
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::string ss = names[v] + "z_" + ToString<float>(fZSlicePosition[s]) + "_" + opt;
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      TGraph2D* graph1 = fhBGraph[v][i];

      DrawGraph2D(graph1, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(2);
      TH1* hist2 = (opt != "grid") ? fhBPolynomialErrH1[v][i][fPolynomDegreeIndex] : fhBGridErrH1[v][i];
      DrawH1(hist2, kLitLinear, kLitLog);

      canvas[i]->cd(3);
      TH2* hist3 = (opt != "grid") ? fhBPolynomialErrH2[v][i][fPolynomDegreeIndex] : fhBGridErrH2[v][i];
      DrawH2(hist3, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(4);
      TGraph2D* graph2 = (opt != "grid") ? fhBAprGraph[v][i][fPolynomDegreeIndex] : fhBGridGraph[v][i];
      DrawGraph2D(graph2, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(5);
      TH1* hist4 = (opt != "grid") ? fhBPolynomialRelErrH1[v][i][fPolynomDegreeIndex] : fhBGridRelErrH1[v][i];
      DrawH1(hist4, kLitLinear, kLitLog);

      canvas[i]->cd(6);
      TH2* hist5 = (opt != "grid") ? fhBPolynomialRelErrH2[v][i][fPolynomDegreeIndex] : fhBGridRelErrH2[v][i];
      DrawH2(hist5, kLitLinear, kLitLinear, kLitLinear, "colz");

      SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitFieldApproximationQa::DrawPoly(
   const std::string& opt)
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::stringstream ss;
      ss << "fieldapr_qa_" + opt + "_degree_z_" << fZSlicePosition[s];
      canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      TLegend* l1 = new TLegend(0.1,0.1,0.9,0.9);
      l1->SetFillColor(kWhite);
      l1->SetTextSize(0.1);
      l1->SetLineWidth(1);
      l1->SetHeader("Polynomial degree");
      for (Int_t v = 0; v < 4; v++) {
         TH1* firsthist = (opt == "rel") ? fhBPolynomialRelErrH1[v][i][0] : fhBPolynomialErrH1[v][i][0];
         Double_t max = firsthist->GetMaximum();
         for (Int_t j = 0; j < fNofPolynoms; j++) {
            canvas[i]->cd(v+2);
            TH1* hist1 = (opt == "rel") ? fhBPolynomialRelErrH1[v][i][j] : fhBPolynomialErrH1[v][i][j];
            if (max < hist1->GetMaximum()) { max = hist1->GetMaximum(); }
            string draw_opt = (j == 0) ? "" : "SAME";
            DrawH1(hist1, kLitLinear, kLitLog, draw_opt.c_str(), 1 + j, LitDrawingOptions::LineWidth(), 1 + j, LitDrawingOptions::MarkerSize(), kDot);

            if (v == 0) {
               string str = lit::ToString<Int_t>(fPolynomDegrees[j]);
               l1->AddEntry(hist1, str.c_str(), "lp");
            }
         }
         firsthist->SetMaximum(1.2 * max);
      }

      canvas[i]->cd(1);
      l1->Draw();

      SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

ClassImp(CbmLitFieldApproximationQa);
