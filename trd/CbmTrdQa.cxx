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
    fDistanceMcToHitAll(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL),
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
    fDistanceMcToHitAll(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL),
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

  delete fDistanceMcToHit;
  delete fDistanceMcToHitAll;
  delete fdEdxPionMc;
  delete fdEdxPionHit;
  delete fdEdxElectronMc;
  delete fdEdxElectronHit;
  delete fGhostHitVsR;
  delete fLostPointVsR;
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
  fDistanceMcToHit->SetMarkerStyle(22);
  fDistanceMcToHitAll = new TH1I("DistanceMcToHitAll", "DistanceMcToHitAll", 1500, 0, 150);
  fDistanceMcToHitAll->SetXTitle("#Delta r (MC-Hit) all [cm]");
  fDistanceMcToHitAll->SetYTitle("#");
  fDistanceMcToHitAll->SetMarkerStyle(21);
  fdEdxPionMc = new TH1I("dEdxPionMc", "dEdxPionMc", 1.0e3, 0, 30.0e-6);
  fdEdxPionMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxPionMc->SetYTitle("#");
  fdEdxPionHit = new TH1I("dEdxPionHit", "dEdxPionHit", 1.0e3, 0, 30.0e-6);
  fdEdxPionHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxPionHit->SetYTitle("#");
  fdEdxElectronMc = new TH1I("dEdxElectronMc", "dEdxElectronMc", 1.0e3, 0, 30.0e-6);
  fdEdxElectronMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxElectronMc->SetYTitle("#");
  fdEdxElectronHit = new TH1I("dEdxElectronHit", "dEdxElectronHit", 1.0e3, 0, 30.0e-6);
  fdEdxElectronHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxElectronHit->SetYTitle("#");
  fGhostHitVsR = new TProfile("GhostHitVsR", "GhostHitVsR", 785, 0, 785);
  fGhostHitVsR->SetXTitle("r [cm]");
  fGhostHitVsR->SetYTitle("average ghost hit percentage per module");
  fLostPointVsR = new TProfile("LostPointVsR", "LostPointVsR", 785, 0, 785);
  fLostPointVsR->SetXTitle("r [cm]");
  fLostPointVsR->SetYTitle("average lost point percentage per module");

  return kSUCCESS;
}
void CbmTrdQa::Exec(Option_t * option)
{
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
	title.Form("G%i",moduleId);
	fModuleGhostMap[moduleId] = new TH1F(title,title,1000,0,1000);
	title.Form("L%i",moduleId);
	fModuleLostMap[moduleId]  = new TH1F(title,title,1000,0,1000);
      }
      fModulePointMap[moduleId].push_back(iPoint);       
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] =  new TCanvas(title,title,800,800);
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
	fLayerMap[combiId] =  new TCanvas(title,title,800,800);
	fLayerDummy->DrawCopy();
      }
    }
  }
  /*
  //Clusters
  if (fC) {
  nEntries = fClusters->GetEntries();
  printf("%i\n",nEntries);
  for (Int_t iCluster=0; iCluster < nEntries; iCluster++ ) {
  cluster = (CbmTrdCluster*) fClusters->At(iCluster);
  moduleId = cluster->GetDetId();
  Station  = fGeoHandler->GetStation(moduleId);
  Layer    = fGeoHandler->GetLayer(moduleId);
  printf ("det: %i S%i L%i  \n",moduleId,Station,Layer);//GetDetId()); 
  combiId = 10 * Station + Layer;   
  fModuleInfo = fDigiPar->GetModule(moduleId);
  if (fModuleClusterMap.find(moduleId) == fModuleClusterMap.end()){
  fModuleClusterMap[moduleId].push_back(iCluster);
  }
  }
  }
  */
  //Hits
  if (fH) {
    nEntries = fHits->GetEntries();
    printf("%i Hits\n",nEntries);
    for (Int_t iHit=0; iHit < nEntries; iHit++ ) {
      hit = (CbmTrdHit*) fHits->At(iHit);
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
	fLayerMap[combiId] = new TCanvas(title,title,800,800);
	fLayerMap[combiId]->cd();
	fLayerDummy->DrawCopy();
      }
    }
  }
  Int_t allPoints(0), allHits(0);
  for (fModulePointMapIt = fModulePointMap.begin();
       fModulePointMapIt != fModulePointMap.end(); ++fModulePointMapIt) {
    if (fModuleHitMap.find(fModulePointMapIt->first) == fModuleHitMap.end()) {
      printf("module %i not found in hit map\n",fModulePointMapIt->first);
    } 
    else {
      //fModuleHitMapIt = fModuleHitMap[fModulePointMapIt->first];
      moduleId = fModulePointMapIt->first;
      allPoints = Int_t(fModulePointMapIt->second.size());
      allHits = Int_t(fModuleHitMap[moduleId].size());
      Double_t r = sqrt(fModuleInfo->GetX() * fModuleInfo->GetX() + fModuleInfo->GetY() * fModuleInfo->GetY());
      //for (Int_t iPoint = 0; iPoint < Int_t(fModulePointMapIt->second.size()); iPoint++) {
      for (Int_t iPoint = Int_t(fModulePointMapIt->second.size())-1; iPoint >= 0; iPoint--) { // its better to go backwards since found point and hit pairs are erased -> vector sice is dynamic
	point = (CbmTrdPoint*) fPoints->At(fModulePointMapIt->second[iPoint]);
	iTrack = point->GetTrackID();
	track = (CbmMCTrack*) fMCTracks->At(iTrack);
	Int_t Pdg_code = track->GetPdgCode();
	fModuleInfo = fDigiPar->GetModule(moduleId);
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
	//printf(" (%.2f, %.2f) (%.2f, %.2f) (%.2f, %.2f) (%.2f, %.2f) (%.2f, %.2f)\n",p_global[0],p_global[1],fModuleInfo->GetX(),fModuleInfo->GetY(),p_local[0],p_local[1],2*fModuleInfo->GetSizex(),2*fModuleInfo->GetSizey(),p_local[0]+fModuleInfo->GetSizex(),p_local[1]+fModuleInfo->GetSizey());
	Int_t xPpad(0), yPpad(0); // correspoondig digi from point point of view
	Double_t xPPadSize(0), yPPadSize(0); // correspoondig pad sizes from point point of view
	GetPadInfos( moduleId, p_local[0], p_local[1], xPpad, yPpad, xPPadSize, yPPadSize);
	if (Pdg_code == 211 || Pdg_code == -211)
	  fdEdxPionMc->Fill(point->GetEnergyLoss());
	if (Pdg_code == 11 || Pdg_code == -11 )
	  fdEdxElectronMc->Fill(point->GetEnergyLoss());
	Double_t minimumDistance = sqrt(xPPadSize * xPPadSize + yPPadSize * yPPadSize);
	Int_t minDHitId = -1;
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
	  if (distance < minimumDistance || (xHpad == xPpad && yHpad == yPpad )) {
	    //printf("xH%02i xP%02i   yH%02i yP%02i    %5.2fcm\n",xHpad, xPpad, yHpad, yPpad, distance);
	    minimumDistance = distance;
	    minDHitId = iHit;
	  }
	}
	if (minDHitId > -1){ // If minimumDistance < pad diagonal -> delete point and hit from map
	  fModuleHitMap[moduleId].erase(fModuleHitMap[moduleId].begin() + minDHitId);
	  fModulePointMapIt->second.erase(fModulePointMapIt->second.begin() + iPoint);
	  if (Pdg_code == 211 || Pdg_code == -211) // pdg code from associated point is used
	    fdEdxPionHit->Fill(hit->GetELoss());
	  if (Pdg_code == 11 || Pdg_code == -11 )
	    fdEdxElectronHit->Fill(hit->GetELoss());
	  fDistanceMcToHit->Fill(minimumDistance);
	}		
      }
      // left over points == lost
      fModuleLostMap[moduleId]->Fill( 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      fLostPointVsR->Fill(r, 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      // left over hits == ghosts
      fModuleGhostMap[moduleId]->Fill( 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      fGhostHitVsR->Fill(r, 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
    }
  }
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");
  /*
    if (track)
    delete track;
    if (point)
    delete point;
    if (digi)
    delete digi;
    if (cluster)
    delete cluster;
    if (hit)
    delete hit;
    if (point_temp)
    delete point_temp;
  */
}
  void CbmTrdQa::SaveHistos()
  {
    gDirectory->pwd();
    if (fdEdxPionMc)
      fdEdxPionMc->Write("", TObject::kOverwrite);
    if (fdEdxPionHit)
      fdEdxPionHit->Write("", TObject::kOverwrite);
    if (fdEdxElectronMc)
      fdEdxElectronMc->Write("", TObject::kOverwrite);
    if (fdEdxElectronHit)
      fdEdxElectronHit->Write("", TObject::kOverwrite);
    if (fDistanceMcToHit)
      fDistanceMcToHit->Write("", TObject::kOverwrite);
   if (fDistanceMcToHitAll)
      fDistanceMcToHitAll->Write("", TObject::kOverwrite);
    if (fGhostHitVsR)
      fGhostHitVsR->Write("", TObject::kOverwrite);
    if (fLostPointVsR)
      fLostPointVsR->Write("", TObject::kOverwrite);
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
    if (fModuleGhostMapIt == fModuleGhostMap.begin())
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
    //fLayerMapIt->second->SaveAs("pics/");
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
    if (fModuleLostMapIt == fModuleLostMap.begin())
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
    //fLayerMapIt->second->SaveAs("pics/");
  }
  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); ++fModuleLostMapIt) {
    fModuleLostMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
}
  void CbmTrdQa::FinishTask()
  {
    SaveHistos();
    CreateLayerView();  
  }
  void CbmTrdQa::Register()
  {
    //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  ClassImp(CbmTrdQa)
