/** CbmMuchHit.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since 22.03.07
 **
 ** Data class for a reconstructed hit in the MUCH
 ** Data level RECO
 ** Derived from FairHit.
 ** Additional members: Channel numbers and covariance in x and y
 **/

#include "CbmMuchHit.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchHit::CbmMuchHit() {
  fCluster = -1;
  for(Int_t i=0;i<3;i++){
    fTime[i] = -1;
  }
  fDTime = 8e-2;
  fStationNr = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchHit::CbmMuchHit(Int_t detId, TVector3& pos, TVector3& dpos,
		     Double_t covXY)
  : CbmHit(detId, pos, dpos, covXY, -1) {
  fCluster = -1;
  for(Int_t i=0;i<3;i++){
    fTime[i] = -1;
  }
  fDTime = 8e-2;
  fStationNr = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchHit::CbmMuchHit(Int_t detId, TVector3& pos, TVector3& dpos,
		       Double_t covXY, Int_t iCluster, Double_t* times,
		       Double_t dTime, Int_t stationNr)
  : CbmHit(detId, pos, dpos, covXY, -1) {
    fCluster = -1;
    for(Int_t i = 0;i<3;i++){
      fTime[i] = times[i];
    }
    fCluster = iCluster;
    fDTime = dTime;
    fStationNr = stationNr;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchHit::  CbmMuchHit(Int_t detId, TVector3& pos, TVector3& dpos,
			 Double_t covXY, Int_t iCluster, Int_t stationNr)
  : CbmHit(detId, pos, dpos, covXY, -1) {
    fRefIndex = -1;
    fCluster  = iCluster;
    fStationNr = stationNr;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMuchHit::~CbmMuchHit() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
/*
void CbmMuchHit::Print(Option_t* opt) const {
  cout.precision(5);
  cout << "MuchHit at (" << fX << ", " << fY << ", " << fZ << ") cm, "
       << "Station " << CbmMuchGeoScheme::GetStationIndex(fDetectorID)
       << ", Layer " << CbmMuchGeoScheme::GetLayerIndex(fDetectorID)
       << ", Side " << CbmMuchGeoScheme::GetLayerSideIndex(fDetectorID)
       << ", Module " << CbmMuchGeoScheme::GetModuleIndex(fDetectorID)
       << ", Sector " << CbmMuchGeoScheme::GetSectorIndex(fDetectorID)
       << ", Digi " << fRefIndex << endl;
}
*/
// -------------------------------------------------------------------------


ClassImp(CbmMuchHit)
