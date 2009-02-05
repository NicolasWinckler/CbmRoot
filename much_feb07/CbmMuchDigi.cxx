// TODO comment to be changed
/** CbmMuchDigi.cxx
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
 ** The detector ID consists of:
 **   system ID (0-15, MUCH=4), bits 24-27
 **   station number (0-255), bits 16-23 
 **   sector number (0-32767), bits 1-15
 **   side (0=front side, 1=back side, bit 31, always 0 for pixel sectors).
 ** 
 **/

#include "CbmMuchDigi.h"

#include "CbmDetectorList.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchDigi::CbmMuchDigi() {
  fDetectorId   =  0;
  fChannelNr    =  0;
  fCharge = 0;
  fTime[0] = fTime[1] = fTime[2] = -1;
  fDTime = 8e-2;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(Int_t iStation, Int_t iSector, Int_t iChannel, 
			 Double_t time, Double_t dTime) {

  // Check range for station, sector and side
  if ( ! ( iStation >= 0 && iStation <= 255 ) ) {
    cout << "-E- CbmMuchDigi: Illegal station number " << iStation << endl;
    Fatal("", "Illegal station number");
  }
  if ( ! ( iSector >= 0 && iSector <= 32767 ) ) {
    cout << "-E- CbmMuchDigi: Illegal sector number " << iSector << endl;
    Fatal("", "Illegal sector number");
  }

  fDetectorId = 0;                   // old: side        on bit      0
  fDetectorId |= (iSector << 1);     // sector nr.  on bits  1-15
  fDetectorId |= (iStation << 16);   // station nr. on bits 16-23
  fDetectorId |= kMUCH << 24;        // system ID   on bits 24-27
  fChannelNr = iChannel;
  fCharge = 0;
  fDTime = dTime;
  fTime[0] = time;
  fTime[1] = fTime[2] = -1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(CbmMuchDigi* digi){
  fDetectorId = digi->GetDetectorId();
  fChannelNr  = digi->GetChannelNr();
  fCharge     = digi->GetCharge();
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
