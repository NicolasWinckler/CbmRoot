/**
 * \file CbmLitFieldQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmLitFieldQa.h"
#include "CbmLitFieldQaReport.h"
#include "utils/CbmLitUtils.h"
#include "CbmDrawHist.h"
#include "CbmHistManager.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"

#include "TGraph.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <sstream>
#include <cmath>
#include <boost/assign/list_of.hpp>

using lit::ToString;
using lit::SaveCanvasAsImage;
using boost::assign::list_of;

CbmLitFieldQa::CbmLitFieldQa():
   fField(NULL),
   fNofSlices(0),
   fZSlicePosition(),
   fXSlicePosition(),
   fYSlicePosition(),
   fAlongZAngles(),
   fAlongZXY(),
   fZMin(-10.),
   fZMax(300.),
   fZStep(5.),
   fAcceptanceAngleX(35.),
   fAcceptanceAngleY(35.),
   fNofBinsX(30),
   fNofBinsY(30),
   fMinZFieldInt(160.),
   fMaxZFieldInt(330.),
   fHM(NULL),
   fOutputDir("./")
{

}

CbmLitFieldQa::~CbmLitFieldQa()
{
}

InitStatus CbmLitFieldQa::Init()
{
   // Set draw styles
   SetDefaultDrawStyle();

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

   fAlongZXY.push_back(std::make_pair(0., 0.));
   fAlongZXY.push_back(std::make_pair(100., 0.));
   fAlongZXY.push_back(std::make_pair(0., 100.));

   fField = FairRunAna::Instance()->GetField();

   fHM = new CbmHistManager();

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
   CreateSimulationReport();
   fHM->WriteToFile();
}

void CbmLitFieldQa::CreateHistos()
{
   std::string names[] = {"Bx", "By", "Bz", "Mod"};
   string zTitle[] = {"B_{x} [kGauss]", "B_{y} [kGauss]", "B_{z} [kGauss]", "|B| [kGauss]"};
   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fNofSlices; i++) {
         TGraph2D* graph = new TGraph2D();
         string name = "hmf_" + names[v] + "_Graph2D_" + ToString<Int_t>(i);
         string title = name + ";X [cm];Y [cm];" + zTitle[v];
         graph->SetNameTitle(name.c_str(), title.c_str());
         fHM->Add(name, graph);
      }
   }

   for (Int_t v = 0; v < 4; v++) {
      for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
         TGraph* graph = new TGraph();
         string name = "hmf_" + names[v] + "AlongZAngle_Graph_" + ToString<Int_t>(i);
         string title = name + ";Z [cm];B [kGauss]";
         graph->SetNameTitle(name.c_str(), title.c_str());
         fHM->Add(name, graph);
      }
      for (Int_t i = 0; i < fAlongZXY.size(); i++) {
         TGraph* graph = new TGraph();
         string name = "hmf_" + names[v] + "AlongZXY_Graph_" + ToString<Int_t>(i);
         string title = name + ";Z [cm];B [kGauss]";
         graph->SetNameTitle(name.c_str(), title.c_str());
         fHM->Add(name, graph);
      }
      for (Int_t i = 0; i < fAlongZXY.size(); i++) {
         TGraph* graph = new TGraph();
         string name = "hmf_" + names[v] + "AlongZXYIntegral_Graph_" + ToString<Int_t>(i);
         string title = name + ";Z [cm];B_{Int_t} [kGauss*m]";
         graph->SetNameTitle(name.c_str(), title.c_str());
         fHM->Add(name, graph);
      }
   }
}

void CbmLitFieldQa::FillBHistos()
{
   std::string names[] = {"Bx", "By", "Bz", "Mod"};
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
            Double_t B[4];
            fField->GetFieldValue(pos, B);

            B[3] = std::sqrt(B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);
            for (Int_t v = 0; v < 4; v++) {
            	string name = string("hmf_") + names[v] + "_Graph2D_" + ToString<Int_t>(iSlice);
            	fHM->G2(name)->SetPoint(cnt, X, Y, B[v]);
            }
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
         Double_t B[4];
         fField->GetFieldValue(pos, B);

         B[3] = std::sqrt(B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);
         for (Int_t v = 0; v < 4; v++) {
         	string name = string("hmf_") + names[v] + "AlongZAngle_Graph_" + ToString<Int_t>(i);
         	fHM->G1(name)->SetPoint(istep, Z, B[v]);
         }
      }
   }
   // Fill histograms for magnetic field along Z for different X position
   for (Int_t i = 0; i < fAlongZXY.size(); i++) {
      Int_t nofSteps = Int_t((fZMax - fZMin) / fZStep);
      Double_t integralB[4] = {0., 0., 0., 0.};
      for (Int_t istep = 0; istep < nofSteps; istep++) {
         Double_t Z = fZMin + istep * fZStep;
         Double_t X = fAlongZXY[i].first;
         Double_t Y = fAlongZXY[i].second;

         // Get field value
         Double_t pos[3] = {X, Y, Z};
         Double_t B[4];
         fField->GetFieldValue(pos, B);

         B[3] = std::sqrt(B[0]*B[0] + B[1]*B[1] + B[2]*B[2]);

         for (Int_t v = 0; v < 4; v++){
        	string name = string("hmf_") + names[v] + "AlongZXY_Graph_" + ToString<Int_t>(i);
        	fHM->G1(name)->SetPoint(istep, Z, B[v]);
            // Calculate field integral in the RICH detector
            if (Z >= fMinZFieldInt && Z <= fMaxZFieldInt){
               integralB[v] += 0.01 * fZStep * fabs(B[v]); // in kGauss * meter
               string name = string("hmf_") + names[v] + "AlongZXYIntegral_Graph_" + ToString<Int_t>(i);
               fHM->G1(name)->SetPoint(istep, Z, integralB[v]);
               fHM->G1(name)->SetMaximum(1.1 * integralB[v]);
            }
         }
      }
   }
}

void CbmLitFieldQa::CreateSimulationReport()
{
   CbmSimulationReport* report = new CbmLitFieldQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitFieldQa::DrawFieldSlices()
{
   TCanvas* canvas[fNofSlices];
   string names[] = {"Bx", "By", "Bz", "Mod"};
   for (Int_t i = 0; i < fNofSlices; i++) {
	   string str = "field_qa_map_at_z_" + ToString<Double_t>(fZSlicePosition[i]);
	   canvas[i] = new TCanvas(str.c_str(), str.c_str(), 1000, 1000);
	   canvas[i]->Divide(2, 2);
	   for (Int_t v = 0; v < 4; v++) {
		  canvas[i]->cd(v + 1);
		  DrawGraph2D(fHM->G2(string("hmf_") + names[v] + "_Graph2D_" + ToString<Int_t>(i)));
	   }
       SaveCanvasAsImage(canvas[i], fOutputDir);
   }
}

void CbmLitFieldQa::DrawFieldAlongZ()
{
	// Draw for different angles
   for (Int_t i = 0; i < fAlongZAngles.size(); i++) {
      string canvasName = string("field_qa_map_along_z_angle_") + ToString<Int_t>(fAlongZAngles[i]);
      TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 600);
      string s = ToString<Int_t>(i);
      DrawGraph(list_of(fHM->G1(string("hmf_BxAlongZAngle_Graph_") + s))(fHM->G1(string("hmf_ByAlongZAngle_Graph_") + s))
    		  (fHM->G1(string("hmf_BzAlongZAngle_Graph_") + s))(fHM->G1(string("hmf_ModAlongZAngle_Graph_") + s)),
            list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLinear, kLinear, true, 0.7, 0.5, 0.9, 0.3);
      gPad->SetGrid(true, true);
      SaveCanvasAsImage(c, fOutputDir);
   }
   // Draw for different XY positions
   for (Int_t i = 0; i < fAlongZXY.size(); i++){
      string canvasName = string("field_qa_map_along_z_xy_") + ToString<Int_t>(fAlongZXY[i].first) + "_" + ToString<Int_t>(fAlongZXY[i].second);
      TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 600);
      string s = ToString<Int_t>(i);
      DrawGraph(list_of(fHM->G1(string("hmf_BxAlongZXY_Graph_") + s))(fHM->G1(string("hmf_ByAlongZXY_Graph_") + s))
          		  (fHM->G1(string("hmf_BzAlongZXY_Graph_") + s))(fHM->G1(string("hmf_ModAlongZXY_Graph_") + s)),
                  list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLinear, kLinear, true, 0.7, 0.5, 0.9, 0.3);
      gPad->SetGrid(true, true);
      SaveCanvasAsImage(c, fOutputDir);
   }

   for (Int_t i = 0; i < fAlongZXY.size(); i++){
      std::stringstream ss;
      ss << "field_qa_map_along_z_xy_" << (Int_t)fAlongZXY[i].first << "_" << (Int_t)fAlongZXY[i].second<<"_integral";
      TCanvas* c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 600, 600);
      string s = ToString<Int_t>(i);
      fHM->G1(string("hmf_BxAlongZXYIntegral_Graph_") + s)->GetXaxis()->SetRangeUser(fMinZFieldInt - 2., fMaxZFieldInt + 2);
      DrawGraph(list_of(fHM->G1(string("hmf_BxAlongZXYIntegral_Graph_") + s))(fHM->G1(string("hmf_ByAlongZXYIntegral_Graph_") + s))
                  (fHM->G1(string("hmf_BzAlongZXYIntegral_Graph_") + s))(fHM->G1(string("hmf_ModAlongZXYIntegral_Graph_") + s)),
                  list_of("B_{x}")("B_{y}")("B_{z}")("|B|"), kLinear, kLinear, true, 0.7, 0.5, 0.9, 0.3);

      gPad->SetGrid(true, true);
      SaveCanvasAsImage(c, fOutputDir);
   }
}
ClassImp(CbmLitFieldQa);
