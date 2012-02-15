/**
* \file CbmRichReconstruction.cxx
*
* \author Semen Lebedev
* \date 2012
**/

#include "CbmRichReconstruction.h"
#include "CbmRichRing.h"

#include "CbmRichRingFinderHough.h"
#include "CbmRichRingFinderIdeal.h"

#include "CbmRichRingFitterCircle.h"
#include "CbmRichRingFitterTAU.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterRobustCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichRingFitterEllipseMinuit.h"

#include "FairHit.h"
#include "FairRootManager.h"
#include "CbmRichConverter.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

CbmRichReconstruction::CbmRichReconstruction()
: FairTask("CbmRichReconstruction"),
fRichHits(NULL),
fRichRings(NULL),
fRichProjections(NULL),
//fRichRingMatches(NULL),

fRingFinder(NULL),
fRingFitter(NULL),

fRunExtrapolation(true),
fRunProjection(true),
fRunFinder(true),
fRunFitter(true),
fRunTrackMatch(true),

fExtrapolationName(""),
fProjectionName(""),
fFinderName("hough"),
fFitterName("ellipse_tau"),
fTrackMatchName("")
{

}

CbmRichReconstruction::~CbmRichReconstruction()
{
   if (NULL != fRingFinder) delete fRingFinder;
   if (NULL != fRingFitter) delete fRingFitter;
}

InitStatus CbmRichReconstruction::Init()
{
   cout << "CbmRichReconstruction::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichReconstruction::Init","RootManager not instantised!"); }

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) { Fatal("CbmRichReconstruction::Init","No RichHit array!"); }

  // fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
  // if ( NULL == fRichRingMatches) { Fatal("CbmRichReconstruction::Init","No RichRingMatch array!"); }

   fRichProjections = (TClonesArray*) ioman->GetObject("RichProjection");
   //if (NULL == fRichProjections) { Fatal("CbmRichReconstruction::Init","No RichProjection array!"); }

   // fRichProjections = new TClonesArray("RichProjection", 600);
   //ioman->Register("RichRing", "RICH", fRichProjections, kTRUE);

   fRichRings = new TClonesArray("CbmRichRing", 100);
   ioman->Register("RichRing", "RICH", fRichRings, kTRUE);

   InitExtrapolation();
   InitProjection();
   InitFinder();
   InitFitter();
   InitTrackMatch();

   return kSUCCESS;
}

void CbmRichReconstruction::Exec(
      Option_t* opt)
{
   RunExtrapolation();
   RunProjection();
   RunFinder();
   RunFitter();
   RunTrackMatch();
}

void CbmRichReconstruction::InitExtrapolation()
{

}

void CbmRichReconstruction::InitProjection()
{

}

void CbmRichReconstruction::InitFinder()
{
   if (fFinderName == "hough"){
      fRingFinder = new CbmRichRingFinderHough();
   } else if (fFinderName == "ideal"){
      fRingFinder = new CbmRichRingFinderIdeal();
   } else {
      fRingFinder = new CbmRichRingFinderHough();
   }

   fRingFinder->Init();
}

void CbmRichReconstruction::InitFitter()
{
   if (fFitterName == "circle_cop"){
      fRingFitter = new CbmRichRingFitterCOP();
   } else if (fFitterName == "circle_simple") {
      fRingFitter = new CbmRichRingFitterCircle();
   } else if (fFitterName == "circle_tau") {
      fRingFitter = new CbmRichRingFitterTAU();
   } else if (fFitterName == "circle_robust_cop") {
      fRingFitter = new CbmRichRingFitterRobustCOP();
   } else if (fFitterName == "ellipse_tau") {
      fRingFitter = new CbmRichRingFitterEllipseTau();
   } else if (fFitterName == "ellipse_minuit") {
      fRingFitter = new CbmRichRingFitterEllipseMinuit();
   } else {
      fRingFitter = new CbmRichRingFitterCOP();
   }
}

void CbmRichReconstruction::InitTrackMatch()
{

}

void CbmRichReconstruction::RunExtrapolation()
{

}

void CbmRichReconstruction::RunProjection()
{

}

void CbmRichReconstruction::RunFinder()
{
   fRichRings->Clear();
   fRingFinder->DoFind(fRichHits, fRichProjections, fRichRings);
}

void CbmRichReconstruction::RunFitter()
{
   int nofRings = fRichRings->GetEntriesFast();
   for (int iRing = 0; iRing < nofRings; iRing++) {
      CbmRichRing* ring = (CbmRichRing*)fRichRings->At(iRing);
      if (NULL == ring) continue;
      CbmRichRingLight ringL;
      CbmRichConverter::CopyHitsToRingLight(ring, &ringL);
      fRingFitter->DoFit(&ringL);
      CbmRichConverter::CopyParamsToRing(&ringL, ring);
   }
}

void CbmRichReconstruction::RunTrackMatch()
{

}


void CbmRichReconstruction::Finish()
{

}

ClassImp(CbmRichReconstruction)
