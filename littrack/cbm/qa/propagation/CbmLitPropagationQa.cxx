/**
 * \file CbmLitPropagationQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 **/
#include "CbmLitPropagationQa.h"
#include "CbmLitPropagationQaReport.h"
#include "CbmLitPropagationQaPTreeCreator.h"
#include "CbmLitPropagationQaHistCreator.h"
#include "CbmLitPropagationQaDraw.h"
#include "CbmLitPropagationQaCalculator.h"
#include "qa/base/CbmLitHistManager.h"
#include "qa/base/CbmLitResultChecker.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

using boost::property_tree::ptree;
using boost::property_tree::write_json;

CbmLitPropagationQa::CbmLitPropagationQa():
   fHM(NULL),
   fOutputDir(""),
   fFixedBounds(true),
   fNofPlanes(10),
   fPropagationQa(NULL)
{
}

CbmLitPropagationQa::~CbmLitPropagationQa()
{
   if (fPropagationQa) delete fPropagationQa;
   if (fHM != NULL) delete fHM;
}

InitStatus CbmLitPropagationQa::Init()
{
   fHM = new CbmLitHistManager();

   CbmLitPropagationQaHistCreator histCreator;
   histCreator.SetFixedBounds(fFixedBounds);
   histCreator.SetNofPlanes(fNofPlanes);
   histCreator.Create(fHM);

   // TODO Aet all parameters of CbmLitPropagationQaCalculator
   fPropagationQa = new CbmLitPropagationQaCalculator(fHM);
   fPropagationQa->SetNofPlanes(fNofPlanes);
   fPropagationQa->Init();

   return kSUCCESS;
}

void CbmLitPropagationQa::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitPropagationQa::Exec(
   Option_t* opt)
{
   fPropagationQa->Exec();
   static Int_t eventNo = 0;
   std::cout << "CbmLitPropagationQa::Exec: event=" << eventNo++ << std::endl;
}

void CbmLitPropagationQa::Finish()
{
   fPropagationQa->Finish();
   fHM->WriteToFile();

   string qaFile = fOutputDir + "/propagation_qa.json";
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/propagation/propagation_qa_ideal.json");
   string checkFile = fOutputDir + "/propagation_qa_check.json";

   CbmLitPropagationQaPTreeCreator ptc;
   ptree qa = ptc.Create(fHM);
   if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qaFile, idealFile, checkFile);

   CbmLitPropagationQaDraw draw;
   draw.SetNofPlanes(fNofPlanes);
   draw.Draw(fHM, fOutputDir);

   CreateSimulationReport("Propagation QA", fOutputDir);
}

void CbmLitPropagationQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmLitSimulationReport* report = new CbmLitPropagationQaReport();
   report->SetTitle(title);
   ((CbmLitPropagationQaReport*)report)->SetNofPlanes(fNofPlanes);
   ofstream foutHtml(string(fOutputDir + "/propagation_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/propagation_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/propagation_qa.txt").c_str());
//   report.Create(kLitText, cout, resultDirectory);
   report->Create(kLitHtml, foutHtml, resultDirectory);
   report->Create(kLitLatex, foutLatex, resultDirectory);
   report->Create(kLitText, foutText, resultDirectory);
   delete report;
}

ClassImp(CbmLitPropagationQa);
