/**
 * \file CbmLitClusteringQaCalculator.cxx
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitClusteringQaCalculator.h"
#include "../base/CbmHistManager.h"
#include "CbmHit.h"
#include "CbmBaseHit.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include <assert.h>

CbmLitClusteringQaCalculator::CbmLitClusteringQaCalculator(
      CbmHistManager* histManager):
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
   fHM->H1F("hen_EventNo_ClusteringQa")->Fill(0.5);
   std::cout << "CbmLitClusteringQaCalculator::Exec: event=" << fHM->H1F("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;

   ProcessHits();
   IncreaseCounters();
}

void CbmLitClusteringQaCalculator::Finish()
{
	Int_t nofEvents = fHM->H1("hen_EventNo_ClusteringQa")->GetEntries();
	fHM->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);
	fHM->ShrinkEmptyBinsByPattern("hno_NofObjects_.*_Station");
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
   if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits_Station")) {
      for (Int_t i = 0; i < fMvdHits->GetEntriesFast(); i++) {
         const CbmHit* hit = static_cast<const CbmHit*>(fMvdHits->At(i));
         fHM->H1("hno_NofObjects_MvdHits_Station")->Fill(hit->GetStationNr());
      }
   }
   if (NULL != fStsHits && fHM->Exists("hno_NofObjects_StsHits_Station")) {
      for (Int_t i = 0; i < fStsHits->GetEntriesFast(); i++) {
         const CbmHit* hit = static_cast<const CbmHit*>(fStsHits->At(i));
         fHM->H1("hno_NofObjects_StsHits_Station")->Fill(hit->GetStationNr());
      }
   }
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits_Station")) {
      for (Int_t i = 0; i < fTrdHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fTrdHits->At(i));
         fHM->H1("hno_NofObjects_TrdHits_Station")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fMuchPixelHits && fHM->Exists("hno_NofObjects_MuchHits_Station")) {
      for (Int_t i = 0; i < fMuchPixelHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(i));
         fHM->H1("hno_NofObjects_MuchHits_Station")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fMuchStrawHits && fHM->Exists("hno_NofObjects_MuchHits_Station")) {
      for (Int_t i = 0; i < fMuchStrawHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fMuchStrawHits->At(i));
         fHM->H1("hno_NofObjects_MuchHits_Station")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fTofHits && fHM->Exists("hno_NofObjects_TofHits_Station")) {
      for (Int_t i = 0; i < fTofHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fTofHits->At(i));
         fHM->H1("hno_NofObjects_TofHits_Station")->Fill(hit->GetPlaneId());
      }
   }
}

void CbmLitClusteringQaCalculator::IncreaseCounters()
{
   if (NULL != fMvdPoints && fHM->Exists("hno_NofObjects_MvdPoints")) fHM->H1("hno_NofObjects_MvdPoints")->Fill(fMvdPoints->GetEntriesFast());
   if (NULL != fMvdDigis && fHM->Exists("hno_NofObjects_MvdDigis")) fHM->H1F("hno_NofObjects_MvdDigis")->Fill(fMvdDigis->GetEntriesFast());
   if (NULL != fMvdClusters && fHM->Exists("hno_NofObjects_MvdClusters")) fHM->H1F("hno_NofObjects_MvdClusters")->Fill(fMvdClusters->GetEntriesFast());
   if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits")) fHM->H1F("hno_NofObjects_MvdHits")->Fill(fMvdHits->GetEntriesFast());

   if (NULL != fStsPoints && fHM->Exists("hno_NofObjects_StsPoints")) fHM->H1F("hno_NofObjects_StsPoints")->Fill(fStsPoints->GetEntriesFast());
   if (NULL != fStsDigis && fHM->Exists("hno_NofObjects_StsDigis")) fHM->H1F("hno_NofObjects_StsDigis")->Fill(fStsDigis->GetEntriesFast());
   if (NULL != fStsClusters && fHM->Exists("hno_NofObjects_StsClusters")) fHM->H1F("hno_NofObjects_StsClusters")->Fill(fStsClusters->GetEntriesFast());
   if (NULL != fStsHits && fHM->Exists("hno_NofObjects_StsHits")) fHM->H1F("hno_NofObjects_StsHits")->Fill(fStsHits->GetEntriesFast());

   if (NULL != fRichHits && fHM->Exists("hno_NofObjects_RichHits")) fHM->H1("hno_NofObjects_RichHits")->Fill(fRichHits->GetEntriesFast());
   if (NULL != fRichPoints && fHM->Exists("hno_NofObjects_RichPoints")) fHM->H1("hno_NofObjects_RichPoints")->Fill(fRichPoints->GetEntriesFast());

   if (NULL != fTrdPoints && fHM->Exists("hno_NofObjects_TrdPoints")) fHM->H1F("hno_NofObjects_TrdPoints")->Fill(fTrdPoints->GetEntriesFast());
   if (NULL != fTrdDigis && fHM->Exists("hno_NofObjects_TrdDigis")) fHM->H1F("hno_NofObjects_TrdDigis")->Fill(fTrdDigis->GetEntriesFast());
   if (NULL != fTrdClusters && fHM->Exists("hno_NofObjects_TrdClusters")) fHM->H1F("hno_NofObjects_TrdClusters")->Fill(fTrdClusters->GetEntriesFast());
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits")) fHM->H1F("hno_NofObjects_TrdHits")->Fill(fTrdHits->GetEntriesFast());

   if (NULL != fMuchPoints && fHM->Exists("hno_NofObjects_MuchPoints")) fHM->H1F("hno_NofObjects_MuchPoints")->Fill(fMuchPoints->GetEntriesFast());
   if (NULL != fMuchDigis && fHM->Exists("hno_NofObjects_MuchDigis")) fHM->H1F("hno_NofObjects_MuchDigis")->Fill(fMuchDigis->GetEntriesFast());
   if (NULL != fMuchClusters && fHM->Exists("hno_NofObjects_MuchClusters")) fHM->H1F("hno_NofObjects_MuchClusters")->Fill(fMuchClusters->GetEntriesFast());
   if (NULL != fMuchPixelHits && fHM->Exists("hno_NofObjects_MuchPixelHits")) fHM->H1F("hno_NofObjects_MuchPixelHits")->Fill(fMuchPixelHits->GetEntriesFast());
   if (NULL != fMuchStrawHits && fHM->Exists("hno_NofObjects_MuchStrawHits")) fHM->H1F("hno_NofObjects_MuchStrawHits")->Fill(fMuchStrawHits->GetEntriesFast());

   if (NULL != fTofPoints && fHM->Exists("hno_NofObjects_TofPoints")) fHM->H1F("hno_NofObjects_TofPoints")->Fill(fTofPoints->GetEntriesFast());
   if (NULL != fTofHits && fHM->Exists("hno_NofObjects_TofHits")) fHM->H1F("hno_NofObjects_TofHits")->Fill(fTofHits->GetEntriesFast());
}
