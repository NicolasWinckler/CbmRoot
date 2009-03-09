// TODO comment to be changed
/** CbmMuchDigi.cxx
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
 **
 **/

#include "CbmMuchDigi.h"

#include "CbmDetectorList.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchDigi::CbmMuchDigi() {
  fDetectorId   =  0;
  fChannelId = 0;
  fCharge = 0;
  fTime[0] = fTime[1] = fTime[2] = -1;
  fDTime = 8e-2;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(Int_t detectorId, Int_t channelId, Double_t time, Double_t dTime) {
  fDetectorId = detectorId;
  fChannelId = channelId;
  fCharge = fADCCharge = 0;
  fDTime = dTime;
  fTime[0] = time;
  fTime[1] = fTime[2] = -1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(CbmMuchDigi* digi){
  fDetectorId = digi->GetDetectorId();
  fChannelId  = digi->GetChannelId();
  fCharge     = digi->GetCharge();
  fADCCharge  = digi->GetADCCharge();
  fDTime      = digi->GetDTime();
  for(int i=0;i<3;i++){
    fTime[i] = digi->GetTime(i);
  }
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMuchDigi::~CbmMuchDigi() { }
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMuchDigi::AddTime(Double_t time) {
  if ( time < 0 ) {
    cout << "-W- CbmMuchDigi::AddTime: Illegal time value "
	 << time << endl;
    return 0;
  }
  for (Int_t i=0; i<3; i++) {
    if ( fTime[i] < 0 ) {
      fTime[i] = time;
      return i+1;
    }
  }
  return 4;
}
// -------------------------------------------------------------------------
UInt_t CbmMuchDigi::AddCharge(UInt_t iCharge) {
  fCharge += iCharge;
  return fCharge;
}

ClassImp(CbmMuchDigi)
