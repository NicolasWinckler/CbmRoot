// -------------------------------------------------------------------------
// -----                      CbmZdcPoint source file                 -----
// -----                   Created 28/07/04  by V. Friese              -----
// -------------------------------------------------------------------------


#include "CbmZdcPoint.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmZdcPoint::CbmZdcPoint() : CbmMCPoint() { }
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmZdcPoint::CbmZdcPoint(Int_t trackID, Int_t detID, TVector3 pos, 
			   TVector3 mom, Double_t tof, Double_t length, 
			   Double_t eLoss)
  : CbmMCPoint(trackID, detID, pos, mom, tof, length, eLoss) { }
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmZdcPoint::~CbmZdcPoint() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmZdcPoint::Print(const Option_t* opt) const {
  cout << "-I- CbmZdcPoint: ECAL point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmZdcPoint)
