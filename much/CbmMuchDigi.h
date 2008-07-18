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
   **@param detectorId   Unique detector ID for sector containing this digi
   **@param time         time since event start [ns]
   **@param dTime        time resolution [ns]
   **/
  CbmMuchDigi(Long64_t detectorId, Double_t time, Double_t dTime);

  /** Constructor from a given digi object.
   *@param digi  Digi object
   **/
  CbmMuchDigi(CbmMuchDigi* digi);

  /** Destructor **/
  virtual ~CbmMuchDigi();


  /** Accessors **/
  Long64_t GetDetectorId()  const  { return fDetectorId; }
  /** Gets time since event start **/
  Double_t  GetTime(Int_t i)       { return fTime[i]; }
  Double_t* GetTimes()             { return fTime; }
  Double_t  GetDTime()             { return fDTime; }
  /** Gets charge collected by the channel. **/ 
  UInt_t GetCharge()        const  { return fCharge; }
  /** Gets charge in ADC channels collected by the channel. **/
  UInt_t GetADCCharge()     const  {return fADCCharge; }
  /** Sets charge in ADC channels collected by the channel. **/
  void SetADCCharge(UInt_t adcCharge) { fADCCharge = adcCharge; }
  /** Adds one more time information **/
  Int_t AddTime(Double_t time);
  /** Adds charge to digi **/
  UInt_t AddCharge(UInt_t iCharge);

  
 private:

  Long64_t fDetectorId;      // Detector Id
  UInt_t   fCharge;          // Charge for the digi
  UInt_t   fADCCharge;       // Charge for the digi in ADC channels
  Double_t fTime[3];         // Time since event start [ns]
  Double_t fDTime;           // Time resolution [ns]

  ClassDef(CbmMuchDigi,1);

};

#endif
