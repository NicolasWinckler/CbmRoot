/**
 * \file CbmLitFitQaPTreeCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#include "CbmLitFitQaPTreeCreator.h"
#include "../base/CbmLitHistManager.h"
#include "TH1.h"
#include "TF1.h"

#include <cassert>

CbmLitFitQaPTreeCreator::CbmLitFitQaPTreeCreator()
{

}

CbmLitFitQaPTreeCreator::~CbmLitFitQaPTreeCreator()
{

}

ptree CbmLitFitQaPTreeCreator::Create(
      CbmLitHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   string parNames[] = {
         "ResX", "ResY", "ResTx", "ResTy", "ResQp",
         "PullX", "PullY", "PullTx", "PullTy", "PullQp"
   };
   string rpHistNames[] = {
         "hStsFirst", "hStsLast", "hTrdFirst", "hTrdLast","hMuchFirst", "hMuchLast"
   };
   string wHistNames[] = {
         "hStsFirstWrongCov", "hStsLastWrongCov", "hTrdFirstWrongCov", "hTrdLastWrongCov", "hMuchFirstWrongCov", "hMuchLastWrongCov"
   };

   for (unsigned int iHist = 0; iHist < 6; iHist++) {
      for (unsigned int iPar = 0; iPar < 10; iPar++) {
         ResAndPullToPtree(pt, rpHistNames[iHist] + parNames[iPar]);
      }

      for (unsigned int iPar = 0; iPar < 5; iPar++) {
         WrongCovToPtree(pt, wHistNames[iHist] + parNames[iPar]);
      }
   }

   return pt;
}

void CbmLitFitQaPTreeCreator::ResAndPullToPtree(
      ptree& pt,
      const string& histName)
{
   if (!fHM->Exists(histName)) return;
   TH1* hist = fHM->H1(histName);
   hist->Fit("gaus");
   TF1* fit = hist->GetFunction("gaus");
   double sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
   pt.put(histName + ".mean", hist->GetMean());
   pt.put(histName + ".rms", hist->GetRMS());
   pt.put(histName + ".sigma", sigma);
}

void CbmLitFitQaPTreeCreator::WrongCovToPtree(
      ptree& pt,
      const string& histName)
{
   if (!fHM->Exists(histName)) return;
   TH1* hist = fHM->H1(histName);
   pt.put(histName + ".entries", hist->GetEntries());
}
