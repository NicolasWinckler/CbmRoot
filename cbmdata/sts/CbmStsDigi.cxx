/** CbmStsDigi.cxx
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.2006
 **@version 2.0
 **/


#include "CbmStsDigi.h"

#include "FairLogger.h"

/*
// -----   Bit fields for charge and time stamp   --------------------------
const Int_t CbmStsDigi::fgkCharBits = 12;   // in data0
const Int_t CbmStsDigi::fgkTimeBits = 14;   // in data1
const Int_t CbmStsDigi::fgkAddrBits = 
  CbmStsDigi::fgkStatBits +
  CbmStsDigi::fgkSectBits +
  CbmStsDigi::fgkSideBits + 
  CbmStsDigi::fgkChanBits;
// -------------------------------------------------------------------------



// -----   Bit shift for charge and time stamp   ---------------------------
const Int_t CbmStsDigi::fgkCharShift = CbmStsDigi::fgkAddrBits;
const Int_t CbmStsDigi::fgkTimeShift = CbmStsDigi::fgkCharShift + CbmStsDigi::fgkCharBits;
// -------------------------------------------------------------------------



// -----   Bit masks for charge and time stamp   ---------------------------
const Long64_t CbmStsDigi::fgkAddrMask = (1 << CbmStsDigi::fgkAddrBits) - 1;
const Long64_t CbmStsDigi::fgkCharMask = (1 << CbmStsDigi::fgkCharBits) - 1;
const Long64_t CbmStsDigi::fgkTimeMask = (1 << CbmStsDigi::fgkTimeBits) - 1;
// -------------------------------------------------------------------------


*/



// -----   Default constructor   -------------------------------------------
CbmStsDigi::CbmStsDigi() 
  : CbmDigi(), fAddress(0), fTime(0), fCharge(0), fSectorNr(0) {
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmStsDigi::CbmStsDigi(UInt_t address, ULong64_t time, UShort_t charge)
  : CbmDigi(),
    fAddress(address),
    fTime(time),
    fCharge(charge),
    fSectorNr(0) {
}
// -------------------------------------------------------------------------


// -----   Old constructor   -----------------------------------------------
CbmStsDigi::CbmStsDigi(UInt_t address, ULong64_t time, UShort_t charge,
		                   Int_t sectorNr)
	: CbmDigi(),
	  fAddress(address),
	  fTime(time),
	  fCharge(charge),
	  fSectorNr(sectorNr) {
}
	// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsDigi::~CbmStsDigi() { }
// -------------------------------------------------------------------------



/*
// -----   Add charge   ----------------------------------------------------
void CbmStsDigi::AddAdc(Int_t adc) {
  Int_t maxAdc = (1 << fgkCharBits) - 1;
  Int_t newAdc = GetAdc() + adc;
  if ( newAdc > maxAdc ) newAdc = maxAdc;
  SetAdc(newAdc);
}
// -------------------------------------------------------------------------

 */



/*
// -----   Set new charge   ------------------------------------------------
void CbmStsDigi::SetAdc(Int_t adc) {

  // First set old charge to zero
  fData = fData & ~(fgkCharMask << fgkCharShift);

  // Check for ADC range
  if ( adc > GetMaxAdc() ) {
    FairLogger::GetLogger()->Warning(MESSAGE_ORIGIN,
        "ADC %i exceeds ADC range %i", adc, GetMaxAdc());
    return;
  }

  // Now set new charge
  fData = fData | ( (adc & fgkCharMask) << fgkCharShift );

}
// -------------------------------------------------------------------------

*/

ClassImp(CbmStsDigi)
