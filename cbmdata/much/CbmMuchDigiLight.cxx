/** CbmMuchDigiLight.cxx
 **@author E.Kryshen <e.kryshen@gsi.de>
 **@since 06.05.2013
 **@version 1.0
 **
 ** Data class for MUCH information 
 ** Data level: RAW **
 **/

#include "CbmMuchDigiLight.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"

// -------------------------------------------------------------------------
const Int_t CbmMuchDigiLight::fgkCharBits = 12;   // in data0
const Int_t CbmMuchDigiLight::fgkTimeBits = 14;   // in data1
const Int_t CbmMuchDigiLight::fgkDetrBits = 18;
const Int_t CbmMuchDigiLight::fgkChanBits = 20;
const Int_t CbmMuchDigiLight::fgkAddrBits = CbmMuchDigiLight::fgkDetrBits + CbmMuchDigiLight::fgkChanBits;
// -------------------------------------------------------------------------



// -----   Bit shift for charge and time stamp   ---------------------------
const Int_t CbmMuchDigiLight::fgkChanShift = CbmMuchDigiLight::fgkDetrBits;
const Int_t CbmMuchDigiLight::fgkCharShift = CbmMuchDigiLight::fgkAddrBits;
const Int_t CbmMuchDigiLight::fgkTimeShift = CbmMuchDigiLight::fgkCharShift + CbmMuchDigiLight::fgkCharBits;
// -------------------------------------------------------------------------



// -----   Bit masks for charge and time stamp   ---------------------------
const Long64_t CbmMuchDigiLight::fgkDetrMask = (1LL << CbmMuchDigiLight::fgkDetrBits) - 1;
const Long64_t CbmMuchDigiLight::fgkChanMask = (1LL << CbmMuchDigiLight::fgkChanBits) - 1;
const Long64_t CbmMuchDigiLight::fgkAddrMask = (1LL << CbmMuchDigiLight::fgkAddrBits) - 1;
const Long64_t CbmMuchDigiLight::fgkCharMask = (1LL << CbmMuchDigiLight::fgkCharBits) - 1;
const Long64_t CbmMuchDigiLight::fgkTimeMask = (1LL << CbmMuchDigiLight::fgkTimeBits) - 1;
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
CbmMuchDigiLight::CbmMuchDigiLight() 
  : CbmDigi(),
    fData(0),
    fMatch(0)
{
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
CbmMuchDigiLight::CbmMuchDigiLight(Int_t detectorId, Int_t channelId, Int_t charge, Double_t time)
  : CbmDigi(),
    fData(0),
    fMatch(0)
{
  fData |= (detectorId >> 4) & fgkDetrMask;
  fData |= (channelId        & fgkChanMask) << fgkChanShift;
  fData |= (charge           & fgkCharMask) << fgkCharShift;
  fData |= (Int_t(time)      & fgkTimeMask) << fgkTimeShift; // TODO think about time format
}
// -------------------------------------------------------------------------


CbmMuchDigiLight::CbmMuchDigiLight(CbmMuchDigi* digi, CbmMuchDigiMatch* match)
  : CbmDigi(),
    fData(0),
    fMatch(match)
{
  if (!digi) printf("pointer to CbmMuchDigi is NULL\n");
  Int_t detectorId = digi->GetDetectorId();
  Int_t channelId  = digi->GetChannelId();
  Int_t charge     = digi->GetADCCharge();
  Double_t time    = digi->GetTime();
  fData |= (detectorId >> 4) & fgkDetrMask;
  fData |= (channelId        & fgkChanMask) << fgkChanShift;
  fData |= (charge           & fgkCharMask) << fgkCharShift;
  fData |= (Int_t(time)      & fgkTimeMask) << fgkTimeShift; // TODO think about time format
}


// -----   Add charge   ----------------------------------------------------
void CbmMuchDigiLight::AddAdc(Int_t adc) {
  Int_t newAdc = GetAdc() + adc;
  if (newAdc > fgkCharMask) newAdc = fgkCharMask;
  SetAdc(newAdc);
}
// -------------------------------------------------------------------------




// -----   Set new charge   ------------------------------------------------
void CbmMuchDigiLight::SetAdc(Int_t adc) {
  // reset to 0
  fData &= ~(fgkCharMask << fgkCharShift);
  // set new value
  fData |=  (adc & fgkCharMask) << fgkCharShift;
}
// -------------------------------------------------------------------------



ClassImp(CbmMuchDigiLight)
