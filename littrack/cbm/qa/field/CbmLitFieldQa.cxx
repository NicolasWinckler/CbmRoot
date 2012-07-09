/**
 * \file CbmLitFieldQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmLitFieldQa.h"
#include "CbmLitFieldQaReport.h"
#include "cbm/base/CbmLitEnvironment.h"
#include "utils/CbmLitUtils.h"
#include "cbm/qa/draw/CbmLitDrawHist.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TGraph.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <sstream>
#include <cmath>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>

using lit::ToString;
using lit::SaveCanvasAsImage;
using boost::assign::list_of;

CbmLitFieldQa::CbmLitFieldQa():
   fField(NULL),
   fHistoList(NULL),
   fNofSlices(0),
   fZSlicePosition(),
   fXSlicePosition(),
   fYSlicePosition(),
   fgB(),
   fgBAlongZAngle(),
   fgBAlongZXPosition(),
   fgBAlongZYPosition(),
   fAlongZAngles(),
   fAlongZXPosition(),
   fAlongZYPosition(),
   fZMin(-10.),
   fZMax(300.),
   fZStep(5.),
   fAcceptanceAngleX(35.),
   fAcceptanceAngleY(35.),
   fNofBinsX(30),
   fNofBinsY(30),
   fOutputDir("./field/")
{

}

CbmLitFieldQa::~CbmLitFieldQa()
{
}

InitStatus CbmLitFieldQa::Init()
{
   // Set draw styles
   SetStyles();

   fNofSlices = fZSlicePosition.size();

   // Calculate (X, Y) window for each slice
   fXSlicePosition.resize(fNofSlices);
   fYSlicePosition.resize(fNofSlices);
   for (Int_t i = 0; i < fNofSlices; i++) {
      Double_t tanXangle = std::tan(fAcceptanceAngleX * 3.14159265 / 180); //
      Double_t tanYangle = std::tan(fAcceptanceAngleY * 3.14159265 / 180); //
      fXSlicePosition[i] = fZSlicePosition[i] * tanXangle;
      fYSlicePosition[i] = fZSlicePosition[i] * tanYangle;
   }

   fAlongZAngles = list_of(0.)(10.)(20.)(30.);
   fAlongZXPosition = list_of(0.)(50.)(100.)(150.);
   fAlongZYPosition = list_of(0.)(50.)(100.)(150.);

   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fField = env->GetField();

   return kSUCCESS;
}

void CbmLitFieldQa::Exec(
   Option_t* opt)
{

}

void CbmLitFieldQa::Finish()
{
   CreateHistos();
   FillBHistos();
   DrawFieldSlices();
   DrawFieldAlongZ();
   CreatePropertyTree();

   TIter next(fHistoList);
   while ( TObject* histo = ((TObject*)next()) ) { histo->Write(); }
}

void CbmLitFieldQa::CreateHistos()
{
   fHistoList = new TList();

   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};

   // [BX, BY, BZ, MOD] components
   fgB.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fgB[i].resize(fNofSlices);
   }
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         fgB[v][i] = new TGraph2D();
         string graphName = "fhBGraph";
         string graphTitle = string("fhBGraph;X [cm]; Y [cm];") + zTitle[v];
         fgB[v][i]->SetNameTitle(graphName.c_str(), graphTitle.c_str());
         fHistoList->Add(fgB[v][i]);
      }
   }

   // [BX, BY, BZ, MOD]
   fgBAlongZAngle.resize(4);
   fgBAlongZXPosition.resize(4);
   fgBAlongZYPosition.resize(4);
   for (Int_t i = 0; i < 4; i++) {
      fgBAlongZAngle[i].resize(fAlongZAngles.size());
      fgBAlongZXPosition[i].resize(fAlongZXPosition.size());
      fgBAlongZYPosition[i].resize(fAlongZYPosition.size());
   }
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
         fgBAlongZAngle[v][i] = new TGraph();
         fgBAlongZAngle[v][i]->SetNameTitle("fhBAlongZGraph", "fhBAlongZGraph;Z [cm];B [kGauss]");
      }
      for (Int_t i = 0; i < fAlongZXPosition.size(); i++) {
         fgBAlongZXPosition[v][i] = new TGraph();
         fgBAlongZXPosition[v][i]->SetNameTitle("fgBAlongZXPosition", "fgBAlongZXPosition;Z [cm];B [kGauss]");
      }
      for (Int_t i = 0; i < fAlongZYPosition.size(); i++) {
         fgBAlongZYPosition[v][i] = new TGraph();
         fgBAlongZYPosition[v][i]->SetNameTitle("fgBAlongZYPosition", "fgBAlongZYPosition;Z [cm];B [kGauss]");
      }
   }
}

void CbmLitFieldQa::FillBHistos()
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

            // Get field value
            Double_t pos[3] = {X, Y, Z};
            Double_t B[3];
            fField->GetFieldValue(pos, B);

            Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

            fgB[BX][iSlice]->SetPoint(cnt, X, Y, B[BX]);
            fgB[BY][iSlice]->SetPoint(cnt, X, Y, B[BY]);
            fgB[BZ][iSlice]->SetPoint(cnt, X, Y, B[BZ]);
            fgB[MOD][iSlice]->SetPoint(cnt, X, Y, Bmod);
            cnt++;
         }
      }
   }

   // Fill histograms for magnetic field along Z for different angles
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

         Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

         fgBAlongZAngle[BX][i]->SetPoint(istep, Z, B[0]);
         fgBAlongZAngle[BY][i]->SetPoint(istep, Z, B[1]);
         fgBAlongZAngle[BZ][i]->SetPoint(istep, Z, B[2]);
         fgBAlongZAngle[MOD][i]->SetPoint(istep, Z, Bmod);
      }
   }

   // Fill histograms for magnetic field along Z for different X position
   for (Int_t i = 0; i < fAlongZXPosition.size(); i++) {
      Int_t nofSteps = Int_t((fZMax - fZMin) / fZStep);
      for (Int_t istep = 0; istep < nofSteps; istep++) {
         Double_t Z = fZMin + istep * fZStep;
         Double_t X = fAlongZXPosition[i];
         Double_t Y = 0;

         // Get field value
         Double_t pos[3] = {X, Y, Z};
         Double_t B[3];
         fField->GetFieldValue(pos, B);

         Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

         fgBAlongZXPosition[BX][i]->SetPoint(istep, Z, B[0]);
         fgBAlongZXPosition[BY][i]->SetPoint(istep, Z, B[1]);
         fgBAlongZXPosition[BZ][i]->SetPoint(istep, Z, B[2]);
         fgBAlongZXPosition[MOD][i]->SetPoint(istep, Z, Bmod);
      }
   }

   // Fill histograms for magnetic field along Z for different Y position
   for (Int_t i = 0; i < fAlongZYPosition.size(); i++) {
      Int_t nofSteps = Int_t((fZMax - fZMin) / fZStep);
      for (Int_t istep = 0; istep < nofSteps; istep++) {
         Double_t Z = fZMin + istep * fZStep;
         Double_t X = 0;
         Double_t Y = fAlongZYPosition[i];

         // Get field value
         Double_t pos[3] = {X, Y, Z};
         Double_t B[3];
         fField->GetFieldValue(pos, B);

         Double_t Bmod = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

         fgBAlongZYPosition[BX][i]->SetPoint(istep, Z, B[0]);
         fgBAlongZYPosition[BY][i]->SetPoint(istep, Z, B[1]);
         fgBAlongZYPosition[BZ][i]->SetPoint(istep, Z, B[2]);
         fgBAlongZYPosition[MOD][i]->SetPoint(istep, Z, Bmod);
      }
   }
}

void CbmLitFieldQa::CreatePropertyTree()
{
   // Create and serialize property tree
   boost::property_tree::ptree qa;
   qa.put("NofSlices", fNofSlices);
//   std::string vnames[4] = {"BX", "BY", "BZ", "MOD"};
//
//   for (Int_t iSlice = 0; iSlice < fNofSlices; iSlice++) {
//      std::string slice = "slice" + ToString<Int_t>(iSlice);
//      qa.put(slice + ".Z", fZpos[iSlice]);
//      for (Int_t iPolynom = 0; iPolynom < fNofPolynoms; iPolynom++) {
//         for (Int_t v = 0; v < 4; v++) {
//            std::string name = slice + ".polynomial" + ToString<Int_t>(iPolynom);
//            qa.put(name + ".degree", fPolynomDegrees[iPolynom]);
//            qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBErrH1D[v][iSlice][iPolynom]->GetMean());
//            qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBErrH1D[v][iSlice][iPolynom]->GetRMS());
//            qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBRelErrH1D[v][iSlice][iPolynom]->GetMean());
//            qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBRelErrH1D[v][iSlice][iPolynom]->GetRMS());
//         }
//      }
//      std::string name = slice + ".grid";
//      for (Int_t v = 0; v < 4; v++) {
//         qa.put(name + ".err." + vnames[v] + ".abs.mean", fhBGridErrH1D[v][iSlice]->GetMean());
//         qa.put(name + ".err." + vnames[v] + ".abs.rms", fhBGridErrH1D[v][iSlice]->GetRMS());
//         qa.put(name + ".err." + vnames[v] + ".rel.mean", fhBGridRelErrH1D[v][iSlice]->GetMean());
//         qa.put(name + ".err." + vnames[v] + ".rel.rms", fhBGridRelErrH1D[v][iSlice]->GetRMS());
//      }
//   }

   write_json(std::string(fOutputDir + "field_qa.json").c_str(), qa);

   // Create report
   CbmSimulationReport* report = new CbmLitFieldQaReport();
   ofstream foutHtml(string(fOutputDir + "field_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "field_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "field_qa.txt").c_str());
   report->Create(kHtmlReport, foutHtml, fOutputDir);
   report->Create(kLatexReport, foutLatex, fOutputDir);
   report->Create(kTextReport, foutText, fOutputDir);
   delete report;
}

void CbmLitFieldQa::DrawFieldSlices()
{
   TCanvas* canvas[fNofSlices];
   for (Int_t s = 0; s < fNofSlices; s++) {
      std::string str = "field_qa_map_at_z_" + ToString<Double_t>(fZSlicePosition[s]);
      canvas[s] = new TCanvas(str.c_str(), str.c_str(), 1000, 1000);
      canvas[s]->Divide(2, 2);
   }

   for (Int_t i = 0; i < fNofSlices; i++) {
	   for (Int_t v = 0; v < 4; v++) {
		  canvas[i]->cd(v + 1);
		  DrawGraph2D(fgB[v][i], kLitLinear, kLitLinear, kLitLinear, "colz");
	   }
       SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitFieldQa::DrawFieldAlongZ()
{
	// Draw for different angles
   TCanvas* canvas = new TCanvas("field_qa_map_along_z_angle", "field_qa_map_along_z_angle", 1000, 1000);
   canvas->Divide(2, 2);
   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      canvas->cd(i+1);
      DrawGraph(list_of(fgBAlongZAngle[BX][i])(fgBAlongZAngle[BY][i])(fgBAlongZAngle[BZ][i])(fgBAlongZAngle[MOD][i]),
            list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLitLinear, kLitLinear, true, 0.7, 0.5, 0.9, 0.3);
   }
   SaveCanvasAsImage(canvas, fOutputDir);

   // Draw for different X positions
   TCanvas* canvasX = new TCanvas("field_qa_map_along_z_x_position", "field_qa_map_along_z_x_position", 1000, 1000);
   canvasX->Divide(2, 2);
   for (Int_t i = 0; i < fAlongZXPosition.size(); i++) {
      canvasX->cd(i+1);
      DrawGraph(list_of(fgBAlongZXPosition[BX][i])(fgBAlongZXPosition[BY][i])(fgBAlongZXPosition[BZ][i])(fgBAlongZXPosition[MOD][i]),
            list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLitLinear, kLitLinear, true, 0.7, 0.5, 0.9, 0.3);
   }
   SaveCanvasAsImage(canvasX, fOutputDir);

   // Draw for different Y positions
   TCanvas* canvasY = new TCanvas("field_qa_map_along_z_y_position", "field_qa_map_along_z_y_position", 1000, 1000);
   canvasY->Divide(2, 2);
   for (Int_t i = 0; i < fAlongZYPosition.size(); i++) {
      canvasY->cd(i+1);
      DrawGraph(list_of(fgBAlongZYPosition[BX][i])(fgBAlongZYPosition[BY][i])(fgBAlongZYPosition[BZ][i])(fgBAlongZYPosition[MOD][i]),
            list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLitLinear, kLitLinear, true, 0.7, 0.5, 0.9, 0.3);
   }
   SaveCanvasAsImage(canvasY, fOutputDir);
}
ClassImp(CbmLitFieldQa);
