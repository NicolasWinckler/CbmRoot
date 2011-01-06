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
  CbmMuchDigi(Int_t detectorId, Long64_t channelId, Double_t time, Double_t dTime, Double_t deadTime=0);

  /** Constructor from a given digi object.
   *@param digi  Digi object
   **/
  CbmMuchDigi(CbmMuchDigi* digi);

  /** Destructor **/
  virtual ~CbmMuchDigi();


  /** Accessors **/
  Int_t GetDetectorId()  const  { return fDetectorId; }
  Long64_t GetChannelId()   const  { return fChannelId; }
  /** Gets time since event start [ns] **/
  Double_t  GetTime() const {return fTime;}
  /** Gets time resolution [ns] */
  Double_t  GetDTime() const { return fDTime; }
  /** Gets channel dead time [ns] */
  Double_t GetDeadTime() const {return fDeadTime; }
  /** Gets charge in ADC channels collected by the channel. **/
  UInt_t GetADCCharge() const  {return fADCCharge; }
  /** Sets charge in ADC channels collected by the channel. **/
  void SetADCCharge(UInt_t adcCharge) { fADCCharge = adcCharge; }
  /** Set time **/
  void SetTime(Double_t time) {fTime = time;}
  /** Set time uncertainty**/
  void SetDTime(Double_t dtime) {fDTime = dtime;}
  /** Set channel dead time**/
  void SetDeadTime(Double_t time) {fDeadTime = time;}


 private:

  Int_t    fDetectorId;      // Detector Id (including module number)
  Long64_t    fChannelId;       // Channel Id within the module
  UInt_t   fADCCharge;       // Charge for the digi in ADC channels
  Double_t fTime;            // Time since event start [ns]
  Double_t fDTime;           // Time resolution [ns]
  Double_t fDeadTime;        // Channel dead time
  
  ClassDef(CbmMuchDigi,2);

};

#endif
