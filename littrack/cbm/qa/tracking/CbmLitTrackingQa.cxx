/** CbmLitTrackingQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 **/

#include "qa/tracking/CbmLitTrackingQa.h"
#include "qa/tracking/CbmLitTrackingQaHistCreator.h"
#include "qa/tracking/CbmLitTrackingQaCalculator.h"
#include "qa/tracking/CbmLitTrackingQaStudyReport.h"
#include "qa/tracking/CbmLitTrackingQaDraw.h"
#include "qa/tracking/CbmLitTrackingQaPTreeCreator.h"
#include "qa/tracking/CbmLitTrackingQaReport.h"
#include "qa/base/CbmLitResultChecker.h"
#include "qa/base/CbmLitHistManager.h"

#include "TSystem.h"

#include <fstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using std::cout;
using boost::property_tree::ptree;

CbmLitTrackingQa::CbmLitTrackingQa():
   FairTask("LitTrackingQA", 1),
   fTrackingQa(NULL),
   fHM(NULL),
   fOutputDir(""),
   fDet(),
   fMinNofPointsSts(4),
   fMinNofPointsTrd(8),
   fMinNofPointsMuch(10),
   fMinNofPointsTof(1),
   fMinNofHitsRich(7),
   fQuota(0.7),
   fQuotaRich(0.6),
   fMinNofHitsTrd(8),
   fMinNofHitsMuch(10),
   fUseConsecutivePointsInSts(true)
{
}

CbmLitTrackingQa::~CbmLitTrackingQa()
{
   if (fTrackingQa) delete fTrackingQa;
   if (fHM) delete fHM;
}

InitStatus CbmLitTrackingQa::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmLitHistManager();

   fDet.DetermineSetup();

   // Create histograms
   CbmLitTrackingQaHistCreator histCreator;
   histCreator.Create(fHM);

   fTrackingQa = new CbmLitTrackingQaCalculator(fHM);
   fTrackingQa->Init();
   fTrackingQa->SetMinNofPointsSts(fMinNofPointsSts);
   fTrackingQa->SetMinNofPointsTrd(fMinNofPointsTrd);
   fTrackingQa->SetMinNofPointsMuch(fMinNofPointsMuch);
   fTrackingQa->SetMinNofPointsTof(fMinNofPointsTof);
   fTrackingQa->SetMinNofHitsRich(fMinNofHitsRich);
   fTrackingQa->SetQuota(fQuota);
   fTrackingQa->SetQuotaRich(fQuotaRich);
   fTrackingQa->SetMinNofHitsTrd(fMinNofHitsTrd);
   fTrackingQa->SetMinNofHitsMuch(fMinNofHitsMuch);
   fTrackingQa->SetUseConsecutivePointsInSts(fUseConsecutivePointsInSts);

   return kSUCCESS;
}

void CbmLitTrackingQa::Exec(
   Option_t* opt)
{
   fTrackingQa->Exec();
}

void CbmLitTrackingQa::Finish()
{
   fTrackingQa->Finish();
   fHM->WriteToFile();

   CbmLitTrackingQaDraw drawQa;
   drawQa.Draw(fHM, fOutputDir);

   string qaFile = fOutputDir + "/tracking_qa.json";
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/tracking_qa_ideal.json");
   string checkFile = fOutputDir + "/tracking_qa_check.json";

   CbmLitTrackingQaPTreeCreator ptc;
   ptree qa = ptc.Create(fHM);
   if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qaFile, idealFile, checkFile);

   CreateSimulationReport("Tracking QA", fOutputDir);
}

void CbmLitTrackingQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmLitSimulationReport* report = new CbmLitTrackingQaReport();
   report->SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/tracking_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/tracking_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/tracking_qa.txt").c_str());
   report->Create(kLitText, cout, resultDirectory);
   report->Create(kLitHtml, foutHtml, resultDirectory);
   report->Create(kLitLatex, foutLatex, resultDirectory);
   report->Create(kLitText, foutText, resultDirectory);
   delete report;
}

void CbmLitTrackingQa::CreateStudyReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CbmLitStudyReport* report = new CbmLitTrackingQaStudyReport();
   report->SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/tracking_qa_study.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/tracking_qa_study.tex").c_str());
   ofstream foutText(string(fOutputDir + "/tracking_qa_study.txt").c_str());
   //report->Create(kLitText, cout, resultDirectories, studyNames);
   report->Create(kLitHtml, foutHtml, resultDirectories, studyNames);
   report->Create(kLitLatex, foutLatex, resultDirectories, studyNames);
   report->Create(kLitText, foutText, resultDirectories, studyNames);
   delete report;
}

void CbmLitTrackingQa::DrawHistosFromFile(
      const std::string& fileName)
{
   // read histograms from file
   fHM = new CbmLitHistManager();
   TFile* file = new TFile(fileName.c_str());
   fHM->ReadFromFile(file);
   // recalculate efficiency histograms
   fTrackingQa = new CbmLitTrackingQaCalculator(fHM);
   fTrackingQa->CalculateEfficiencyHistos();
   // draw histograms
   CbmLitTrackingQaDraw drawQa;
   drawQa.Draw(fHM, "");
}

ClassImp(CbmLitTrackingQa);
