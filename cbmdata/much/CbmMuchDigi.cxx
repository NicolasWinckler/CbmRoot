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

// -----   Default constructor   -------------------------------------------
CbmMuchDigi::CbmMuchDigi() 
  : TObject(),
    fDetectorId(0),
    fChannelId(0),
    fADCCharge(0),
    fTime(-1.),
    fDTime(8e-2),
    fDeadTime(-1.)
{
  /*
  fDetectorId   =  0;
  fChannelId = 0;
  fTime = -1;
  fDTime = 8e-2;
  */
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(Int_t detectorId, Long64_t channelId, Double_t time, Double_t dTime, Double_t deadTime) 
  : TObject(),
    fDetectorId(detectorId),
    fChannelId(channelId),
    fADCCharge(0),
    fTime(time),
    fDTime(dTime),
    fDeadTime(deadTime)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigi::CbmMuchDigi(CbmMuchDigi* digi)
  : TObject(),
    fDetectorId(digi->GetDetectorId()),
    fChannelId(digi->GetChannelId()),
    fADCCharge(digi->GetADCCharge()),
    fTime(digi->GetTime()),
    fDTime(digi->GetDTime()),
    fDeadTime(digi->GetDeadTime())
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMuchDigi::~CbmMuchDigi() { }
// -------------------------------------------------------------------------


ClassImp(CbmMuchDigi)
