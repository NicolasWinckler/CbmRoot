// TODO comment to be changed
/** CbmMuchDigi.h
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
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
   **@param detectorId   Unique detector ID (including module number)
   **@param channelId    Unique channel ID within the module
   **@param time         time since event start [ns]
   **@param dTime        time resolution [ns]
   **/
  CbmMuchDigi(Int_t detectorId, Int_t channelId, Double_t time, Double_t dTime);

  /** Constructor from a given digi object.
   *@param digi  Digi object
   **/
  CbmMuchDigi(CbmMuchDigi* digi);

  /** Destructor **/
  virtual ~CbmMuchDigi();


  /** Accessors **/
  Int_t GetDetectorId()  const  { return fDetectorId; }
  Int_t GetChannelId()   const  { return fChannelId; }
  /** Gets time since event start **/
  Double_t  GetTime(Int_t i)       { return fTime[i]; }
  Double_t* GetTimes()             { return fTime; }
  Double_t  GetDTime()             { return fDTime; }
  /** Gets charge in ADC channels collected by the channel. **/
  UInt_t GetADCCharge()     const  {return fADCCharge; }
  /** Sets charge in ADC channels collected by the channel. **/
  void SetADCCharge(UInt_t adcCharge) { fADCCharge = adcCharge; }
  /** Adds one more time information **/
  Int_t AddTime(Double_t time);


 private:

  Int_t    fDetectorId;      // Detector Id (including module number)
  Int_t    fChannelId;       // Channel Id within the module
  UInt_t   fADCCharge;       // Charge for the digi in ADC channels
  Double_t fTime[3];         // Time since event start [ns]
  Double_t fDTime;           // Time resolution [ns]

  ClassDef(CbmMuchDigi,1);

};

#endif
