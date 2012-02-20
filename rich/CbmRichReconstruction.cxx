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

#include "CbmRichRingTrackAssignClosestD.h"

#include "FairHit.h"
#include "CbmGlobalTrack.h"
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
   fRingTrackAssign(NULL),

   fRunExtrapolation(true),
   fRunProjection(true),
   fRunFinder(true),
   fRunFitter(true),
   fRunTrackAssign(true),

   fExtrapolationName("kf"),
   fProjectionName(""),
   fFinderName("hough"),
   fFitterName("ellipse_tau"),
   fTrackAssignName("closest_distance"),

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

   fRichProjections = new TClonesArray("FairTrackParam");
   ioman->Register("RichProjection", "RICH", fRichProjections, kTRUE);

   fRichRings = new TClonesArray("CbmRichRing", 100);
   ioman->Register("RichRing", "RICH", fRichRings, kTRUE);

   InitExtrapolation();
   InitProjection();
   InitFinder();
   InitFitter();
   InitTrackAssign();

   return kSUCCESS;
}

void CbmRichReconstruction::Exec(
      Option_t* opt)
{
   if (fRunExtrapolation) RunExtrapolation();
   if (fRunProjection) RunProjection();
   if (fRunFinder) RunFinder();
   if (fRunFitter) RunFitter();
   if (fRunTrackAssign) RunTrackAssign();
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
   } else {
      Fatal("CbmRichReconstruction::InitExtrapolation",
           (fExtrapolationName + " is not correct name for extrapolation algorithm.").c_str());
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
      Fatal("CbmRichReconstruction::InitFinder",
            (fFinderName + " is not correct name for ring finder algorithm.").c_str());
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
      Fatal("CbmRichReconstruction::InitFitter",
            (fFitterName + " is not correct name for ring fitter algorithm.").c_str());
   }
}

void CbmRichReconstruction::InitTrackAssign()
{
   if (fTrackAssignName == "closest_distance"){
      fRingTrackAssign = new CbmRichRingTrackAssignClosestD();
   } else {
      Fatal("CbmRichReconstruction::InitTrackAssign",
            (fTrackAssignName + " is not correct name for ring-track assignment algorithm.").c_str());
   }
   fRingTrackAssign->Init();
}

void CbmRichReconstruction::RunExtrapolation()
{
   fRichTrackParamZ->Clear();
   fTrackExtrapolation->DoExtrapolation(fGlobalTracks, fRichTrackParamZ, fZTrackExtrapolation);
}

void CbmRichReconstruction::RunProjection()
{
   fProjectionProducer->DoProjection(fRichProjections);
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

void CbmRichReconstruction::RunTrackAssign()
{
   // check whether track were assigned to rings.
   Int_t nTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nTracks; iTrack++){
     CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
     Int_t iRing = gTrack->GetRichRingIndex();
     if (iRing != -1) return;
   }

   fRingTrackAssign->DoAssign(fRichRings, fRichProjections);
}

void CbmRichReconstruction::Finish()
{

}

ClassImp(CbmRichReconstruction)
