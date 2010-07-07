// -------------------------------------------------------------------------
// -----                       CbmZdc source file                     -----
// -----                  Created 27/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmZdc.h"

#include "CbmGeoZdc.h"
#include "CbmGeoZdcPar.h"
#include "CbmZdcPoint.h"
#include "CbmDetectorList.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmZdc::CbmZdc() : FairDetector("ZDC", kTRUE, kZDC) {
  fZdcCollection = new TClonesArray("CbmZdcPoint");
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmZdc::CbmZdc(const char* name, Bool_t active)
  : FairDetector(name, active, kZDC) {
  fZdcCollection = new TClonesArray("CbmZdcPoint");
  fPosIndex = 0;
  fDebug    = "";
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmZdc::~CbmZdc() {
 if (fZdcCollection) {
   fZdcCollection->Delete(); 
   delete fZdcCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmZdc::ProcessHits(FairVolume* vol)
{
  // Stores all particles at entrance to active detector. 
 

  // Set parameters at entrance of volume. ELoss is set to the
  // kinetic energy, mimicking complete stopping in the calorimeter.
  if ( gMC->IsTrackEntering() ) {

    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    fTime     = gMC->TrackTime() * 1.0e09;   // conversion to ns
    fLength   = gMC->TrackLength();
    Double_t mass = gMC->TrackMass();

    // Calculate kinetic energy
    Double_t ekin = TMath::Sqrt( fMom.Px()*fMom.Px() +
				 fMom.Py()*fMom.Py() +
				 fMom.Pz()*fMom.Pz() +
				 mass * mass ) - mass;
    fELoss = ekin;

    // Create CbmZdcPoint
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, 
	   fELoss);

    // Increment number of ecal points for TParticle
    Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
    Int_t nZdcPoints = (points & (15<<16)) >> 16;
    nZdcPoints ++;
    if (nZdcPoints > 15) nZdcPoints = 15;
    points = ( points & ( ~ (15<<16) ) ) | (nZdcPoints << 16);
    gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

    ResetParameters();
    
  }

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void CbmZdc::EndOfEvent() {
  if (fVerboseLevel) Print();
  fZdcCollection->Clear();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void CbmZdc::Register() {
  FairRootManager::Instance()->Register("ZdcPoint", "Zdc", 
  				       fZdcCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* CbmZdc::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fZdcCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmZdc::Print() const {
  Int_t nHits = fZdcCollection->GetEntriesFast();
  cout << "-I- CbmZdc: " << nHits << " points registered in this event." 
       << endl;
  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++) (*fZdcCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmZdc::Reset() {
  fZdcCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void CbmZdc::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset){
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmZdc: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmZdcPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmZdcPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmZdcPoint(*oldpoint);
    fPosIndex++;
  }
  cout << "-I- CbmZdc: " << cl2->GetEntriesFast() << " merged entries." 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Public method ConstructGeometry   -------------------------------
void CbmZdc::ConstructGeometry() {

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoZdc*       trdGeo  = new CbmGeoZdc();
  trdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(trdGeo);

  Bool_t rc = geoFace->readSet(trdGeo);
  if (rc) trdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = trdGeo->getListOfVolumes();
 // store TRD geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoZdcPar* par=(CbmGeoZdcPar*)(rtdb->getContainer("CbmGeoZdcPar"));
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
  ProcessNodes( volList );
}
// -------------------------------------------------------------------------



// -----   Private method AddHit   -----------------------------------------
CbmZdcPoint* CbmZdc::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss) {
  TClonesArray& clref = *fZdcCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmZdcPoint(trackID, detID, pos, mom,
				      time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(CbmZdc)
