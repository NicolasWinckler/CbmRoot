/**
 * \file CbmStsSimulationQa.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#include "CbmStsSimulationQa.h"
#include "CbmStsSimulationQaHistCreator.h"
#include "CbmStsSimulationQaCalculator.h"
#include "CbmStsSimulationQaDraw.h"
#include "CbmStsSimulationQaPTreeCreator.h"
#include "CbmStsSimulationQaReport.h"
#include "CbmStsSimulationQaStudyReport.h"
#include "../base/CbmLitResultChecker.h"
#include "CbmHistManager.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"

#include "TH1.h"
#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <iostream>

using std::stringstream;
using std::cout;

CbmStsSimulationQa::CbmStsSimulationQa():
		FairTask(),
		fHM(NULL),
		fStsSimulationQa(NULL),
		fOutputDir("./test/")
{

}

CbmStsSimulationQa::~CbmStsSimulationQa()
{
	if (fHM) delete fHM;
	if (fStsSimulationQa) delete fStsSimulationQa;
}

InitStatus CbmStsSimulationQa::Init()
{
	// Create histogram manager which is used throughout the program
	fHM = new CbmHistManager();

	// Create histograms
	CbmStsSimulationQaHistCreator histCreator;
	histCreator.Create(fHM);

	fStsSimulationQa = new CbmStsSimulationQaCalculator(fHM);
	fStsSimulationQa->Init();

	return kSUCCESS;
}

void CbmStsSimulationQa::Exec(Option_t* opt)
{
	fStsSimulationQa->Exec();
}

void CbmStsSimulationQa::Finish() {
	fStsSimulationQa->Finish();

    gDirectory->mkdir("StsSimulationQa");
    gDirectory->cd("StsSimulationQa");
    fHM->WriteToFile();
    gDirectory->cd("..");

    CbmStsSimulationQaDraw drawQa;
    drawQa.Draw(fHM, fOutputDir);

    string qaFile = fOutputDir + "/sts_simulation_qa.json";
    string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + "/littrack/cbm/qa/sts/sts_simulation_qa_ideal.json";
    string checkFile = fOutputDir + "/sts_simulation_qa_check.json";

    CbmStsSimulationQaPTreeCreator ptCreator;
    ptree qa = ptCreator.Create(fHM);
    if (fOutputDir != "") { write_json(qaFile.c_str(), qa); }

    CbmLitResultChecker qaChecker;
    qaChecker.DoCheck(qaFile, idealFile, checkFile);

    CreateSimulationReport("STS Simulation QA", fOutputDir);
}

void CbmStsSimulationQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmSimulationReport* report = new CbmStsSimulationQaReport();
   report->SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/sts_simulation_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/sts_simulation_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/sts_simulation_qa.txt").c_str());
   report->Create(kTextReport, cout, fOutputDir);
   report->Create(kHtmlReport, foutHtml, fOutputDir);
   report->Create(kLatexReport, foutLatex, fOutputDir);
   report->Create(kTextReport, foutText, fOutputDir);
   delete report;
}

void CbmStsSimulationQa::CreateStudyReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CbmStudyReport* report = new CbmStsSimulationQaStudyReport();
   report->SetTitle(title);
   ofstream foutHtml(string(fOutputDir + "/sts_simulation_qa.html").c_str());
   ofstream foutLatex(string(fOutputDir + "/sts_simulation_qa.tex").c_str());
   ofstream foutText(string(fOutputDir + "/sts_simulation_qa.txt").c_str());
//   report->Create(kLitText, cout, resultDirectories, studyNames);
   report->Create(kHtmlReport, foutHtml, resultDirectories, studyNames);
   report->Create(kLatexReport, foutLatex, resultDirectories, studyNames);
   report->Create(kTextReport, foutText, resultDirectories, studyNames);
   delete report;
}


//// -------   GetGeometry   --------------------------------------------------
//Bool_t CbmStsSimulationQa::GetGeometry() {
//
//  fNofStations = 0;
//
//  // Get STS node
//  TGeoNode* sts = NULL;
//  gGeoManager->CdTop();
//  TGeoNode* cave = gGeoManager->GetCurrentNode();
//  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
//    TGeoNode* node = cave->GetDaughter(iNode);
//     TString name = node->GetName();
//     if ( name.Contains("STS") ) {
//       sts = node;
//       gGeoManager->CdDown(iNode);
//       break;
//     }
//   }
//   if ( ! sts ) {
//       fLogger->Error(MESSAGE_ORIGIN, "No top STS node");
//     return kFALSE;
//   }
//
//   // Loop over stations in STS
//   for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {
//     gGeoManager->CdDown(iNode);
//     TGeoNode* station = gGeoManager->GetCurrentNode();
//     TString statName = station->GetName();
//     if ( ! statName.Contains("Station") ) {
//       gGeoManager->CdUp();
//       continue;
//     }
//     Int_t statNr = station->GetNumber();
//     Double_t* statTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
//     fStatZ[statNr] = statTrans[2];
//     fNofStations++;
//     gGeoManager->CdUp();
//   }
//
//   fLogger->Info(MESSAGE_ORIGIN, "Found %d stations at position:", fNofStations);
//   for (Int_t iStat = 1; iStat <= fNofStations; iStat++)
//     fLogger->Info(MESSAGE_ORIGIN, "  Station %i, z = %f cm",
//                   iStat, fStatZ[iStat]);
//
//   return kTRUE;
//}
//// --------------------------------------------------------------------------
ClassImp(CbmStsSimulationQa);
