// -------------------------------------------------------------------------
// -----                       CbmMvdHit source file                   -----
// -----                  Created 07/11/06  by V. Friese               -----
// -----               Based on CbmStsMapsHit by M. Deveaux            -----
// -------------------------------------------------------------------------

#include "CbmMvdHit.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdHit::CbmMvdHit() {
  fFlag = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdHit::CbmMvdHit(Int_t statNr, TVector3& pos, TVector3& dpos, 
		     Int_t flag) 
  : CbmTrkHit(0, pos, dpos, 0., -1) {
  fDetectorID = (statNr << 16) | (1<<24);
  fFlag = flag;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdHit::~CbmMvdHit() {} 
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMvdHit::Print(const Option_t* opt) const {
  cout << "MvdHit in station " << GetStationNr() << " at (" << cout.width(8) 
       << GetX() << ", " << cout.width(8) << GetY() << ", " << cout.width(8)
       << GetZ() << ") cm" << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmMvdHit)
