// -------------------------------------------------------------------------
// -----                       CbmTrkHit source file                   -----
// -----                   Created 16/11/07  by V. Friese              -----
// -------------------------------------------------------------------------


#include <iostream>
#include "CbmTrkHit.h"

using namespace std;


// -----   Default constructor   -------------------------------------------
CbmTrkHit::CbmTrkHit() {
  fCovXY = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrkHit::CbmTrkHit(Int_t detId, TVector3& pos, TVector3& dpos, 
		     Double_t covXY, Int_t index) 
  : FairHit(detId, pos, dpos, index) {
  fCovXY = covXY;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrkHit::~CbmTrkHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void CbmTrkHit::Print(Option_t* opt) const {
  cout.precision(5);
  cout << "Hit at (" << fX << ", " << fY << ", " << fZ << ") cm, "
       << "Detector " << fDetectorID << ", Station " << GetStationNr() 
       << endl;
}
// -------------------------------------------------------------------------



ClassImp(CbmTrkHit)
