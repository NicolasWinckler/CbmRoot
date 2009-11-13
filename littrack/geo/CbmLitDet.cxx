#include "CbmLitDet.h"

#include "CbmLitDetPoint.h"
#include "CbmLitDetGeo.h"
#include "CbmLitDetGeoPar.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "CbmDetectorList.h"
#include "CbmStack.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"

#include <iostream>
using std::cout;
using std::endl;

CbmLitDet::CbmLitDet() :
  FairDetector("LitDet", kTRUE, kTutDet) {
  /** create your collection for data points */
  fCbmLitDetPointCollection = new TClonesArray("CbmLitDetPoint");
  
}

CbmLitDet::CbmLitDet(const char* name, Bool_t active)
  : FairDetector(name, active, kTutDet) {
  fCbmLitDetPointCollection = new TClonesArray("CbmLitDetPoint");
  
}
 
CbmLitDet::~CbmLitDet() {
   if (fCbmLitDetPointCollection) {
     fCbmLitDetPointCollection->Delete();
     delete fCbmLitDetPointCollection;
   }
}

void CbmLitDet::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmLitDetGeoPar* par=(CbmLitDetGeoPar*)(rtdb->getContainer("CbmLitDetGeoPar"));
}
 
Bool_t  CbmLitDet::ProcessHits(FairVolume* vol)
{ 
  /** This method is called from the MC stepping */
  
  //Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  
  // Create CbmLitDetPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    if (fELoss == 0. ) return kFALSE;
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
	   fELoss);
    
    // Increment number of lit det points in TParticle
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kTutDet);
  }
  
  return kTRUE;
}
 
void CbmLitDet::EndOfEvent() {
  
  fCbmLitDetPointCollection->Clear();
  
}



void CbmLitDet::Register() {

/** This will create a branch in the output tree called  
    CbmLitDetPoint, setting the last parameter to kFALSE means:
    this collection will not be written to the file, it will exist 
    only during the simulation. 
*/
 
  FairRootManager::Instance()->Register("LitDetPoint", "LitDet",
				       fCbmLitDetPointCollection, kTRUE);
 
}


TClonesArray* CbmLitDet::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fCbmLitDetPointCollection;
  else return NULL;
}

void CbmLitDet::Reset() {
  fCbmLitDetPointCollection->Clear();
}

void CbmLitDet::ConstructGeometry() {
  /** If you are using the standard ASCII input for the geometry 
      just copy this and use it for your detector, otherwise you can
      implement here you own way of constructing the geometry. */
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmLitDetGeo*  Geo  = new CbmLitDetGeo();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);
  
  Bool_t rc = geoFace->readSet(Geo);
  if (rc) Geo->create(geoLoad->getGeoBuilder());
  TList* volList = Geo->getListOfVolumes();
  
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmLitDetGeoPar* par=(CbmLitDetGeoPar*)(rtdb->getContainer("CbmLitDetGeoPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();
  
  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;
  
  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }else{
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);
  
  ProcessNodes ( volList );
}

CbmLitDetPoint* CbmLitDet::AddHit(Int_t trackID, Int_t detID,
					    TVector3 pos, TVector3 mom, 
					    Double_t time, Double_t length,
					    Double_t eLoss) {
  TClonesArray& clref = *fCbmLitDetPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmLitDetPoint(trackID, detID, pos, mom,
					      time, length, eLoss);
}

ClassImp(CbmLitDet)
