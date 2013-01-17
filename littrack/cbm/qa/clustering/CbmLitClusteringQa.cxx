/**
 * \file CbmLitClusteringQa.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitClusteringQa.h"
#include "CbmLitClusteringQaReport.h"
#include "CbmHistManager.h"
#include "CbmHit.h"
#include "CbmBaseHit.h"
#include "FairRootManager.h"

#include "TSystem.h"
#include "TClonesArray.h"
#include "TH1.h"

#include <fstream>
using std::cout;

CbmLitClusteringQa::CbmLitClusteringQa():
   fHM(NULL),
   fOutputDir(""),
   fDet(),
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
   fTofHits(NULL)
{

}

CbmLitClusteringQa::~CbmLitClusteringQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitClusteringQa::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   fDet.DetermineSetup();
   CreateHistograms();
   ReadDataBranches();
   return kSUCCESS;
}

void CbmLitClusteringQa::Exec(
    Option_t* opt)
{
   // Increase event counter
   fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
   std::cout << "CbmLitClusteringQaCalculator::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;

   ProcessHits();
   IncreaseCounters();
}

void CbmLitClusteringQa::Finish()
{
   fHM->ShrinkEmptyBinsByPattern("hno_NofObjects_.*_Station");
   fHM->WriteToFile();
   CbmSimulationReport* report = new CbmLitClusteringQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitClusteringQa::ReadDataBranches()
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

void CbmLitClusteringQa::ProcessHits()
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

void CbmLitClusteringQa::IncreaseCounters()
{
   if (NULL != fMvdPoints && fHM->Exists("hno_NofObjects_MvdPoints_Event")) fHM->H1("hno_NofObjects_MvdPoints_Event")->Fill(fMvdPoints->GetEntriesFast());
   if (NULL != fMvdDigis && fHM->Exists("hno_NofObjects_MvdDigis_Event")) fHM->H1("hno_NofObjects_MvdDigis_Event")->Fill(fMvdDigis->GetEntriesFast());
   if (NULL != fMvdClusters && fHM->Exists("hno_NofObjects_MvdClusters_Event")) fHM->H1("hno_NofObjects_MvdClusters_Event")->Fill(fMvdClusters->GetEntriesFast());
   if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits_Event")) fHM->H1("hno_NofObjects_MvdHits_Event")->Fill(fMvdHits->GetEntriesFast());

   if (NULL != fStsPoints && fHM->Exists("hno_NofObjects_StsPoints_Event")) fHM->H1("hno_NofObjects_StsPoints_Event")->Fill(fStsPoints->GetEntriesFast());
   if (NULL != fStsDigis && fHM->Exists("hno_NofObjects_StsDigis_Event")) fHM->H1("hno_NofObjects_StsDigis_Event")->Fill(fStsDigis->GetEntriesFast());
   if (NULL != fStsClusters && fHM->Exists("hno_NofObjects_StsClusters_Event")) fHM->H1("hno_NofObjects_StsClusters_Event")->Fill(fStsClusters->GetEntriesFast());
   if (NULL != fStsHits && fHM->Exists("hno_NofObjects_StsHits_Event")) fHM->H1("hno_NofObjects_StsHits_Event")->Fill(fStsHits->GetEntriesFast());

   if (NULL != fRichHits && fHM->Exists("hno_NofObjects_RichHits_Event")) fHM->H1("hno_NofObjects_RichHits_Event")->Fill(fRichHits->GetEntriesFast());
   if (NULL != fRichPoints && fHM->Exists("hno_NofObjects_RichPoints_Event")) fHM->H1("hno_NofObjects_RichPoints_Event")->Fill(fRichPoints->GetEntriesFast());

   if (NULL != fTrdPoints && fHM->Exists("hno_NofObjects_TrdPoints_Event")) fHM->H1("hno_NofObjects_TrdPoints_Event")->Fill(fTrdPoints->GetEntriesFast());
   if (NULL != fTrdDigis && fHM->Exists("hno_NofObjects_TrdDigis_Event")) fHM->H1("hno_NofObjects_TrdDigis_Event")->Fill(fTrdDigis->GetEntriesFast());
   if (NULL != fTrdClusters && fHM->Exists("hno_NofObjects_TrdClusters_Event")) fHM->H1("hno_NofObjects_TrdClusters_Event")->Fill(fTrdClusters->GetEntriesFast());
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits_Event")) fHM->H1("hno_NofObjects_TrdHits_Event")->Fill(fTrdHits->GetEntriesFast());

   if (NULL != fMuchPoints && fHM->Exists("hno_NofObjects_MuchPoints_Event")) fHM->H1("hno_NofObjects_MuchPoints_Event")->Fill(fMuchPoints->GetEntriesFast());
   if (NULL != fMuchDigis && fHM->Exists("hno_NofObjects_MuchDigis_Event")) fHM->H1("hno_NofObjects_MuchDigis_Event")->Fill(fMuchDigis->GetEntriesFast());
   if (NULL != fMuchClusters && fHM->Exists("hno_NofObjects_MuchClusters_Event")) fHM->H1("hno_NofObjects_MuchClusters_Event")->Fill(fMuchClusters->GetEntriesFast());
   if (NULL != fMuchPixelHits && fHM->Exists("hno_NofObjects_MuchPixelHits_Event")) fHM->H1("hno_NofObjects_MuchPixelHits_Event")->Fill(fMuchPixelHits->GetEntriesFast());
   if (NULL != fMuchStrawHits && fHM->Exists("hno_NofObjects_MuchStrawHits_Event")) fHM->H1("hno_NofObjects_MuchStrawHits_Event")->Fill(fMuchStrawHits->GetEntriesFast());

   if (NULL != fTofPoints && fHM->Exists("hno_NofObjects_TofPoints_Event")) fHM->H1("hno_NofObjects_TofPoints_Event")->Fill(fTofPoints->GetEntriesFast());
   if (NULL != fTofHits && fHM->Exists("hno_NofObjects_TofHits_Event")) fHM->H1("hno_NofObjects_TofHits_Event")->Fill(fTofHits->GetEntriesFast());
}

void CbmLitClusteringQa::CreateHistograms()
{
   CreateNofObjectsHistograms(kMVD, "Mvd", "Station", "Station number");
   CreateNofObjectsHistograms(kSTS, "Sts", "Station", "Station number");
   CreateNofObjectsHistograms(kTRD, "Trd", "Station", "Station number");
   CreateNofObjectsHistograms(kMUCH, "Much", "Station", "Station number");
   CreateNofObjectsHistograms(kTOF, "Tof", "Station", "Station number");

   CreateNofObjectsHistograms(kMVD, "Mvd");
   CreateNofObjectsHistograms(kSTS, "Sts");
   CreateNofObjectsHistograms(kTRD, "Trd");
   CreateNofObjectsHistograms(kMUCH, "Much");
   CreateNofObjectsHistograms(kTOF, "Tof");
   CreateNofObjectsHistograms(kRICH, "Rich");

   // Histogram stores number of events
   CreateH1F("hen_EventNo_ClusteringQa", "", "", 1, 0, 1.);
}

void CbmLitClusteringQa::CreateH1F(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TH1F* h = new TH1F(name.c_str(), (name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}

void CbmLitClusteringQa::CreateNofObjectsHistograms(
      DetectorId detId,
      const string& detName)
{
   assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
   Int_t nofBins = 100000;
   Double_t minX = 0.;
   Double_t maxX = 100000.;
   if (fDet.GetDet(detId)) {
      string name = "hno_NofObjects_" + detName;
      CreateH1F(name + "Points_Event", "Points per event", "Counter", nofBins, minX, maxX);
      CreateH1F(name + "Digis_Event", "Digis per event", "Counter", nofBins, minX, maxX);
      CreateH1F(name + "Clusters_Event", "Clusters per event", "Counter", nofBins, minX, maxX);
      if (detId == kMUCH) {
         CreateH1F(name + "PixelHits_Event", "Hits per event", "Counter", nofBins, minX, maxX);
         CreateH1F(name + "StrawHits_Event", "Hits per event", "Counter", nofBins, minX, maxX);
      } else {
         CreateH1F(name + "Hits_Event", "Hits per event", "Counter", nofBins, minX, maxX);
      }
   }
}

void CbmLitClusteringQa::CreateNofObjectsHistograms(
      DetectorId detId,
      const string& detName,
      const string& parameter,
      const string& xTitle)
{
   assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
   Int_t nofBins = 100;
   Double_t minX = 0.;
   Double_t maxX = 100.;
   if (fDet.GetDet(detId)) {
      string name = "hno_NofObjects_" + detName;
      CreateH1F(name + "Points_" + parameter, xTitle, "Points per event", nofBins, minX, maxX);
      CreateH1F(name + "Digis_" + parameter, xTitle, "Digis per event", nofBins, minX, maxX);
      CreateH1F(name + "Clusters_" + parameter, xTitle, "Clusters per event", nofBins, minX, maxX);
      CreateH1F(name + "Hits_" + parameter, xTitle, "Hits per event", nofBins, minX, maxX);
   }
}

ClassImp(CbmLitClusteringQa);
