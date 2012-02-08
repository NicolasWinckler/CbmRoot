/**
 * \file CbmLitClusteringQaCalculator.cxx
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitClusteringQaCalculator.h"
#include "../base/CbmLitHistManager.h"
#include "CbmHit.h"
#include "CbmBaseHit.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include <assert.h>

CbmLitClusteringQaCalculator::CbmLitClusteringQaCalculator(
      CbmLitHistManager* histManager):
      fMvdPoints(NULL),
      fMvdDigis(NULL),
      fMvdClusters(NULL),
      fMvdHits(NULL),
      fStsPoints(NULL),
      fStsDigis(NULL),
      fStsClusters(NULL),
      fStsHits(NULL),
      fRichHits(NULL),
      fRichPoints(NULL),
      fMuchPoints(NULL),
      fMuchDigis(NULL),
      fMuchClusters(NULL),
      fMuchPixelHits(NULL),
      fMuchStrawHits(NULL),
      fTrdPoints(NULL),
      fTrdDigis(NULL),
      fTrdClusters(NULL),
      fTrdHits(NULL),
      fTofPoints(NULL),
      fTofHits(NULL),
      fHM(histManager)
{

}

CbmLitClusteringQaCalculator::~CbmLitClusteringQaCalculator()
{

}

void CbmLitClusteringQaCalculator::Init()
{
   ReadDataBranches();
}

void CbmLitClusteringQaCalculator::Exec()
{
   // Increase event counter
   fHM->H1F("hEventNo")->Fill(0.5);
   std::cout << "CbmLitClusteringQaCalculator::Exec: event=" << fHM->H1F("hEventNo")->GetEntries() << std::endl;

   ProcessHits();
   IncreaseCounters();
}

void CbmLitClusteringQaCalculator::Finish()
{
   NormalizeHistograms();
}

void CbmLitClusteringQaCalculator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
   fMvdDigis = (TClonesArray*) ioman->GetObject("MvdDigi");
   fMvdClusters = (TClonesArray*) ioman->GetObject("MvdCluster");
   fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");

   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   fStsDigis = (TClonesArray*) ioman->GetObject("StsDigi");
   fStsClusters = (TClonesArray*) ioman->GetObject("StsCluster");
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");

   fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
   fMuchDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
   fMuchClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");

   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
}

void CbmLitClusteringQaCalculator::ProcessHits()
{
   if (NULL != fMvdHits) {
      for (Int_t i = 0; i < fMvdHits->GetEntriesFast(); i++) {
         const CbmHit* hit = static_cast<const CbmHit*>(fMvdHits->At(i));
         fHM->H1("hMvdNofHitsInStation")->Fill(hit->GetStationNr());
      }
   }
   if (NULL != fStsHits) {
      for (Int_t i = 0; i < fStsHits->GetEntriesFast(); i++) {
         const CbmHit* hit = static_cast<const CbmHit*>(fStsHits->At(i));
         fHM->H1("hStsNofHitsInStation")->Fill(hit->GetStationNr());
      }
   }
   if (NULL != fTrdHits) {
      for (Int_t i = 0; i < fTrdHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fTrdHits->At(i));
         fHM->H1("hTrdNofHitsInStation")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fMuchPixelHits) {
      for (Int_t i = 0; i < fMuchPixelHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(i));
         fHM->H1("hMuchNofHitsInStation")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fMuchStrawHits) {
      for (Int_t i = 0; i < fMuchStrawHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fMuchStrawHits->At(i));
         fHM->H1("hMuchNofHitsInStation")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fTofHits) {
      for (Int_t i = 0; i < fTofHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fTofHits->At(i));
         fHM->H1("hTofNofHitsInStation")->Fill(hit->GetPlaneId());
      }
   }
}

void CbmLitClusteringQaCalculator::IncreaseCounters()
{
   if (NULL != fMvdPoints) fHM->H1("hNofMvdPoints")->Fill(fMvdPoints->GetEntriesFast());
   if (NULL != fMvdHits) fHM->H1F("hNofMvdHits")->Fill(fMvdHits->GetEntriesFast());
   if (NULL != fStsPoints) fHM->H1F("hNofStsPoints")->Fill(fStsPoints->GetEntriesFast());
   if (NULL != fStsDigis) fHM->H1F("hNofStsDigis")->Fill(fStsDigis->GetEntriesFast());
   if (NULL != fStsClusters) fHM->H1F("hNofStsClusters")->Fill(fStsClusters->GetEntriesFast());
   if (NULL != fStsHits) fHM->H1F("hNofStsHits")->Fill(fStsHits->GetEntriesFast());
   if (NULL != fRichHits) fHM->H1("hNofRichHits")->Fill(fRichHits->GetEntriesFast());
   if (NULL != fRichPoints) fHM->H1("hNofRichPoints")->Fill(fRichPoints->GetEntriesFast());
   if (NULL != fTrdPoints) fHM->H1F("hNofTrdPoints")->Fill(fTrdPoints->GetEntriesFast());
   if (NULL != fTrdDigis) fHM->H1F("hNofTrdDigis")->Fill(fTrdDigis->GetEntriesFast());
   if (NULL != fTrdClusters) fHM->H1F("hNofTrdClusters")->Fill(fTrdClusters->GetEntriesFast());
   if (NULL != fTrdHits) fHM->H1F("hNofTrdHits")->Fill(fTrdHits->GetEntriesFast());
   if (NULL != fMuchPoints) fHM->H1F("hNofMuchPoints")->Fill(fMuchPoints->GetEntriesFast());
   if (NULL != fMuchDigis) fHM->H1F("hNofMuchDigis")->Fill(fMuchDigis->GetEntriesFast());
   if (NULL != fMuchClusters) fHM->H1F("hNofMuchClusters")->Fill(fMuchClusters->GetEntriesFast());
   if (NULL != fMuchPixelHits) fHM->H1F("hNofMuchPixelHits")->Fill(fMuchPixelHits->GetEntriesFast());
   if (NULL != fMuchStrawHits) fHM->H1F("hNofMuchStrawHits")->Fill(fMuchStrawHits->GetEntriesFast());
   if (NULL != fTofPoints) fHM->H1F("hNofTofPoints")->Fill(fTofPoints->GetEntriesFast());
   if (NULL != fTofHits) fHM->H1F("hNofTofHits")->Fill(fTofHits->GetEntriesFast());
}

void CbmLitClusteringQaCalculator::NormalizeHistograms()
{
   Int_t nofEvents = fHM->H1("hEventNo")->GetEntries();
   fHM->H1("hMvdNofHitsInStation")->Scale(1. / nofEvents);
   fHM->H1("hStsNofHitsInStation")->Scale(1. / nofEvents);
   fHM->H1("hTrdNofHitsInStation")->Scale(1. / nofEvents);
   fHM->H1("hMuchNofHitsInStation")->Scale(1. / nofEvents);
   fHM->H1("hTofNofHitsInStation")->Scale(1. / nofEvents);
   fHM->ShrinkEmptyBins("hMvdNofHitsInStation");
   fHM->ShrinkEmptyBins("hStsNofHitsInStation");
   fHM->ShrinkEmptyBins("hTrdNofHitsInStation");
   fHM->ShrinkEmptyBins("hMuchNofHitsInStation");
   fHM->ShrinkEmptyBins("hTofNofHitsInStation");
}
