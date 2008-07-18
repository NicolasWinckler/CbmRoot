
#include "CbmSttPoint.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmSttPoint::CbmSttPoint() : CbmMCPoint() { }
// -------------------------------------------------------------------------

// ------   Copy constructor     ----------------------------------------------
/*
CbmSttPoint::CbmSttPoint(const CbmSttPoint& point)
  : CbmMCPoint(point)
{
 *this = point;
}
*/
// ----------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmSttPoint::CbmSttPoint(Int_t trackID, Int_t detID, TVector3 pos,
			 TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss)
  : CbmMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
    fStation(0),
    fLayer(0),
    fChamber(0)
{ }
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmSttPoint::~CbmSttPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmSttPoint::Print(const Option_t* opt) const {
  cout << "-I- CbmSttPoint: STT point for track " << fTrackID
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(CbmSttPoint)
