/**
 * \file CbmLitTrackingQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */

#include "CbmLitTrackingQa.h"
#include "CbmLitTrackingQaHistCreator.h"
#include "CbmLitTrackingQaCalculator.h"
#include "CbmLitTrackingQaStudyReport.h"
#include "CbmLitTrackingQaReport.h"
#include "qa/base/CbmLitResultChecker.h"
#include "CbmHistManager.h"

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
   fUseConsecutivePointsInSts(true),
   fPRangeMin(0.),
   fPRangeMax(10.),
   fPRangeBins(20.),
   fYRangeMin(0.),
   fYRangeMax(4.),
   fYRangeBins(16),
   fPtRangeMin(0.),
   fPtRangeMax(3.),
   fPtRangeBins(12.)
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
   fHM = new CbmHistManager();

   fDet.DetermineSetup();

   // Create histograms
   CbmLitTrackingQaHistCreator* hc = CbmLitTrackingQaHistCreator::Instance();
   hc->SetPRange(fPRangeBins, fPRangeMin, fPRangeMax);
   hc->SetPtRange(fPtRangeBins, fPtRangeMin, fPtRangeMax);
   hc->SetYRange(fYRangeBins, fYRangeMin, fYRangeMax);
   hc->Create(fHM);

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
   CreateSimulationReport();
}

void CbmLitTrackingQa::CreateSimulationReport()
{
   CbmSimulationReport* report = new CbmLitTrackingQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitTrackingQa::CreateStudyReport(
      const string& title,
      const vector<string>& fileNames,
      const vector<string>& studyNames)
{
   CbmStudyReport* report = new CbmLitTrackingQaStudyReport();
   report->Create(fileNames, studyNames, fOutputDir);
   delete report;
}

ClassImp(CbmLitTrackingQa);
