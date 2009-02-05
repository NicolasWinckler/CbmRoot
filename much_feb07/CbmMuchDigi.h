// TODO comment to be changed
/** CbmMuchDigi.h
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
 ** 
 **/


#ifndef CBMMUCHDIGI_H
#define CBMMUCHDIGI_H 1


#include <iostream>
#include "TObject.h"



class CbmMuchDigi : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchDigi();

  /** Constructor from station number, sector number, 
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iChannel  channel number
   **@param time      time since event start [ns]
   **@param dTime     time resolution [ns]
   **/
  CbmMuchDigi(Int_t iStation, Int_t iSector, Int_t iChannel, Double_t time, Double_t dTime);

  /** Constructor from a given digi object.
   *@param digi  Digi object
   **/
  CbmMuchDigi(CbmMuchDigi* digi);

  /** Destructor **/
  virtual ~CbmMuchDigi();


  /** Accessors **/
  inline Int_t GetDetectorId() const { return fDetectorId; }
  inline Int_t GetSystemId()   const {
    return ( fDetectorId & (15<<24) ) >> 24; }
  inline Int_t GetStationNr()    const { 
    return ( fDetectorId & (255<<16) ) >> 16; }
  inline Int_t GetSectorNr()     const {
    return ( fDetectorId & (32767<<1) ) >> 1; }
  inline Int_t GetChannelNr()    const { return fChannelNr; }
  /** Gets time since event start **/
  inline Double_t  GetTime(Int_t i) { return fTime[i]; }
  inline Double_t* GetTimes()       { return fTime; }
  inline Double_t  GetDTime()       { return fDTime; }
  /** Gets charge collected by channel **/ 
  inline UInt_t GetCharge() const { return fCharge; }
  /** Adds one more time information **/
  Int_t AddTime(Double_t time);
  /** Adds charge to digi **/
  UInt_t AddCharge(UInt_t iCharge);

  
 private:

  Int_t    fDetectorId;      // Detector Id
  Int_t    fChannelNr;       // Channel number which this digi corresponds to
  UInt_t   fCharge;          // Charge for the digi
  Double_t fTime[3];         // Time since event start [ns]
  Double_t fDTime;           // Time resolution [ns]

  ClassDef(CbmMuchDigi,1);

};

#endif
