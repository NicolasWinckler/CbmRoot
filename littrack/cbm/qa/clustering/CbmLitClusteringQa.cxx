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
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchPoint.h"
#include "CbmMuchCluster.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleGemRadial.h"
#include "CbmMuchSector.h"
#include "CbmMuchSectorRadial.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"

#include "TSystem.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <fstream>
#include <math.h>
#include <sstream>
using std::cout;
using std::vector;
using std::map;
using std::binary_search;

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
   fTofHits(NULL),
   fMuchGeoScheme(CbmMuchGeoScheme::Instance())
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
   ReadDataBranches();
   if(fDet.GetDet(kMUCH)){
      if(fMuchDigiFile != NULL){
         MuchGeoSchemeInit(fMuchDigiFile);
         CreateMuchHistograms();
      }
      else{
         std::cout<<"Error! MuchDigiFile is not loaded.\n";
      }
   }
   CreateHistograms();
   return kSUCCESS;
}

void CbmLitClusteringQa::Exec(
    Option_t* opt)
{
   // Increase event counter
   ProcessHits();
   IncreaseCounters();

   fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
   std::cout << "CbmLitClusteringQaCalculator::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;

   fBestPoints.clear();
   fBestPointsForHits.clear();

   if(fDet.GetDet(kMUCH)){
      fNofMuchClusters = fMuchClusters->GetEntriesFast();
      fNofMuchHits = fMuchPixelHits->GetEntriesFast();
      fNofMuchDigis = fMuchDigis->GetEntriesFast();
      fNofMuchPoints = fMuchPoints->GetEntriesFast();
      SetMuchClustersArray();
      FillMuchHistograms();
   }
}

void CbmLitClusteringQa::Finish()
{

   if(fDet.GetDet(kMUCH)){
      FillMuchAccuracyHistograms();
   }

   fHM->WriteToFile();
   CbmSimulationReport* report = new CbmLitClusteringQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
   fBestPoints.clear();
   fBestPointsForHits.clear();
}

void CbmLitClusteringQa::MuchGeoSchemeInit(const TString& digiFileName)
{
	TFile* oldfile=gFile;
	TFile* file=new TFile(digiFileName);
	TObjArray* stations = (TObjArray*) file->Get("stations");
	file->Close();
	file->Delete();
	gFile=oldfile;
	fMuchGeoScheme->Init(stations);
}

void CbmLitClusteringQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

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

   if(fDet.GetDet(kMUCH)){
      fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
      fMuchDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
      fMuchDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
      fMuchClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
      fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
      fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
      fMuchTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
   }

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

void CbmLitClusteringQa::CreateP1(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TProfile* h = new TProfile(name.c_str(), (name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}

void CbmLitClusteringQa::CreateH2F(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      const string& zTitle,
      Int_t nofBinsX,
      Double_t minBinX,
      Double_t maxBinX,
      Int_t nofBinsY,
      Double_t minBinY,
      Double_t maxBinY)
{
   TH2F* h = new TH2F(name.c_str(), (name + ";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
   fHM->Add(name, h);
}

void CbmLitClusteringQa::CreateP2(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      const string& zTitle,
      Int_t nofBinsX,
      Double_t minBinX,
      Double_t maxBinX,
      Int_t nofBinsY,
      Double_t minBinY,
      Double_t maxBinY)
{
   TProfile2D* h = new TProfile2D(name.c_str(), (name + ";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
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

void CbmLitClusteringQa::SetMuchClustersArray()
{
   vector<Int_t> totalDigisByPoint;
   for(Int_t iPoint = 0; iPoint < fNofMuchPoints; iPoint++){
      totalDigisByPoint.push_back(0);
   }
   for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
      CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*)fMuchDigiMatches->At(iDigi);
      for(Int_t iRef = 0; iRef < digiMatch->GetNPoints(); iRef++){
         totalDigisByPoint[digiMatch->GetRefIndex(iRef)]++;
      }
   }
   vector<Int_t> pointsInCluster;
   vector<Int_t> digisByPoint;
   vector<Int_t> clusterToPointRatio;
   for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
      CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
      for(Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
         Int_t digiIndex = cluster->GetDigiIndex(iDigi);
         if(isnan(digiIndex))continue;
         CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
         for(Int_t iPoint = 0; iPoint < digiMatch->GetNPoints(); iPoint++){
            Int_t refIndex = digiMatch->GetRefIndex(iPoint);
            if(pointsInCluster.size() == 0){
               pointsInCluster.push_back(refIndex);
               clusterToPointRatio.push_back(0);
               digisByPoint.push_back(1);
            }
            else{
               Bool_t pointExist = 0;
               for(Int_t i = 0; i < pointsInCluster.size(); i++){
                  if(pointsInCluster[i] == refIndex){
                     pointExist = 1;
                     digisByPoint[i]++;
                  }
               }
               if(pointExist == 0){
                  pointsInCluster.push_back(refIndex);
                  clusterToPointRatio.push_back(0);
                  digisByPoint.push_back(1);
               }
            }
         }
      }
      Int_t bp = 0;
      for(Int_t iPoint = 0; iPoint < pointsInCluster.size(); iPoint++){
         clusterToPointRatio.push_back(100.0 * (Float_t)digisByPoint[iPoint] /
               (Float_t)totalDigisByPoint[pointsInCluster[iPoint]]);
         if(clusterToPointRatio[iPoint] > 100) clusterToPointRatio[iPoint] = 100;
         if(digisByPoint[iPoint] > bp){
            bp = digisByPoint[iPoint];
            fBestPoints[iCl] = pointsInCluster[iPoint];
         }
      }
      if(fNofMuchClusters == fNofMuchHits){
         fBestPointsForHits[iCl] = fBestPoints[iCl];
      }
      else{
         Float_t minDist = 100000;
         for(Int_t iHit = 0; iHit < fNofMuchHits; iHit++){
            CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchPixelHits->At(iHit);
            minDist = 100000;
            if(hit->GetRefId() == iCl){
               for(Int_t iPoint = 0; iPoint < pointsInCluster.size(); iPoint++){
                  CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(pointsInCluster[iPoint]);
                  Float_t xP = (point->GetXIn() + point->GetXOut()) / 2;
                  Float_t yP = (point->GetYIn() + point->GetYOut()) / 2;
                  Float_t dist = sqrt((xP - hit->GetX()) * (xP - hit->GetX()) + (yP - hit->GetY()) * (yP - hit->GetY()));
                  if(dist < minDist){
                     bp = digisByPoint[iPoint];
                     fBestPointsForHits[iHit/*iCl*/] = pointsInCluster[iPoint];
                     minDist = dist;
                  }
               }
            }
         }
      }
      pointsInCluster.clear();
      digisByPoint.clear();
      clusterToPointRatio.clear();
   }
}

Float_t CbmLitClusteringQa::CalculateMuchClusterToPointRatio(Int_t nCluster, Int_t nPoint)
{
   Int_t nofDigisInCluster = 0;
   Int_t nofDigisInPoint = 0;
   for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
      CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*)fMuchDigiMatches->At(iDigi);
      for(Int_t iRef = 0; iRef < digiMatch->GetNPoints(); iRef++){
         if(digiMatch->GetRefIndex(iRef) == nPoint)nofDigisInPoint++;
      }
   }
   CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(nCluster);
   for(Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
      Int_t digiIndex = cluster->GetDigiIndex(iDigi);
      if(isnan(digiIndex))continue;
      CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
      for(Int_t iPoint = 0; iPoint < digiMatch->GetNPoints(); iPoint++){
         if(digiMatch->GetRefIndex(iPoint) == nPoint)nofDigisInCluster++;
      }
   }
   Float_t clusterToPointRatio = 100.0 * (Float_t)nofDigisInCluster / (Float_t)nofDigisInPoint;
   if(clusterToPointRatio > 100)clusterToPointRatio = 100;
   return clusterToPointRatio;
}

Float_t CbmLitClusteringQa::CalculateMuchDigisToClusterRatio(Int_t nCluster, Int_t nPoint)
{
   Int_t nofDigisInCluster = 0;
   CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(nCluster);
   for(Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
      Int_t digiIndex = cluster->GetDigiIndex(iDigi);
      if(isnan(digiIndex))continue;
      CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
      for(Int_t iPoint = 0; iPoint < digiMatch->GetNPoints(); iPoint++){
         if(digiMatch->GetRefIndex(iPoint) == nPoint)nofDigisInCluster++;
      }
   }
   Float_t clusterToPointRatio = 100.0 * (Float_t)nofDigisInCluster / (Float_t)(cluster->GetNDigis());
   if(clusterToPointRatio > 100)clusterToPointRatio = 100;
   return clusterToPointRatio;
}

Int_t CbmLitClusteringQa::GetNofPointsInCluster(Int_t nCluster)
{
   CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(nCluster);
   vector<Int_t> pointsInCluster;
   Int_t nofPoints = 0;
   for(Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++)
   {
      Int_t digiIndex = cluster->GetDigiIndex(iDigi);
      if(isnan(digiIndex))continue;
      CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
      for(Int_t iPoint = 0; iPoint < digiMatch->GetNPoints(); iPoint++){
         std::sort(pointsInCluster.begin(), pointsInCluster.end());
         if(!std::binary_search(pointsInCluster.begin(), pointsInCluster.end(), digiMatch->GetRefIndex(iPoint))){
            pointsInCluster.push_back(digiMatch->GetRefIndex(iPoint));
            nofPoints++;
         }
      }
   }
   pointsInCluster.clear();
   return nofPoints;
}

void CbmLitClusteringQa::CreateMuchHistograms()
{
	CreateH1F("hss_Much_ClusterToPointRatio", "Layer number", "Quality per event", 100, -0.5, 99.5);
	CreateH1F("hss_Much_ClusterQuality", "Layer number", "Quality per event", 100, -0.5, 99.5);
	CreateH1F("hss_Much_NofPointsInCluster", "Layer number", "MCPoints in clusters per event", 100, -0.5, 99.5);
	CreateH1F("hss_Much_NofPointsInDigi", "Layer number", "MCPoints by digi per event", 100, -0.5, 99.5);

	CreateP1("hsh_Much_PixelHits", "Layer number", "Hits", 100, -0.5, 99.5);
	CreateP1("hsh_Much_Digis", "Layer number", "Digis", 100, -0.5, 99.5);
	CreateP1("hsh_Much_Points", "Layer number", "Points", 100, -0.5, 99.5);
	CreateP1("hsh_Much_Clusters", "Layer number", "Clusters", 100, -0.5, 99.5);

	CreateH1F("hss_Much_NofDigisByPoint", "Layer number", "Clusters", 100, -0.5, 99.5);

	//---
	Int_t nofLayers = 0;
	Int_t nofSteps = 10;
	Float_t rMin = 0;
	Float_t rMax = 0;
	for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
	   CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
	   nofLayers += station->GetNLayers();
	   if(iStation == 0)rMin = station->GetRmin();
	   if(iStation == (fMuchGeoScheme->GetNStations() - 1))rMax = station->GetRmax();
	}
	CreateH2F("he_MuchPixelHitsH2", "Layer", "Radius", "nofHits", nofLayers, 0., nofLayers, nofSteps, 0., nofSteps);
	CreateH2F("hsc_Much_ClusterQuality_2D", "Layer", "Radius", "Quality", nofLayers, 0., nofLayers, nofSteps, 0., nofSteps);
	CreateH2F("hsc_Much_ClusterToPointRatio_2D", "Layer", "Radius", "ClusterToPointRatio", nofLayers, 0., nofLayers, nofSteps, 0., nofSteps);
	CreateH2F("hsc_Much_NofPointsInCluster_2D", "Layer", "Radius", "MCPointsInCluster", nofLayers, 0., nofLayers, nofSteps, 0., nofSteps);
	CreateH2F("hsc_Much_NofPointsInDigi_2D", "Layer", "Radius", "MCPointsInDigi", nofLayers, 0., nofLayers, nofSteps, 0., nofSteps);

	//Distributions
	CreateH1F("hss_Much_NofDigisByPoint_Total", "Number of digis", "Points counter", 100, -0.5, 99.5);
	CreateH1F("hss_Much_NofPointsInDigi_Total", "Number of points", "Digis counter", 100, -0.5, 99.5);
	CreateH1F("hss_Much_NofDigisInCluster_Total", "Number of digis", "Clusters counter", 100, -0.5, 99.5);

	//---
	CreateH2F("hr_Much_ResidualX_2D", "Layer", "Residual X", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	CreateH2F("hr_Much_ResidualY_2D", "Layer", "Residual Y", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	CreateH2F("hr_Much_PullX_2D", "Layer", "Pull X", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	CreateH2F("hr_Much_PullY_2D", "Layer", "Pull Y", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	//---
	CreateH1F("h_Much_RMS_X", "Layer", "RMS", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_RMS_Y", "Layer", "RMS", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_PullRMS_X", "Layer", "RMS", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_PullRMS_Y", "Layer", "RMS", nofLayers, 0.5, (nofLayers + 0.5));

	CreateH1F("h_Much_Mean_X", "Layer", "Mean", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_Mean_Y", "Layer", "Mean", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_PullMean_X", "Layer", "Mean", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_PullMean_Y", "Layer", "Mean", nofLayers, 0.5, (nofLayers + 0.5));

	CreateH1F("h_Much_Sigma_X", "Layer", "Sigma", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_Sigma_Y", "Layer", "Sigma", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_PullSigma_X", "Layer", "Sigma", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_PullSigma_Y", "Layer", "Sigma", nofLayers, 0.5, (nofLayers + 0.5));

	CreateP1("hsh_Much_True_PixelHits", "Layer number", "Hits", nofLayers, -0.5, (nofLayers - 0.5));

	//---Muon only---
	CreateP1("hsh_Much_Muon_PixelHits", "Layer number", "Hits", nofLayers, -0.5, (nofLayers - 0.5));
	CreateP1("hsh_Much_Muon_Digis", "Layer number", "Digis", nofLayers, -0.5, (nofLayers - 0.5));
	CreateP1("hsh_Much_Muon_Points", "Layer number", "Points", nofLayers, -0.5, (nofLayers - 0.5));
	CreateP1("hsh_Much_Muon_Clusters", "Layer number", "Clusters", nofLayers, -0.5, (nofLayers - 0.5));

	CreateH2F("hr_Much_Muon_ResidualX_2D", "Layer", "Residual X", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	CreateH2F("hr_Much_Muon_ResidualY_2D", "Layer", "Residual Y", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	CreateH2F("hr_Much_Muon_PullX_2D", "Layer", "Residual X", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);
	CreateH2F("hr_Much_Muon_PullY_2D", "Layer", "Residual Y", "Counter", nofLayers, 0., nofLayers, 50, -5., 5.);

	CreateH1F("h_Much_Muon_PullRMS_X", "Layer", "RMS", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_Muon_PullRMS_Y", "Layer", "RMS", nofLayers, 0.5, (nofLayers + 0.5));

	CreateH1F("h_Much_Muon_PullMean_X", "Layer", "Mean", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_Muon_PullMean_Y", "Layer", "Mean", nofLayers, 0.5, (nofLayers + 0.5));

	CreateH1F("h_Much_Muon_PullSigma_X", "Layer", "Sigma", nofLayers, 0.5, (nofLayers + 0.5));
	CreateH1F("h_Much_Muon_PullSigma_Y", "Layer", "Sigma", nofLayers, 0.5, (nofLayers + 0.5));
}

void CbmLitClusteringQa::FillMuchHistograms()
{
	FillMuchClusterQualityHistogram();
	FillMuchMCPointsInClusterHistogrm();
	FillMuchMCPointsByDigiHistogram();

	FillMuchPixelHitsHistogram();
	FillMuchPointsHistogram();
	FillMuchClustersHistogram();
	FillMuchDigisHistogram();

	FillDigiByPointHistogrm();

	FillMuchClusterQuality2DHistogram();
	FillMuchMCPointsInCluster2DHistogrm();
	FillMuchMCPointsByDigi2DHistogram();

	FillDigiByPointDistributionHistogrm();
	FillPointsInDigiDistributionHistogrm();
	FillDigisInClusterDistributionHistogrm();

	FillMuchResidualHistograms();
}

void CbmLitClusteringQa::FillMuchPixelHitsHistogram()
{
   if (NULL != fMuchPixelHits && fHM->Exists("hsh_Much_PixelHits")){
      vector<Int_t> nofPixelHits;
      vector<Int_t> nofPixelHits_Muon;
      vector<Int_t> nofTruePixelHits;
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            nofPixelHits.push_back(0);
            nofPixelHits_Muon.push_back(0);
            nofTruePixelHits.push_back(0);
         }
      }
      vector<Int_t> muonPoints;
      vector<Int_t> trueHits;
      for(Int_t iHit = 0; iHit < fNofMuchHits; iHit++){
         CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchPixelHits->At(iHit);
         if(isnan(hit->GetX()) || (isnan(hit->GetY())))continue;
         Int_t nStation = fMuchGeoScheme->GetStationIndex(hit->GetDetectorId());
         if((nStation < 0) || (nStation >fMuchGeoScheme->GetNStations()))continue;
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStationByDetId(hit->GetDetectorId());
         Int_t nofLayers = station->GetNLayers();
         Int_t nLayer = fMuchGeoScheme->GetLayerIndex(hit->GetDetectorId());
         if((nLayer < 0) || (nLayer > nofLayers))continue;
         Int_t iLayer = (nStation * nofLayers) + nLayer;
         nofPixelHits[iLayer]++;
         CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(fBestPointsForHits[/*hit->GetRefId()*/iHit]);
         CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(point->GetTrackID());
         if(fabs(mcTrack->GetPdgCode()) == 13){
            if(!std::binary_search(muonPoints.begin(), muonPoints.end(), fBestPointsForHits[/*hit->GetRefId()*/iHit])){
               muonPoints.push_back(fBestPointsForHits[/*hit->GetRefId()*/iHit]);
               nofPixelHits_Muon[iLayer]++;
               std::sort(muonPoints.begin(), muonPoints.end());
            }
         }
         if(!std::binary_search(trueHits.begin(), trueHits.end(), fBestPointsForHits[/*hit->GetRefId()*/iHit])){
            trueHits.push_back(fBestPointsForHits[/*hit->GetRefId()*/iHit]);
            nofTruePixelHits[iLayer]++;
            std::sort(trueHits.begin(), trueHits.end());
         }
      }
      muonPoints.clear();
      trueHits.clear();
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            Int_t nLayer = (iStation * station->GetNLayers()) + iLayer;
            fHM->P1("hsh_Much_PixelHits")->Fill(nLayer, nofPixelHits[nLayer]);
            fHM->P1("hsh_Much_Muon_PixelHits")->Fill(nLayer, nofPixelHits_Muon[nLayer]);
            fHM->P1("hsh_Much_True_PixelHits")->Fill(nLayer, nofTruePixelHits[nLayer]);
         }
      }
   }
}

void CbmLitClusteringQa::FillMuchClustersHistogram()
{
   if (NULL != fMuchClusters && fHM->Exists("hsh_Much_Clusters")){
      vector<Int_t> nofClusters;
      vector<Int_t> nofClusters_Muon;
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            nofClusters.push_back(0);
            nofClusters_Muon.push_back(0);
         }
      }
      vector<Int_t> muonPoints;
      for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
         CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
         if(cluster->GetNDigis() < 1)continue;
         CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(cluster->GetDigiIndex(0));
         Int_t nStation = fMuchGeoScheme->GetStationIndex(digi->GetDetectorId());
         if((nStation < 0) || (nStation >fMuchGeoScheme->GetNStations()))continue;
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStationByDetId(digi->GetDetectorId());
         Int_t nofLayers = station->GetNLayers();
         Int_t nLayer = fMuchGeoScheme->GetLayerIndex(digi->GetDetectorId());
         if((nLayer < 0) || (nLayer > nofLayers))continue;
         Int_t iLayer = (nStation * nofLayers) + nLayer;
         nofClusters[iLayer]++;
         CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(fBestPoints[iCl]);
         CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(point->GetTrackID());
         if(fabs(mcTrack->GetPdgCode()) == 13){
            if(!std::binary_search(muonPoints.begin(), muonPoints.end(), fBestPoints[iCl])){
               muonPoints.push_back(fBestPoints[iCl]);
               nofClusters_Muon[iLayer]++;
               std::sort(muonPoints.begin(), muonPoints.end());
            }
         }
      }
      muonPoints.clear();
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            Int_t nLayer = (iStation * station->GetNLayers()) + iLayer;
            fHM->P1("hsh_Much_Clusters")->Fill(nLayer, nofClusters[nLayer]);
            fHM->P1("hsh_Much_Muon_Clusters")->Fill(nLayer, nofClusters_Muon[nLayer]);
         }
      }
   }
}

void CbmLitClusteringQa::FillMuchPointsHistogram()
{
   if (NULL != fMuchPoints && fHM->Exists("hsh_Much_Points")){
      vector<Int_t> nofPoints;
      vector<Int_t> nofPoints_Muon;
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            nofPoints.push_back(0);
            nofPoints_Muon.push_back(0);
         }
      }
      for(Int_t iPoint = 0; iPoint < fNofMuchPoints; iPoint++){
         CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(iPoint);
         Int_t nStation = fMuchGeoScheme->GetStationIndex(point->GetDetectorId());
         if((nStation < 0) || (nStation >fMuchGeoScheme->GetNStations()))continue;
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStationByDetId(point->GetDetectorId());
         Int_t nofLayers = station->GetNLayers();
         Int_t nLayer = fMuchGeoScheme->GetLayerIndex(point->GetDetectorId());
         if((nLayer < 0) || (nLayer > nofLayers))continue;
         Int_t iLayer = (nStation * nofLayers) + nLayer;
         nofPoints[iLayer]++;
         CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(point->GetTrackID());
         if(fabs(mcTrack->GetPdgCode()) == 13){
            nofPoints_Muon[iLayer]++;
         }
      }
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            Int_t nLayer = (iStation * station->GetNLayers()) + iLayer;
            fHM->P1("hsh_Much_Points")->Fill(nLayer, nofPoints[nLayer]);
            fHM->P1("hsh_Much_Muon_Points")->Fill(nLayer, nofPoints_Muon[nLayer]);
         }
      }
   }
}

void CbmLitClusteringQa::FillMuchDigisHistogram()
{
   if (NULL != fMuchDigis && fHM->Exists("hsh_Much_Digis")){
      vector<Int_t> nofDigis;
      vector<Int_t> nofDigis_Muon;
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            nofDigis.push_back(0);
            nofDigis_Muon.push_back(0);
         }
      }
      for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
         CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
         Int_t nStation = fMuchGeoScheme->GetStationIndex(digi->GetDetectorId());
         if((nStation < 0) || (nStation >fMuchGeoScheme->GetNStations()))continue;
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStationByDetId(digi->GetDetectorId());
         Int_t nofLayers = station->GetNLayers();
         Int_t nLayer = fMuchGeoScheme->GetLayerIndex(digi->GetDetectorId());
         if((nLayer < 0) || (nLayer > nofLayers))continue;
         Int_t iLayer = (nStation * nofLayers) + nLayer;
         nofDigis[iLayer]++;
         CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
         for(Int_t i = 0; i < digiMatch->GetNPoints(); i++){
            CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(digiMatch->GetRefIndex(i));
            CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(point->GetTrackID());
            if(fabs(mcTrack->GetPdgCode()) == 13){
               nofDigis_Muon[iLayer]++;
            }
         }
      }
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            Int_t nLayer = (iStation * station->GetNLayers()) + iLayer;
            fHM->P1("hsh_Much_Digis")->Fill(nLayer, nofDigis[nLayer]);
            fHM->P1("hsh_Much_Muon_Digis")->Fill(nLayer, nofDigis_Muon[nLayer]);
         }
      }
   }
}

void CbmLitClusteringQa::FillDigiByPointHistogrm()
{
   if (NULL != fMuchDigis && NULL != fMuchPoints && fHM->Exists("hss_Much_NofDigisByPoint")){
      vector<Int_t> nofPointsByLayer;
      vector<Int_t> digisInPointsByLayer;
      for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
         for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
            nofPointsByLayer.push_back(0);
            digisInPointsByLayer.push_back(0);
         }
      }
      vector<Int_t> digisInPoint;
      for(Int_t iPoint = 0; iPoint < fNofMuchPoints; iPoint++){
         digisInPoint.push_back(0);
      }
      for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
         CbmMuchDigiMatch* digi = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
         if(isnan(digi->GetNPoints()))continue;
         for(Int_t iRef = 0; iRef < digi->GetNPoints(); iRef++){
            digisInPoint[digi->GetRefIndex(iRef)]++;
         }
      }
      for(Int_t iPoint = 0; iPoint < fNofMuchPoints; iPoint++){
         CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(iPoint);
         Int_t nStation = fMuchGeoScheme->GetStationIndex(point->GetDetectorId());
         if((nStation < 0) || (nStation >fMuchGeoScheme->GetNStations()))continue;
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStationByDetId(point->GetDetectorId());
         Int_t nofLayers = station->GetNLayers();
         Int_t nLayer = fMuchGeoScheme->GetLayerIndex(point->GetDetectorId());
         if((nLayer < 0) || (nLayer > nofLayers))continue;
         Int_t iLayer = (nStation * nofLayers) + nLayer;
         nofPointsByLayer[iLayer]++;
         digisInPointsByLayer[iLayer] += digisInPoint[iPoint];
      }
      for(Int_t iLayer = 0; iLayer < digisInPointsByLayer.size(); iLayer++){
         Float_t dBL = 0;
         if(nofPointsByLayer[iLayer] > 0){
            dBL = (Float_t)digisInPointsByLayer[iLayer] / (Float_t)nofPointsByLayer[iLayer];
         }
         else{
            dBL = 1;
         }
         fHM->H1("hss_Much_NofDigisByPoint")->Fill(iLayer, dBL);
      }
      nofPointsByLayer.clear();
      digisInPointsByLayer.clear();
      digisInPoint.clear();
   }
}

void CbmLitClusteringQa::FillMuchClusterQualityHistogram()
{
	if (NULL != fMuchClusters && fHM->Exists("hss_Much_ClusterToPointRatio") && fHM->Exists("hss_Much_ClusterQuality")){
		Int_t nofStations = fMuchGeoScheme->GetNStations();
		std::vector<Int_t> nofLayers;
		Int_t totalLayers = 0;
		for(Int_t iStation = 0; iStation < nofStations; iStation++){
			CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
			nofLayers.push_back(station->GetNLayers());
			totalLayers += station->GetNLayers();
		}
		std::vector<Float_t> clToPointsPerLayer;
		std::vector<Float_t> digiToClPerLayer;
		std::vector<Int_t> hitsPerLayer;
		for(Int_t i = 0; i < totalLayers; i++){
			clToPointsPerLayer.push_back(0);
			digiToClPerLayer.push_back(0);
			hitsPerLayer.push_back(0);
		}
		for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
		   CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
		   CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(cluster->GetDigiIndex(0));
			Int_t nStation = fMuchGeoScheme->GetStationIndex(digi->GetDetectorId());
			Int_t nLayer = fMuchGeoScheme->GetLayerIndex(digi->GetDetectorId());
			Int_t iLayer = (nStation * nofLayers[nStation]) + nLayer;
			if((iLayer < totalLayers) && (iLayer >= 0)){
			   clToPointsPerLayer[iLayer] += CalculateMuchClusterToPointRatio(iCl, fBestPoints[iCl]);
			   digiToClPerLayer[iLayer] += CalculateMuchDigisToClusterRatio(iCl, fBestPoints[iCl]);
			   hitsPerLayer[iLayer]++;
			}
		}
		for(Int_t i = 0; i < totalLayers; i++){
			if(hitsPerLayer[i] == 0){
				clToPointsPerLayer[i] = 100;
				digiToClPerLayer[i] = 100;
				hitsPerLayer[i] = 1;
			}
			if(hitsPerLayer[i] != 0){
			clToPointsPerLayer[i] = clToPointsPerLayer[i] / (Float_t)hitsPerLayer[i];
			digiToClPerLayer[i] = digiToClPerLayer[i] / (Float_t)hitsPerLayer[i];
			fHM->H1("hss_Much_ClusterToPointRatio")->Fill(i, (Float_t)clToPointsPerLayer[i]);
			fHM->H1("hss_Much_ClusterQuality")->Fill(i, (Float_t)digiToClPerLayer[i]);}
		}
		nofLayers.clear();
		clToPointsPerLayer.clear();
		digiToClPerLayer.clear();
		hitsPerLayer.clear();
	}
}

void CbmLitClusteringQa::FillMuchClusterQuality2DHistogram()
{
   Int_t nofSteps = fHM->H2("hsc_Much_ClusterQuality_2D")->GetNbinsY();
   for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
      CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
      Float_t rMin = station->GetRmin();
      Float_t rMax = station->GetRmax();
      Float_t step = (rMax - rMin) / nofSteps;
      Int_t nLayer;
      for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
         std::vector<Float_t> qualityClusterToPointsByRadiusStep;
         std::vector<Float_t> qualityDigisInClusterByRadiusStep;
         std::vector<Int_t> clustersByRadiusStep;
         for(Int_t iStep = 0; iStep < nofSteps; iStep++){
            qualityClusterToPointsByRadiusStep.push_back(0);
            qualityDigisInClusterByRadiusStep.push_back(0);
            clustersByRadiusStep.push_back(0);
         }
         for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
            CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(fBestPoints[iCl]);
            if((iStation == fMuchGeoScheme->GetStationIndex(point->GetDetectorId())) &&
                  (iLayer == fMuchGeoScheme->GetLayerIndex(point->GetDetectorId()))){
               Float_t xP = (point->GetXIn() + point->GetXOut()) / 2;
               Float_t yP = (point->GetYIn() + point->GetYOut()) / 2;
               Float_t rHit = sqrt((xP * xP) + (yP * yP));
               Int_t iStep;
               if((rHit < rMin) || (rHit > rMax)){
                  if(rHit < rMin){
                     iStep = 0;
                  }
                  else{
                     iStep = nofSteps - 1;
                  }
               }
               else{
                  iStep = (Int_t)((rHit - rMin) / step);
               }
               if((iStep >= nofSteps) || (iStep < 0))continue;
               if(iStep < nofSteps){
                  qualityClusterToPointsByRadiusStep[iStep] += CalculateMuchClusterToPointRatio(iCl, fBestPoints[iCl]);
                  qualityDigisInClusterByRadiusStep[iStep] += CalculateMuchDigisToClusterRatio(iCl, fBestPoints[iCl]);
                  clustersByRadiusStep[iStep]++;
               }
            }
         }
         nLayer = (iStation * station->GetNLayers()) + iLayer;
         for(Int_t iStep = 0; iStep < nofSteps; iStep++){
            if(clustersByRadiusStep[iStep] == 0){
               qualityClusterToPointsByRadiusStep[iStep] = 100;
               qualityDigisInClusterByRadiusStep[iStep] = 100;
               clustersByRadiusStep[iStep] = 1;
            }
            if(clustersByRadiusStep[iStep] != 0){qualityClusterToPointsByRadiusStep[iStep] = qualityClusterToPointsByRadiusStep[iStep] / clustersByRadiusStep[iStep];
            fHM->H2("hsc_Much_ClusterToPointRatio_2D")->Fill(nLayer, iStep, (Float_t)qualityClusterToPointsByRadiusStep[iStep]);}
            if(clustersByRadiusStep[iStep] != 0){qualityDigisInClusterByRadiusStep[iStep] = qualityDigisInClusterByRadiusStep[iStep] / clustersByRadiusStep[iStep];
            fHM->H2("hsc_Much_ClusterQuality_2D")->Fill(nLayer, iStep, (Float_t)qualityDigisInClusterByRadiusStep[iStep]);}
         }
         qualityClusterToPointsByRadiusStep.clear();
         qualityDigisInClusterByRadiusStep.clear();
         clustersByRadiusStep.clear();
      }
   }
}

void CbmLitClusteringQa::FillMuchMCPointsInClusterHistogrm()
{
	if (NULL != fMuchClusters && fHM->Exists("hss_Much_NofPointsInCluster")){
		Int_t nofStations = fMuchGeoScheme->GetNStations();
		std::vector<Int_t> nofLayers;
		Int_t totalLayers = 0;
		for(Int_t iStation = 0; iStation < nofStations; iStation++){
			CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
			nofLayers.push_back(station->GetNLayers());
			totalLayers += station->GetNLayers();
		}
		std::vector<Float_t> pointsInClustersPerLayer;
		std::vector<Int_t> clustersPerLayer;
		for(Int_t i = 0; i < totalLayers; i++){
			pointsInClustersPerLayer.push_back(0);
			clustersPerLayer.push_back(0);
		}
		for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
		   CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
		   CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(cluster->GetDigiIndex(0));
			Int_t nStation = fMuchGeoScheme->GetStationIndex(digi->GetDetectorId());
			Int_t nLayer = fMuchGeoScheme->GetLayerIndex(digi->GetDetectorId());
			Int_t iLayer = (nStation * nofLayers[nStation]) + nLayer;
			if((iLayer < totalLayers) && (iLayer >= 0)){
			   pointsInClustersPerLayer[iLayer] += GetNofPointsInCluster(iCl);
			   clustersPerLayer[iLayer]++;
			}
		}
		for(Int_t i = 0; i < totalLayers; i++){
			if(clustersPerLayer[i] == 0){
				pointsInClustersPerLayer[i] = 1;
				clustersPerLayer[i] = 1;
			}
			if(clustersPerLayer[i] != 0){
		pointsInClustersPerLayer[i] = pointsInClustersPerLayer[i] / (Float_t)clustersPerLayer[i];
			fHM->H1("hss_Much_NofPointsInCluster")->Fill(i, (Float_t)pointsInClustersPerLayer[i]);}
		}
		nofLayers.clear();
		pointsInClustersPerLayer.clear();
		clustersPerLayer.clear();
	}
}

void CbmLitClusteringQa::FillMuchMCPointsInCluster2DHistogrm()
{
   Int_t nofSteps = fHM->H2("hsc_Much_NofPointsInCluster_2D")->GetNbinsY();
   for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
      CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
      Float_t rMin = station->GetRmin();
      Float_t rMax = station->GetRmax();
      Float_t step = (rMax - rMin) / nofSteps;
      Int_t nLayer;
      for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
         std::vector<Float_t> pointsInClustersByRadiusStep;
         std::vector<Int_t> clustersByRadiusStep;
         for(Int_t iStep = 0; iStep < nofSteps; iStep++){
            pointsInClustersByRadiusStep.push_back(0);
            clustersByRadiusStep.push_back(0);
         }
         for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
            CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(fBestPoints[iCl]);
            if((iStation == fMuchGeoScheme->GetStationIndex(point->GetDetectorId())) &&
                  (iLayer == fMuchGeoScheme->GetLayerIndex(point->GetDetectorId()))){
               Float_t xP = (point->GetXIn() + point->GetXOut()) / 2;
               Float_t yP = (point->GetYIn() + point->GetYOut()) / 2;
               Float_t rHit = sqrt((xP * xP) + (yP * yP));
               Int_t iStep;
               if((rHit < rMin) || (rHit > rMax)){
                  if(rHit < rMin){
                     iStep = 0;
                  }
                  else{
                     iStep = nofSteps - 1;
                  }
               }
               else{
                  iStep = (Int_t)((rHit - rMin) / step);
               }
               if((iStep >= nofSteps) || (iStep < 0))continue;
               if(iStep < nofSteps){
                  pointsInClustersByRadiusStep[iStep] += GetNofPointsInCluster(iCl);
                  clustersByRadiusStep[iStep]++;
               }
            }
         }
         nLayer = (iStation * station->GetNLayers()) + iLayer;
         for(Int_t iStep = 0; iStep < nofSteps; iStep++){
            if(clustersByRadiusStep[iStep] == 0){
               pointsInClustersByRadiusStep[iStep] = 1;
               clustersByRadiusStep[iStep] = 1;
            }
            if(clustersByRadiusStep[iStep] != 0){pointsInClustersByRadiusStep[iStep] = pointsInClustersByRadiusStep[iStep] / clustersByRadiusStep[iStep];
            fHM->H2("hsc_Much_NofPointsInCluster_2D")->Fill(nLayer, iStep, (Float_t)pointsInClustersByRadiusStep[iStep]);}
         }
         pointsInClustersByRadiusStep.clear();
         clustersByRadiusStep.clear();
      }
   }
}

void CbmLitClusteringQa::CreateSingleLayerHistogram(Int_t nStation, Int_t nLayer,
		const string& histName, const string& histNameX, const string& histNameY, Int_t nofBins, Double_t minX, Double_t maxX)
{
	string nSt;
    string nL;
    std::stringstream stream;
    stream<<nStation;
    stream>>nSt;
    stream.clear();
    stream<<nLayer;
    stream>>nL;
	CreateH1F(histName + "Station_" + nSt + "_Layer_" + nL + "_Radius", histNameX, histNameY, nofBins, minX, maxX);
}

void CbmLitClusteringQa::FillMuchMCPointsByDigiHistogram()
{
	if (NULL != fMuchDigis && fHM->Exists("hss_Much_NofPointsInDigi")){
		Int_t nofStations = fMuchGeoScheme->GetNStations();
		std::vector<Int_t> nofLayers;
		Int_t totalLayers = 0;
		for(Int_t iStation = 0; iStation < nofStations; iStation++){
			CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
			nofLayers.push_back(station->GetNLayers());
			totalLayers += station->GetNLayers();
		}
		std::vector<Float_t> pointsByDigiPerLayer;
		std::vector<Int_t> digisPerLayer;
		for(Int_t i = 0; i < totalLayers; i++){
			pointsByDigiPerLayer.push_back(0);
			digisPerLayer.push_back(0);
		}
		for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
			CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
			Int_t detId = digi->GetDetectorId();
			Int_t nStation = fMuchGeoScheme->GetStationIndex(detId);
			Int_t nLayer = fMuchGeoScheme->GetLayerIndex(detId);
			Int_t iLayer = (nStation * nofLayers[nStation]) + nLayer;
			if((iLayer < totalLayers) && (iLayer >= 0)){
			   CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
			   pointsByDigiPerLayer[iLayer] += digiMatch->GetNPoints();
			   digisPerLayer[iLayer]++;
			}
		}
		for(Int_t i = 0; i < totalLayers; i++){
			if(digisPerLayer[i] == 0){
				pointsByDigiPerLayer[i] = 1;
				digisPerLayer[i] = 1;
			}
			if(digisPerLayer[i] != 0){
				pointsByDigiPerLayer[i] = pointsByDigiPerLayer[i] / (Float_t)digisPerLayer[i];
			fHM->H1("hss_Much_NofPointsInDigi")->Fill(i, (Float_t)pointsByDigiPerLayer[i]);}
		}
		nofLayers.clear();
		pointsByDigiPerLayer.clear();
		digisPerLayer.clear();
	}
}

void CbmLitClusteringQa::FillMuchMCPointsByDigi2DHistogram()
{
   Int_t nofSteps = fHM->H2("hsc_Much_NofPointsInDigi_2D")->GetNbinsY();
   for(Int_t iStation = 0; iStation < fMuchGeoScheme->GetNStations(); iStation++){
      CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStation(iStation);
      Float_t rMin = station->GetRmin();
      Float_t rMax = station->GetRmax();
      Float_t step = (rMax - rMin) / nofSteps;
      Int_t nLayer;
      for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++){
         std::vector<Float_t> pointsByDigiByRadiusStep;
         std::vector<Int_t> digisByRadiusStep;
         for(Int_t iStep = 0; iStep < nofSteps; iStep++){
            pointsByDigiByRadiusStep.push_back(0);
            digisByRadiusStep.push_back(0);
         }
         for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
            CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
            Int_t detId = digi->GetDetectorId();
            if((iStation == fMuchGeoScheme->GetStationIndex(detId)) &&
                  (iLayer == fMuchGeoScheme->GetLayerIndex(detId))){
               CbmMuchModuleGem* module = (CbmMuchModuleGem*) fMuchGeoScheme->GetModuleByDetId(detId);
               CbmMuchPad* pad = (CbmMuchPad*) module->GetPad(digi->GetChannelId());
               if(isnan(pad->GetX()) || (isnan(pad->GetY())))continue;
               Float_t rDigi = sqrt((pad->GetX() * pad->GetX()) + (pad->GetY() * pad->GetY()));
               if((rDigi < rMin) || (rDigi > rMax)){std::cout<<"Warning: wrong digi "<<iDigi<<" with radius "<<rDigi<<"\n";continue;}
               Int_t iStep = (Int_t)((rDigi - rMin) / step);
               if(iStep < nofSteps){
                  CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
                  pointsByDigiByRadiusStep[iStep] += digiMatch->GetNPoints();
                  digisByRadiusStep[iStep]++;
               }
            }
         }
         nLayer = (iStation * station->GetNLayers()) + iLayer;
         for(Int_t iStep = 0; iStep < nofSteps; iStep++){
            if(digisByRadiusStep[iStep] == 0){
               pointsByDigiByRadiusStep[iStep] = 1;
               digisByRadiusStep[iStep] = 1;
            }
            if(digisByRadiusStep[iStep] != 0){pointsByDigiByRadiusStep[iStep] = pointsByDigiByRadiusStep[iStep] / digisByRadiusStep[iStep];
            fHM->H2("hsc_Much_NofPointsInDigi_2D")->Fill(nLayer, iStep, (Float_t)pointsByDigiByRadiusStep[iStep]);}
         }
         pointsByDigiByRadiusStep.clear();
         digisByRadiusStep.clear();
      }
   }
}

void CbmLitClusteringQa::FillDigiByPointDistributionHistogrm()
{
   if (NULL != fMuchDigis && NULL != fMuchPoints && fHM->Exists("hss_Much_NofDigisByPoint_Total")){
      vector<Int_t> digisInPoint;
      for(Int_t iPoint = 0; iPoint < fNofMuchPoints; iPoint++){
         digisInPoint.push_back(0);
      }
      for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
         CbmMuchDigiMatch* digi = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
         if(isnan(digi->GetNPoints()))continue;
         for(Int_t iRef = 0; iRef < digi->GetNPoints(); iRef++){
            digisInPoint[digi->GetRefIndex(iRef)]++;
         }
      }
      for(Int_t iPoint = 0; iPoint < fNofMuchPoints; iPoint++){
         fHM->H1("hss_Much_NofDigisByPoint_Total")->Fill(digisInPoint[iPoint], 1);
      }
   }
}

void CbmLitClusteringQa::FillPointsInDigiDistributionHistogrm()
{
   if (NULL != fMuchDigis && NULL != fMuchPoints && fHM->Exists("hss_Much_NofPointsInDigi_Total")){
      for(Int_t iDigi = 0; iDigi < fNofMuchDigis; iDigi++){
         CbmMuchDigiMatch* digi = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
         if(isnan(digi->GetNPoints()))continue;
         fHM->H1("hss_Much_NofPointsInDigi_Total")->Fill(digi->GetNPoints(), 1);
      }
   }
}

void CbmLitClusteringQa::FillDigisInClusterDistributionHistogrm()
{
   if (NULL != fMuchDigis && NULL != fMuchClusters && fHM->Exists("hss_Much_NofDigisInCluster_Total")){
      for(Int_t iCl = 0; iCl < fNofMuchClusters; iCl++){
         CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
         if(isnan(cluster->GetNDigis()))continue;
         fHM->H1("hss_Much_NofDigisInCluster_Total")->Fill(cluster->GetNDigis(), 1);
      }
   }
}

void CbmLitClusteringQa::FillMuchResidualHistograms()
{
   if (NULL != fMuchPixelHits && NULL != fMuchPoints && fHM->Exists("hr_Much_ResidualX_2D") && fHM->Exists("hr_Much_ResidualY_2D")){
      for(Int_t iHit = 0; iHit < fNofMuchHits; iHit++){
         CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchPixelHits->At(iHit);
         if(isnan(hit->GetX()) || (isnan(hit->GetY())))continue;
         const CbmMuchPoint* muchPoint = static_cast<const CbmMuchPoint*>(fMuchPoints->At(fBestPointsForHits[/*hit->GetRefId()*/iHit]));
         Float_t xPoint = (muchPoint->GetXIn() + muchPoint->GetXOut()) / 2;
         Float_t yPoint = (muchPoint->GetYIn() + muchPoint->GetYOut()) / 2;
         Float_t residualX =  xPoint - hit->GetX();
         Float_t residualY =  yPoint - hit->GetY();
         CbmMuchStation* station = (CbmMuchStation*) fMuchGeoScheme->GetStationByDetId(hit->GetDetectorId());
         Int_t nofLayers = station->GetNLayers();
         Int_t nLayer = (fMuchGeoScheme->GetStationIndex(hit->GetDetectorId())) * nofLayers +
               fMuchGeoScheme->GetLayerIndex(hit->GetDetectorId());
         fHM->H2("hr_Much_ResidualX_2D")->Fill(nLayer, residualX, 1);
         fHM->H2("hr_Much_ResidualY_2D")->Fill(nLayer, residualY, 1);
         //---
         fHM->H2("hr_Much_PullX_2D")->Fill(nLayer, residualX, 1/hit->GetDx());
         fHM->H2("hr_Much_PullY_2D")->Fill(nLayer, residualY, 1/hit->GetDy());
         CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(muchPoint->GetTrackID());
         if(fabs(mcTrack->GetPdgCode()) == 13){
            fHM->H2("hr_Much_Muon_ResidualX_2D")->Fill(nLayer, residualX, 1);
            fHM->H2("hr_Much_Muon_ResidualY_2D")->Fill(nLayer, residualY, 1);
            fHM->H2("hr_Much_Muon_PullX_2D")->Fill(nLayer, residualX, 1);
            fHM->H2("hr_Much_Muon_PullY_2D")->Fill(nLayer, residualY, 1);
         }
      }
   }
}

void CbmLitClusteringQa::FillMuchAccuracyHistograms()
{
   for (Int_t i = 1; i <= fHM->H2("hr_Much_ResidualX_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream;
      stream << string(fHM->H2("hr_Much_ResidualX_2D")->GetName()) << "_layer" << i;
      TH1* lHistX = fHM->H2("hr_Much_ResidualX_2D")->ProjectionY(stream.str().c_str(), i, i);
      if (lHistX == NULL) continue;
      fHM->H1("h_Much_RMS_X")->Fill(i, lHistX->GetRMS());
      fHM->H1("h_Much_Mean_X")->Fill(i, lHistX->GetMean());
      lHistX->Fit("gaus");
      TF1* fitX = lHistX->GetFunction("gaus");
      Float_t sigma = (NULL != fitX) ? fitX->GetParameter(2) : 0.;
      fHM->H1("h_Much_Sigma_X")->Fill(i, sigma);
   }
   for (Int_t i = 1; i <= fHM->H2("hr_Much_ResidualY_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream;
      stream << string(fHM->H2("hr_Much_ResidualY_2D")->GetName()) << "_layer" << i;
      TH1* lHistY = fHM->H2("hr_Much_ResidualY_2D")->ProjectionY(stream.str().c_str(), i, i);
      if (lHistY == NULL) continue;
      fHM->H1("h_Much_RMS_Y")->Fill(i, lHistY->GetRMS());
      fHM->H1("h_Much_Mean_Y")->Fill(i, lHistY->GetMean());
      lHistY->Fit("gaus");
      TF1* fitY = lHistY->GetFunction("gaus");
      Float_t sigma = (NULL != fitY) ? fitY->GetParameter(2) : 0.;
      fHM->H1("h_Much_Sigma_Y")->Fill(i, sigma);
   }
   for (Int_t i = 1; i <= fHM->H2("hr_Much_PullX_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream;
      stream << string(fHM->H2("hr_Much_PullX_2D")->GetName()) << "_layer" << i;
      TH1* lHistX = fHM->H2("hr_Much_PullX_2D")->ProjectionY(stream.str().c_str(), i, i);
      if (lHistX == NULL) continue;
      fHM->H1("h_Much_PullRMS_X")->Fill(i, lHistX->GetRMS());
      fHM->H1("h_Much_PullMean_X")->Fill(i, lHistX->GetMean());
      lHistX->Fit("gaus");
      TF1* fitX = lHistX->GetFunction("gaus");
      Float_t sigma = (NULL != fitX) ? fitX->GetParameter(2) : 0.;
      fHM->H1("h_Much_PullSigma_X")->Fill(i, sigma);
      //---
      stream.clear();
      stream << string(fHM->H2("hr_Much_Muon_PullX_2D")->GetName()) << "_layer" << i;
      TH1* lHistMX = fHM->H2("hr_Much_Muon_PullX_2D")->ProjectionY(stream.str().c_str(), i, i);
      if (lHistMX == NULL) continue;
      fHM->H1("h_Much_Muon_PullRMS_X")->Fill(i, lHistMX->GetRMS());
      fHM->H1("h_Much_Muon_PullMean_X")->Fill(i, lHistMX->GetMean());
      lHistMX->Fit("gaus");
      TF1* fitMX = lHistMX->GetFunction("gaus");
      sigma = (NULL != fitMX) ? fitMX->GetParameter(2) : 0.;
      fHM->H1("h_Much_Muon_PullSigma_X")->Fill(i, sigma);
   }
   for (Int_t i = 1; i <= fHM->H2("hr_Much_PullY_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream;
      stream << string(fHM->H2("hr_Much_PullY_2D")->GetName()) << "_layer" << i;
      TH1* lHistY = fHM->H2("hr_Much_PullY_2D")->ProjectionY(stream.str().c_str(), i, i);
      if (lHistY == NULL) continue;
      fHM->H1("h_Much_PullRMS_Y")->Fill(i, lHistY->GetRMS());
      fHM->H1("h_Much_PullMean_Y")->Fill(i, lHistY->GetMean());
      lHistY->Fit("gaus");
      TF1* fitY = lHistY->GetFunction("gaus");
      Float_t sigma = (NULL != fitY) ? fitY->GetParameter(2) : 0.;
      fHM->H1("h_Much_PullSigma_Y")->Fill(i, sigma);
      //---
      stream.clear();
      stream << string(fHM->H2("hr_Much_Muon_PullY_2D")->GetName()) << "_layer" << i;
      TH1* lHistMY = fHM->H2("hr_Much_Muon_PullY_2D")->ProjectionY(stream.str().c_str(), i, i);
      if (lHistMY == NULL) continue;
      fHM->H1("h_Much_Muon_PullRMS_Y")->Fill(i, lHistMY->GetRMS());
      fHM->H1("h_Much_Muon_PullMean_Y")->Fill(i, lHistMY->GetMean());
      lHistMY->Fit("gaus");
      TF1* fitMY = lHistMY->GetFunction("gaus");
      sigma = (NULL != fitMY) ? fitMY->GetParameter(2) : 0.;
      fHM->H1("h_Much_Muon_PullSigma_Y")->Fill(i, sigma);
   }
}

ClassImp(CbmLitClusteringQa);
