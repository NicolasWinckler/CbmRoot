// -------------------------------------------------------------------------
// -----                        CbmMvd source file                     -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmMvd.h"

#include "CbmMvdGeo.h"
#include "CbmMvdGeoPar.h"
#include "CbmMvdPoint.h"

#include "CbmDetectorList.h"
#include "CbmGeoInterface.h"
#include "CbmGeoLoader.h"
#include "CbmGeoNode.h"
#include "CbmGeoVolume.h"
#include "CbmRootManager.h"
#include "CbmRun.h"
#include "CbmRuntimeDb.h"
#include "CbmStack.h"
#include "CbmVolume.h"

#include "TClonesArray.h"
#include "TList.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVirtualMCStack.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvd::CbmMvd() : CbmDetector("MVD", kTRUE, kMVD) {
  ResetParameters();
  fCollection = new TClonesArray("CbmMvdPoint");
  fPosIndex   = 0;
  kGeoSaved   = kFALSE;
  fGeoPar     = new TList();
  fGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvd::CbmMvd(const char* name, Bool_t active) 
  : CbmDetector(name, active, kMVD) {
  ResetParameters();
  fCollection = new TClonesArray("CbmMvdPoint");
  fPosIndex   = 0;
  kGeoSaved   = kFALSE;
  fGeoPar     = new TList();
  fGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvd::~CbmMvd() {
  if ( fGeoPar ) delete flGeoPar;
  if (fCollection) {
    fCollection->Delete();
    delete fCollection;
  }
}
// -------------------------------------------------------------------------



// -----   Virtual public method ProcessHits  ------------------------------
Bool_t CbmMvd::ProcessHits(CbmVolume* vol) {


  // Store track parameters at entrance of sensitive volume
  if ( gMC->IsTrackEntering() ) {
    fPdg    = gMC->TrackPid();
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create CbmMvdPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    if (fELoss == 0. ) return kFALSE;
    AddHit(fTrackID, fPdg, fVolumeID,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);

    // Increment number of MvdPoints for this track
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kMVD);
    
    ResetParameters();
  }

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method BeginEvent   --------------------------------------
void CbmMvd::BeginEvent() { }
// -------------------------------------------------------------------------



// -----   Virtual public method EndOfEvent   ------------------------------
void CbmMvd::EndOfEvent() {
  if (fVerboseLevel) Print();
  fCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Virtual public method Register   --------------------------------
void CbmMvd::Register() {
  CbmRootManager::Instance()->Register("MVDPoint", GetName(), 
				       fCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Virtual public method GetCollection   ---------------------------
TClonesArray* CbmMvd::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -----   Virtual public method Print   -----------------------------------
void CbmMvd::Print() const {
  Int_t nHits = fCollection->GetEntriesFast();
  cout << "-I- CbmMvd: " << nHits << " points registered in this event." 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Virtual public method Reset   -----------------------------------
void CbmMvd::Reset() {
  fCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Virtual public method CopyClones   ------------------------------
void CbmMvd::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmMvd: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmMvdPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmMvdPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmMvdPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- CbmMvd: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// -------------------------------------------------------------------------



// -----   Virtual public method ConstructGeometry   -----------------------
void CbmMvd::ConstructGeometry() {
  
  CbmGeoLoader*    geoLoad = CbmGeoLoader::Instance();
  CbmGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmMvdGeo*       mvdGeo  = new CbmMvdGeo();
  mvdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(mvdGeo);

  Bool_t rc = geoFace->readSet(mvdGeo);
  if (rc) mvdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = mvdGeo->getListOfVolumes();
  // store geo parameter
  CbmRun *fRun = CbmRun::Instance();
  CbmRuntimeDb *rtdb= CbmRun::Instance()->GetRuntimeDb();
  CbmMvdGeoPar* geoPar = (CbmMvdGeoPar*)(rtdb->getContainer("CbmMvdGeoPar"));
  TObjArray *fSensNodes = geoPar->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = geoPar->GetGeoPassiveNodes();

  TListIter iter(volList);
  CbmGeoNode*   node = NULL;
  CbmGeoVolume* vol  = NULL;

  while( (node = (CbmGeoNode*)iter.Next()) ) {
      vol = dynamic_cast<CbmGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( vol );
       }else{
           fPassNodes->AddLast( vol );
       }
  }
  geoPar->setChanged();
  geoPar->setInputVersion(fRun->GetRunId(),1);
  ProcessNodes( volList );

}
// -------------------------------------------------------------------------



// -----   Private method AddHit   --------------------------------------------
CbmMvdPoint* CbmMvd::AddHit(Int_t trackID, Int_t pdg, Int_t detID, 
			    TVector3 posIn, TVector3 posOut, 
			    TVector3 momIn, TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- CbmMvd: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) CbmMvdPoint(trackID, pdg, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// ----------------------------------------------------------------------------




ClassImp(CbmMvd)
