// -------------------------------------------------------------------------
// -----                       CbmStt source file                      -----
// -----       Updated 3/03/08  by A. Zinchenko, JINR, Dubna           -----
// -------------------------------------------------------------------------
#include "CbmStt.h"

#include "CbmGeoStt.h"
#include "CbmGeoSttPar.h"
#include "CbmSttPoint.h"

#include "FairDetector.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairVolume.h"
#include "FairRootManager.h"
#include "CbmStack.h"

//#include "TObjArray.h"
#include "TList.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmStt::CbmStt() 
  : FairDetector("STT", kTRUE, kTRD),
    fTrackID(-1),
    fVolumeID(-1),
    fELoss(-1.),
    fTime(-1.),
    fLength(-1.),
    fPos(),
    fMom(),
    fPosIndex(-0),
    fSttCollection(new TClonesArray("CbmSttPoint"))
{
  fVerboseLevel = 1;
}

// -----   Standard constructor   ------------------------------------------
CbmStt::CbmStt(const char* name, Bool_t active)
  : FairDetector(name, active, kTRD),
    fTrackID(-1),
    fVolumeID(-1),
    fELoss(-1.),
    fTime(-1.),
    fLength(-1.),
    fPos(),
    fMom(),
    fPosIndex(-0),
    fSttCollection(new TClonesArray("CbmSttPoint"))
{
  fVerboseLevel = 1;
}
 
CbmStt::~CbmStt() {
  if (fSttCollection) {
    fSttCollection->Delete();
    delete fSttCollection;
  }
}
 
void CbmStt::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoSttPar* par=(CbmGeoSttPar*)(rtdb->getContainer("CbmGeoSttPar"));
}

Bool_t  CbmStt::ProcessHits(FairVolume* vol)
{
 
  /** This method is called from the MC stepping */

  //Set parameters at entrance of volume. Reset ELoss.
  Double_t tmp[3] = {0};
  static Double_t xyzLoc[3] = {0};
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    fPos.Vect().GetXYZ(tmp);
    gMC->Gmtod(tmp, xyzLoc, 1); // local coord.
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create CbmSttDetPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    if (fELoss == 0. ) return kFALSE;

    Int_t station, layer, chamber, doublet;
    gMC->CurrentVolOffID(1, layer);
    gMC->CurrentVolOffID(2, doublet);
    layer = doublet * 2 - layer % 2;
    sscanf(&(gMC->CurrentVolName()[3]),"%d",&station);
    sscanf(&(gMC->CurrentVolName()[5]),"%d",&chamber);
    if (chamber == 1 && fPos.Y() < 0) chamber = -chamber;
    else if (chamber == 2 && fPos.X() < 0) chamber = -chamber;
    //cout << gMC->CurrentVolName() << " " << layer << " " << station << " " << chamber << endl;

    // Take average of entrance and exit points
    TLorentzVector xyz, dxyz;
    gMC->TrackPosition(xyz);
    dxyz = xyz - fPos; // diff. between entrance and exit
    xyz += fPos;
  
    /*
    CbmSttPoint *point = 
      AddHit(fTrackID, fVolumeID, TVector3(fPos.X(), fPos.Y(), fPos.Z()),
	     TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss);
    */
    TVector3 xyz3(xyz.X()/2., xyz.Y()/2., xyz.Z()/2.);
    Double_t xyzLoc1[3];
    xyz3.GetXYZ(tmp);
    gMC->Gmtod(tmp, xyzLoc1, 1); // local coord.
    if (xyzLoc[2] * xyzLoc1[2] < 0.) {
      Double_t dz = dxyz.Z();
      if (TMath::Abs(xyzLoc1[2]) > 0.001 && TMath::Abs(dz) > 1.e-9) {
	//cout << tmp1[2] << " " << xyz3.Z() << endl;
	// Too much off-centered in Z - put it at the center
	TVector3 dxyz3 = dxyz.Vect();
	dxyz3 = TVector3(dxyz3.X()/dz, dxyz3.Y()/dz, dxyz3.Z()/dz);
	xyz3 = xyz3 - dxyz3 * xyzLoc1[2];
	//cout << xyz3.Z() << endl;
	//exit(0);
      }
    }

    CbmSttPoint *point = 
      AddHit(fTrackID, fVolumeID, xyz3, fMom.Vect(), fTime, fLength, fELoss);
    point->SetStationNo(station);
    point->SetChamberNo(chamber);
    point->SetLayerNo(layer);

    // Increment number of Stt points in TParticle
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kTRD);

    ResetParameters();
  }

  return kTRUE;
}
 
void CbmStt::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
}

void CbmStt::EndOfEvent() {
  
  if (fVerboseLevel) Print();
  fSttCollection->Clear();

  ResetParameters();
}

void CbmStt::Register() {

/** This will create a branch in the output tree called  CbmSttDetPoint, setting the last parameter to kFALSE means:
this collection will not be written to the file, it will exist only during the simulation. */

  FairRootManager::Instance()->Register("SttPoint", "Stt", fSttCollection, kTRUE);

}

TClonesArray* CbmStt::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fSttCollection;
  else return NULL;
}

// -----   Public method Print   ----------------------------------------------
void CbmStt::Print() const {
  Int_t nHits = fSttCollection->GetEntriesFast();
  cout << "-I- CbmStt: " << nHits << " points registered in this event." << endl;
  for (Int_t i = 0; i < nHits; ++i) {
    CbmSttPoint *p = (CbmSttPoint*) fSttCollection->UncheckedAt(i);
    if (p->GetTrackID() < 0) cout << i << " " << p->GetTrackID() << endl;
  }
}
// ----------------------------------------------------------------------------

void CbmStt::Reset() {
  fSttCollection->Clear();
  ResetParameters();
}

// -----   Public method CopyClones   -----------------------------------------
void CbmStt::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmStt: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmSttPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmSttPoint*) cl1->At(i);
   Int_t index = oldpoint->GetTrackID() + offset;
   oldpoint->SetTrackID(index);
   new (clref[fPosIndex]) CbmSttPoint(*oldpoint);
   fPosIndex++;
   }
   cout << " -I- CbmStt: " << cl2->GetEntriesFast() << " merged entries."
	<< endl;
}
// ----------------------------------------------------------------------------

void CbmStt::ConstructGeometry() {
/** If you are using the standard ASCII input for the geometry just copy this and use it for your detector, otherwise you can
implement here you own way of constructing the geometry. */

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoStt*       sttGeo  = new CbmGeoStt();
  sttGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(sttGeo);

  Bool_t rc = geoFace->readSet(sttGeo);
  if (rc) sttGeo->create(geoLoad->getGeoBuilder());
  TList* volList = sttGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoSttPar* par=(CbmGeoSttPar*)(rtdb->getContainer("CbmGeoSttPar"));
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

CbmSttPoint* CbmStt::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
    TVector3 mom, Double_t time, Double_t length,
    Double_t eLoss) {
  TClonesArray& clref = *fSttCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmSttPoint(trackID, detID, pos, mom,
      time, length, eLoss);
}

void CbmStt::SetSpecialPhysicsCuts(){
  FairRun* run = FairRun::Instance();
  if (strcmp(run->GetName(),"TGeant3") == 0) {
    Int_t mat = gGeoManager->GetMaterialIndex("TRDgasJINR");
    gMC->Gstpar(mat+1,"LOSS",2.0);
  }
}

ClassImp(CbmStt)

