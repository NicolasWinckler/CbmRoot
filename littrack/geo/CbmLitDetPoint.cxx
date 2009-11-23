#include "CbmLitDetPoint.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmLitDetPoint::CbmLitDetPoint() : FairMCPoint() { }
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
//CbmLitDetPoint::CbmLitDetPoint(Int_t trackID, Int_t detID,
//					 TVector3 pos, TVector3 mom,
//					 Double_t tof, Double_t length,
//					 Double_t eLoss)
//  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss) { }

// -------------------------------------------------------------------------
CbmLitDetPoint::CbmLitDetPoint(Int_t trackID, Int_t detID, TVector3 posIn,
			   TVector3 posOut, TVector3 momIn, TVector3 momOut,
			   Double_t tof, Double_t length, Double_t eLoss)
  : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) {
  fX_out  = posOut.X();
  fY_out  = posOut.Y();
  fZ_out  = posOut.Z();
  fPx_out = momOut.Px();
  fPy_out = momOut.Py();
  fPz_out = momOut.Pz();
}

// -----   Destructor   ----------------------------------------------------
CbmLitDetPoint::~CbmLitDetPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmLitDetPoint::Print(const Option_t* opt) const {
  cout << "-I- CbmLitDetPoint: LitDet point for track " << fTrackID
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(CbmLitDetPoint)
  
