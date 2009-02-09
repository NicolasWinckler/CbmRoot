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

  CbmStsDigi(Int_t iStation, Int_t iSector, Int_t iSide, Double_t iChannel, Double_t iADC, Double_t iTDC);

  /** Destructor **/
  virtual ~CbmStsDigi();

  void SetADC(Double_t iADC) { fDigiADC = iADC; }
  void SetTDC(Double_t iTDC) { fDigiTDC = iTDC; }
  void SetCor(Double_t iCor) { fDigiCor = iCor; }

  void AddADC(Double_t iADC) { fDigiADC+= iADC; }

  /** Accessors **/
  Int_t    GetDetectorId() const { return fDetectorId; }

  Int_t    GetStationNr()  const { 
    return ( fDetectorId & (255<<16) ) >> 16; }
  Int_t    GetSectorNr()   const {  // sector number within station
    return ( fDetectorId & (4095<<4) ) >> 4; }
  Int_t    GetSide()        const {
    return ( fDetectorId & (1<<0) ) >> 0; }  // 0=front, 1=back

  Double_t GetADC()          const { return fDigiADC; }
  Double_t GetTDC()          const { return fDigiTDC; }
  Double_t GetCor()          const { return fDigiCor; }

 private:

  Double_t fDigiADC;
  Double_t fDigiTDC;
  Double_t fDigiCor;

  ClassDef(CbmStsDigi,1);

};

#endif
