/**
 * \file CbmRichFitRings.cxx
 *
 * \author Supriya Das <s.lebedev@gsi.de>
 * \date 2006
 **/

#include "CbmRichFitRings.h"
#include "CbmRichRingFitter.h"
#include "CbmRichRing.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


CbmRichFitRings::CbmRichFitRings(
      CbmRichRingFitter* fitter)
:FairTask("CbmRichFitRings")
{
   fFitter = fitter;
   fRingArray = NULL;
}

CbmRichFitRings::~CbmRichFitRings()
{

}

InitStatus CbmRichFitRings::Init()
{
   if (NULL == fFitter){ Fatal("CbmRichFitRings::Init", "No fitter selected");}

   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichFitRings::Init", "RootManager not instantised!"); }

   fRingArray  = (TClonesArray*) ioman->GetObject("RichRing");
   if ( NULL == fRingArray) { Fatal("CbmRichFitRings::Init", "No RichRing array"); }

   fFitter->Init();

   return kSUCCESS;
}

void CbmRichFitRings::Exec(
      Option_t* opt)
{
   if ( NULL == fRingArray ) {
      cout << "-E- CbmRichFitRings::Exec: No Ring Array" << endl;
      return;
   }

   Int_t nofRings = fRingArray->GetEntriesFast();
   for (Int_t iRing=0; iRing < nofRings; iRing++) {
      CbmRichRing* ring = (CbmRichRing*)fRingArray->At(iRing);
      if (NULL == ring) continue;
      fFitter->DoFit(ring);
  }
}

void CbmRichFitRings::Finish()
{

}

ClassImp(CbmRichFitRings)
