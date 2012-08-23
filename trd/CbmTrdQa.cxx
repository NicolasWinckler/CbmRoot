#include "CbmTrdQa.h"
#include "CbmTrdDigiPar.h"
#include "CbmMCTrack.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TMath.h"
#include "TLegend.h"
#include "TF1.h"
#include "TH2.h"
#include "TH1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"
#include "TBox.h"
#include "TPaveText.h"
#include <iostream>
#include <cmath>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;

CbmTrdQa::CbmTrdQa() 
  : FairTask("TrdQa"),
    fMCTracks(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fHits(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fLayerMap(),
    fLayerMapIt(),
    fLayerPointMap(),
    fLayerPointMapIt(),
    fLayerHitMap(),
    fLayerHitMapIt(),
    fModulePointMap(),
    fModulePointMapIt(), 
    fModuleDigiMap(),
    fModuleDigiMapIt(), 
    fModuleClusterMap(),
    fModuleClusterMapIt(), 
    fModuleHitMap(),
    fModuleHitMapIt(), 
    fModuleGhostMap(),
    fModuleGhostMapIt(), 
    fModuleLostMap(),
    fModuleLostMapIt(),
    fModuleEfficiencyMap(),
    fModuleEfficiencyMapIt(),
    fTriggerThreshold(1e-6),
    fGeo(),
    fDistanceMcToHit(NULL),
    fDistanceMcToHitAll(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxPionGhost(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fdEdxElectronGhost(NULL),
    fdEdxPoint(NULL),
    fdEdxDigi(NULL),
    fdEdxCluster(NULL),
    fdEdxHit(NULL),
    fMultiHitsVsR(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL),
    fHitToPointEfficiencyVsR(NULL),
    fMultiHitsVsAlpha(NULL),
    fGhostHitVsAlpha(NULL),
    fLostPointVsAlpha(NULL),
    fHitToPointEfficiencyVsAlpha(NULL),
    fPositionResolutionShort(NULL),
    fPositionResolutionLong(NULL),  
    fClusterSize(NULL),  
    fPointsPerDigi(NULL),
    fDigiPerCluster(NULL),
    fClusterPerHit(NULL),
    fPRF_1D(NULL),
    fPRF_2D(NULL),
    fD(true),
    fT(true),
    fP(true),
    fC(true),
    fH(true)
{
}
CbmTrdQa::CbmTrdQa(const char *name, const char *title, const char *geo, Double_t triggerThreshold) 
  : FairTask("TrdQa"),
    fMCTracks(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fHits(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fLayerMap(),
    fLayerMapIt(),
    fLayerPointMap(),
    fLayerPointMapIt(),
    fLayerHitMap(),
    fLayerHitMapIt(),
    fModulePointMap(),
    fModulePointMapIt(), 
    fModuleDigiMap(),
    fModuleDigiMapIt(), 
    fModuleClusterMap(),
    fModuleClusterMapIt(), 
    fModuleHitMap(),
    fModuleHitMapIt(), 
    fModuleGhostMap(),
    fModuleGhostMapIt(), 
    fModuleLostMap(),
    fModuleLostMapIt(), 
    fModuleEfficiencyMap(),
    fModuleEfficiencyMapIt(),
    fTriggerThreshold(triggerThreshold),
    fGeo(geo),
    fDistanceMcToHit(NULL),
    fDistanceMcToHitAll(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxPionGhost(NULL),
    fdEdxPoint(NULL),
    fdEdxDigi(NULL),
    fdEdxCluster(NULL),
    fdEdxHit(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fdEdxElectronGhost(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL),
    fPositionResolutionShort(NULL),
    fPositionResolutionLong(NULL),
    fMultiHitsVsR(NULL),
    fHitToPointEfficiencyVsR(NULL),
    fGhostHitVsAlpha(NULL),
    fMultiHitsVsAlpha(NULL),
    fLostPointVsAlpha(NULL),
    fHitToPointEfficiencyVsAlpha(NULL),
    fClusterSize(NULL),  
    fPointsPerDigi(NULL),
    fDigiPerCluster(NULL),
    fClusterPerHit(NULL),
    fPRF_1D(NULL),
    fPRF_2D(NULL),
    fD(true),
    fT(true),
    fP(true),
    fC(true),
    fH(true)
{
}
CbmTrdQa::~CbmTrdQa()
{  
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  if(fMCTracks){
    fMCTracks->Clear("C");
    fMCTracks->Delete();
    delete fMCTracks;
  }
  if(fPoints){
    fPoints->Delete();
    delete fPoints;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }
  if(fHits){
    fHits->Delete();
    delete fHits;
  }
  if(fDigiPar)
    delete fDigiPar;
  
  for (fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); ++fLayerMapIt) {
    delete fLayerMapIt->second;
  } 
  fLayerMap.clear();
  for (fLayerPointMapIt = fLayerPointMap.begin();
       fLayerPointMapIt != fLayerPointMap.end(); ++fLayerPointMapIt) {
    delete fLayerPointMapIt->second;
  } 
  fLayerPointMap.clear();
  for (fLayerHitMapIt = fLayerHitMap.begin();
       fLayerHitMapIt != fLayerHitMap.end(); ++fLayerHitMapIt) {
    delete fLayerHitMapIt->second;
  } 
  fLayerHitMap.clear();
  for (fModulePointMapIt = fModulePointMap.begin();
       fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
    fModulePointMapIt->second.clear();
  } 
  fModulePointMap.clear();

  for (fModuleDigiMapIt = fModuleDigiMap.begin();
       fModuleDigiMapIt != fModuleDigiMap.end(); ++fModuleDigiMapIt) {
    fModuleDigiMapIt->second.clear();
  } 
  fModuleDigiMap.clear();

  for (fModuleClusterMapIt = fModuleClusterMap.begin();
       fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
    fModuleClusterMapIt->second.clear();
  } 
  fModuleClusterMap.clear();

  for (fModuleHitMapIt = fModuleHitMap.begin();
       fModuleHitMapIt != fModuleHitMap.end(); ++fModuleHitMapIt) {
    fModuleHitMapIt->second.clear();
  } 
  fModuleHitMap.clear();

  for (fModuleGhostMapIt = fModuleGhostMap.begin();
       fModuleGhostMapIt != fModuleGhostMap.end(); ++fModuleGhostMapIt) {
    delete fModuleGhostMapIt->second;
  } 
  fModuleGhostMap.clear();

  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); ++fModuleLostMapIt) {
    delete fModuleLostMapIt->second;
  } 
  fModuleLostMap.clear();

  for (fModuleEfficiencyMapIt = fModuleEfficiencyMap.begin();
       fModuleEfficiencyMapIt != fModuleEfficiencyMap.end(); ++fModuleEfficiencyMapIt) {
    delete fModuleEfficiencyMapIt->second;
  } 
  fModuleEfficiencyMap.clear();

  delete fDistanceMcToHit;
  delete fDistanceMcToHitAll;
  delete fdEdxPionMc;
  delete fdEdxPionHit;
  delete fdEdxPionGhost;
  delete fdEdxElectronMc;
  delete fdEdxElectronHit;
  delete fdEdxElectronGhost;
  delete fdEdxPoint;
  delete fdEdxDigi;
  delete fdEdxCluster;
  delete fdEdxHit;
  delete fMultiHitsVsR;
  delete fGhostHitVsR;
  delete fLostPointVsR;
  delete fPositionResolutionShort;
  delete fPositionResolutionLong;
  delete fHitToPointEfficiencyVsR;
  delete fMultiHitsVsAlpha;
  delete fGhostHitVsAlpha;
  delete fLostPointVsAlpha;
  delete fHitToPointEfficiencyVsAlpha;
  delete fClusterSize;
  delete fPointsPerDigi;
  delete fDigiPerCluster;
  delete fClusterPerHit;
  delete fPRF_1D;
  delete fPRF_2D;
  //delete fLayerDummy;
  
}
void CbmTrdQa::SetParContainers()
{
    cout<<" * CbmTrdQa * :: SetParContainers() "<<endl;
    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));
}
InitStatus CbmTrdQa::ReInit()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar")); 
  return kSUCCESS;
}
InitStatus CbmTrdQa::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();  
  fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-W CbmTrdQa::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fT = false;
  }
  fPoints =(TClonesArray *)  ioman->GetObject("TrdPoint");
  if ( ! fPoints ) {
    cout << "-W CbmTrdQa::Init: No TrdPoint array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fP = false;
  }
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdQa::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fD = false;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdQa::Init: No TrdCluster array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fC = false;
  }
  fHits =(TClonesArray *)  ioman->GetObject("TrdHit");
  if ( ! fHits ) {
    cout << "-W CbmTrdQa::Init: No TrdHits array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fH = false;
  }
  fGeoHandler->Init();

  fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
  fLayerDummy->GetXaxis()->SetLabelSize(0.02);
  fLayerDummy->GetYaxis()->SetLabelSize(0.02);
  fLayerDummy->GetZaxis()->SetLabelSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleOffset(1.5);
  fLayerDummy->GetYaxis()->SetTitleSize(0.02);
  fLayerDummy->GetYaxis()->SetTitleOffset(2);
  fLayerDummy->GetZaxis()->SetTitleSize(0.02);
  fLayerDummy->GetZaxis()->SetTitleOffset(-2);
  fDistanceMcToHit = new TH1I("DistanceMcToHit", "DistanceMcToHit", 1500, 0, 150);
  fDistanceMcToHit->SetXTitle("#Delta r (MC-Hit) pairs [cm]");
  fDistanceMcToHit->SetYTitle("#");
  fDistanceMcToHit->SetMarkerStyle(24);
  fDistanceMcToHitAll = new TH1I("DistanceMcToHitAll", "DistanceMcToHitAll", 1500, 0, 150);
  fDistanceMcToHitAll->SetXTitle("#Delta r (MC-Hit) all [cm]");
  fDistanceMcToHitAll->SetYTitle("#");
  fDistanceMcToHitAll->SetMarkerStyle(24);
  fdEdxPionMc = new TH1F("dEdxPionMc", "dEdxPionMc", 250, 0, 30.0e-6);
  fdEdxPionMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxPionMc->SetYTitle("#");
  fdEdxPionMc->SetLineColor(1);
  fdEdxPionMc->SetLineStyle(1);
  fdEdxPionHit = new TH1F("dEdxPionHit", "dEdxPionHit", 250, 0, 30.0e-6);
  fdEdxPionHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxPionHit->SetYTitle("#");
  fdEdxPionHit->SetLineColor(1);
  fdEdxPionHit->SetLineStyle(2);
  fdEdxPionGhost = new TH1F("dEdxPionGhost", "dEdxPionGhost", 250, 0, 30.0e-6);
  fdEdxPionGhost->SetXTitle("dE/dx Ghost [GeV]");
  fdEdxPionGhost->SetYTitle("#");
  fdEdxPionGhost->SetLineColor(1);
  fdEdxPionGhost->SetLineStyle(3);
  fdEdxElectronMc = new TH1F("dEdxElectronMc", "dEdxElectronMc", 250, 0, 30.0e-6);
  fdEdxElectronMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxElectronMc->SetYTitle("#");
  fdEdxElectronMc->SetLineColor(2);
  fdEdxElectronMc->SetLineStyle(1);
  fdEdxElectronHit = new TH1F("dEdxElectronHit", "dEdxElectronHit", 250, 0, 30.0e-6);
  fdEdxElectronHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxElectronHit->SetYTitle("#");
  fdEdxElectronHit->SetLineColor(2);
  fdEdxElectronHit->SetLineStyle(2);
  fdEdxElectronGhost = new TH1F("dEdxElectronGhost", "dEdxElectronGhost", 250, 0, 30.0e-6);
  fdEdxElectronGhost->SetXTitle("dE/dx Ghost [GeV]");
  fdEdxElectronGhost->SetYTitle("#");
  fdEdxElectronGhost->SetLineColor(2);
  fdEdxElectronGhost->SetLineStyle(3);
  fdEdxPoint = new TH1F("dEdxPoint", "dEdxPoint", 1.0e3, 0, 30.0e-6);
  fdEdxPoint->SetXTitle("dE/dx Hit [GeV]");
  fdEdxPoint->SetYTitle("#");
  fdEdxDigi = new TH1F("dEdxDigi", "dEdxDigi", 1.0e3, 0, 30.0e-6);
  fdEdxDigi->SetXTitle("dE/dx Hit [GeV]");
  fdEdxDigi->SetYTitle("#");
  fdEdxCluster = new TH1F("dEdxCluster", "dEdxCluster", 1.0e3, 0, 30.0e-6);
  fdEdxCluster->SetXTitle("dE/dx Hit [GeV]");
  fdEdxCluster->SetYTitle("#");
  fdEdxHit = new TH1F("dEdxHit", "dEdxHit", 1.0e3, 0, 30.0e-6);
  fdEdxHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxHit->SetYTitle("#");
  fMultiHitsVsR = new TProfile("MultiHitVsR", "MultiHitVsR", 785, 0, 785);
  fMultiHitsVsR->SetXTitle("r [cm]");
  fMultiHitsVsR->SetYTitle("hits per point on same pad/ all points per module [%]");
  fMultiHitsVsR->SetMarkerStyle(24);
  fMultiHitsVsR->SetMarkerColor(4);
  fMultiHitsVsR->SetLineColor(4);
  fGhostHitVsR = new TProfile("GhostHitVsR", "GhostHitVsR", 785, 0, 785);
  fGhostHitVsR->SetXTitle("r [cm]");
  fGhostHitVsR->SetYTitle("left over hits / all points per module [%]");
  fGhostHitVsR->SetMarkerStyle(24);
  fGhostHitVsR->SetMarkerColor(2);
  fGhostHitVsR->SetLineColor(2);
  fLostPointVsR = new TProfile("LostPointVsR", "LostPointVsR", 785, 0, 785);
  fLostPointVsR->SetXTitle("r [cm]");
  fLostPointVsR->SetYTitle("left over points / all points per module [%]");
  fLostPointVsR->SetMarkerStyle(24);
  fLostPointVsR->SetMarkerColor(3);
  fLostPointVsR->SetLineColor(3);
  fHitToPointEfficiencyVsR = new TProfile("fHitToPointEfficiencyVsR", "fHitToPointEfficiencyVsR", 785, 0, 785);
  fHitToPointEfficiencyVsR->SetXTitle("r [cm]");
  fHitToPointEfficiencyVsR->SetYTitle("point hit pairs / all points per module [%]");
  fHitToPointEfficiencyVsR->SetMarkerStyle(24);
  fHitToPointEfficiencyVsR->SetMarkerColor(1);
  fHitToPointEfficiencyVsR->SetLineColor(1);
  fMultiHitsVsAlpha = new TProfile("MultiHitVsAlpha", "MultiHitVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fMultiHitsVsAlpha->SetXTitle("#alpha [mrad]");
  fMultiHitsVsAlpha->SetYTitle("hits per point on same pad/ all points per module [%]");
  fMultiHitsVsAlpha->SetMarkerStyle(24);
  fMultiHitsVsAlpha->SetMarkerColor(4);
  fMultiHitsVsAlpha->SetLineColor(4);
  fGhostHitVsAlpha = new TProfile("GhostHitVsAlpha", "GhostHitVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fGhostHitVsAlpha->SetXTitle("#alpha [mrad]");
  fGhostHitVsAlpha->SetYTitle("left over hits / all points per module [%]");
  fGhostHitVsAlpha->SetMarkerStyle(24);
  fGhostHitVsAlpha->SetMarkerColor(2);
  fGhostHitVsAlpha->SetLineColor(2);
  fLostPointVsAlpha = new TProfile("LostPointVsAlpha", "LostPointVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fLostPointVsAlpha->SetXTitle("#alpha [mrad]");
  fLostPointVsAlpha->SetYTitle("left over points / all points per module [%]");
  fLostPointVsAlpha->SetMarkerStyle(24);
  fLostPointVsAlpha->SetMarkerColor(3);
  fLostPointVsAlpha->SetLineColor(3);
  fHitToPointEfficiencyVsAlpha = new TProfile("fHitToPointEfficiencyVsAlpha", "fHitToPointEfficiencyVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fHitToPointEfficiencyVsAlpha->SetXTitle("#alpha [mrad]");
  fHitToPointEfficiencyVsAlpha->SetYTitle("point hit pairs / all points per module [%]");
  fHitToPointEfficiencyVsAlpha->SetMarkerStyle(24);
  fHitToPointEfficiencyVsAlpha->SetMarkerColor(1);
  fHitToPointEfficiencyVsAlpha->SetLineColor(1);
  fPositionResolutionShort = new TH1I("fPositionResolutionShort", "fPositionResolutionShort", 20000, -10, 10);
  fPositionResolutionShort->SetXTitle("position deviation along short pad side [cm]");
  fPositionResolutionShort->SetYTitle("#");
  fPositionResolutionLong = new TH1I("fPositionResolutionLong", "fPositionResolutionLong", 20000, -10, 10);
  fPositionResolutionLong->SetXTitle("position deviation along long pad side  [cm]");
  fPositionResolutionLong->SetYTitle("#");
  fClusterSize = new TH1I("fClusterSize","fClusterSize",21,-0.5,20.5);
  fClusterSize->SetXTitle("cluster size [pads]");
  fClusterSize->SetYTitle("#");
  fPointsPerDigi = new TH1I("fPointsPerDigi","fPointsPerDigi",101,-0.5,100.5);
  fPointsPerDigi->SetXTitle("point per digi");
  fPointsPerDigi->SetYTitle("#");
  fDigiPerCluster = new TH1I("fDigiPerCluster","fDigiPerCluster",101,-0.5,100.5);
  fDigiPerCluster->SetXTitle("digi per cluster");
  fDigiPerCluster->SetYTitle("#");
  fClusterPerHit = new TH1I("fClusterPerHit","fClusterPerHit",101,-0.5,100.5);
  fClusterPerHit->SetXTitle("cluster per hit");
  fClusterPerHit->SetYTitle("#");
  fPRF_1D = new TProfile("fPRF_1D","fPRF_1D",300,-1.5,1.5);
  fPRF_2D = new TH2I("fPRF_2D","fPRF_2D",300,-1.5,1.5,100,0,1);

  return kSUCCESS;
}
void CbmTrdQa::Exec(Option_t * option)
{
  Bool_t samePadMerge = false;//true;
  TStopwatch timer;
  timer.Start();
  cout << endl << "==================CbmTrdQa===================" << endl;
  Int_t nEntries(0), iTrack(-1), moduleId(-1), Station(-1), Layer(-1), combiId(-1), dEdx(0), rPos(0), PDG(-1), ghostCount(0), lostCount(0);
  TString title;
  Bool_t debug = false;
  CbmMCTrack *track = NULL;
  CbmTrdPoint *point = NULL;
  CbmTrdDigi *digi = NULL;
  CbmTrdCluster *cluster = NULL;
  CbmTrdHit *hit = NULL;
  CbmTrdPoint *point_temp = NULL;
  // MC-points
  if (fP && fT) {
    nEntries = fPoints->GetEntries();
    printf("%i Points\n",nEntries);
    for (Int_t iPoint=0; iPoint < nEntries; iPoint++ ) {
      point = (CbmTrdPoint*) fPoints->At(iPoint);
      fdEdxPoint->Fill(point->GetEnergyLoss());
      moduleId = point->GetDetectorID();
      Station  = fGeoHandler->GetStation(moduleId);
      Layer    = fGeoHandler->GetLayer(moduleId);
      //printf ("P det: %i S%i L%i  \n",moduleId,Station,Layer);//GetDetId()); 
      combiId = 10 * Station + Layer;
      iTrack = point->GetTrackID();
      track = (CbmMCTrack*) fMCTracks->At(iTrack);
      PDG = track->GetPdgCode();
      fModuleInfo = fDigiPar->GetModule(moduleId);
      if (fModuleGhostMap.find(moduleId) == fModuleGhostMap.end()){
	title.Form("G%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleId,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleGhostMap[moduleId] = new TH1F(title,title,200,0,200);
	fModuleGhostMap[moduleId]->SetXTitle("left over points / all points [%]");
	fModuleGhostMap[moduleId]->SetYTitle("#");
	title.Form("L%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleId,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleLostMap[moduleId]  = new TH1F(title,title,200,0,200);
	fModuleLostMap[moduleId]->SetXTitle("left over hits / all points [%]");
	fModuleLostMap[moduleId]->SetYTitle("#");
	title.Form("E%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleId,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleEfficiencyMap[moduleId]  = new TH1F(title,title,200,0,200);
	fModuleEfficiencyMap[moduleId]->SetXTitle("found point hit pairs / all points [%]");
	fModuleEfficiencyMap[moduleId]->SetYTitle("#");
      }
      fModulePointMap[moduleId].push_back(iPoint);       
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] =  new TCanvas(title,title,1200,1000);
	fLayerDummy->DrawCopy();
      }
    }
  }
  // Digis
  if (fD) {
    nEntries = fDigis->GetEntries();
    printf("%i Digis \n",nEntries);
    for (Int_t iDigi=0; iDigi < nEntries; iDigi++ ) {
      digi = (CbmTrdDigi*) fDigis->At(iDigi);
      fPointsPerDigi->Fill((Int_t)digi->GetMCIndex().size());
      fdEdxDigi->Fill(digi->GetCharge());
      moduleId = digi->GetDetId();
      Station  = fGeoHandler->GetStation(moduleId);
      Layer    = fGeoHandler->GetLayer(moduleId);
      //printf ("D det: %i S%i L%i  \n",moduleId,Station,Layer);//GetDetId()); 
      combiId = 10 * Station + Layer;
      fModuleInfo = fDigiPar->GetModule(moduleId);
      if (fModuleDigiMap.find(moduleId) == fModuleDigiMap.end()){
      }
      fModuleDigiMap[moduleId].push_back(iDigi);
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] =  new TCanvas(title,title,1200,1000);
	fLayerDummy->DrawCopy();
      }
    }
  }
  
  //Clusters
  if (fC) {
    nEntries = fClusters->GetEntries();
    printf("%i\n",nEntries);
    for (Int_t iCluster=0; iCluster < nEntries; iCluster++ ) {
      cluster = (CbmTrdCluster*) fClusters->At(iCluster);
      fdEdxCluster->Fill(cluster->GetCharge());
      fClusterSize->Fill(cluster->GetNofDigis());
      fDigiPerCluster->Fill(cluster->GetNofDigis());
      if (cluster->GetNofDigis() >= 3) {
	Double_t qLeft(0.0), qCenter(.0), qRight(0.0);
	for (Int_t i = 0; i < cluster->GetNofDigis(); i++) {
	  digi = (CbmTrdDigi*) fDigis->At(cluster->GetDigi(i));
	 
	}
	//fPRF_2D->Fill();
	//fPRF_1D->Fill();
      }
      //moduleId = cluster->GetDetId();
      //Station  = fGeoHandler->GetStation(moduleId);
      //Layer    = fGeoHandler->GetLayer(moduleId);
      //printf ("det: %i S%i L%i  \n",moduleId,Station,Layer);//GetDetId()); 
      //combiId = 10 * Station + Layer;   
      //fModuleInfo = fDigiPar->GetModule(moduleId);
      //if (fModuleClusterMap.find(moduleId) == fModuleClusterMap.end()){
      //fModuleClusterMap[moduleId].push_back(iCluster);
      //}
    }
  }
  
  //Hits
  if (fH) {
    nEntries = fHits->GetEntries();
    printf("%i Hits\n",nEntries);
    for (Int_t iHit=0; iHit < nEntries; iHit++ ) {
      hit = (CbmTrdHit*) fHits->At(iHit);
      fdEdxHit->Fill(hit->GetELoss());
      moduleId = hit->GetDetectorId();//hit->GetDetId();     
      Station  = fGeoHandler->GetStation(moduleId);
      Layer    = fGeoHandler->GetLayer(moduleId);
      //printf ("H det: %i S%i L%i  \n",moduleId,Station,Layer);//GetDetId()); 
      combiId = 10 * Station + Layer;
      fModuleInfo = fDigiPar->GetModule(moduleId);
      if (fModuleHitMap.find(moduleId) == fModuleHitMap.end()){
      }
      fModuleHitMap[moduleId].push_back(iHit);
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] = new TCanvas(title,title,1200,1000);
	fLayerMap[combiId]->cd();
	fLayerDummy->DrawCopy();
      }
    }
  }
  Int_t allPoints(0), allHits(0);
  for (fModulePointMapIt = fModulePointMap.begin(); // Loop for all module ids which have mc points
       fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
    if (fModuleHitMap.find(fModulePointMapIt->first) == fModuleHitMap.end()) {
      printf("module %i not found in hit map\n",fModulePointMapIt->first);
    } 
    else {
      //fModuleHitMapIt = fModuleHitMap[fModulePointMapIt->first];
      moduleId = fModulePointMapIt->first;
      fModuleInfo = fDigiPar->GetModule(moduleId);
      allPoints = Int_t(fModulePointMapIt->second.size());
      allHits = Int_t(fModuleHitMap[moduleId].size());
      Double_t multiHitCounterModule = 0;
      Double_t r = sqrt(fModuleInfo->GetX() * fModuleInfo->GetX() + fModuleInfo->GetY() * fModuleInfo->GetY()); // radial position of the module center
      Double_t alpha = atan(r / fModuleInfo->GetZ()) * 1000.; //[mrad]
      //printf("%.2f %.2f %.2f %.2f %.2f\n",fModuleInfo->GetX(), fModuleInfo->GetY(), fModuleInfo->GetZ(), r, alpha);
      //for (Int_t iPoint = 0; iPoint < Int_t(fModulePointMapIt->second.size()); iPoint++) {
      for (Int_t iPoint = Int_t(fModulePointMapIt->second.size())-1; iPoint >= 0; iPoint--) { // its better to go backwards since found point and hit pairs are erased -> vector sice is dynamic
	point = (CbmTrdPoint*) fPoints->At(fModulePointMapIt->second[iPoint]);
	iTrack = point->GetTrackID();
	track = (CbmMCTrack*) fMCTracks->At(iTrack);
	Int_t Pdg_code = track->GetPdgCode();
	
	/*
	  Double_t p_global[3] = {0.5 * (point->GetXIn() + point->GetXOut()),
	  0.5 * (point->GetYIn() + point->GetYOut()),
	  0.5 * (point->GetZIn() + point->GetZOut())};
	*/
	
	Double_t p_global[3] = {point->GetXOut(),
				point->GetYOut(),
				point->GetZOut()};
	
	gGeoManager->FindNode(p_global[0], p_global[1], p_global[2]);
	const Double_t *global_point = gGeoManager->GetCurrentPoint();
	Double_t p_local[3] = {0.0};
	gGeoManager->MasterToLocal(p_global, p_local);
	//p_local[0] = p_global[0] - fModuleInfo->GetX();
	//p_local[1] = p_global[1] - fModuleInfo->GetY();
	Int_t xPpad(0), yPpad(0); // correspoondig digi from point point of view
	Double_t xPPadSize(0), yPPadSize(0); // correspoondig pad sizes from point point of view
	GetPadInfos( moduleId, p_local[0], p_local[1], xPpad, yPpad, xPPadSize, yPPadSize);
	if (point->GetEnergyLoss() > 0.0) {
	  if (Pdg_code == 211 || Pdg_code == -211)
	    fdEdxPionMc->Fill(point->GetEnergyLoss());
	  if (Pdg_code == 11 || Pdg_code == -11 )
	    fdEdxElectronMc->Fill(point->GetEnergyLoss());
	}
	Double_t minimumDistance = sqrt(xPPadSize * xPPadSize + yPPadSize * yPPadSize);
	Double_t xDiviation(0), yDiviation(0);
	Int_t minDHitId = -1;
	Double_t mergedELoss = 0.0;
	Double_t hitELoss = 0.0;
	Int_t multiHitCounter = 0;
	for (Int_t iHit = 0; iHit < Int_t(fModuleHitMap[moduleId].size()); iHit++) {
	  hit = (CbmTrdHit*) fHits->At(fModuleHitMap[moduleId][iHit]);
	  Double_t h_global[3] = {hit->GetX(),
				  hit->GetY(),
				  hit->GetZ()};
	  gGeoManager->FindNode(h_global[0], h_global[1], h_global[2]);
	  const Double_t *global_hit = gGeoManager->GetCurrentPoint();
	  Double_t h_local[3] = {0.0};
	  gGeoManager->MasterToLocal(h_global, h_local);
	  //h_local[0] = h_global[0] - fModuleInfo->GetX();
	  //h_local[1] = h_global[1] - fModuleInfo->GetY();
	  Int_t xHpad(0), yHpad(0); // correspoondig digi from hit point of view
	  Double_t xHPadSize(0), yHPadSize(0); // correspoondig pad sizes from hit point of view
	  GetPadInfos( moduleId, h_local[0], h_local[1], xHpad, yHpad, xHPadSize, yHPadSize);
	  //point_temp = new CbmTrdPoint(iTrack, hit->GetDetId(), );
	  //r = sqrt(h_global[0] * h_global[0] + h_global[1] * h_global[1]);
	  //r = sqrt(hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY());
	  Double_t distance = sqrt(
				   (p_local[0]-h_local[0]) * (p_local[0]-h_local[0]) 
				   + 
				   (p_local[1]-h_local[1]) * (p_local[1]-h_local[1]));
	  fDistanceMcToHitAll->Fill(distance);
	  if (distance < minimumDistance || (xHpad == xPpad && yHpad == yPpad)) { // distance between point and hit is smaller one pad diagonal or on the same pad
	    minimumDistance = distance;
	    minDHitId = iHit;
	    xDiviation = p_local[0]-h_local[0];
	    yDiviation = p_local[1]-h_local[1];
	    hitELoss = hit->GetELoss();
	    if (xHpad == xPpad && yHpad == yPpad){
	      multiHitCounter++;
	      if (samePadMerge)
		mergedELoss += hit->GetELoss();
	    }
	  }
	}
	if (minDHitId > -1){ // If minimumDistance < pad diagonal -> delete point and hit from map
	  multiHitCounterModule += multiHitCounter / Float_t(fModulePointMapIt->second.size());
	  fModuleHitMap[moduleId].erase(fModuleHitMap[moduleId].begin() + minDHitId);
	  fModulePointMapIt->second.erase(fModulePointMapIt->second.begin() + iPoint);
	  if (samePadMerge){
	    //if (mergedELoss == 0.0)
	    //printf("(%02i, %02i) (%02i, %02i) %.2e  %.2e\n",xPpad, yPpad, fModuleInfo->GetnCol(), fModuleInfo->GetnRow(), mergedELoss, point->GetEnergyLoss());
	    if (Pdg_code == 211 || Pdg_code == -211) // pdg code from associated point is used
	      fdEdxPionHit->Fill(mergedELoss);
	    if (Pdg_code == 11 || Pdg_code == -11 )
	      fdEdxElectronHit->Fill(mergedELoss);
	  }
	  else {
	    if (Pdg_code == 211 || Pdg_code == -211) // pdg code from associated point is used
	      fdEdxPionHit->Fill(hitELoss);
	    if (Pdg_code == 11 || Pdg_code == -11 )
	      fdEdxElectronHit->Fill(hitELoss);
	  }
	  fDistanceMcToHit->Fill(minimumDistance);
	  if (xPPadSize > yPPadSize) { // ok to use here only the pad size of the point because the sapect ration does not change within one module
	    fPositionResolutionShort->Fill(yDiviation);
	    fPositionResolutionLong->Fill(xDiviation);
	  }
	  else {
	    fPositionResolutionShort->Fill(xDiviation);
	    fPositionResolutionLong->Fill(yDiviation);
	  }
	}		
      }
      fMultiHitsVsAlpha->Fill(alpha, 100. * multiHitCounterModule / Float_t(allPoints));
      fMultiHitsVsR->Fill(r ,100. * multiHitCounterModule / Float_t(allPoints));
      // left over points == lost
      fModuleLostMap[moduleId]->Fill( 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      fLostPointVsR->Fill(r, 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      fLostPointVsAlpha->Fill(alpha, 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      // left over hits == ghosts
      fModuleGhostMap[moduleId]->Fill( 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      fGhostHitVsR->Fill(r, 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      fGhostHitVsAlpha->Fill(alpha, 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      // found point hit pairs / all points == detection efficiency
      fModuleEfficiencyMap[moduleId]->Fill(100. * ( allPoints - Float_t(fModulePointMapIt->second.size() )) / Float_t(allPoints));
      fHitToPointEfficiencyVsR->Fill(r, 100. * ( allPoints - Float_t(fModulePointMapIt->second.size() )) / Float_t(allPoints));
      fHitToPointEfficiencyVsAlpha->Fill(alpha, 100. * ( allPoints - Float_t(fModulePointMapIt->second.size() )) / Float_t(allPoints));
    }
  }
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");
}
void CbmTrdQa::SaveHistos()
{
  gDirectory->pwd();
  if (!gDirectory->Cd("TrdQa")) 
    gDirectory->mkdir("TrdQa");
  gDirectory->Cd("TrdQa");
  TCanvas *c = new TCanvas("c","c",800,600);
  c->cd();
  TLegend *l = new TLegend(0.65,0.65,0.85,0.85);
  l->SetTextSize(0.025);
  l->SetTextColor(1);
  l->SetFillStyle(0);
  l->SetLineStyle(0);
  l->SetLineColor(0);
  fdEdxPionMc->DrawCopy();
  l->AddEntry(fdEdxElectronMc,"MC electron","l");
  fdEdxElectronMc->DrawCopy("same");
  l->AddEntry(fdEdxElectronHit,"Hit electron","l");
  fdEdxElectronHit->DrawCopy("same");
  l->AddEntry(fdEdxPionMc,"MC pion","l");
  fdEdxPionMc->DrawCopy("same");
  l->AddEntry(fdEdxPionHit,"Hit pion","l");
  fdEdxPionHit->DrawCopy("same");
  l->Draw("same");
  c->SaveAs("pics/TrdQadEdx.pdf");
  /*
    delete l;
    TLegend *l = new TLegend(0.65,0.65,0.85,0.85);
    l->SetTextSize(0.025);
    l->SetTextColor(1);
    l->SetFillStyle(0);
    l->SetLineStyle(0);
    l->SetLineColor(0);
  */
  l->Clear();
  l->AddEntry(fHitToPointEfficiencyVsR,"Hit to point efficiency","p");
  fHitToPointEfficiencyVsR->DrawCopy();
  l->AddEntry(fGhostHitVsR,"Ghost hits","p");
  fGhostHitVsR->DrawCopy("same");
  l->AddEntry(fLostPointVsR,"Lost points","p");
  fLostPointVsR->DrawCopy("same");
  l->Draw("same");
  c->SaveAs("pics/TrdQaStatisticVsR.pdf");
  fHitToPointEfficiencyVsAlpha->DrawCopy();
  fGhostHitVsAlpha->DrawCopy("same");
  fLostPointVsAlpha->DrawCopy("same");
  l->Draw("same");
  c->SaveAs("pics/TrdQaStatisticVsAlpha.pdf");
  gDirectory->pwd();
  fdEdxPoint->Write("", TObject::kOverwrite);
  fdEdxDigi->Write("", TObject::kOverwrite);
  fdEdxCluster->Write("", TObject::kOverwrite);
  fdEdxHit->Write("", TObject::kOverwrite);
  fdEdxPionMc->Write("", TObject::kOverwrite);
  fdEdxPionHit->Write("", TObject::kOverwrite);
  fdEdxPionGhost->Write("", TObject::kOverwrite);
  fdEdxElectronMc->Write("", TObject::kOverwrite);
  fdEdxElectronHit->Write("", TObject::kOverwrite);
  fdEdxElectronGhost->Write("", TObject::kOverwrite);
  fDistanceMcToHit->Write("", TObject::kOverwrite);
  fDistanceMcToHitAll->Write("", TObject::kOverwrite);
  fMultiHitsVsR->Write("", TObject::kOverwrite);
  fGhostHitVsR->Write("", TObject::kOverwrite);
  fLostPointVsR->Write("", TObject::kOverwrite);
  fHitToPointEfficiencyVsR->Write("", TObject::kOverwrite);
  fMultiHitsVsAlpha->Write("", TObject::kOverwrite);
  fGhostHitVsAlpha->Write("", TObject::kOverwrite);
  fLostPointVsAlpha->Write("", TObject::kOverwrite);
  fHitToPointEfficiencyVsAlpha->Write("", TObject::kOverwrite);
  fPositionResolutionShort->Write("", TObject::kOverwrite);
  fPositionResolutionLong->Write("", TObject::kOverwrite);
  fClusterSize->Write("", TObject::kOverwrite);
  fPointsPerDigi->Write("", TObject::kOverwrite);
  fDigiPerCluster->Write("", TObject::kOverwrite);
  fClusterPerHit->Write("", TObject::kOverwrite);
  fPRF_1D->Write("", TObject::kOverwrite);
  fPRF_2D->Write("", TObject::kOverwrite);
  delete l;
gDirectory->Cd("..");
}
void CbmTrdQa::FinishEvent()
{ 
  if (fT) 
    fMCTracks->Delete();
  if (fP) {
    fPoints->Delete();
    for (fModulePointMapIt = fModulePointMap.begin();
	 fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
      fModulePointMapIt->second.clear();
    } 
    fModulePointMap.clear();
  }
  if (fD) {
    fDigis->Delete();
    for (fModuleDigiMapIt = fModuleDigiMap.begin();
	 fModuleDigiMapIt != fModuleDigiMap.end(); ++fModuleDigiMapIt) {
      fModuleDigiMapIt->second.clear();
    } 
    fModuleDigiMap.clear();
  }
  if (fC) {
    fClusters->Delete();
    for (fModuleClusterMapIt = fModuleClusterMap.begin();
	 fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
      fModuleClusterMapIt->second.clear();
    } 
    fModuleClusterMap.clear();
  }
  if (fH) {
    fHits->Delete();
    for (fModuleHitMapIt = fModuleHitMap.begin();
	 fModuleHitMapIt != fModuleHitMap.end(); ++fModuleHitMapIt) {
      fModuleHitMapIt->second.clear();
    } 
    fModuleHitMap.clear();
  }
}
void CbmTrdQa::GetPadInfos(Int_t moduleId, Double_t x, Double_t y, Int_t &iCol, Int_t &iRow, Double_t &padSizeX, Double_t &padSizeY)
{
  x += fModuleInfo->GetSizex(); // move origin from module center to lower right corner
  y += fModuleInfo->GetSizey(); // move origin from module center to lower right corner
  fModuleInfo = fDigiPar->GetModule(moduleId);
  Int_t nSector = fModuleInfo->GetNoSectors();
  Int_t iSectorX(0), iSectorY(0);
  Double_t iSectorSizeX = fModuleInfo->GetSectorSizex(iSectorX);
  Double_t iSectorSizeY = fModuleInfo->GetSectorSizey(iSectorY);
  Double_t iPadSizeX = fModuleInfo->GetPadSizex(iSectorX);
  Double_t iPadSizeY = fModuleInfo->GetPadSizey(iSectorY);
  Int_t iPadX = 0;
  Int_t iPadY = 0;
  if (x > 2 * fModuleInfo->GetSizex() || y > 2 * fModuleInfo->GetSizey() || x < 0 || y < 0) {
    printf("point out of module::\n   module %i (%.2f, %.2f) (%.2f, %.2f)\n",moduleId,x,y,2 * fModuleInfo->GetSizex(),2 * fModuleInfo->GetSizey());
  } else {   
    while (x > iSectorSizeX && iSectorX < nSector) {    
      x -= iSectorSizeX;
      iPadX += iSectorSizeX / iPadSizeX;
      iSectorX++;
      iSectorSizeX = fModuleInfo->GetSectorSizex(iSectorX);
      iPadSizeX = fModuleInfo->GetPadSizex(iSectorX);          
    }   
    padSizeX = iPadSizeX;
    iPadX += Int_t((x / padSizeX) + 0.5);
    iCol = iPadX;
    while (y > iSectorSizeY && iSectorY < nSector) {  
      y -= iSectorSizeY;
      iPadY += iSectorSizeY / iPadSizeY;
      iSectorY++;
      iSectorSizeY = fModuleInfo->GetSectorSizey(iSectorY);
      iPadSizeY = fModuleInfo->GetPadSizey(iSectorY);      
    }
    padSizeY = iPadSizeY;
    iPadY += Int_t((x / padSizeX) + 0.5);
    iRow = iPadY;
  }
}
void CbmTrdQa::CreateLayerView()
{
  /*
    for (fLayerPointMapIt = fLayerPointMap.begin();
    fLayerPointMapIt != fLayerPointMap.end(); ++fLayerPointMapIt) {
    fLayerPointMapIt->second;
    } 
    for (fLayerHitMapIt = fLayerHitMap.begin();
    fLayerHitMapIt != fLayerHitMap.end(); ++fLayerHitMapIt) {
    fLayerHitMapIt->second;
    } 
  */
  gDirectory->pwd();
  if (!gDirectory->Cd("TrdQa")) 
    gDirectory->mkdir("TrdQa");
  gDirectory->Cd("TrdQa");

  TString title;
  printf("fModuleGhostMap: %i\n",(Int_t)fModuleGhostMap.size());
  for (fModuleGhostMapIt = fModuleGhostMap.begin();
       fModuleGhostMapIt != fModuleGhostMap.end(); ++fModuleGhostMapIt) {
    Double_t value = fModuleGhostMapIt->second->GetMean(1);
    Double_t valueE = fModuleGhostMapIt->second->GetRMS(1);
    fModuleInfo = fDigiPar->GetModule(fModuleGhostMapIt->first);
    Int_t Station = fGeoHandler->GetStation(fModuleGhostMapIt->first);
    Int_t Layer   = fGeoHandler->GetLayer(fModuleGhostMapIt->first);
    Int_t combiId = 10 * Station + Layer;
    fLayerMap[combiId]->cd();
    if (fModuleGhostMapIt == fModuleGhostMap.begin()){
      fLayerDummy->SetZTitle("left over hits / all points [%]");
      fLayerDummy->DrawCopy();
    }
    TPaveText *text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()-fModuleInfo->GetSizey(),
				    fModuleInfo->GetX()+fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()+fModuleInfo->GetSizey());
    text->SetFillStyle(1001);
    text->SetLineColor(1);
    if (value >= 0 && value <= 5)
      text->SetFillColor(kViolet);
    if (value > 5 && value <= 10){
      text->SetFillColor(kAzure);
      text->SetTextColor(kWhite);
    }
    if (value > 10 && value <= 15)
      text->SetFillColor(kTeal);
    if (value > 15 && value <= 20)
      text->SetFillColor(kSpring);
    if (value > 20 && value <= 25)
      text->SetFillColor(kYellow);
    if (value > 25 && value <= 30)
      text->SetFillColor(kOrange);
    if (value > 30)
      text->SetFillColor(kRed);
    title.Form("%.1f#pm%.1f",value,valueE);
    text->AddText(title);
    text->Draw("same");
  }
  gDirectory->pwd();
  if (!gDirectory->Cd("Ghost")) 
    gDirectory->mkdir("Ghost");
  gDirectory->Cd("Ghost");
  for (fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); ++fLayerMapIt) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/Ghost_S%i_L%i_%s.pdf",(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleGhostMapIt = fModuleGhostMap.begin();
       fModuleGhostMapIt != fModuleGhostMap.end(); ++fModuleGhostMapIt) {
    fModuleGhostMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
  printf("fModuleLostMap: %i\n",(Int_t)fModuleLostMap.size());
  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); ++fModuleLostMapIt) {
    Double_t value = fModuleLostMapIt->second->GetMean(1);
    Double_t valueE = fModuleLostMapIt->second->GetRMS(1);
    fModuleInfo = fDigiPar->GetModule(fModuleLostMapIt->first);
    Int_t Station = fGeoHandler->GetStation(fModuleLostMapIt->first);
    Int_t Layer   = fGeoHandler->GetLayer(fModuleLostMapIt->first);
    Int_t combiId = 10 * Station + Layer;
    fLayerMap[combiId]->cd();
    if (fModuleLostMapIt == fModuleLostMap.begin()){
      fLayerDummy->SetZTitle("left over points / all points [%]");
      fLayerDummy->DrawCopy();
    }
    TPaveText *text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()-fModuleInfo->GetSizey(),
				    fModuleInfo->GetX()+fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()+fModuleInfo->GetSizey());
    text->SetFillStyle(1001);
    text->SetLineColor(1);
    if (value >= 0 && value <= 5)
      text->SetFillColor(kViolet);
    if (value > 5 && value <= 10){
      text->SetFillColor(kAzure);
      text->SetTextColor(kWhite);
    }
    if (value > 10 && value <= 15)
      text->SetFillColor(kTeal);
    if (value > 15 && value <= 20)
      text->SetFillColor(kSpring);
    if (value > 20 && value <= 25)
      text->SetFillColor(kYellow);
    if (value > 25 && value <= 30)
      text->SetFillColor(kOrange);
    if (value > 30)
      text->SetFillColor(kRed);
    title.Form("%.1f#pm%.1f",value,valueE);
    text->AddText(title);
    text->Draw("same");
  }
  gDirectory->pwd();
  if (!gDirectory->Cd("Lost")) 
    gDirectory->mkdir("Lost");
  gDirectory->Cd("Lost");

  for (fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); ++fLayerMapIt) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/Lost_S%i_L%i_%s.pdf",(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); ++fModuleLostMapIt) {
    fModuleLostMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  printf("fModuleEfficiencyMap: %i\n",(Int_t)fModuleEfficiencyMap.size());
  for (fModuleEfficiencyMapIt = fModuleEfficiencyMap.begin();
       fModuleEfficiencyMapIt != fModuleEfficiencyMap.end(); ++fModuleEfficiencyMapIt) {
    Double_t value = fModuleEfficiencyMapIt->second->GetMean(1);
    Double_t valueE = fModuleEfficiencyMapIt->second->GetRMS(1);
    fModuleInfo = fDigiPar->GetModule(fModuleEfficiencyMapIt->first);
    Int_t Station = fGeoHandler->GetStation(fModuleEfficiencyMapIt->first);
    Int_t Layer   = fGeoHandler->GetLayer(fModuleEfficiencyMapIt->first);
    Int_t combiId = 10 * Station + Layer;
    fLayerMap[combiId]->cd();
    if (fModuleEfficiencyMapIt == fModuleEfficiencyMap.begin()){
      fLayerDummy->SetZTitle("found point hit pairs / all points [%]");
      fLayerDummy->DrawCopy();
    }
    TPaveText *text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()-fModuleInfo->GetSizey(),
				    fModuleInfo->GetX()+fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()+fModuleInfo->GetSizey());
    text->SetFillStyle(1001);
    text->SetLineColor(1);
    if (value >= 0 && value <= 12.5)
      text->SetFillColor(kViolet);
    if (value > 12.5 && value <= 25){
      text->SetFillColor(kAzure);
      text->SetTextColor(kWhite);
    }
    if (value > 0 && value <= 50)
      text->SetFillColor(kTeal);
    if (value > 50 && value <= 70)
      text->SetFillColor(kSpring);
    if (value > 70 && value <= 80)
      text->SetFillColor(kYellow);
    if (value > 80 && value <= 90)
      text->SetFillColor(kOrange);
    if (value > 90)
      text->SetFillColor(kRed);
    title.Form("%.1f#pm%.1f",value,valueE);
    text->AddText(title);
    text->Draw("same");
  }
  gDirectory->pwd();
  if (!gDirectory->Cd("Efficiency")) 
    gDirectory->mkdir("Efficiency");
  gDirectory->Cd("Efficiency");

  for (fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); ++fLayerMapIt) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/Efficiency_S%i_L%i_%s.pdf",(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleEfficiencyMapIt = fModuleEfficiencyMap.begin();
       fModuleEfficiencyMapIt != fModuleEfficiencyMap.end(); ++fModuleEfficiencyMapIt) {
    fModuleEfficiencyMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
  gDirectory->Cd("..");

}
void CbmTrdQa::NormalizeHistos()
{
  Double_t Entries = fdEdxPoint->GetEntries();
  fdEdxPoint->Scale(1./Entries);
  Entries = fdEdxDigi->GetEntries();
  fdEdxDigi->Scale(1./Entries);
  Entries = fdEdxCluster->GetEntries();
  fdEdxCluster->Scale(1./Entries);
  Entries = fdEdxHit->GetEntries();
  fdEdxHit->Scale(1./Entries);
  Entries = fdEdxPionMc->GetEntries();
  fdEdxPionMc->Scale(1./Entries);
  Entries = fdEdxPionHit->GetEntries();
  fdEdxPionHit->Scale(1./Entries);
  Entries = fdEdxElectronMc->GetEntries();
  fdEdxElectronMc->Scale(1./Entries);
  Entries = fdEdxElectronHit->GetEntries();
  fdEdxElectronHit->Scale(1./Entries);
}
  void CbmTrdQa::FinishTask()
  {
    NormalizeHistos();
    SaveHistos();
    CreateLayerView();  
  }
  void CbmTrdQa::Register()
  {
    //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  ClassImp(CbmTrdQa)
