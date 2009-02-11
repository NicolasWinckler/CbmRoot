/** CbmMuch.cxx
 *
 * @author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 * @version 1.0
 * @since   21.03.07
 * 
 *  Class for MUon CHambers detector
 *
 */

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

#include <iostream>

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
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    if (fELoss == 0. ) return kFALSE;
    //AZ
    //cout << fPosIn.Z() << " " << fVolumeID << endl;
    Int_t station, layer, doublet;
    gMC->CurrentVolOffID(0, layer);
    gMC->CurrentVolOffID(1, doublet);
    sscanf(gMC->CurrentVolName(),"muchstation%d",&station);
    if (doublet > 0) {
      // Straw tubes
      station += (doublet - 1);
      fVolumeID = (layer-1) | (station << 2);
    }
    //cout << gMC->CurrentVolName() << " " << layer << " " << doublet << " " << station << " " << fVolumeID << endl;
    //AZ
    AddHit(fTrackID, fVolumeID,
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
// ----------------------------------------------------------------------------
//void CbmMuch::SaveGeoParams(){
//
//  cout << " -I Save Much geo params " << endl;
//
//  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
//  cout << " mf: " << mf << endl;
//  TFolder *Muchf = NULL;
//  if (mf ) Muchf = (TFolder*) mf->FindObjectAny(GetName());
//  cout << " Muchf: " << Muchf << endl;
//  if (Muchf) Muchf->Add( flGeoPar0 ) ;
//  FairRootManager::Instance()->WriteFolder();
//  mf->Write("cbmroot",TObject::kWriteDelete);
//}


// -----   Public method EndOfEvent   -----------------------------------------
void CbmMuch::BeginEvent() {

  //  if (! kGeoSaved ) {
  //      SaveGeoParams();
  //  cout << "-I Much geometry parameters saved " << endl;
  //  kGeoSaved = kTRUE;
  //  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void CbmMuch::EndOfEvent() {

  if (fVerboseLevel) Print();
  fMuchCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void CbmMuch::Register() {
  FairRootManager::Instance()->Register("MuchPoint", GetName(), fMuchCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* CbmMuch::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fMuchCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void CbmMuch::Print() const {
  Int_t nHits = fMuchCollection->GetEntriesFast();
  cout << "-I- CbmMuch: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void CbmMuch::Reset() {
  fMuchCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
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
// ----------------------------------------------------------------------------



// -----   Public method ConstructGeometry   ----------------------------------
void CbmMuch::ConstructGeometry() {

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoMuch*       MuchGeo  = new CbmGeoMuch();
  MuchGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(MuchGeo);

  Bool_t rc = geoFace->readSet(MuchGeo);
  if (rc) MuchGeo->create(geoLoad->getGeoBuilder());
  TList* volList = MuchGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoMuchPar* par=(CbmGeoMuchPar*)(rtdb->getContainer("CbmGeoMuchPar"));
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
// ----------------------------------------------------------------------------



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

  // DEBUG info
//   if(detID == 23){
//     Double_t xp = posIn.X();
//     Double_t yp = posIn.Y();    
//     Double_t zp = posIn.Z();
//     Double_t rp = TMath::Sqrt(xp*xp + yp*yp);
//     if(TMath::Abs(posIn.Z() - 125) > 1e-3)
//       cout << "DEBUG: \t Z coordinate of the point fails. posIn.Z()=" 
// 	   << posIn.Z() << " (x,y) = (" << xp << ", " << yp << ")\n";
//     if(rp < 12.6 || rp > 69 )
//       cout << "DEBUG: \t X & Y of the point fails: ("
// 	   << xp << ", " << yp << ", " << posIn.Z() << ")\n";
//   }

  return new(clref[size]) CbmMuchPoint(trackID, detID, posIn, posOut,
				       momIn, momOut, time, length, eLoss);
}
// ----------------------------------------------------------------------------
