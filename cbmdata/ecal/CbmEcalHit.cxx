// -------------------------------------------------------------------------
// -----                      CbmEcalHit source file                   -----
// -----               Created 11/03/05  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------


#include "CbmEcalHit.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmEcalHit::CbmEcalHit() 
  : TObject(),
    fEnergy(-1.),
    fPSEnergy(1.),
    fTime(-1.),
    fTrackId(-1),
    fDetectorId(-1)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalHit::~CbmEcalHit() {}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmEcalHit::Print(const Option_t* opt) const {
  cout << "ECAL hit: cellid=" << GetDetectorId() << ", Energy=" << fEnergy;
  cout << ", PS Energy=" << fPSEnergy;
  if (fTrackId>0) cout << ", TrackID= " << fTrackId;
//  cout << ", x=" << GetX() << ", y=" << GetY() << endl << flush; 
}
// -------------------------------------------------------------------------

ClassImp(CbmEcalHit)
