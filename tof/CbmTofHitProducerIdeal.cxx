// -------------------------------------------------------------------------
// -----                CbmTofHitProducerIdeal source file             -----
// -----                  Created 07/03/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmTofHitProducerIdeal.h"

#include "CbmTofHit.h"
#include "CbmTofPoint.h"

#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmTofHitProducerIdeal::CbmTofHitProducerIdeal() :
  CbmTask("Ideal TOF Hit Producer") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTofHitProducerIdeal::~CbmTofHitProducerIdeal() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmTofHitProducerIdeal::Init() {

  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmTofHitProducerIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("TOFPoint");
  if ( ! fPointArray ) {
    cout << "-W- CbmTofHitProducerIdeal::Init: "
	 << "No TOFPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("CbmTofHit");
  ioman->Register("TOFHit", "TOF", fHitArray, kTRUE);

  cout << "-I- CbmTofHitProducerIdeal: Intialisation successfull" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTofHitProducerIdeal::Exec(Option_t* opt) {

  // Reset output array
  if ( ! fHitArray ) Fatal("Exec", "No TofHitArray");

  fHitArray->Clear();

  // Declare some variables
  CbmTofPoint* point = NULL;
  Int_t detID   = 0;        // Detector ID
  Int_t trackID = 0;        // Track index
  Double_t x, y, z;         // Position
  Double_t dx = 0.0001;     // Position error
  Double_t tof = 0.;        // Time of flight
  TVector3 pos, dpos;       // Position and error vectors

  // Loop over TofPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point = (CbmTofPoint*) fPointArray->At(iPoint);
    if ( ! point) continue;

    // Detector ID
    detID = point->GetDetectorID();

    // MCTrack ID
    trackID = point->GetTrackID();

    // Determine hit position
    x  = point->GetX();
    y  = point->GetY();
    z  = point->GetZ();

    // Time of flight
    tof = point->GetTime();

    // Create new hit
    pos.SetXYZ(x,y,z);
    dpos.SetXYZ(dx, dx, 0.);
    new ((*fHitArray)[iPoint]) CbmTofHit(detID, pos, dpos, iPoint, tof);

  }   // Loop over MCPoints

  // Event summary
  cout << "-I- CbmTofHitProducerIdeal: " << nPoints << " TofPoints, "
       << nPoints << " Hits created." << endl;

}
// -------------------------------------------------------------------------



ClassImp(CbmTofHitProducerIdeal)
