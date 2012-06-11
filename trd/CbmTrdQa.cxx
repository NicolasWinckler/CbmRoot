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
    fTriggerThreshold(1e-6),
    fGeo(),
    fDistanceMcToHit(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL)
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
    fTriggerThreshold(triggerThreshold),
    fGeo(geo),
    fDistanceMcToHit(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL)
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
  for (fModulePointMapIt = fModulePointMap.begin();
       fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
    fModulePointMapIt->second.clear();
  } 
  for (fModuleDigiMapIt = fModuleDigiMap.begin();
       fModuleDigiMapIt != fModuleDigiMap.end(); ++fModuleDigiMapIt) {
    fModuleDigiMapIt->second.clear();
  } 
  for (fModuleClusterMapIt = fModuleClusterMap.begin();
       fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
    fModuleClusterMapIt->second.clear();
  } 
  for (fModuleHitMapIt = fModuleHitMap.begin();
       fModuleHitMapIt != fModuleHitMap.end(); ++fModuleHitMapIt) {
    fModuleHitMapIt->second.clear();
  } 
  for (fModuleGhostMapIt = fModuleGhostMap.begin();
       fModuleGhostMapIt != fModuleGhostMap.end(); ++fModuleGhostMapIt) {
    delete fModuleGhostMapIt->second;
  } 
  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); ++fModuleLostMapIt) {
    delete fModuleLostMapIt->second;
  } 
  delete fLayerDummy;
  delete fDistanceMcToHit;
  delete fdEdxPionMc;
  delete fdEdxPionHit;
  delete fdEdxElectronMc;
  delete fdEdxElectronHit;
  delete fGhostHitVsR;
  delete fLostPointVsR;
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
    cout << "-W CbmTrdCluster::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  fPoints =(TClonesArray *)  ioman->GetObject("TrdPoints");
  if ( ! fPoints ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdPoint array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdCluster array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
  }
  fHits =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fHits ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdHits array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
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
  fDistanceMcToHit = new TH1I("DistanceMcToHit", "DistanceMcToHit", 300, -150, 150);
  fDistanceMcToHit->SetXTitle("#Delta r (MC-Hit) [cm]");
  fDistanceMcToHit->SetYTitle("#");
  fdEdxPionMc = new TH1I("dEdxPionMc", "dEdxPionMc", 1.0e9, 0, 1);
  fdEdxPionMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxPionMc->SetYTitle("#");
  fdEdxPionHit = new TH1I("dEdxPionHit", "dEdxPionHit", 1.0e9, 0, 1);
  fdEdxPionHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxPionHit->SetYTitle("#");
  fdEdxElectronMc = new TH1I("dEdxElectronMc", "dEdxElectronMc", 1.0e9, 0, 1);
  fdEdxElectronMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxElectronMc->SetYTitle("#");
  fdEdxElectronHit = new TH1I("dEdxElectronHit", "dEdxElectronHit", 1.0e9, 0, 1);
  fdEdxElectronHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxElectronHit->SetYTitle("#");
  fGhostHitVsR = new TH1I("GhostHitVsR", "GhostHitVsR", 785, 0, 785);
  fGhostHitVsR->SetXTitle("r [cm]");
  fGhostHitVsR->SetYTitle("#");
  fLostPointVsR = new TH1I("LostPointVsR", "LostPointVsR", 785, 0, 785);
  fLostPointVsR->SetXTitle("r [cm]");
  fLostPointVsR->SetYTitle("#");

  return kSUCCESS;
}
void CbmTrdQa::Exec(Option_t * option)
{
  Int_t nEntries(0), iTrack(-1), moduleId(-1), Station(-1), Layer(-1), combiId(-1), dEdx(0), rPos(0), PDG(-1), ghostCount(0), lostCount(0);
  TString title;
  Bool_t debug = false;
  CbmMCTrack *track = NULL;
  CbmTrdPoint *point = NULL;
  CbmTrdDigi *digi = NULL;
  CbmTrdCluster *cluster = NULL;
  CbmTrdHit *hit = NULL;
  // MC-points
  nEntries = fPoints->GetEntries();
  for (Int_t iPoint=0; iPoint < nEntries; iPoint++ ) {
    point = (CbmTrdPoint*) fPoints->At(iPoint);
    moduleId = point->GetDetectorID();
    Station  = fGeoHandler->GetStation(moduleId);
    Layer    = fGeoHandler->GetLayer(moduleId);
    combiId = 10 * Station + Layer;
    iTrack = point->GetTrackID();
    track = (CbmMCTrack*) fMCTracks->At(iTrack);
    PDG = track->GetPdgCode();
    fModuleInfo = fDigiPar->GetModule(moduleId);
    if (fModulePointMap.find(moduleId) == fModulePointMap.end()){
      fModulePointMap[moduleId].push_back(iPoint);
    }
    if (fLayerMap.find(combiId) == fLayerMap.end()){
      title.Form("Station%i_Layer%i",Station,Layer);
      fLayerMap[combiId] =  new TCanvas(title,title,800,800);
      fLayerDummy->DrawCopy();
    }
  }
  // Digis
  nEntries = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nEntries; iDigi++ ) {
    digi = (CbmTrdDigi*) fDigis->At(iDigi);
    moduleId = digi->GetDetId();
    Station  = fGeoHandler->GetStation(moduleId);
    Layer    = fGeoHandler->GetLayer(moduleId);
    combiId = 10 * Station + Layer;
    fModuleInfo = fDigiPar->GetModule(moduleId);
    if (fModuleDigiMap.find(moduleId) == fModuleDigiMap.end()){
      fModuleDigiMap[moduleId].push_back(iDigi);
    }
    if (fLayerMap.find(combiId) == fLayerMap.end()){
      title.Form("Station%i_Layer%i",Station,Layer);
      fLayerMap[combiId] =  new TCanvas(title,title,800,800);
      fLayerDummy->DrawCopy();
    }
  }
  /*
  //Clusters
  nEntries = fClusters->GetEntries();
  for (Int_t iCluster=0; iCluster < nEntries; iCluster++ ) {
  cluster = (CbmTrdCluster*) fClusters->At(iCluster);
  moduleId = cluster->GetDetId();
  Station  = fGeoHandler->GetStation(moduleId);
  Layer    = fGeoHandler->GetLayer(moduleId);
  combiId = 10 * Station + Layer;
  fModuleInfo = fDigiPar->GetModule(moduleId);
  if (fModuleClusterMap.find(moduleId) == fModuleClusterMap.end()){
  fModuleClusterMap[moduleId].push_back(iCluster);
  }
  }
  */
  //Hits
  nEntries = fHits->GetEntries();
  for (Int_t iHit=0; iHit < nEntries; iHit++ ) {
    hit = (CbmTrdHit*) fHits->At(iHit);
    moduleId = hit->GetDetId();
    Station  = fGeoHandler->GetStation(moduleId);
    Layer    = fGeoHandler->GetLayer(moduleId);
    combiId = 10 * Station + Layer;
    fModuleInfo = fDigiPar->GetModule(moduleId);
    if (fModuleHitMap.find(moduleId) == fModuleHitMap.end()){
      fModuleHitMap[moduleId].push_back(iHit);
    }
    if (fLayerMap.find(combiId) == fLayerMap.end()){
      title.Form("Station%i_Layer%i",Station,Layer);
      fLayerMap[combiId] = new TCanvas(title,title,800,800);
      fLayerDummy->DrawCopy();
    }
  }
  for (fModulePointMapIt = fModulePointMap.begin();
       fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
    if (fModuleHitMap.find(fModulePointMapIt->first) != fModuleHitMap.end()) {
      for (Int_t iPoint = 0; iPoint < Int_t(fModulePointMapIt->second.size()); iPoint++) {
	point = (CbmTrdPoint*) fPoints->At(iPoint);
	iTrack = point->GetTrackID();
	track = (CbmMCTrack*) fMCTracks->At(iTrack);
	Int_t Pdg_code = track->GetPdgCode();
	Double_t xPpos = 0.5 * (point->GetXIn() + point->GetXOut());
	Double_t yPpos = 0.5 * (point->GetYIn() + point->GetYOut());
	if (Pdg_code == 211 || Pdg_code == -211)
	  fdEdxPionMc->Fill(point->GetEnergyLoss());
	if (Pdg_code == 11 || Pdg_code == -11 )
	  fdEdxElectronMc->Fill(point->GetEnergyLoss());
	Double_t minimumDistance = 1000;
	Int_t minDHitId = -1;
	for (Int_t iHit = 0; iHit < Int_t(fModuleHitMapIt->second.size()); iHit++) {
	  hit = (CbmTrdHit*) fHits->At(iHit);
	  Double_t xHpos = hit->GetX();
	  Double_t yHpos = hit->GetY();
	  Double_t distance = sqrt((xPpos-xHpos) * (xPpos-xHpos) + (yPpos-yHpos) * (yPpos-yHpos));
	  if (distance < minimumDistance) {
	    minimumDistance = distance;
	    minDHitId = iHit;
	  }
	}
	fDistanceMcToHit->Fill(minimumDistance);

	// If minimumDistance < pad diagonal -> delete point and hit from map
	// left over hits == ghosts
	// left over points == lost
      }
    }
  }

}
void CbmTrdQa::SaveHistos()
{
  gDirectory->pwd();
  fDistanceMcToHit->Write("", TObject::kOverwrite);
  fdEdxPionMc->Write("", TObject::kOverwrite);
  fdEdxPionHit->Write("", TObject::kOverwrite);
  fdEdxElectronMc->Write("", TObject::kOverwrite);
  fdEdxElectronHit->Write("", TObject::kOverwrite);
  fGhostHitVsR->Write("", TObject::kOverwrite);
  fLostPointVsR->Write("", TObject::kOverwrite);
}
void CbmTrdQa::FinishEvent()
{
  fPoints->Delete();
  for (fModulePointMapIt = fModulePointMap.begin();
       fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
    fModulePointMapIt->second.clear();
  } 
  fModulePointMap.clear();
  fDigis->Delete();
  for (fModuleDigiMapIt = fModuleDigiMap.begin();
       fModuleDigiMapIt != fModuleDigiMap.end(); ++fModuleDigiMapIt) {
    fModuleDigiMapIt->second.clear();
  } 
  fModuleDigiMap.clear();
  fClusters->Delete();
  for (fModuleClusterMapIt = fModuleClusterMap.begin();
       fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
    fModuleClusterMapIt->second.clear();
  } 
  fModuleClusterMap.clear();
  fHits->Delete();
  for (fModuleHitMapIt = fModuleHitMap.begin();
       fModuleHitMapIt != fModuleHitMap.end(); ++fModuleHitMapIt) {
    fModuleHitMapIt->second.clear();
  } 
  fModuleHitMap.clear();
}
void CbmTrdQa::CreateLayerView()
{

  fLayerDummy->DrawCopy();
  TString title;
  for (fModuleGhostMapIt = fModuleGhostMap.begin();
       fModuleGhostMapIt != fModuleGhostMap.end(); ++fModuleGhostMapIt) {
    Double_t value = fModuleGhostMapIt->second->GetMean(1);
    Double_t valueE = fModuleGhostMapIt->second->GetRMS(1);
    fModuleInfo = fDigiPar->GetModule(fModuleGhostMapIt->first);
    Int_t Station = fGeoHandler->GetStation(fModuleGhostMapIt->first);
    Int_t Layer   = fGeoHandler->GetLayer(fModuleGhostMapIt->first);
    Int_t combiId = 10 * Station + Layer;
    fLayerMap[combiId]->cd();
    TPaveText *text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()-fModuleInfo->GetSizey(),
				    fModuleInfo->GetX()+fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()+fModuleInfo->GetSizey());
    text->SetFillStyle(1001);
    text->SetLineColor(1);
    if (value > 0 && value <= 5)
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
    //fLayerMapIt->second->SaveAs("pics/");
  }
  gDirectory->Cd("..");
  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); ++fModuleLostMapIt) {
    Double_t value = fModuleLostMapIt->second->GetMean(1);
    Double_t valueE = fModuleLostMapIt->second->GetRMS(1);
    fModuleInfo = fDigiPar->GetModule(fModuleLostMapIt->first);
    Int_t Station = fGeoHandler->GetStation(fModuleLostMapIt->first);
    Int_t Layer   = fGeoHandler->GetLayer(fModuleLostMapIt->first);
    Int_t combiId = 10 * Station + Layer;
    fLayerMap[combiId]->cd();
    fLayerDummy->DrawCopy();
    TPaveText *text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()-fModuleInfo->GetSizey(),
				    fModuleInfo->GetX()+fModuleInfo->GetSizex(),
				    fModuleInfo->GetY()+fModuleInfo->GetSizey());
    text->SetFillStyle(1001);
    text->SetLineColor(1);
    if (value > 0 && value <= 5)
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
    //fLayerMapIt->second->SaveAs("pics/");
  }
  gDirectory->Cd("..");
}
void CbmTrdQa::FinishTask()
{
  CreateLayerView();
  //SaveHistos2File();
}
void CbmTrdQa::Register()
{
  //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
  //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
}
ClassImp(CbmTrdQa)
