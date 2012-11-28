/**
 * \file CbmSimulationReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmSimulationReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
using std::cout;
using std::ofstream;

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
   delete fHM;
   delete file;
}

ClassImp(CbmSimulationReport)
