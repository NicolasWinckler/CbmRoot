// -------------------------------------------------------------------------
// -----                       CbmTof source file                      -----
// -----                  Created 28/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmTof.h"

#include "CbmTofPoint.h"
#include "CbmGeoTofPar.h"
#include "CbmGeoTof.h"
#include "CbmDetectorList.h"
#include "CbmStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TObjArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTof::CbmTof() 
  : FairDetector("TOF", kTRUE, kTOF),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1.),
    fPosIndex(0),
    fTofCollection(new TClonesArray("CbmTofPoint"))
{
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTof::CbmTof(const char* name, Bool_t active)
  : FairDetector(name, active, kTOF),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1.),
    fPosIndex(0),
    fTofCollection(new TClonesArray("CbmTofPoint"))
{
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
CbmTof::~CbmTof() {
 if (fTofCollection) {
   fTofCollection->Delete(); 
   delete fTofCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmTof::ProcessHits(FairVolume* vol)
{
  Int_t gap, cell, module, region;
  TString Volname;

  // Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create CbmTofPoint at exit of active volume
  if ( gMC->TrackCharge()!=0 &&
      (gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()) 
       ) {

    fTrackID       = gMC->GetStack()->GetCurrentTrackNumber();
    Volname = vol->GetName();
    region = Volname[5]-'0';
    gMC->CurrentVolID(gap);
    gMC->CurrentVolOffID(1, cell);
    gMC->CurrentVolOffID(2, module);
    
    fVolumeID = ((region-1)<<24) + ((module-1)<<14) + ((cell-1)<<4) + (gap-1);

    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, 
	   fELoss);

    // Increment number of tof points for TParticle
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kTOF);

    ResetParameters();
  }

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void CbmTof::EndOfEvent() {
  if (fVerboseLevel) Print();
  fTofCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void CbmTof::Register() {
  FairRootManager::Instance()->Register("TofPoint", "Tof", fTofCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* CbmTof::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fTofCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmTof::Print() const {
  Int_t nHits = fTofCollection->GetEntriesFast();
  cout << "-I- CbmTof: " << nHits << " points registered in this event." 
       << endl;
  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++) (*fTofCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmTof::Reset() {
  fTofCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void CbmTof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset){
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmTof: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmTofPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmTofPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmTofPoint(*oldpoint);
    fPosIndex++;
  }
  cout << "-I- CbmTof: " << cl2->GetEntriesFast() << " merged entries." 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Public method ConstructGeometry   -------------------------------
void CbmTof::ConstructGeometry() {
  
  Int_t count=0;
  Int_t count_tot=0;

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoTof*       tofGeo  = new CbmGeoTof();
  tofGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(tofGeo);

  Bool_t rc = geoFace->readSet(tofGeo);
  if (rc) tofGeo->create(geoLoad->getGeoBuilder());
  TList* volList = tofGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoTofPar* par=(CbmGeoTofPar*)(rtdb->getContainer("CbmGeoTofPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
	   count++;
       }else{
           fPassNodes->AddLast( aVol );
       }
       count_tot++;
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);  
  ProcessNodes ( volList );
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------
CbmTofPoint* CbmTof::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss) {
  TClonesArray& clref = *fTofCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmTofPoint(trackID, detID, pos, mom,
				      time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(CbmTof)
