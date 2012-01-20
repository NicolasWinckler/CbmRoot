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
#include "CbmLitClusteringQaDraw.h"
#include "../base/CbmLitHistManager.h"

#include <fstream>

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
   CbmLitClusteringQaPTreeCreator ptCreator;
   boost::property_tree::ptree qa = ptCreator.Create(fHM);

   CbmLitClusteringQaDraw drawQa;
   drawQa.Draw(fHM, fOutputDir);

   CbmLitClusteringQaReport report(kLitText);
   report.Create(std::cout, &qa, NULL, NULL);

   if (fOutputDir != "") {
      ofstream foutHtml(string(fOutputDir + "clustering_qa.html").c_str());
      CbmLitClusteringQaReport reportHtml(kLitHtml);
      reportHtml.Create(foutHtml, &qa, NULL, NULL);

      ofstream foutLatex(string(fOutputDir + "clustering_qa.tex").c_str());
      CbmLitClusteringQaReport reportLatex(kLitLatex);
      reportLatex.Create(foutLatex, &qa, NULL, NULL);
   }

   fClusteringQa->Finish();
   fHM->WriteToFile();
}

ClassImp(CbmLitClusteringQa);
