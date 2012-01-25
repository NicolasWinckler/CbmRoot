/**
 * \file CbmLitClusteringQa.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitClusteringQa.h"
#include "CbmLitClusteringQaCalculator.h"
#include "CbmLitClusteringQaHistCreator.h"
#include "CbmLitClusteringQaPTreeCreator.h"
#include "CbmLitClusteringQaReport.h"
#include "CbmLitClusteringQaStudyReport.h"
#include "CbmLitClusteringQaDraw.h"
#include "../base/CbmLitHistManager.h"
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
   fHM = new CbmLitHistManager();

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

   CbmLitClusteringQaDraw drawQa;
   drawQa.Draw(fHM, fOutputDir);

   string qaFile = fOutputDir + "/clustering_qa.json";
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + "/littrack/cbm/qa/clustering/clustering_qa_ideal.json";
   string checkFile = fOutputDir + "/clustering_qa_check.json";

   CbmLitClusteringQaPTreeCreator ptCreator;
   ptree qa = ptCreator.Create(fHM);
   if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qaFile, idealFile, checkFile);

   CreateSimulationReport("Clustering QA", fOutputDir);
}

void CbmLitClusteringQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmLitClusteringQaReport report;
   report.SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/clustering_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/clustering_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/clustering_qa.txt").c_str());
   report.Create(kLitText, cout, fOutputDir);
   report.Create(kLitHtml, foutHtml, fOutputDir);
   report.Create(kLitLatex, foutLatex, fOutputDir);
   report.Create(kLitText, foutText, fOutputDir);
}

void CbmLitClusteringQa::CreateStudyReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CbmLitClusteringQaStudyReport report;
   report.SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/clustering_qa_study.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/clustering_qa_study.tex").c_str());
   ofstream foutText(string(fOutputDir + "/clustering_qa_study.txt").c_str());
   report.Create(kLitText, cout, resultDirectories, studyNames);
   report.Create(kLitHtml, foutHtml, resultDirectories, studyNames);
   report.Create(kLitLatex, foutLatex, resultDirectories, studyNames);
   report.Create(kLitText, foutText, resultDirectories, studyNames);
}
ClassImp(CbmLitClusteringQa);
