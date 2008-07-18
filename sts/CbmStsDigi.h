//* $Id: */

// -------------------------------------------------------------------------
// -----                      CbmStsDigi header file                   -----
// -----                  Created 28/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsDigi.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.06
 **@version 1.0
 **
 ** Data class for digital STS information
 ** Data level: RAW
 **
 ** The detector ID consists of:
 **   system ID (0-31, kSTS=2), bits 0-4, see base class
 **   station number (0-255), bits 5-12 
 **   sector number (0-32767), bits 13-27
 **   side (0=front side, 1=back side, bit 28, always 0 for pixel sectors).
 ** 
 ** The index of the (first) MCPoint having activated this channel
 ** is accessible via GetMcPointIndex(). 
 **/


#ifndef CBMSTSDIGI_H
#define CBMSTSDIGI_H 1


#include "CbmDetectorList.h"
#include "CbmDigi.h"

#include "TObject.h"

class CbmStsDigi : public CbmDigi
{

 public:

  /** Default constructor **/
  CbmStsDigi();


  /** Constructor from station number, sector number, 
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iSide     0=front side; 1=back side
   **@param iChannel  channel number
   **/
  CbmStsDigi(Int_t iStation, Int_t iSector, Int_t iSide, Int_t iChannel);


  /** Destructor **/
  virtual ~CbmStsDigi();


  /** Accessors **/
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetStationNr()    const { 
    return ( fDetectorId & (   255 <<  5 ) ) >>  5; }
  Int_t GetSectorNr()     const {
    return ( fDetectorId & ( 32767 << 13 ) ) >> 13; }
  Int_t GetSide()        const {
    return ( fDetectorId & (     1 << 28 ) ) >> 28; } // 0=front, 1=back
  

  ClassDef(CbmStsDigi,1);

};

#endif
