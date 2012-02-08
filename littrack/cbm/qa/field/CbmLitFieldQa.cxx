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
#include "utils/CbmLitDrawHist.h"
#include "../field/CbmLitFieldQaReport.h"

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

CbmLitFieldQa::CbmLitFieldQa():
   fField(NULL),
   fHistoList(NULL),
   fNofSlices(0),
   fZpos(),
   fXpos(),
   fYpos(),
   fhBGraph(),
   fhBAprGraph(),
   fhBErrH2D(),
   fhBErrH1D(),
   fhBRelErrH1D(),
   fhBRelErrH2D(),
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
   fPolynomDegrees(),
   fImageList()
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
         fFitter[i]->SetNofBinsX(100);//fNofBinsX);
         fFitter[i]->SetNofBinsY(100);//fNofBinsY);
         fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
      }
   }

   // Create grid creator tool
   if (fCheckGridCreator) {
      fGridCreator = new CbmLitFieldGridCreator();
   }

   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fField = env->GetField();

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
}

void CbmLitFieldQa::Finish()
{
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
   fhBGraph.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBGraph[i].resize(fNofSlices);
   }
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         fhBGraph[v][i] = new TGraph2D();
         fhBGraph[v][i]->GetXaxis()->SetTitle("X [cm]");
         fhBGraph[v][i]->GetYaxis()->SetTitle("Y [cm]");
         fhBGraph[v][i]->GetZaxis()->SetTitle(zTitle[v].c_str());
         fHistoList->Add(fhBGraph[v][i]);
      }
   }

   // [BX, BY, BZ]
   fhBAlongZGraph.resize(3);
   for (Int_t i = 0; i < 3; i++) {
      fhBAlongZGraph[i].resize(fAlongZAngles.size());
   }
   for (Int_t v = 0; v < 3; v++) {
      for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
         fhBAlongZGraph[v][i] = new TGraph();
         fhBGraph[v][i]->GetXaxis()->SetTitle("Z [cm]");
         fhBGraph[v][i]->GetYaxis()->SetTitle(zTitle[v].c_str());
      }
   }
}

void CbmLitFieldQa::CreateFitterHistos()
{
   // Resize histogram vectors
   fhBAprGraph.resize(4);
   fhBErrH1D.resize(4);
   fhBErrH2D.resize(4);
   fhBRelErrH1D.resize(4);
   fhBRelErrH2D.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBAprGraph[i].resize(fNofSlices);
      fhBErrH1D[i].resize(fNofSlices);
      fhBErrH2D[i].resize(fNofSlices);
      fhBRelErrH1D[i].resize(fNofSlices);
      fhBRelErrH2D[i].resize(fNofSlices);
      for (Int_t j = 0; j < fNofSlices; j++) {
         fhBAprGraph[i][j].resize(fNofPolynoms);
         fhBErrH1D[i][j].resize(fNofPolynoms);
         fhBErrH2D[i][j].resize(fNofPolynoms);
         fhBRelErrH1D[i][j].resize(fNofPolynoms);
         fhBRelErrH2D[i][j].resize(fNofPolynoms);
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
            fhBAprGraph[v][i][j]->GetXaxis()->SetTitle("X [cm]");
            fhBAprGraph[v][i][j]->GetYaxis()->SetTitle("Y [cm]");
            fhBAprGraph[v][i][j]->GetZaxis()->SetTitle(zTitle[v].c_str());
            fHistoList->Add(fhBAprGraph[v][i][j]);

            std::string histName = "hBErrH1D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBErrH1D[v][i][j] = new TH1D(histName.c_str(), string(histName + ";" + errTitle[v] + ";Counter").c_str(), nofBinsErrB, minErrB, maxErrB);
            fHistoList->Add(fhBErrH1D[v][i][j]);

            histName = "hBErrH2D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBErrH2D[v][i][j] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + errTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBErrH2D[v][i][j]);

            histName = "hBRelErrH1D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBRelErrH1D[v][i][j] = new TH1D(histName.c_str(), string(histName + ";" + relErrTitle[v] + ";Counter").c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
            fHistoList->Add(fhBRelErrH1D[v][i][j]);

            histName = "hBRelErrH2D" + names[v] + ToString<Int_t>(i) + "_" + ToString<Int_t>(j);
            fhBRelErrH2D[v][i][j] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + relErrTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBRelErrH2D[v][i][j]);
         }
      }
   }
   std::cout << "-I- CbmLitFieldQa::CreateFitterErrHistos: Field fitter error histograms created" << std::endl;
}

void CbmLitFieldQa::CreateGridHistos()
{
   // Resize histogram vectors
   fhBGridGraph.resize(4);
   fhBGridErrH1D.resize(4);
   fhBGridErrH2D.resize(4);
   fhBGridRelErrH1D.resize(4);
   fhBGridRelErrH2D.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBGridGraph[i].resize(fNofSlices);
      fhBGridErrH1D[i].resize(fNofSlices);
      fhBGridErrH2D[i].resize(fNofSlices);
      fhBGridRelErrH1D[i].resize(fNofSlices);
      fhBGridRelErrH2D[i].resize(fNofSlices);
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
         fhBGridGraph[v][i]->GetXaxis()->SetTitle("X [cm]");
         fhBGridGraph[v][i]->GetYaxis()->SetTitle("Y [cm]");
         fhBGridGraph[v][i]->GetZaxis()->SetTitle(zTitle[v].c_str());
         fHistoList->Add(fhBGridGraph[v][i]);

         std::string histName = "hGridBErrH1D" + names[v] + ToString<Int_t>(i);
         fhBGridErrH1D[v][i] = new TH1D(histName.c_str(), string(histName + ";" + errTitle[v] + ";Counter").c_str(), nofBinsErrB, minErrB, maxErrB);
         fHistoList->Add(fhBGridErrH1D[v][i]);

         histName = "hGridBErrH2D" + names[v] + ToString<Int_t>(i);
         fhBGridErrH2D[v][i] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + errTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhBGridErrH2D[v][i]);

         histName = "hGridBRelErrH1D" + names[v] + ToString<Int_t>(i);
         fhBGridRelErrH1D[v][i] = new TH1D(histName.c_str(), string(histName + ";" + relErrTitle[v] + ";Counter").c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
         fHistoList->Add(fhBGridRelErrH1D[v][i]);

         histName = "hGridBRelErrH2D" + names[v] + ToString<Int_t>(i);
         fhBGridRelErrH2D[v][i] = new TH2D(histName.c_str(), string(histName + ";X [cm];Y [cm];" + relErrTitle[v]).c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhBGridRelErrH2D[v][i]);
      }
   }
   std::cout << "-I- CbmLitFieldQa::CreateGridErrHistos(): Grid creator error histograms created" << std::endl;
}

void CbmLitFieldQa::FillBHistos()
{
   // Fill graphs for magnetic field for each (X, Y) slice
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // loop over slices
//      Double_t max[4] = {
//         std::numeric_limits<Double_t>::min(),
//         std::numeric_limits<Double_t>::min(),
//         std::numeric_limits<Double_t>::min(),
//         std::numeric_limits<Double_t>::min()
//      };
//      Double_t min[4] = {
//         std::numeric_limits<Double_t>::max(),
//         std::numeric_limits<Double_t>::max(),
//         std::numeric_limits<Double_t>::max(),
//         std::numeric_limits<Double_t>::max()
//      };

      Double_t Z = fZpos[iSlice];

      Int_t cnt = 0;

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

//            // Find maximum entry in the histogram
//            if (max[BX] < B[0]) { max[BX] = B[0]; }
//            if (max[BY] < B[1]) { max[BY] = B[1]; }
//            if (max[BZ] < B[2]) { max[BZ] = B[2]; }
//            if (max[MOD] < Bmod) { max[MOD] = Bmod; }
//            // Find minimum entry in the histogram
//            if (min[BX] > B[0]) { min[BX] = B[0]; }
//            if (min[BY] > B[1]) { min[BY] = B[1]; }
//            if (min[BZ] > B[2]) { min[BZ] = B[2]; }
//            if (min[MOD] > Bmod) { min[MOD] = Bmod; }

            fhBGraph[BX][iSlice]->SetPoint(cnt, X, Y, B[BX]);
            fhBGraph[BY][iSlice]->SetPoint(cnt, X, Y, B[BY]);
            fhBGraph[BZ][iSlice]->SetPoint(cnt, X, Y, B[BZ]);
            fhBGraph[MOD][iSlice]->SetPoint(cnt, X, Y, Bmod);
            cnt++;
         }
      }
//      fhBGraph[BX][iSlice]->SetMinimum(min[BX] - 0.1 * std::abs(min[BX]));
//      fhBGraph[BY][iSlice]->SetMinimum(min[BY] - 0.1 * std::abs(min[BY]));
//      fhBGraph[BZ][iSlice]->SetMinimum(min[BZ] - 0.1 * std::abs(min[BZ]));
//      fhBGraph[MOD][iSlice]->SetMinimum(min[MOD] - 0.1 * std::abs(min[MOD]));
//
//      fhBGraph[BX][iSlice]->SetMaximum(max[BX] + 0.1 * std::abs(max[BX]));
//      fhBGraph[BY][iSlice]->SetMaximum(max[BY] + 0.1 * std::abs(max[BY]));
//      fhBGraph[BZ][iSlice]->SetMaximum(max[BZ] + 0.1 * std::abs(max[BZ]));
//      fhBGraph[MOD][iSlice]->SetMaximum(max[MOD] + 0.1 * std::abs(max[MOD]));
   }

   // Fill histograms for magnetic field along Z
   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      Int_t nofSteps = Int_t((fZMax - fZMin) / fZStep);
      for (Int_t istep = 0; istep < nofSteps; istep++) {
         Double_t Z = fZMin + istep * fZStep;
         Double_t tanXangle = std::tan(fAlongZAngles[i]*3.14159265/180); //
         Double_t tanYangle = std::tan(fAlongZAngles[i]*3.14159265/180); //
         Double_t X = Z * tanXangle;
         Double_t Y = Z * tanYangle;

         // Get field value
         Double_t pos[3] = {X, Y, Z};
         Double_t B[3];
         fField->GetFieldValue(pos, B);

         fhBAlongZGraph[BX][i]->SetPoint(istep, Z, B[0]);
         fhBAlongZGraph[BY][i]->SetPoint(istep, Z, B[1]);
         fhBAlongZGraph[BZ][i]->SetPoint(istep, Z, B[2]);
      }
   }
}

void CbmLitFieldQa::FillFieldApproximationHistos()
{
   std::vector<std::vector<LitFieldSlice<float> > > slices;
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
      Int_t cnt = 0;

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
      Double_t Z = fZpos[iSlice];
      Double_t HX = 2 * fXpos[iSlice] / nofBinsX; // step size for X position
      Double_t HY = 2 * fYpos[iSlice] / nofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < nofBinsX; iX++) { // loop over x position
         Double_t X = -fXpos[iSlice] + (iX+0.5) * HX;
         for (Int_t iY = 0; iY < nofBinsY; iY++) { // loop over y position
            Double_t Y = -fYpos[iSlice] + (iY+0.5) * HY;

            // Check acceptance for ellipse
            Double_t el = (X*X)/(fXpos[iSlice]*fXpos[iSlice]) + (Y*Y)/(fYpos[iSlice]*fYpos[iSlice]);
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
               Double_t relErrBx = 0., relErrBy = 0., relErrBz = 0., relErrMod = 0.;
               if (B[0] != 0.) { relErrBx = (errBx / B[0]) * 100.; }
               else { relErrBx = 0.; }
               if (B[1] != 0.) { relErrBy = (errBy / B[1]) * 100.; }
               else { relErrBy = 0.; }
               if (B[2] != 0.) { relErrBz = (errBz / B[2]) * 100.; }
               else { relErrBz = 0.; }
               if (Bmod != 0.) { relErrMod = (errMod / Bmod) * 100.; }
               else { relErrMod = 0.; }

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

void CbmLitFieldQa::FillGridCreatorHistos()
{
   std::vector<LitFieldGrid> grids;
   grids.resize(fNofSlices);
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      fGridCreator->CreateGrid(fZpos[iSlice], grids[iSlice]);
   }

   // Fill graph
   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      Int_t cnt = 0;
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
      Double_t Z = fZpos[iSlice];
      Double_t HX = 2 * fXpos[iSlice] / nofBinsX; // step size for X position
      Double_t HY = 2 * fYpos[iSlice] / nofBinsY; // step size for Y position
      for (Int_t iX = 0; iX < nofBinsX; iX++) { // loop over x position
         Double_t X = -fXpos[iSlice] + (iX+0.5) * HX;
         for (Int_t iY = 0; iY < nofBinsY; iY++) { // loop over y position
            Double_t Y = -fYpos[iSlice] + (iY+0.5) * HY;

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
            Double_t relErrBx = 0., relErrBy = 0., relErrBz = 0., relErrMod = 0.;
            if (B[0] != 0.) { relErrBx = (errBx / B[0]) * 100.; }
            else { relErrBx = 0.; }
            if (B[1] != 0.) { relErrBy = (errBy / B[1]) * 100.; }
            else { relErrBy = 0.; }
            if (B[2] != 0.) { relErrBz = (errBz / B[2]) * 100.; }
            else { relErrBz = 0.; }
            if (Bmod != 0.) { relErrMod = (errMod / Bmod) * 100.; }
            else { relErrMod = 0.; }

            fhBGridErrH2D[BX][iSlice]->Fill(X, Y, errBx);
            fhBGridErrH1D[BX][iSlice]->Fill(errBx);
            fhBGridRelErrH1D[BX][iSlice]->Fill(relErrBx);
            fhBGridRelErrH2D[BX][iSlice]->Fill(X, Y, relErrBx);
            fhBGridErrH2D[BY][iSlice]->Fill(X, Y, errBy);
            fhBGridErrH1D[BY][iSlice]->Fill(errBy);
            fhBGridRelErrH1D[BY][iSlice]->Fill(relErrBy);
            fhBGridRelErrH2D[BY][iSlice]->Fill(X, Y, relErrBy);
            fhBGridErrH2D[BZ][iSlice]->Fill(X, Y, errBz);
            fhBGridErrH1D[BZ][iSlice]->Fill(errBz);
            fhBGridRelErrH1D[BZ][iSlice]->Fill(relErrBz);
            fhBGridRelErrH2D[BZ][iSlice]->Fill(X, Y, relErrBz);
            fhBGridErrH2D[MOD][iSlice]->Fill(X, Y, errMod);
            fhBGridErrH1D[MOD][iSlice]->Fill(errMod);
            fhBGridRelErrH1D[MOD][iSlice]->Fill(relErrMod);
            fhBGridRelErrH2D[MOD][iSlice]->Fill(X, Y, relErrMod);

         }
      }
   }
}

void CbmLitFieldQa::CreatePropertyTree()
{
   // Create and serialize property tree
   boost::property_tree::ptree qa;
   qa.put("NofPolynoms", fNofPolynoms);
   qa.put("NofSlices", fNofSlices);
   std::string vnames[4] = {"BX", "BY", "BZ", "MOD"};

   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
      std::string slice = "slice" + ToString<Int_t>(iSlice);
      qa.put(slice + ".Z", fZpos[iSlice]);
      for (Int_t iPolynom = 0; iPolynom < fNofPolynoms; iPolynom++) {
         for (Int_t v = 0; v < 4; v++) {
            std::string name = slice + ".polynomial" + ToString<Int_t>(iPolynom);
            qa.put(name + ".degree", fPolynomDegrees[iPolynom]);
            qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBErrH1D[v][iSlice][iPolynom]->GetMean());
            qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBErrH1D[v][iSlice][iPolynom]->GetRMS());
            qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBRelErrH1D[v][iSlice][iPolynom]->GetMean());
            qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBRelErrH1D[v][iSlice][iPolynom]->GetRMS());
         }
      }
      std::string name = slice + ".grid";
      for (Int_t v = 0; v < 4; v++) {
         qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBGridErrH1D[v][iSlice]->GetMean());
         qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBGridErrH1D[v][iSlice]->GetRMS());
         qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBGridRelErrH1D[v][iSlice]->GetMean());
         qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBGridRelErrH1D[v][iSlice]->GetRMS());
      }
   }

   // Put image names to property tree
   for (Int_t i = 0; i < fImageList.size(); i++) {
      qa.put("images.png." + fImageList[i], fImageList[i] + ".png");
      qa.put("images.eps." + fImageList[i], fImageList[i] + ".eps");
   }

   write_json(std::string(fOutputDir + "field_qa.json").c_str(), qa);

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
   const std::string& opt)
{
   std::string names[] = {"field_slice_Bx_", "field_slice_By_", "field_slice_Bz_", "field_slice_Mod_"};
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::string ss = names[v] + "z_" + ToString<float>(fZpos[s]) + "_" + opt;
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      TGraph2D* graph1 = fhBGraph[v][i];

      DrawGraph2D(graph1, kLitLinear, kLitLinear, kLitLinear, "TRI1");

      canvas[i]->cd(2);
      TH1D* hist2 = (opt != "grid") ? fhBErrH1D[v][i][fPolynomDegreeIndex] : fhBGridErrH1D[v][i];
      DrawH1(hist2, kLitLinear, kLitLog);

      canvas[i]->cd(3);
      TH2D* hist3 = (opt != "grid") ? fhBErrH2D[v][i][fPolynomDegreeIndex] : fhBGridErrH2D[v][i];
      DrawH2(hist3, kLitLinear, kLitLinear, kLitLinear, "colz");

      canvas[i]->cd(4);
      TGraph2D* graph2 = (opt != "grid") ? fhBAprGraph[v][i][fPolynomDegreeIndex] : fhBGridGraph[v][i];
      DrawGraph2D(graph2, kLitLinear, kLitLinear, kLitLinear, "TRI1");

      canvas[i]->cd(5);
      TH1D* hist4 = (opt != "grid") ? fhBRelErrH1D[v][i][fPolynomDegreeIndex] : fhBGridRelErrH1D[v][i];
      DrawH1(hist4, kLitLinear, kLitLog);

      canvas[i]->cd(6);
      TH2D* hist5 = (opt != "grid") ? fhBRelErrH2D[v][i][fPolynomDegreeIndex] : fhBGridRelErrH2D[v][i];
      DrawH2(hist5, kLitLinear, kLitLinear, kLitLinear, "colz");

      SaveCanvasAsImage(canvas[i], fOutputDir);
      fImageList.push_back(canvas[i]->GetName());
   }
}

void CbmLitFieldQa::DrawPoly(
   const std::string& opt)
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::stringstream ss;
      ss << "field_" + opt + "_degree_z_" << fZpos[s];
      canvas[s] = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      TLegend* l1 = new TLegend(0.1,0.1,0.9,0.9);
      l1->SetFillColor(kWhite);
      l1->SetTextSize(0.1);
      l1->SetLineWidth(1);
      l1->SetHeader("Polynom degree");
      for (Int_t v = 0; v < 4; v++) {
         TH1D* firsthist;
         if (opt == "rel") { firsthist = fhBRelErrH1D[v][i][0]; }
         else { firsthist = fhBErrH1D[v][i][0]; }
         Double_t max = firsthist->GetMaximum();
         for (Int_t j = 0; j < fNofPolynoms; j++) {
            canvas[i]->cd(v+2);
            TH1D* hist1;
            if (opt == "rel") { hist1 = fhBRelErrH1D[v][i][j]; }
            else { hist1 = fhBErrH1D[v][i][j]; }
            if (max < hist1->GetMaximum()) { max = hist1->GetMaximum(); }
            string draw_opt = (j == 0) ? "" : "SAME";
            DrawH1(hist1, kLitLinear, kLitLog, draw_opt.c_str(),
                       1 + j, LitDrawingOptions::LineWidth(), 1 + j, LitDrawingOptions::MarkerSize(), kDot);

            if (v == 0) {
               std::stringstream ss;
               ss << fPolynomDegrees[j];
               l1->AddEntry(hist1, ss.str().c_str(),"lp");
            }
         }
         firsthist->SetMaximum(1.2 * max);
      }

      canvas[i]->cd(1);
      l1->Draw();

      SaveCanvasAsImage(canvas[i], fOutputDir);
      fImageList.push_back(canvas[i]->GetName());
   }
}

void CbmLitFieldQa::DrawFieldSlices()
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::string ss = "field_map_at_z_" + ToString<Double_t>(fZpos[s]);
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 800, 800);
      canvas[s]->Divide(2, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      DrawGraph2D(fhBGraph[BX][i], kLitLinear, kLitLinear, kLitLinear, "TRI1");

      canvas[i]->cd(2);
      TGraph2D* graphBy = fhBGraph[BY][i];
      DrawGraph2D(fhBGraph[BY][i], kLitLinear, kLitLinear, kLitLinear, "TRI1");

      canvas[i]->cd(3);
      TGraph2D* graphBz = fhBGraph[BZ][i];
      DrawGraph2D(fhBGraph[BZ][i], kLitLinear, kLitLinear, kLitLinear, "TRI1");

      canvas[i]->cd(4);
      TGraph2D* graphMod = fhBGraph[MOD][i];
      DrawGraph2D(fhBGraph[MOD][i], kLitLinear, kLitLinear, kLitLinear, "TRI1");

      SaveCanvasAsImage(canvas[i], fOutputDir);
      fImageList.push_back(canvas[i]->GetName());
   }
}

void CbmLitFieldQa::DrawFieldAlongZ()
{
   TCanvas* canvas = new TCanvas("field_map_along_z", "field_map_along_z", 1200, 800);
   canvas->Divide(3, 2);

   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      canvas->cd(i+1);
      DrawGraph(list_of(fhBAlongZGraph[BX][i])(fhBAlongZGraph[BY][i])(fhBAlongZGraph[BZ][i]),
            list_of("B_{x}")("B_{y}")("B_{z}"), kLitLinear, kLitLinear, true, 0.7, 0.5, 0.9, 0.3);
   }
   SaveCanvasAsImage(canvas, fOutputDir);
   fImageList.push_back(canvas->GetName());
}
ClassImp(CbmLitFieldQa);
