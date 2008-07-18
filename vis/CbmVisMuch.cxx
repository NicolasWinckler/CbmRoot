// -------------------------------------------------------------------------
// -----                       CbmVisMuch source file                  -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------


#include "CbmVisMuch.h"

#include "TApplication.h"
#include "TClonesArray.h"
#include "TGClient.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TGStatusBar.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoMuchPar.h"
#include "CbmMCTrack.h"

#include "CbmMuchStation.h"
#include "CbmMuchSector.h"
#include "CbmMuchPoint.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchHit.h"
#include "CbmMuchGeoScheme.h"

#include "CbmVisMuchStationFrame.h"
#include "CbmVisMuchModuleFrame.h"
#include "CbmVisPoint.h"
#include "CbmVisHit.h"
#include "CbmVisMuchCluster.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchPad.h"
#include "TColor.h"

// -----   Default constructor   ------------------------------------------
CbmVisMuch::CbmVisMuch():CbmTask("Task",0){
  fDigiFileName = "";
  fEvent  = 0;
  fMCTracks    = NULL;
  fPoints      = NULL;
  fDigis       = NULL; 
  fDigiMatches = NULL;
  fHits        = NULL;
  fNstations   = 0;
  fOpenModules = new TObjArray();
  fSumEvents   = kFALSE;
  fVisClusters = new TClonesArray("CbmVisMuchCluster",10);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmVisMuch::~CbmVisMuch() { 
  if ( fOpenModules ) fOpenModules->Delete(); delete fOpenModules;
}
// -------------------------------------------------------------------------


// -----   Private method SetParContainers   -------------------------------
void CbmVisMuch::SetParContainers() {
  CbmRunAna* fRun = CbmRunAna::Instance();
  CbmRuntimeDb* db = fRun->GetRuntimeDb();
  fGeoPar  = (CbmGeoMuchPar*)  db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmVisMuch::Init() {
  TFile* f = new TFile(fDigiFileName,"R");
  if (!f->IsOpen()) Fatal("Init","Geo file failed to open");
  TObjArray* stations = (TObjArray*) f->Get("stations");
  fGeoScheme=CbmMuchGeoScheme::Instance();
  fGeoScheme->Init(stations);
  fGeoScheme->InitGrid();
  fGeoScheme->CreatePointArrays("CbmVisPoint");
  fGeoScheme->CreateHitArrays("CbmVisHit");
  fGeoScheme->CreateClusterArrays("CbmVisMuchCluster");
  
  fRootManager = CbmRootManager::Instance();
  fMCTracks    = (TClonesArray*) fRootManager->GetObject("MCTrack");
  fPoints      = (TClonesArray*) fRootManager->GetObject("MuchPoint");
  fHits        = (TClonesArray*) fRootManager->GetObject("MuchHit");
  fDigis       = (TClonesArray*) fRootManager->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) fRootManager->GetObject("MuchDigiMatch");
  fClusters    = (TClonesArray*) fRootManager->GetObject("MuchCluster");

  CreateDisplay();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void CbmVisMuch::CloseWindow(){
    gApplication->Terminate(0);
}


// -------------------------------------------------------------------------
void CbmVisMuch::CreateDisplay(){
  fDisplay = new TGMainFrame(gClient->GetRoot(),800,800,kVerticalFrame);
  fDisplay->SetWindowName("MUCH Event Display");
  fDisplay->Connect("CloseWindow()","TApplication",gApplication,"Terminate()");

  fMain = new TGVerticalFrame(fDisplay);

  Int_t parts[] = {20,65,15};
  fStatusBar = new TGStatusBar(fDisplay,50,10,kHorizontalFrame);
  fStatusBar->SetParts(parts,3);

  fStationFrame = new CbmVisMuchStationFrame(fMain,this);

  //Button Frame
  fButtonFrame = new TGHorizontalFrame(fMain,500,100);

  fPrev = new TGTextButton(fButtonFrame,"&Previous");
  fNext = new TGTextButton(fButtonFrame,"&Next");
  fExit = new TGTextButton(fButtonFrame,"&Exit","gApplication->Terminate(0)");

  fNext->Connect("Clicked()","CbmVisMuch",this,"ReadNextEvent()");
  fPrev->Connect("Clicked()","CbmVisMuch",this,"ReadPrevEvent()");

  TGLayoutHints* lhButton = new TGLayoutHints(kLHintsCenterX,5,5,2,2);
  fButtonFrame->AddFrame(fPrev,lhButton);
  fButtonFrame->AddFrame(fNext,lhButton);
  fButtonFrame->AddFrame(fExit,lhButton);

  fMain->AddFrame(fStationFrame, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,2,2,2,2));
  fMain->AddFrame(fButtonFrame , new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));

  fDisplay->AddFrame(fMain     ,new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX|kLHintsExpandY,2,2,2,2));
  fDisplay->AddFrame(fStatusBar,new TGLayoutHints(kLHintsBottom|kLHintsLeft |kLHintsExpandX,0,0,2,0));


  fDisplay->MapSubwindows();
  fDisplay->Resize(fDisplay->GetDefaultSize());
  fDisplay->MapWindow();
  ReadEvent(fEvent);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuch::ReadEvent(Int_t event){
  int verbose = 0;
  fEvent=event;
  printf("Event:%i\n",fEvent);
  fRootManager->ReadEvent(fEvent);

  //Clear point and hit arrays
  fGeoScheme->ClearPointArrays();
  fGeoScheme->ClearHitArrays();
  fGeoScheme->ClearClusterArrays();
  fGeoScheme->ResetPads();
  fVisClusters->Clear();
  

  printf("Fill arrays of CbmVisPoints... nPoints = %i\n",fPoints->GetEntriesFast());
  //Create arrays of CbmVisPoints
  for (Int_t i=0;i<fPoints->GetEntriesFast();i++){
    CbmMuchPoint*  point  = (CbmMuchPoint*) fPoints->At(i);
    if ( ! point->IsUsable() ) continue;
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(point->GetDetectorID());
    TClonesArray &points = *(module->GetPoints());
    Int_t index = points.GetEntriesFast();
    new (points[index]) CbmVisPoint(point);
    SetPointInfo((CbmVisPoint*) points[index]);
  }

  printf("Fill arrays of CbmVisHits... nHits = %i\n",fHits->GetEntriesFast());
  //Create arrays of CbmVisHits
  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchHit* hit = (CbmMuchHit*) fHits->At(i);
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(hit->GetDetectorId());
    TClonesArray &hits = *(module->GetHits());
    Int_t index = hits.GetEntriesFast();
    new (hits[index]) CbmVisHit(hit);
    //SetHitInfo((CbmVisHit*) hits[index]);
  }
  
  printf("Mark fired pads ... nPads = %i\n",fDigis->GetEntriesFast());
  for (Int_t i=0;i<fDigis->GetEntriesFast();i++){
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(i);
    CbmMuchSector* sector = fGeoScheme->GetSectorByDetId(digi->GetDetectorId());
    CbmMuchPad* pad = sector->GetPad(CbmMuchGeoScheme::GetChannelIndex(digi->GetDetectorId()));
    pad->SetFired(i,digi->GetCharge(),digi->GetADCCharge());
  }
  
  printf("Fill arrays of CbmVisMuchClusters... nClusters = %i\n",fClusters->GetEntriesFast());
  for (Int_t i=0;i<fClusters->GetEntriesFast();i++){
    new ((*fVisClusters)[i]) CbmVisMuchCluster(i); 
  }

//  for (Int_t i=0;i<fClusters->GetEntriesFast();i++){
//    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(i);
//    if (!cluster->GetNDigis()) {
//      Error("ReadEvent","Cluster with empty array of digi indices");
//      continue;
//    }
//    CbmMuchDigi* digi      = (CbmMuchDigi*) fDigis->At(cluster->GetDigiIndex(0));
//    CbmMuchModule* module  = fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
//    TClonesArray &clusters = *(module->GetClusters());
//    Int_t index = clusters.GetEntriesFast();
//    new (clusters[index]) CbmVisMuchCluster(i);
//  }



  // Update current station view
  fStationFrame->UpdateStation();

  // Update open modules
  for (Int_t i=0;i<fOpenModules->GetEntriesFast();i++){
    CbmVisMuchModuleFrame* modFrame = (CbmVisMuchModuleFrame*) fOpenModules->At(i);
    if (!modFrame) continue;
    modFrame->UpdateModule();
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuch::ReadNextEvent(){
  fEvent++;
  ReadEvent(fEvent);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuch::ReadPrevEvent(){
  fEvent--;
  ReadEvent(fEvent);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmVisMuch::SetPointInfo(CbmVisPoint* vpoint){
  Int_t trackId = vpoint->GetTrackId();
  if (trackId<0) return 2;
  CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(trackId);
  vpoint->SetTrack(track);
  if (!track) return 3;
  vpoint->SetMotherPdg(GetMotherPdg(track));
  return 0;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmVisMuch::SetHitInfo(CbmVisHit* vhit){
  CbmMuchHit* hit  = (CbmMuchHit*) vhit->GetHit();
  Int_t digiId = hit->GetDigi();
  CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiId);

  Int_t pointId = match->GetRefIndex();
  vhit->SetPointId(pointId);

  if (pointId<0) return 1;
  CbmMCPoint*  point  = (CbmMCPoint*) fPoints->At(pointId);
  Int_t trackId = point->GetTrackID();
  vhit->SetTrackId(trackId);

  if (trackId<0) return 2;
  CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(trackId);
  if (!track) return 3;
  vhit->SetTrack(track);
  vhit->SetMotherPdg(GetMotherPdg(track));

  return 0;
}
// -------------------------------------------------------------------------

Int_t CbmVisMuch::GetMotherPdg(CbmMCTrack* track){
  Int_t motherId = track->GetMotherId();
  if (motherId<0) return 0;
  CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
  if (!mother) return 0;
  return mother->GetPdgCode();
}

ClassImp(CbmVisMuch)
