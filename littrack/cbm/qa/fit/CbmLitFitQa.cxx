/**
 * \file CbmLitFitQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFitQa.h"
#include "CbmLitFitQaHistCreator.h"
#include "CbmLitFitQaPTreeCreator.h"
#include "CbmLitFitQaDraw.h"
#include "CbmLitFitQaReport.h"
#include "CbmLitFitQaCalculator.h"
#include "../base/CbmHistManager.h"
#include "../base/CbmLitResultChecker.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

CbmLitFitQa::CbmLitFitQa():
   fIsFixedBounds(true),
   fMvdMinNofHits(0),
   fStsMinNofHits(0),
   fTrdMinNofHits(0),
   fMuchMinNofHits(0),
   fOutputDir("./test/"),
   fHM(NULL),
   fFitQa(NULL)
{

}

CbmLitFitQa::~CbmLitFitQa()
{
   if (fHM) delete fHM;
   if (fFitQa) delete fFitQa;
}

InitStatus CbmLitFitQa::Init()
{
   fHM = new CbmHistManager();

   // Create histograms
   CbmLitFitQaHistCreator histCreator;
   histCreator.Create(fHM);

   fFitQa = new CbmLitFitQaCalculator(fHM);
   fFitQa->SetMvdMinNofHits(fMvdMinNofHits);
   fFitQa->SetStsMinNofHits(fStsMinNofHits);
   fFitQa->SetMuchMinNofHits(fMuchMinNofHits);
   fFitQa->SetTrdMinNofHits(fTrdMinNofHits);
   fFitQa->Init();

   return kSUCCESS;
}

void CbmLitFitQa::Exec(
   Option_t* opt)
{
   fFitQa->Exec();
}

void CbmLitFitQa::Finish()
{
   fHM->WriteToFile();
   fFitQa->Finish();

   CbmLitFitQaDraw drawQa;
   drawQa.Draw(fHM, fOutputDir);

   string qaFile = fOutputDir + "/fit_qa.json";
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/fit/fit_qa_ideal.json");
   string checkFile = fOutputDir + "/fit_qa_check.json";

   CbmLitFitQaPTreeCreator ptc;
   ptree qa = ptc.Create(fHM);
   if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qaFile, idealFile, checkFile);

   CreateSimulationReport("Fit QA", fOutputDir);
}

void CbmLitFitQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmSimulationReport* report = new CbmLitFitQaReport();
   report->SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/fit_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/fit_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/fit_qa.txt").c_str());
//   report.Create(kTextReport, cout, resultDirectory);
   report->Create(kHtmlReport, foutHtml, resultDirectory);
   report->Create(kLatexReport, foutLatex, resultDirectory);
   report->Create(kTextReport, foutText, resultDirectory);
   delete report;
}


ClassImp(CbmLitFitQa)
