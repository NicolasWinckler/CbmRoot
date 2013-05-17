/**
 * \file CbmSimulationReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmSimulationReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "draw/CbmDrawHist.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>
using std::cout;
using std::ofstream;
using std::string;

CbmSimulationReport::CbmSimulationReport():
    CbmReport(),
	fHM(NULL)
{

}

CbmSimulationReport::~CbmSimulationReport()
{

}

void CbmSimulationReport::Create(
      CbmHistManager* histManager,
      const string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   SetOutputDir(outputDir);
   CreateReports();
}

void CbmSimulationReport::Create(
      const string& fileName,
      const string& outputDir)
{
   assert(fHM == NULL);
   fHM = new CbmHistManager();
   TFile* file = new TFile(fileName.c_str());
   fHM->ReadFromFile(file);
   SetOutputDir(outputDir);
   CreateReports();
//   delete fHM;
//   delete file;
}

void CbmSimulationReport::DrawH1ByPattern(
      const string& histNamePattern)
{
   vector<TH1*> histos = HM()->H1Vector(histNamePattern);
   if (histos.size() == 0) return;
   Int_t nofHistos = histos.size();
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      TH1* hist = histos[iHist];
      string canvasName = GetReportName() + hist->GetName();
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
      DrawH1(hist, kLinear, kLinear);
   }
}

void CbmSimulationReport::DrawH2ByPattern(
      const string& histNamePattern)
{
    vector<TH2*> histos = HM()->H2Vector(histNamePattern);
    if (histos.size() == 0) return;
    Int_t nofHistos = histos.size();
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
        TH2* hist = histos[iHist];
        string canvasName = GetReportName() + hist->GetName();
        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
        DrawH2(hist, kLinear, kLinear, kLinear);
    }
}

ClassImp(CbmSimulationReport)
