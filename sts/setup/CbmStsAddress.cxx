/** @file CbmStsAddress.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.05.2013
 **/


#include "CbmDetectorList.h"
#include "setup/CbmStsAddress.h"


//-----    Definition of the address field   -------------------------------
const Int_t CbmStsAddress::fgkBits[] = { 4,   // system = kSTS
                                         4,   // station
                                         4,   // ladder
                                         1,   // halfladder
                                         3,   // module
                                         2,   // sensor
                                         1,   // side
                                        13 }; // channel
// -------------------------------------------------------------------------



//-----    Initialisation of bit shifts ------------------------------------
const Int_t CbmStsAddress::fgkShift[] =
   { 0,
     CbmStsAddress::fgkShift[0] + CbmStsAddress::fgkBits[0],
     CbmStsAddress::fgkShift[1] + CbmStsAddress::fgkBits[1],
     CbmStsAddress::fgkShift[2] + CbmStsAddress::fgkBits[2],
     CbmStsAddress::fgkShift[3] + CbmStsAddress::fgkBits[3],
     CbmStsAddress::fgkShift[4] + CbmStsAddress::fgkBits[4],
     CbmStsAddress::fgkShift[5] + CbmStsAddress::fgkBits[5],
     CbmStsAddress::fgkShift[6] + CbmStsAddress::fgkBits[6] };
// -------------------------------------------------------------------------



//-----    Initialisation of bit masks  ------------------------------------
const Int_t CbmStsAddress::fgkMask[] = { ( 1 << fgkBits[0] ) -1,
                                         ( 1 << fgkBits[1] ) -1,
                                         ( 1 << fgkBits[2] ) -1,
                                         ( 1 << fgkBits[3] ) -1,
                                         ( 1 << fgkBits[4] ) -1,
                                         ( 1 << fgkBits[5] ) -1,
                                         ( 1 << fgkBits[6] ) -1,
                                         ( 1 << fgkBits[7] ) -1  };
// -------------------------------------------------------------------------



// -----  Unique element address   -----------------------------------------
Int_t CbmStsAddress::GetAddress(Int_t station,
                                Int_t ladder,
                                Int_t halfladder,
                                Int_t module,
                                Int_t sensor,
                                Int_t side,
                                Int_t channel) {

  // Catch overrunning of allowed ranges
  if ( station > ( 1 << fgkBits[kStsStation]) ) {
    LOG(FATAL) << "Station Id "  << station << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }
  if ( ladder > ( 1 << fgkBits[kStsLadder]) ) {
    LOG(FATAL) << "Ladder Id "  << ladder << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }
  if ( halfladder > ( 1 << fgkBits[kStsHalfLadder]) ) {
    LOG(FATAL) << "HalfLadder Id "  << halfladder << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }
  if ( module > ( 1 << fgkBits[kStsModule]) ) {
    LOG(FATAL) << "Module Id "  << module << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }
  if ( sensor > ( 1 << fgkBits[kStsSensor]) ) {
    LOG(FATAL) << "Sensor Id "  << sensor << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }
  if ( side > ( 1 << fgkBits[kStsSide]) ) {
    LOG(FATAL) << "Side Id "  << side << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }
  if ( channel > ( 1 << fgkBits[kStsChannel]) ) {
    LOG(FATAL) << "Channel Id "  << channel << " exceeds maximum"
               << FairLogger::endl;
    return 0;
  }

  return kSTS       << fgkShift[kStsSystem]     |
         station    << fgkShift[kStsStation]    |
         ladder     << fgkShift[kStsLadder]     |
         halfladder << fgkShift[kStsHalfLadder] |
         module     << fgkShift[kStsModule]     |
         sensor     << fgkShift[kStsSensor]     |
         side       << fgkShift[kStsSide]       |
         channel    << fgkShift[kStsChannel];

}
// -------------------------------------------------------------------------



