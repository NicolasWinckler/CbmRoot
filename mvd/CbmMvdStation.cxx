// -------------------------------------------------------------------------
// -----                     CbmMvdStation source file                 -----
// -----                  Created 02/12/08  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmMvdStation.h"

#include <iostream>


using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmMvdStation::CbmMvdStation() : fVolumeId(0),
				 fZ(0.),
				 fD(0.),
				 fRmin(0.),
				 fRmax(0.) {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdStation::CbmMvdStation(const char* name, Int_t volumeId,
			     Double_t z, Double_t d,
			     Double_t rmin, Double_t rmax)
  : TNamed(name, ""), fVolumeId(volumeId), fZ(z), fD(d),
    fRmin(rmin), fRmax(rmax) {
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdStation::~CbmMvdStation() {
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMvdStation::Print(Option_t* opt) const {
  cout << " --- " << GetName() << " , Id " << fVolumeId
       << ", z = " << fZ << " cm, d = " << fD << " cm, Rmin = "
       << fRmin << " cm, Rmax = " << fRmax << ", points: " 
       << GetNPoints() << endl;
}
// -------------------------------------------------------------------------



ClassImp(CbmMvdStation)
