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
  fTime = -1;
  fDTime = 8e-2;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(Int_t detectorId, Int_t channelId, Double_t time, Double_t dTime) {
  fDetectorId = detectorId;
  fChannelId = channelId;
  fADCCharge = 0;
  fDTime = dTime;
  fTime = time;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(CbmMuchDigi* digi){
  fDetectorId = digi->GetDetectorId();
  fChannelId  = digi->GetChannelId();
  fADCCharge  = digi->GetADCCharge();
  fDTime      = digi->GetDTime();
  fTime       = digi->GetTime();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMuchDigi::~CbmMuchDigi() { }
// -------------------------------------------------------------------------


ClassImp(CbmMuchDigi)
