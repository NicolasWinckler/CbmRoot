/**
* \file CbmRichReconstruction.cxx
*
* \author Semen Lebedev
* \date 2012
**/

#include "CbmRichReconstruction.h"
#include "CbmRichRing.h"

#include "CbmRichProjectionProducer.h"

#include "CbmRichTrackExtrapolationBase.h"
#include "CbmRichTrackExtrapolationIdeal.h"
#include "CbmRichTrackExtrapolationMirrorIdeal.h"
#include "CbmRichTrackExtrapolationKF.h"

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
   fRichTrackParamZ(NULL),
   fGlobalTracks(NULL),

   fRingFinder(NULL),
   fRingFitter(NULL),
   fTrackExtrapolation(NULL),
   fProjectionProducer(NULL),

   fRunExtrapolation(true),
   fRunProjection(true),
   fRunFinder(true),
   fRunFitter(true),
   fRunTrackMatch(true),

   fExtrapolationName("kf"),
   fProjectionName(""),
   fFinderName("hough"),
   fFitterName("ellipse_tau"),
   fTrackMatchName(""),

   fZTrackExtrapolation(300.)
{
   fProjectionProducer = new CbmRichProjectionProducer(1);
}

CbmRichReconstruction::~CbmRichReconstruction()
{
   if (NULL != fRingFinder) delete fRingFinder;
   if (NULL != fRingFitter) delete fRingFitter;
   if (NULL != fTrackExtrapolation) delete fTrackExtrapolation;
   if (NULL != fProjectionProducer) delete fProjectionProducer;
}

void CbmRichReconstruction::SetParContainers()
{
   fProjectionProducer->SetParContainers();
}

InitStatus CbmRichReconstruction::Init()
{
   cout << "CbmRichReconstruction::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichReconstruction::Init","RootManager not instantised!"); }

   fRichTrackParamZ = new TClonesArray("FairTrackParam",100);
   ioman->Register("RichTrackParamZ", "RICH", fRichTrackParamZ, kFALSE);

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if ( NULL == fGlobalTracks) { Fatal("CbmRichReconstruction::Init", "No GlobalTrack array!");}

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) { Fatal("CbmRichReconstruction::Init","No RichHit array!"); }

  // fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
  // if ( NULL == fRichRingMatches) { Fatal("CbmRichReconstruction::Init","No RichRingMatch array!"); }

   //fRichProjections = (TClonesArray*) ioman->GetObject("RichProjection");
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
   if (fRunExtrapolation) RunExtrapolation();
   if (fRunProjection) RunProjection();
   if (fRunFinder) RunFinder();
   if (fRunFitter) RunFitter();
   if (fRunTrackMatch) RunTrackMatch();
}

void CbmRichReconstruction::InitExtrapolation()
{
   if (fExtrapolationName == "ideal"){
      fTrackExtrapolation = new CbmRichTrackExtrapolationIdeal();
   } else if (fExtrapolationName == "mirror_ideal"){
      fTrackExtrapolation = new CbmRichTrackExtrapolationMirrorIdeal();
      fProjectionProducer->SetZFlag(2);
   } else if (fExtrapolationName == "kf" || fExtrapolationName == "KF"){
      fTrackExtrapolation = new CbmRichTrackExtrapolationKF();
   }
   fTrackExtrapolation->Init();
}

void CbmRichReconstruction::InitProjection()
{
   fProjectionProducer->Init();
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
   fRichTrackParamZ->Clear();
   fTrackExtrapolation->DoExtrapolation(fGlobalTracks, fRichTrackParamZ, fZTrackExtrapolation);
}

void CbmRichReconstruction::RunProjection()
{
   fProjectionProducer->DoProjection();
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
