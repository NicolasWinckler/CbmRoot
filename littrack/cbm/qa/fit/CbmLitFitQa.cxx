/**
 * \file CbmLitFitQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFitQa.h"
#include "CbmLitFitQaHistCreator.h"
#include "CbmLitFitQaReport.h"
#include "CbmLitFitQaCalculator.h"
#include "CbmHistManager.h"
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
   fFitQa(NULL),
   fPRangeMin(0.),
   fPRangeMax(10.),
   fPRangeBins(20)
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
   histCreator.SetPRange(fPRangeBins, fPRangeMin, fPRangeMax);
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
   CreateSimulationReport("Fit QA", fOutputDir);
}

void CbmLitFitQa::CreateSimulationReport(
      const string& title,
      const string& resultDirectory)
{
   CbmSimulationReport* report = new CbmLitFitQaReport();
   report->Create(fHM, resultDirectory);
   delete report;
}

ClassImp(CbmLitFitQa)
