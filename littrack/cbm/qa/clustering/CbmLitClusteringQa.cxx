/**
 * \file CbmLitClusteringQa.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitClusteringQa.h"
#include "CbmLitClusteringQaCalculator.h"
#include "CbmLitClusteringQaHistCreator.h"
#include "CbmLitClusteringQaReport.h"
#include "CbmLitClusteringQaStudyReport.h"
#include "CbmHistManager.h"
#include "../base/CbmLitResultChecker.h"

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>

#include <fstream>
using std::cout;

CbmLitClusteringQa::CbmLitClusteringQa():
   fHM(NULL),
   fClusteringQa(NULL),
   fOutputDir("")
{

}

CbmLitClusteringQa::~CbmLitClusteringQa()
{
   if (fHM) delete fHM;
   if (fClusteringQa) delete fClusteringQa;
}

InitStatus CbmLitClusteringQa::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   // Create histograms
   CbmLitClusteringQaHistCreator histCreator;
   histCreator.Create(fHM);

   fClusteringQa = new CbmLitClusteringQaCalculator(fHM);
   fClusteringQa->Init();

   return kSUCCESS;
}

void CbmLitClusteringQa::Exec(
    Option_t* opt)
{
   fClusteringQa->Exec();
}

void CbmLitClusteringQa::Finish()
{
   fClusteringQa->Finish();
   fHM->WriteToFile();
   CreateSimulationReport("Clustering QA", fOutputDir);
}

void CbmLitClusteringQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmSimulationReport* report = new CbmLitClusteringQaReport();
   report->Create(fHM, resultDirectory);
   delete report;
}

void CbmLitClusteringQa::CreateStudyReport(
      const string& title,
      const vector<string>& fileNames,
      const vector<string>& studyNames)
{
   CbmStudyReport* report = new CbmLitClusteringQaStudyReport();
   report->Create(fileNames, studyNames, fOutputDir);
   delete report;
}

ClassImp(CbmLitClusteringQa);
