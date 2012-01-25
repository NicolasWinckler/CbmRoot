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
   fDet()
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
   CbmLitTrackingQaReport report;
   report.SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/tracking_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/tracking_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/tracking_qa.txt").c_str());
   report.Create(kLitText, cout, resultDirectory);
   report.Create(kLitHtml, foutHtml, resultDirectory);
   report.Create(kLitLatex, foutLatex, resultDirectory);
   report.Create(kLitText, foutText, resultDirectory);
}

void CbmLitTrackingQa::CreateStudyReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CbmLitTrackingQaStudyReport report;
   report.SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/tracking_qa_study.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/tracking_qa_study.tex").c_str());
   ofstream foutText(string(fOutputDir + "/tracking_qa_study.txt").c_str());
   report.Create(kLitText, cout, resultDirectories, studyNames);
   report.Create(kLitHtml, foutHtml, resultDirectories, studyNames);
   report.Create(kLitLatex, foutLatex, resultDirectories, studyNames);
   report.Create(kLitText, foutText, resultDirectories, studyNames);
}

void CbmLitTrackingQa::SetMinNofPointsSts(Int_t minNofPointsSts)
{
//   fTrackingQa->SetMinNofPointsSts(minNofPointsSts);
}

void CbmLitTrackingQa::SetMinNofPointsTrd(Int_t minNofPointsTrd)
{
//   fTrackingQa->SetMinNofPointsTrd(minNofPointsTrd);
}

void CbmLitTrackingQa::SetMinNofPointsMuch(Int_t minNofPointsMuch)
{
//   fTrackingQa->SetMinNofPointsMuch(minNofPointsMuch);
}

void CbmLitTrackingQa::SetMinNofPointsTof(Int_t minNofPointsTof)
{
//   fTrackingQa->SetMinNofPointsTof(minNofPointsTof);
}

void CbmLitTrackingQa::SetQuota(Double_t quota)
{
//   fTrackingQa->SetQuota(quota);
}

void CbmLitTrackingQa::SetMinNofHitsRich(Int_t minNofHits)
{
//   fTrackingQa->SetMinNofHitsRich(minNofHits);
}

void CbmLitTrackingQa::SetQuotaRich(Double_t quota)
{
//   fTrackingQa->SetQuotaRich(quota);
}

void CbmLitTrackingQa::SetMinNofHitsTrd(Int_t minNofHitsTrd)
{
//   fTrackingQa->SetMinNofHitsTrd(minNofHitsTrd);
}

void CbmLitTrackingQa::SetMinNofHitsMuch(Int_t minNofHitsMuch)
{
//   fTrackingQa->SetMinNofHitsMuch(minNofHitsMuch);
}

void CbmLitTrackingQa::SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts)
{
//   fTrackingQa->SetUseConsecutivePointsInSts(useConsecutivePointsInSts);
}

void CbmLitTrackingQa::DrawHistosFromFile(
      const std::string& fileName)
{
//   TFile* file = new TFile(fileName.c_str());
//   CreateHistos(file);
//   CalculateEfficiencyHistos();
//   Draw();
}

ClassImp(CbmLitTrackingQa);
