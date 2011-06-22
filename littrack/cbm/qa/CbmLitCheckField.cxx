/** CbmLitCheckField.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#include "qa/CbmLitCheckField.h"

#include "base/CbmLitEnvironment.h"
#include "base/CbmLitFloat.h"
#include "base/CbmLitFieldFitter.h"
#include "base/CbmLitFieldGridCreator.h"
#include "utils/CbmLitUtils.h"
#include "utils/CbmLitDrawHist.h"

#include "../../parallel/LitField.h"

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
   fIsCheckFieldApproximation(true),
   fIsCheckFieldMap(true),
   fIsCheckGridCreator(true),
   fDrawBx(true),
   fDrawBy(true),
   fDrawBz(true),
   fDrawMod(true),
   fDrawFieldMap(true),
   fDrawPoly(true),
   fDrawSlices(true),
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
   fDegrees()
{

}

CbmLitCheckField::~CbmLitCheckField()
{
}

InitStatus CbmLitCheckField::Init()
{
   // Set draw styles
   SetStyles();

   // Set Z positions of slices
   fZpos.push_back(30.);
   fZpos.push_back(50.);
   fZpos.push_back(100.);
   fZpos.push_back(150.);
   fZpos.push_back(200.);
   fZpos.push_back(300.);
   fZpos.push_back(400.);
   fNofSlices = fZpos.size();

   // Set polynom degrees
// fDegrees.push_back(3);
// fDegrees.push_back(5);
   fDegrees.push_back(7);
// fDegrees.push_back(9);
   fNofPolynoms = fDegrees.size();

   fXpos.resize(fNofSlices);
   fYpos.resize(fNofSlices);
   for (Int_t i = 0; i < fNofSlices; i++) {
      Double_t tanXangle = std::tan(fXangle * 3.14159265 / 180); //
      Double_t tanYangle = std::tan(fYangle * 3.14159265 / 180); //
      fXpos[i] = fZpos[i] * tanXangle;
      fYpos[i] = fZpos[i] * tanYangle;
   }

   fAlongZAngles.push_back(0.);
   fAlongZAngles.push_back(5.);
   fAlongZAngles.push_back(10.);
   fAlongZAngles.push_back(15.);
   fAlongZAngles.push_back(20.);
   fAlongZAngles.push_back(25.);

   // Create field approximation tool for each polynom degree
   if (fIsCheckFieldApproximation) {
      fFitter.resize(fNofPolynoms);
      for (UInt_t i = 0; i < fNofPolynoms; i++) {
         fFitter[i] = new CbmLitFieldFitter(fDegrees[i]);
         fFitter[i]->SetXangle(fXangle);
         fFitter[i]->SetYangle(fYangle);
         fFitter[i]->SetNofBinsX(100);//fNofBinsX);
         fFitter[i]->SetNofBinsY(100);//fNofBinsY);
         fFitter[i]->SetUseEllipseAcc(fUseEllipseAcc);
      }
   }

   // Create grid creator tool
   if (fIsCheckGridCreator) {
      fGridCreator = new CbmLitFieldGridCreator();
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
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fField = env->GetField();

   // Fill and create graphs and histograms
   CreateHistos();
   FillBHistos();
   if (fIsCheckFieldApproximation) { CheckFieldFitter(); }
   if (fIsCheckGridCreator) { CheckGridCreator(); }
   if (fIsCheckFieldMap) { CheckFieldMap(); }

   // Draw graphs and histograms
   if (fIsCheckFieldApproximation) {
      if (fDrawSlices) {
         if (IsDrawBx()) { DrawSlices(BX, "apr"); }
         if (IsDrawBy()) { DrawSlices(BY, "apr"); }
         if (IsDrawBz()) { DrawSlices(BZ, "apr"); }
         if (IsDrawBz()) { DrawSlices(MOD, "apr"); }
      }

      if (fDrawPoly) {
         DrawPoly("rel");
         DrawPoly("abs");
      }
   }
   if (fIsCheckFieldMap) {
      if (fDrawFieldMap) {
         DrawFieldSlices();
         DrawFieldAlongZ();
      }
   }
   if (fIsCheckGridCreator) {
      if (fDrawSlices) {
         if (IsDrawBx()) { DrawSlices(BX, "grid"); }
         if (IsDrawBy()) { DrawSlices(BY, "grid"); }
         if (IsDrawBz()) { DrawSlices(BZ, "grid"); }
         if (IsDrawBz()) { DrawSlices(MOD, "grid"); }
      }
   }
}

void CbmLitCheckField::Finish()
{
}

void CbmLitCheckField::CreateHistos()
{
   // Histogram list
   fHistoList = new TList();

   fhBGraph.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhBGraph[i].resize(fNofSlices);
   }
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         fhBGraph[v][i] = new TGraph2D();
      }
   }

   CreateFitterErrHistos();
   CreateGridErrHistos();

   fhBAlongZGraph.resize(3);
   for (Int_t i = 0; i < 3; i++) {
      fhBAlongZGraph[i].resize(fAlongZAngles.size());
   }
   for (Int_t v = 0; v < 3; v++) {
      for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
         fhBAlongZGraph[v][i] = new TGraph();
      }
   }
}

void CbmLitCheckField::CreateFitterErrHistos()
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

   // Create histograms
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         for(Int_t j = 0; j < fNofPolynoms; j++) {
            fhBAprGraph[v][i][j] = new TGraph2D();

            std::string histName = "hBErrH1D" + names[v] + lit::ToString<Int_t>(i) + "_" + lit::ToString<Int_t>(j);
            fhBErrH1D[v][i][j] = new TH1D(histName.c_str(), histName.c_str(), nofBinsErrB, minErrB, maxErrB);
            fHistoList->Add(fhBErrH1D[v][i][j]);

            histName = "hBErrH2D" + names[v] + lit::ToString<Int_t>(i) + "_" + lit::ToString<Int_t>(j);
            fhBErrH2D[v][i][j] = new TH2D(histName.c_str(), histName.c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBErrH2D[v][i][j]);

            histName = "hBRelErrH1D" + names[v] + lit::ToString<Int_t>(i) + "_" + lit::ToString<Int_t>(j);
            fhBRelErrH1D[v][i][j] = new TH1D(histName.c_str(), histName.c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
            fHistoList->Add(fhBRelErrH1D[v][i][j]);

            histName = "hBRelErrH2D" + names[v] + lit::ToString<Int_t>(i) + "_" + lit::ToString<Int_t>(j);
            fhBRelErrH2D[v][i][j] = new TH2D(histName.c_str(), histName.c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
            fHistoList->Add(fhBRelErrH2D[v][i][j]);
         }
      }
   }
   std::cout << "Field fitter error histograms created" << std::endl;
}

void CbmLitCheckField::CreateGridErrHistos()
{
   // Resize histogram vectors
   fhGridBGraph.resize(4);
   fhGridBErrH1D.resize(4);
   fhGridBErrH2D.resize(4);
   fhGridBRelErrH1D.resize(4);
   fhGridBRelErrH2D.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fhGridBGraph[i].resize(fNofSlices);
      fhGridBErrH1D[i].resize(fNofSlices);
      fhGridBErrH2D[i].resize(fNofSlices);
      fhGridBRelErrH1D[i].resize(fNofSlices);
      fhGridBRelErrH2D[i].resize(fNofSlices);
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

   // Create histograms
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         fhGridBGraph[v][i] = new TGraph2D();

         std::string histName = "hGridBErrH1D" + names[v] + lit::ToString<Int_t>(i);
         fhGridBErrH1D[v][i] = new TH1D(histName.c_str(), histName.c_str(), nofBinsErrB, minErrB, maxErrB);
         fHistoList->Add(fhGridBErrH1D[v][i]);

         histName = "hGridBErrH2D" + names[v] + lit::ToString<Int_t>(i);
         fhGridBErrH2D[v][i] = new TH2D(histName.c_str(), histName.c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhGridBErrH2D[v][i]);

         histName = "hGridBRelErrH1D" + names[v] + lit::ToString<Int_t>(i);
         fhGridBRelErrH1D[v][i] = new TH1D(histName.c_str(), histName.c_str(), nofBinsRelErrB, minRelErrB, maxRelErrB);
         fHistoList->Add(fhGridBRelErrH1D[v][i]);

         histName = "hGridBRelErrH2D" + names[v] + lit::ToString<Int_t>(i);
         fhGridBRelErrH2D[v][i] = new TH2D(histName.c_str(), histName.c_str(), nofBinsErrX, -fXpos[i], fXpos[i], nofBinsErrY, -fYpos[i], fYpos[i]);
         fHistoList->Add(fhGridBRelErrH2D[v][i]);
      }
   }
   std::cout << "Grid creator error histograms created" << std::endl;
}

void CbmLitCheckField::FillBHistos()
{
   for (UInt_t iSlice = 0; iSlice < fNofSlices; iSlice++) { // loop over z positions
      Double_t max[4] = {
         std::numeric_limits<Double_t>::min(),
         std::numeric_limits<Double_t>::min(),
         std::numeric_limits<Double_t>::min(),
         std::numeric_limits<Double_t>::min()
      };
      Double_t min[4] = {
         std::numeric_limits<Double_t>::max(),
         std::numeric_limits<Double_t>::max(),
         std::numeric_limits<Double_t>::max(),
         std::numeric_limits<Double_t>::max()
      };

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

            // Find maximum entry in the histogram
            if (max[BX] < B[0]) { max[BX] = B[0]; }
            if (max[BY] < B[1]) { max[BY] = B[1]; }
            if (max[BZ] < B[2]) { max[BZ] = B[2]; }
            if (max[MOD] < Bmod) { max[MOD] = Bmod; }
            // Find minimum entry in the histogram
            if (min[BX] > B[0]) { min[BX] = B[0]; }
            if (min[BY] > B[1]) { min[BY] = B[1]; }
            if (min[BZ] > B[2]) { min[BZ] = B[2]; }
            if (min[MOD] > Bmod) { min[MOD] = Bmod; }

            fhBGraph[BX][iSlice]->SetPoint(cnt, X, Y, B[BX]);
            fhBGraph[BY][iSlice]->SetPoint(cnt, X, Y, B[BY]);
            fhBGraph[BZ][iSlice]->SetPoint(cnt, X, Y, B[BZ]);
            fhBGraph[MOD][iSlice]->SetPoint(cnt, X, Y, Bmod);
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
   }
}

void CbmLitCheckField::CheckFieldFitter()
{
   std::vector<std::vector<lit::parallel::LitFieldSlice<float> > > slices;
   slices.resize(fNofPolynoms);
   for (UInt_t i = 0; i < fNofPolynoms; i++) {slices[i].resize(fNofSlices);}

   // Approximate field slices in each slices for each polynom degree
   for (UInt_t i = 0; i < fNofPolynoms; i++) {
      for(Int_t j = 0; j < fNofSlices; j++) {
         fFitter[i]->FitSlice(fZpos[j], slices[i][j]);
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
               lit::parallel::LitFieldValue<float> v;
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
               lit::parallel::LitFieldValue<float> v;
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

void CbmLitCheckField::CheckGridCreator()
{
   std::vector<lit::parallel::LitFieldGrid> grids;
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
            lit::parallel::LitFieldValue<float> v;
            grids[iSlice].GetFieldValue(X, Y, v);
            Double_t mod = std::sqrt(v.Bx * v.Bx + v.By * v.By + v.Bz * v.Bz);
            fhGridBGraph[BX][iSlice]->SetPoint(cnt, X, Y, v.Bx);
            fhGridBGraph[BY][iSlice]->SetPoint(cnt, X, Y, v.By);
            fhGridBGraph[BZ][iSlice]->SetPoint(cnt, X, Y, v.Bz);
            fhGridBGraph[MOD][iSlice]->SetPoint(cnt, X, Y, mod);
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

            lit::parallel::LitFieldValue<float> v;
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

            fhGridBErrH2D[BX][iSlice]->Fill(X, Y, errBx);
            fhGridBErrH1D[BX][iSlice]->Fill(errBx);
            fhGridBRelErrH1D[BX][iSlice]->Fill(relErrBx);
            fhGridBRelErrH2D[BX][iSlice]->Fill(X, Y, relErrBx);
            fhGridBErrH2D[BY][iSlice]->Fill(X, Y, errBy);
            fhGridBErrH1D[BY][iSlice]->Fill(errBy);
            fhGridBRelErrH1D[BY][iSlice]->Fill(relErrBy);
            fhGridBRelErrH2D[BY][iSlice]->Fill(X, Y, relErrBy);
            fhGridBErrH2D[BZ][iSlice]->Fill(X, Y, errBz);
            fhGridBErrH1D[BZ][iSlice]->Fill(errBz);
            fhGridBRelErrH1D[BZ][iSlice]->Fill(relErrBz);
            fhGridBRelErrH2D[BZ][iSlice]->Fill(X, Y, relErrBz);
            fhGridBErrH2D[MOD][iSlice]->Fill(X, Y, errMod);
            fhGridBErrH1D[MOD][iSlice]->Fill(errMod);
            fhGridBRelErrH1D[MOD][iSlice]->Fill(relErrMod);
            fhGridBRelErrH2D[MOD][iSlice]->Fill(X, Y, relErrMod);

         }
      }
   }
}

void CbmLitCheckField::CheckFieldMap()
{
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

void CbmLitCheckField::DrawSlices(
   Int_t v,
   const std::string& opt)
{
   std::string names[] = {"field_slice_Bx_", "field_slice_By_", "field_slice_Bz_", "field_slice_Mod_"};
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::string ss = names[v] + "z_" + lit::ToString<float>(fZpos[s]) + "_" + opt;
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 1200, 800);
      canvas[s]->Divide(3, 2);
   }

   std::string title = "B_{x}";
   if (v == 1) { title = "B_{y}"; }
   if (v == 2) { title = "B_{z}"; }
   if (v == 3) { title = "|B|"; }
   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      TGraph2D* graph1 = fhBGraph[v][i];
      DrawGraph2D(graph1, "X [cm]", "Y [cm]", std::string(title + " [kGauss]"),
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "TRI1");

      canvas[i]->cd(2);
      TH1D* hist2 = (opt != "grid") ? fhBErrH1D[v][i][fPolynomDegreeIndex] : fhGridBErrH1D[v][i];
      DrawHist1D(hist2, std::string(title + " [kGauss]"), "Counter",
                 LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
                 LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "");

      canvas[i]->cd(3);
      TH2D* hist3 = (opt != "grid") ? fhBErrH2D[v][i][fPolynomDegreeIndex] : fhGridBErrH2D[v][i];
      DrawHist2D(hist3, "X [cm]", "Y [cm]", std::string(title + " [kGauss]"),
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "colz");

      canvas[i]->cd(4);
      TGraph2D* graph2 = (opt != "grid") ? fhBAprGraph[v][i][fPolynomDegreeIndex] : fhGridBGraph[v][i];
      DrawGraph2D(graph2, "X [cm]", "Y [cm]", std::string(title + " [kGauss]"),
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "TRI1");

      canvas[i]->cd(5);
      TH1D* hist4 = (opt != "grid") ? fhBRelErrH1D[v][i][fPolynomDegreeIndex] : fhGridBRelErrH1D[v][i];
      DrawHist1D(hist4, std::string(title + " relative error [%]"), "Counter",
                 LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
                 LIT_MARKER_STYLE1, kLitLinearScale, kLitLogScale, "");

      canvas[i]->cd(6);
      TH2D* hist5 = (opt != "grid") ? fhBRelErrH2D[v][i][fPolynomDegreeIndex] : fhGridBRelErrH2D[v][i];
      DrawHist2D(hist5, "X [cm]", "Y [cm]", std::string(title + " relative error [%]"),
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "colz");

      lit::SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitCheckField::DrawPoly(
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
            std::string title;
            if (opt == "rel") {
               if (v == 0) { title = "B_{x} relative error [%]"; }
               if (v == 1) { title = "B_{y} relative error [%]"; }
               if (v == 2) { title = "B_{z} relative error [%]"; }
               if (v == 3) { title = "|B| relative error [%]"; }
            } else {
               if (v == 0) { title = "B_{x} [kGauss]"; }
               if (v == 1) { title = "B_{y} [kGauss]"; }
               if (v == 2) { title = "B_{z} [kGauss]"; }
               if (v == 3) { title = "|B| [kGauss]"; }
            }
            std::string draw_opt;
            if (j == 0) { draw_opt = ""; }
            else { draw_opt = "SAME"; }
            DrawHist1D(hist1, title, "Counter",
                       1+j, LIT_LINE_WIDTH, 1+j, LIT_MARKER_SIZE,
                       kDot, kLitLinearScale, kLitLogScale, draw_opt.c_str());

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

      lit::SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitCheckField::DrawFieldSlices()
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::string ss = "field_map_at_z_" + lit::ToString<Double_t>(fZpos[s]);
      canvas[s] = new TCanvas(ss.c_str(), ss.c_str(), 800, 800);
      canvas[s]->Divide(2, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
      canvas[i]->cd(1);
      TGraph2D* graphBx = fhBGraph[BX][i];
      DrawGraph2D(graphBx, "X [cm]", "Y [cm]", "B_{x} [kGauss]",
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "TRI1");

      canvas[i]->cd(2);
      TGraph2D* graphBy = fhBGraph[BY][i];
      DrawGraph2D(graphBy, "X [cm]", "Y [cm]", "B_{y} [kGauss]",
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "TRI1");

      canvas[i]->cd(3);
      TGraph2D* graphBz = fhBGraph[BZ][i];
      DrawGraph2D(graphBz, "X [cm]", "Y [cm]", "B_{z} [kGauss]",
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "TRI1");

      canvas[i]->cd(4);
      TGraph2D* graphMod = fhBGraph[MOD][i];
      DrawGraph2D(graphMod, "X [cm]", "Y [cm]", "|B| [kGauss]",
            kLitLinearScale, kLitLinearScale, kLitLinearScale, "TRI1");

      lit::SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitCheckField::DrawFieldAlongZ()
{
   TCanvas* canvas = new TCanvas("field_map_along_z", "field_map_along_z", 1200, 800);
   canvas->Divide(3, 2);

   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      canvas->cd(i+1);
      TGraph* graphBx = fhBAlongZGraph[BX][i];
      TGraph* graphBy = fhBAlongZGraph[BY][i];
      TGraph* graphBz = fhBAlongZGraph[BZ][i];

      DrawGraph(graphBx, graphBy, graphBz,
                std::string(lit::ToString<Double_t>(fAlongZAngles[i]) + "#circ"),
                "Z [cm]", "B [kGauss]",
                "B_{x}", "B_{y}", "B_{z}", kLitLinearScale, kLitLinearScale, false,
                0.7, 0.5, 0.9, 0.3);
   }
   lit::SaveCanvasAsImage(canvas, fOutputDir);
}
ClassImp(CbmLitCheckField);
