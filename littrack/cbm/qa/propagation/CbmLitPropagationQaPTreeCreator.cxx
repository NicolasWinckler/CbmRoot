/**
 * \file CbmLitPropagationQaPTreeCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#include "CbmLitPropagationQaPTreeCreator.h"
#include "../base/CbmLitHistManager.h"
#include "TH1.h"
#include "TF1.h"

#include <cassert>

CbmLitPropagationQaPTreeCreator::CbmLitPropagationQaPTreeCreator()
{

}

CbmLitPropagationQaPTreeCreator::~CbmLitPropagationQaPTreeCreator()
{

}

ptree CbmLitPropagationQaPTreeCreator::Create(
      CbmLitHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   ptree pt;

   string parNames[] = {
         "ResX", "ResY", "ResTx", "ResTy", "ResQp",
         "PullX", "PullY", "PullTx", "PullTy", "PullQp"
   };

   for (UInt_t iPar = 0; iPar < 10; iPar++) {
      vector<TH1*> histos = fHM->H1Vector(parNames[iPar]);
      for (UInt_t iHist = 0; iHist < histos.size(); iHist++) {
         TH1* hist = histos[iHist];
         ResAndPullToPtree(pt, hist->GetName());
      }
   }

   return pt;
}

void CbmLitPropagationQaPTreeCreator::ResAndPullToPtree(
      ptree& pt,
      const string& histName)
{
   if (!fHM->Exists(histName)) return;
   TH1* hist = fHM->H1(histName);
   hist->Fit("gaus");
   TF1* fit = hist->GetFunction("gaus");
   Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
   pt.put(histName + ".mean", hist->GetMean());
   pt.put(histName + ".rms", hist->GetRMS());
   pt.put(histName + ".sigma", sigma);
}
