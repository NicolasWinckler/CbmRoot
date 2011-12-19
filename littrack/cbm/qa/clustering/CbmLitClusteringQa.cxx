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
#include "../base/CbmLitHistManager.h"

CbmLitClusteringQa::CbmLitClusteringQa():
   fHM(NULL),
   fClusteringQa(NULL)
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
   CbmLitClusteringQaPTreeCreator ptCreator;
   boost::property_tree::ptree pt = ptCreator.Create(fHM);

   CbmLitClusteringQaReport report(kLitText);
   report.Create(std::cout, &pt, NULL, NULL);

   fClusteringQa->Finish();
}

ClassImp(CbmLitClusteringQa);
