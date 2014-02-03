/** CbmStsDigiTb.cxx
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.2006
 **@version 2.0
 **/


#include "CbmStsDigiTb.h"


// -----   Bit fields for charge and time stamp   --------------------------
const Int_t CbmStsDigiTb::fgkCharBits = 12;   // in data0
const Int_t CbmStsDigiTb::fgkTimeBits = 14;   // in data1
const Int_t CbmStsDigiTb::fgkAddrBits =
  CbmStsDigiTb::fgkStatBits +
  CbmStsDigiTb::fgkSectBits +
  CbmStsDigiTb::fgkSideBits +
  CbmStsDigiTb::fgkChanBits;
// -------------------------------------------------------------------------



// -----   Bit shift for charge and time stamp   ---------------------------
const Int_t CbmStsDigiTb::fgkCharShift = CbmStsDigiTb::fgkAddrBits;
const Int_t CbmStsDigiTb::fgkTimeShift = CbmStsDigiTb::fgkCharShift + CbmStsDigiTb::fgkCharBits;
// -------------------------------------------------------------------------



// -----   Bit masks for charge and time stamp   ---------------------------
const Long64_t CbmStsDigiTb::fgkAddrMask = (1 << CbmStsDigiTb::fgkAddrBits) - 1;
const Long64_t CbmStsDigiTb::fgkCharMask = (1 << CbmStsDigiTb::fgkCharBits) - 1;
const Long64_t CbmStsDigiTb::fgkTimeMask = (1 << CbmStsDigiTb::fgkTimeBits) - 1;
// -------------------------------------------------------------------------




// -----   Default constructor   -------------------------------------------
CbmStsDigiTb::CbmStsDigiTb()
  : CbmDigi(),
    CbmStsDetectorId(),
    fData(0) ,
    fTime(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
// CbmStsDigiTb::CbmStsDigiTb(Int_t station, Int_t sector, Int_t side,
// 		       Int_t channel, Int_t adc, Int_t time) 
//   : FairMultiLinkedData(),
//     CbmStsDetectorId(), 
//     fData(0) 
// {
//   AddIndex(index);
//   fData = ( (DetectorId(station, sector, side, channel) >> 4) & fgkAddrMask )
//     | ( (adc  & fgkCharMask) << fgkCharShift )
//     | ( (time & fgkTimeMask) << fgkTimeShift ) ;
//  
// }
// -------------------------------------------------------------------------
CbmStsDigiTb::CbmStsDigiTb(Int_t station, Int_t sector, Int_t side,
		       Int_t channel, Int_t adc, ULong_t time)
  : CbmDigi(),
    CbmStsDetectorId(),
    fData(0),
    fTime(time)
{
//   AddIndex(index,adc);
  fData = ( (DetectorId(station, sector, side, channel) >> 4) & fgkAddrMask )
    | ( (adc  & fgkCharMask) << fgkCharShift )
    | ( (time & fgkTimeMask) << fgkTimeShift ) ; 
}



// -----   Destructor   ----------------------------------------------------
CbmStsDigiTb::~CbmStsDigiTb() { }
// -------------------------------------------------------------------------




// -----   Add charge   ----------------------------------------------------
void CbmStsDigiTb::AddAdc(Int_t adc) {
  Int_t maxAdc = (1 << fgkCharBits) - 1;
  Int_t newAdc = GetAdc() + adc;
  if ( newAdc > maxAdc ) newAdc = maxAdc;
  SetAdc(newAdc);
}
// -------------------------------------------------------------------------




// -----   Set new charge   ------------------------------------------------
void CbmStsDigiTb::SetAdc(Int_t adc) {

  // First set old charge to zero
  fData = fData & ~(fgkCharMask << fgkCharShift);

  // Now set new charge
  fData = fData | ( (adc & fgkCharMask) << fgkCharShift );

}
// -------------------------------------------------------------------------



ClassImp(CbmStsDigiTb)
