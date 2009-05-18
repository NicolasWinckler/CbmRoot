#include "CbmMuch.h"

#include "CbmMuchPoint.h"
#include "CbmGeoMuchPar.h"
#include "CbmGeoMuch.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "CbmStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "TGeoBoolNode.h"

#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchGeoScheme.h"
#include "TGeoMatrix.h"

#include <iostream>
#include <fstream>
#include <cassert>

using std::cout;
using std::endl;

ClassImp(CbmMuch)

  // -----   Default constructor   -------------------------------------------
CbmMuch::CbmMuch() {
  ResetParameters();
  fMuchCollection = new TClonesArray("CbmMuchPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuch::CbmMuch(const char* name, Bool_t active)
  : FairDetector(name, active) {
  ResetParameters();
  fMuchCollection = new TClonesArray("CbmMuchPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuch::~CbmMuch() {

  if ( flGeoPar ) delete flGeoPar;
  if (fMuchCollection) {
    fMuchCollection->Delete();
    delete fMuchCollection;
  }
}
// -------------------------------------------------------------------------


// -----   Public method ProcessHits  --------------------------------------
Bool_t CbmMuch::ProcessHits(FairVolume* vol) {
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create CbmMuchPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    Int_t fDetectorId = GetDetId(vol);
    if (fVerboseLevel>2){
      printf(" TrackId: %i",fTrackID);
      printf(" System: %i" , CbmMuchGeoScheme::GetSystemIndex(fDetectorId));
      printf(" Station: %i", CbmMuchGeoScheme::GetStationIndex(fDetectorId));
      printf(" Layer: %i"  , CbmMuchGeoScheme::GetLayerIndex(fDetectorId));
      printf(" Side: %i"   , CbmMuchGeoScheme::GetLayerSideIndex(fDetectorId));
      printf(" Module: %i" , CbmMuchGeoScheme::GetModuleIndex(fDetectorId));
      printf(" Vol %i \n",fVolumeID);
    }
    Int_t iStation = CbmMuchGeoScheme::GetStationIndex(fDetectorId);
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    assert(iStation >=0 && iStation < fPar->GetNStations());
    CbmMuchStation* station= (CbmMuchStation*) fPar->GetStations()->At(iStation);
    if (fPosIn.Perp() >station->GetRmax()) {return kFALSE; }
    if (fPosOut.Perp()>station->GetRmax()) {return kFALSE; }

    if (fELoss == 0. ) return kFALSE;
    AddHit(fTrackID, fDetectorId,
      TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
      TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
      TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
      TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
      fTime, fLength, fELoss);

    // Increment number of MuchPoints for this track
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kMUCH);

    ResetParameters();
  }
  return kTRUE;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuch::BeginEvent() {
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuch::EndOfEvent() {
  if (fVerboseLevel) Print();
  fMuchCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuch::Register() {
  FairRootManager::Instance()->Register("MuchPoint", GetName(), fMuchCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
TClonesArray* CbmMuch::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fMuchCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuch::Print() const {
  Int_t nHits = fMuchCollection->GetEntriesFast();
  cout << "-I- CbmMuch: " << nHits << " points registered in this event."
       << endl;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuch::Reset() {
  fMuchCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuch::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmMuch: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmMuchPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (CbmMuchPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmMuchPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- CbmMuch: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// -------------------------------------------------------------------------



// -----   Private method AddHit   --------------------------------------------
CbmMuchPoint* CbmMuch::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			      TVector3 posOut, TVector3 momIn,
			      TVector3 momOut, Double_t time,
			      Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fMuchCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    cout << "-I- CbmMuch: Adding Point at (" << posIn.X() << ", " << posIn.Y()
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;

  return new(clref[size]) CbmMuchPoint(trackID, detID, posIn, posOut,
				       momIn, momOut, time, length, eLoss);
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
void CbmMuch::ConstructGeometry() {
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  geoFace->addGeoModule(new CbmGeoMuch());
  FairGeoMedia*     media    = geoFace->getMedia();
  FairGeoBuilder*   geobuild = geoLoad->getGeoBuilder();

  FairRun*        fRun = FairRun::Instance();
  if (!fRun) {Fatal("CreateGeometry","No FairRun found"); return;}
  FairRuntimeDb*  rtdb = FairRuntimeDb::instance();
  fPar  = (CbmGeoMuchPar*)(rtdb->getContainer("CbmGeoMuchPar"));
  TObjArray* stations = fPar->GetStations();

  // Create media
  TGeoMedium* mat = NULL;
  TGeoMedium* air        = CreateMedium("air");
  TGeoMedium* carbon     = CreateMedium("MUCHcarbon");
  TGeoMedium* iron       = CreateMedium("MUCHiron");
  TGeoMedium* wolfram    = CreateMedium("MUCHwolfram");
  TGeoMedium* lead       = CreateMedium("MUCHlead");
  TGeoMedium* argon      = CreateMedium("MUCHargon");
  TGeoMedium* supportMat = CreateMedium("MUCHsupport");
  TGeoMedium* noryl      = CreateMedium("MUCHnoryl");

  Double_t* buf;

  CbmMuchGeoScheme* fGeoScheme = CbmMuchGeoScheme::Instance();
  fGeoScheme->Init(stations);
  fGeoScheme->ReadGeoFile(fgeoName);
  fGeoScheme->Print();
  fGeoScheme->CreateMuchCave();

  // Create much cave
  Double_t muchZ0 = fGeoScheme->GetMuchCaveZ0();
  TGeoCone* shMuch = fGeoScheme->GetMuchCave();
  TGeoVolume*  voMuch = new TGeoVolume("much",shMuch,air);
  gGeoManager->Node("much",0,"cave",0.,0.,muchZ0,0,kTRUE,buf,0);


  // Create absorbers
  TObjArray* absorbers = fGeoScheme->GetAbsorbers();
  for (Int_t i=0;i<absorbers->GetEntriesFast();i++){
    TString absName = Form("muchabsorber%i",i+1);
    switch (fGeoScheme->GetAbsorberMat(i)) {
      case 'C': mat = carbon;  break;
      case 'I': mat = iron;    break;
      case 'L': mat = lead;    break;
      case 'W': mat = wolfram; break;
      default :
        mat = iron;
        Warning("CreateGeometry","Absorber material not defined");
    }
    Double_t z0 = fGeoScheme->GetAbsorberZ0(i)-muchZ0;
    TGeoCone* shAbs = (TGeoCone*) absorbers->At(i);
    TGeoVolume* voAbs = new TGeoVolume(absName,shAbs,mat);
    gGeoManager->Node(absName,0,"much",0.,0.,z0,0,kTRUE,buf,0);
  }

  // Create rotation matrices
  Int_t krotZ=0;
  Int_t krotY=0;
  gMC->Matrix(krotY,90,180,90,90,180,0);  // 180 degrees around y axis
  gMC->Matrix(krotZ,90,180,90,270,0,0);   // 180 degrees around z axis

  // Create GEM module shape which common for all module volumes
  Double_t activeLx = fGeoScheme->GetActiveLx();
  Double_t activeLy = fGeoScheme->GetActiveLy();
  Double_t activeLz = fGeoScheme->GetActiveLz();
  Double_t spacerLx = fGeoScheme->GetSpacerLx();
  Double_t spacerLy = fGeoScheme->GetSpacerLy();
  Double_t mDx  = (activeLx+2*spacerLx)/2.;
  Double_t mDy  = (activeLy+2*spacerLy)/2.;
  Double_t mDz  = (activeLz-0.001)/2.;
  TGeoBBox* shModuleFull = new TGeoBBox("shModule", mDx, mDy, mDz);
  TGeoBBox* shActiveFull = new TGeoBBox("shActive", activeLx/2, activeLy/2, activeLz/2);
  TGeoCompositeShape* shSpacerFull = new TGeoCompositeShape("shSpacer","shModule-shActive");

  // Create stations
  for (Int_t st=0;st<fGeoScheme->GetNStations();st++){
    CbmMuchStation* station = fGeoScheme->GetStation(st);

    // Check if
    Int_t nLayers = station->GetNLayers();
    if (nLayers<=0) {
      Warning("CbmMuch","Station layers are not defined for station %i",st);
      continue;
    }

    // Create station volume
    TString stName = Form("muchstation%i",st+1);
    Double_t stRmin = station->GetTubeRmin();
    Double_t stRmax = station->GetTubeRmax();
    Double_t stDz   = station->GetTubeDz();
    TGeoTube*   shSt = new TGeoTube(stRmin,stRmax,stDz);
    TGeoVolume* voSt = new TGeoVolume(stName,shSt,air);
    gGeoManager->Node(stName,0,"much",0.,0.,station->GetZ()-muchZ0,0,kTRUE,buf,0);

    // Create support shape
    CbmMuchLayer* layerFirst = station->GetLayer(0);
    Double_t supDx  = layerFirst->GetSupportDx();
    Double_t supDy  = layerFirst->GetSupportDy();
    Double_t supDz  = layerFirst->GetSupportDz();

    TString supBoxName   = Form("muchst%ibox",st+1);
    TString supHoleName  = Form("muchst%ihole",st+1);
    TString trName       = Form("muchst%itr",st+1);
    TString supShapeName = Form("muchst%ish",st+1);
    TGeoTube* shHole = new TGeoTube(supHoleName,0.,station->GetTubeRmin(),supDz+0.001);
    TGeoBBox* shBox  = new TGeoBBox(supBoxName,supDx/2.,supDy,supDz);
    TGeoTranslation* trHole = new TGeoTranslation(trName,-supDx/2.,0.,0.);
    trHole->RegisterYourself();
    TString formula = supBoxName+"-"+supHoleName+":"+trName;
    TGeoCompositeShape* shSup = new TGeoCompositeShape(supShapeName,formula);

    // Create layers
    for (Int_t l=0;l<station->GetNLayers();l++){
      CbmMuchLayer* layer = station->GetLayer(l);
      Double_t layerZ  = layer->GetZtoStationCenter();
      Double_t layerDz = layer->GetDz();
      TString layerName   = Form("muchstation%ilayer%i",st+1,l+1);
      TGeoTube*   shLayer = new TGeoTube(stRmin,stRmax,layerDz);
      TGeoVolume* voLayer = new TGeoVolume(layerName,shLayer,air);
      gGeoManager->Node(layerName,0,stName,0.,0.,layerZ,0,kTRUE,buf,0);


      // Straws

//      if (station->GetDetectorType()==2) {
//        if (station->IsModuleDesign()){
//          Fatal("CbmMuch","Station %i - detailed module design not implemented for straws",st);
//        }
//
//        for (Int_t s=0;s<2;s++){
//          CbmMuchLayerSide* side = layer->GetSide(s);
//          for (Int_t m=0;m<side->GetNModules();m++){
//            Char_t cside = (s==1) ? 'b' : 'f';
//            CbmMuchModule* module = side->GetModule(m);
//            TVector3 pos = module->GetPosition();
//            TVector3 size = module->GetSize();
//            TGeoBBox* shActiveBox = new TGeoBBox(Form("shActiveBoxSt%il%i",st+1,l+1),size[0]/2.,size[1]/2.,size[2]/2.);
//            TGeoShape* shActive = new TGeoCompositeShape(Form("shActiveHoleSt%il%i%cm%02i",st+1,l,cside,m+1),Form("shActiveBoxSt%il%i-muchst%ihole",st+1,l+1,st+1));
//            TString activeName = Form("muchstation%ilayer%i%cactive%03i",st+1,l+1,cside,m+1);
//            TGeoVolume* voActive = new TGeoVolume(activeName,shActive,argon);
//            gGeoManager->Node(activeName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
//            AddSensitiveVolume(voActive);
//          }
//        }
//        continue;
//      }

      // GEMs etc.

      TString  supName1  = Form("muchstation%ilayer%isupport1",st+1,l+1);
      TString  supName2  = Form("muchstation%ilayer%isupport2",st+1,l+1);
      TGeoVolume* voSup1 = new TGeoVolume(supName1,shSup,supportMat);
      TGeoVolume* voSup2 = new TGeoVolume(supName2,shSup,supportMat);
      gGeoManager->Node(supName1,0,layerName,+supDx/2.,0.,0.,    0,kTRUE,buf,0);
      gGeoManager->Node(supName2,0,layerName,-supDx/2.,0.,0.,krotZ,kTRUE,buf,0);

      if (!station->IsModuleDesign()){ // simple design
        CbmMuchLayerSide* side = layer->GetSide(0);
        CbmMuchModule* module = side->GetModule(0);
        TVector3 pos = module->GetPosition();
        TVector3 size = module->GetSize();
        TGeoBBox* shActiveBox = new TGeoBBox(Form("shActiveBoxSt%il%i",st+1,l+1),size[0]/2.,size[1]/2.,size[2]/2.);
        TGeoShape* shActive = new TGeoCompositeShape(Form("shActiveHoleSt%il%im%02i",st+1,1,1),Form("shActiveBoxSt%il%i-muchst%ihole",st+1,l+1,st+1));
        TString activeName = Form("muchstation%ilayer%i%cactive%03i",st+1,l+1,'f',1);
        TGeoVolume* voActive = new TGeoVolume(activeName,shActive,argon);
        gGeoManager->Node(activeName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
        AddSensitiveVolume(voActive);
        continue;
      }
      // Create detailed modules
      for (Int_t s=0;s<2;s++){
        CbmMuchLayerSide* side = layer->GetSide(s);
        for (Int_t m=0;m<side->GetNModules();m++){
          CbmMuchModule* module = side->GetModule(m);
          TVector3 pos = module->GetPosition();
          Double_t cutRadius = module->GetCutRadius();
          Char_t cside = (s==1) ? 'b' : 'f';
          TString activeName = Form("muchstation%ilayer%i%cactive%03i",st+1,l+1,cside,m+1);
          TString spacerName = Form("muchstation%ilayer%i%cspacer%03i",st+1,l+1,cside,m+1);
          TGeoShape* shActive = shActiveFull;
          TGeoShape* shSpacer = shSpacerFull;

          if (cutRadius>0) { // Create composite shape with a hole
            TGeoTranslation* tr = new TGeoTranslation(Form("tr%il%i%cm%02i",st+1,l+1,cside,m+1),-pos[0],-pos[1],0.);
            tr->RegisterYourself();
            shActive = new TGeoCompositeShape(Form("shActiveHoleSt%il%im%02i",st+1,l+1,m+1),Form("shActive-muchst%ihole:tr%il%i%cm%02i",st+1,st+1,l+1,cside,m+1));
            shSpacer = new TGeoCompositeShape(Form("shSpacerHoleSt%il%im%02i",st+1,l+1,m+1),Form("shSpacer-muchst%ihole:tr%il%i%cm%02i",st+1,st+1,l+1,cside,m+1));
          }

          TGeoVolume* voActive = new TGeoVolume(activeName,shActive,argon);
          TGeoVolume* voSpacer = new TGeoVolume(spacerName,shSpacer,noryl);
          gGeoManager->Node(activeName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
          gGeoManager->Node(spacerName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
          AddSensitiveVolume(voActive);
        } // modules
      } // sides
    } // layers
  } // stations

  TObjArray* fSensNodes = fPar->GetGeoSensitiveNodes();
  TObjArray* fPassNodes = fPar->GetGeoPassiveNodes();
  TGeoNode* ncave = gGeoManager->GetTopNode();
  TGeoNode* nmuch =(TGeoNode*) ncave->GetNodes()->FindObject("much_0");
  fPassNodes->Add(nmuch);
  //printf("much =%p\n",nmuch);
  TObjArray* nodes = nmuch->GetNodes();
  for (Int_t i=0;i<nodes->GetEntriesFast();i++){
    TGeoNode* node = (TGeoNode*) nodes->At(i);
    TString nodeName = node->GetName();
    //printf("nodeName=%s\n",nodeName.Data());
    if (nodeName.Contains("absorber")) fPassNodes->Add(node);
    if (nodeName.Contains("station")) {
      TObjArray* layers = node->GetNodes();
      for (Int_t l=0;l<layers->GetEntriesFast();l++){
        TGeoNode* layer = (TGeoNode*) layers->At(l);
        if (!TString(layer->GetName()).Contains("layer")) continue;
        TObjArray* layerNodes = layer->GetNodes();
        for (Int_t m=0;m<layerNodes->GetEntriesFast();m++){
          TGeoNode* layerNode = (TGeoNode*)layerNodes->At(m);
          TString layerNodeName = layerNode->GetName();
          if (layerNodeName.Contains("active"))  fSensNodes->Add(layerNode);
          if (layerNodeName.Contains("support")) fPassNodes->Add(layerNode);
        }
      }
    }
  }

  fPar->setChanged();
  fPar->setInputVersion(fRun->GetRunId(),1);
}
// -------------------------------------------------------------------------


Int_t CbmMuch::GetDetId(FairVolume* vol) {
  TString name = vol->GetName();
  Int_t  iStation     = TString(name[11]).Atoi()-1;
  Int_t  iLayer       = TString(name[17]).Atoi()-1;
  Int_t  iSide        = (name[18]=='b') ? 1 : 0;
  Char_t cModuleNr[4] = {name[25],name[26],name[27],' '};
  Int_t  iModule      = atoi(cModuleNr)-1;
  if(iSide!=1 && iSide !=0) printf("side = %i", iSide);
  Int_t detectorId = CbmMuchGeoScheme::GetDetectorId(iStation, iLayer, iSide, iModule);
  assert(CbmMuchGeoScheme::GetStationIndex(detectorId) == iStation);
  assert(CbmMuchGeoScheme::GetLayerIndex(detectorId) == iLayer);
  assert(CbmMuchGeoScheme::GetLayerSideIndex(detectorId) == iSide);
  assert(CbmMuchGeoScheme::GetModuleIndex(detectorId) == iModule);
  assert(detectorId > 0);
  return detectorId;

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TGeoMedium* CbmMuch::CreateMedium(const char* matName){
  FairGeoLoader*    geoLoad   = FairGeoLoader::Instance();
  FairGeoBuilder*   geobuild  = geoLoad->getGeoBuilder();
  FairGeoInterface* geoFace   = geoLoad->getGeoInterface();
  FairGeoMedia*     media     = geoFace->getMedia();
  FairGeoMedium*    medium = media->getMedium(matName);
  if (!medium) Fatal("CreateGeometry",Form("%s not defined in media file",matName));
  Int_t kMat = geobuild->createMedium(medium);
  return gGeoManager->GetMedium(kMat);
}
// -------------------------------------------------------------------------

