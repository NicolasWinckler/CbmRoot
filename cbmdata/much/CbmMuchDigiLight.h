/** CbmMuchDigiLight.h
 **@author E.Kryshen <e.kryshen@gsi.de>
 **@since 06.05.2013
 **@version 1.0
 **
 ** Data class for MUCH information 
 ** Data level: RAW
 ** The information is encoded into 8 bytes (1 Long64_t).
 ** Address:                       38 bits
 **   - Station Nr.                 5 bits
 **   - Layer Nr.                   3 bits
 **   - Side (front/back)           1 bits
 **   - Module                      9 bits
 **   - Sector Nr.                 10 bits
 **   - Channel Nr.                10 bits
 ** Charge (ADC):                  12 bits
 ** Time (ns):                     14 bits
 ** 
 ** Address is backward compatible with CbmMuchGeoScheme.
 **/

#ifndef CBMMUCHDIGILIGHT_H
#define CBMMUCHDIGILIGHT_H 1

#include "CbmDigi.h"
#include "CbmDetectorList.h"

#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"


class CbmMuchDigiLight : public CbmDigi{
 public:
  CbmMuchDigiLight();
  CbmMuchDigiLight(Int_t detectorId, Int_t channelId, Int_t charge, Double_t time);
  CbmMuchDigiLight(CbmMuchDigi* digi, CbmMuchDigiMatch* match);
  virtual ~CbmMuchDigiLight(){}
  Int_t GetSystemId()   const { return kMUCH; }
  Int_t GetAddress()    const { return ( kMUCH | ( (fData & fgkAddrMask) << 4) ); }
  Int_t GetDetectorId() const { return ( kMUCH | ( (fData & fgkDetrMask) << 4) ); }
  Int_t GetChannelId()  const { return ( (fData >> fgkChanShift) & fgkChanMask); }
  Int_t GetAdc()        const { return ( (fData >> fgkCharShift) & fgkCharMask); }
  Int_t GetADCCharge()  const { return GetAdc(); }
//  Double_t GetTime()    const { return Double_t( (fData >> fgkTimeShift) & fgkTimeMask ); }
  Double_t GetTime()    const { return (fData >> fgkTimeShift) & fgkTimeMask; }
  
  void AddAdc(Int_t adc);
  void SetAdc(Int_t adc);

  void SetMatch(CbmMuchDigiMatch* match) { fMatch = match; }
  CbmMuchDigiMatch* GetMatch() const { return fMatch; }
 private:
  Long64_t fData;
  CbmMuchDigiMatch* fMatch;  ///< matches to MC points (to be replaced with Fair links)
  
  static const Int_t fgkDetrBits;
  static const Int_t fgkChanBits;
  static const Int_t fgkAddrBits;
  static const Int_t fgkCharBits;
  static const Int_t fgkTimeBits;
  static const Int_t fgkChanShift;
  static const Int_t fgkCharShift;
  static const Int_t fgkTimeShift;
  static const Long64_t fgkAddrMask;
  static const Long64_t fgkCharMask;
  static const Long64_t fgkTimeMask;
  static const Long64_t fgkDetrMask;
  static const Long64_t fgkChanMask;
  
  ClassDef(CbmMuchDigiLight,1);
};

#endif
