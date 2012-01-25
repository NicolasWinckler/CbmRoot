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

#include <boost/property_tree/json_parser.hpp>
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

   CreateSimulationReport();
}

void CbmLitTrackingQa::CreateSimulationReport()
{
   CbmLitTrackingQaPTreeCreator ptc;
   boost::property_tree::ptree qa = ptc.Create(fHM);
   if (fOutputDir != "") { write_json(std::string(fOutputDir + "rec_qa.json").c_str(), qa); }

   CbmLitTrackingQaReport report(kLitText);
   report.Create(cout, &qa, NULL, NULL);

   boost::property_tree::ptree ideal, check;
   std::string qaIdealFile = std::string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/rec_qa_ideal.json");
   read_json(qaIdealFile.c_str(), ideal);

   CbmLitResultChecker qaChecker;
   qaChecker.DoCheck(qa, ideal, check);
   if (fOutputDir != "") { write_json(std::string(fOutputDir + "rec_qa_check.json").c_str(), check); }

   if (fOutputDir != "") {
      ofstream foutHtml(string(fOutputDir + "rec_qa.html").c_str());
      CbmLitTrackingQaReport reportHtml(kLitHtml);
      reportHtml.Create(foutHtml, &qa, NULL, NULL);

      ofstream foutLatex(string(fOutputDir + "rec_qa.tex").c_str());
      CbmLitTrackingQaReport reportLatex(kLitLatex);
      reportLatex.Create(foutLatex, &qa, NULL, NULL);
   }
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

void CbmLitTrackingQa::CreateSummaryReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CbmLitTrackingQaStudyReport report(kLitHtml);
   string fileType = ".html";
//   if (reportType == kLitHtml) fileType = ".html";
//   if (reportType == kLitText) fileType = ".txt";
   ofstream fout(string(fOutputDir + "/rec_qa_study" + fileType).c_str());
   report.Create(fout, resultDirectories, studyNames);
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
