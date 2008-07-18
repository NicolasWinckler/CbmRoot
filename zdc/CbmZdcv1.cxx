
// -------------------------------------------------------------------------
// -----                       CbmZdcv1 source file                   -----
// -----                  Created 04/02/05  by Alla Maevskaya              -----
// -------------------------------------------------------------------------

#include "CbmZdcv1.h"

#include "CbmZdcPoint.h"
#include "CbmGeoZdc.h"

#include "CbmGeoInterface.h"
#include "CbmGeoLoader.h"
#include "CbmGeoNode.h"
#include "CbmGeoRootBuilder.h"
#include "CbmRootManager.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoBBox.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmZdcv1::CbmZdcv1() {
  fZdcCollection = new TClonesArray("CbmZdcPoint");
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmZdcv1::CbmZdcv1(const char* name, Bool_t active)
  : CbmZdc(name, active) {
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmZdcv1::~CbmZdcv1() {
 if (fZdcCollection) {
   fZdcCollection->Delete(); 
   delete fZdcCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmZdcv1::ProcessHits(CbmVolume* vol)
{
 if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;


  // Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    ResetParameters();
    fEloss   = 0.;
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fTime    = gMC->TrackTime() * 1.0e09;
    fLength  = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    Int_t copyNo;
    gMC->CurrentVolID(copyNo);
    Int_t iCell ;
    gMC->CurrentVolOffID(1, iCell);
    fVolumeID=iCell;
    
    }
  if (gMC->Edep() != 0.) {
    // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
  }
  
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    if (fEloss == 0. ) return kFALSE;
    if (gMC->Edep() != 0.) {
      // Sum energy loss for all steps in the active volume
      fEloss += gMC->Edep();
    }
      if (strstr(fDebug,"hit"))
      Info("ProcessHits","volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), dE=%.3e",
	   fVolumeID, gMC->CurrentVolName(),
	   fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);

  // Create CbmZdcPoint
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
  }
  
  // Increment number of ecal points for TParticle
  Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
  Int_t nZdcPoints = (points & (15<<16)) >> 16;
  nZdcPoints ++;
  if (nZdcPoints > 15) nZdcPoints = 15;
  points = ( points & ( ~ (15<<16) ) ) | (nZdcPoints << 16);
  gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
  
  // ResetParameters();
    
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method CreateMaterial   ----------------------------------
void CbmZdcv1::CreateMaterial() {
}
// -----   Public method ConstructGeometry   -------------------------------
void CbmZdcv1::ConstructGeometry() {
  // Create materials, media and volume

  TGeoMaterial *material = 0;
  TGeoMedium   *medium   = 0;
  Float_t *buf = 0;
  Float_t sumWeight;
  Int_t i;


  // =============== Materials

  // material Lead
  Int_t kMatLead=601;
  material = gGeoManager->Material("Pb", 207.2, 82., 11.35,kMatLead);
  material->Print();

  // mixtrure Scintillator (CH)
  Int_t kMatPoly=602;
  Float_t aP[2] = {12.011, 1.00794} ;
  Float_t zP[2] = {6.0, 1.0} ;
  /*
  Float_t wP[2] = {1.0, 1.0} ;
  Float_t dP = 1.032 ;
  Int_t   nP = 2;
  */
  Float_t wP[2] = {9.0, 8.0} ;
  Float_t dP = 1.032 ;
  Int_t   nP = 2;
  sumWeight = 0;
  for (i=0; i<nP; i++) sumWeight += aP[i]*wP[i];
  for (i=0; i<nP; i++) wP[i] *= aP[i]/sumWeight;
  material = gGeoManager->Mixture("Scintillator",aP,zP,dP,nP,wP,kMatPoly);
  material->Print();

 
  // =============== Media

  Int_t kMedLead=601,kMedScin=602;
  medium = gGeoManager->Medium("Lead"        , kMedLead, kMatLead, 0, 0, 0., 10.0, 0.1, 0.1, 0.1, 0.1);
  medium->Print();
  medium = gGeoManager->Medium("Scintillator", kMedScin, kMatPoly, 1, 0, 0., 10.0, 0.1, 0.1, 0.1, 0.1);
  medium->Print();

  // =============== Volumes

  //  const
  Float_t zdcSize[3] = {60.,45.,60.};

  Float_t zdcCell[3] = {  5.,  5.,1.};
  Float_t zdcPb[3] = {  5.,  5.,0.8};
  Float_t zdcSc[3] = {  4.9,  5.,0.2};

  Float_t zZdc    = 1500.+zdcSize[2];
 TGeoVolume *volume;

  // --------------- Container volume for the whole ECAL
  Float_t xPos, yPos, zPos, zInCell;
  volume = gGeoManager->Volume("ZDC", "BOX", 1, zdcSize, 3);
  gGeoManager->Node("ZDC", 1, "cave", 0.,0.,zZdc+zdcSize[2],0,kTRUE,buf,0);
  volume->Print();
  //one piece
  Int_t ncell=0;
  volume = gGeoManager->Volume("ZDCCell", "BOX", 1, zdcCell , 3);
  volume->Print();
  zPos=-zdcSize[2]+zdcCell[2];
  for (Int_t iLay=0; iLay<60; iLay++){
      yPos=-zdcSize[1]+ zdcCell[1];
    for (Int_t iCol=0; iCol<9; iCol++){
      xPos=-zdcSize[0]+ zdcCell[0];
     for (Int_t iRaw=0; iRaw<12; iRaw++){
       ncell++;
       gGeoManager->Node("ZDCCell", ncell, "ZDC", xPos,yPos,zPos,0,kTRUE,buf,0);
       xPos += 2*zdcCell[0];
     }
     yPos += 2*zdcCell[1];
    }
    zPos += 2*zdcCell[2];
  }

  cout<<" #####################Number of Volumes "<<ncell<<endl; 
  //lead
  volume = gGeoManager->Volume("ZDCLead", "BOX",kMedLead , zdcPb , 3);
  zInCell=-zdcCell[2] + zdcPb[2];
  gGeoManager->Node("ZDCLead", 1, "ZDCCell", 0.,0.,zInCell,0,kTRUE,buf,0);
  gGeoManager->Node("ZDCLead", 1, "ZDCCell1", 0.,0.,zInCell,0,kTRUE,buf,0);
  //scintillator

  // Scintillator tile is created differently to satisfy
  // CBM framework around sensitive volumes
  TGeoBBox *shape = new TGeoBBox("ZDCScin", zdcSc[0], zdcSc[1], zdcSc[2]);
  volume = new TGeoVolume("ZDCScin",shape,gGeoManager->GetMedium(kMedScin));
  gGeoManager->AddVolume(volume);
  zInCell=-zdcCell[2] + 2*zdcPb[2] + zdcSc[2];;
   Float_t xInCell=-zdcCell[0]+zdcSc[0];
   Float_t yInCell=-zdcCell[1]+zdcSc[1];
   gGeoManager->Node("ZDCScin", 1, "ZDCCell", xInCell,yInCell,zInCell,0,kTRUE,buf,0);
   AddSensitiveVolume(volume);
   fNbOfSensitiveVol++;
   
}
// -------------------------------------------------------------------------

ClassImp(CbmZdcv1)
