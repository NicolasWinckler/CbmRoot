/** CbmStsDigi.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.2006
 **@version 2.0
 **
 ** Data class for digital STS information
 ** Data level: RAW
 **
 ** The information is encoded into 6 bytes (1 Int_t, 1 Short_t).
 ** Address:                       22 bits
 **   - Station Nr.        4 bits
 **   - Sector Nr.        10 bits
 **   - Side (front/back)  1 bit
 **   - Channel Nr.        7 bits
 ** Charge (ADC):                  12 bits
 ** Time (ns):                     14 bits
 **
 ** The time is relative to the event time. 
 **/


#ifndef CBMSTSDIGI_H
#define CBMSTSDIGI_H 1


#include "CbmDetectorList.h"
#include "CbmStsDetectorId.h"


#include "TObject.h"



class CbmStsDigi : public TObject, public CbmStsDetectorId
{

 public:

  /**   Default constructor   **/
  CbmStsDigi();


  /**  Constructor from station number, sector number, 
   **  front/back side and channel number
   **@param station  station number (1 -   15)
   **@param sector   sector number  (1 - 1023)
   **@param side     0=front side; 1=back side
   **@param channel  channel number (1 -  1023)
   **@param adc      Charge ADC channel (0 - 4095)
   **@param time     Time within event [ns]
   **/
  CbmStsDigi(Int_t station, Int_t sector, Int_t side, 
	     Int_t channel, Int_t adc, Int_t time);


  /**   Destructor   **/
  virtual ~CbmStsDigi();


  /**   Add charge   **/
  void AddAdc(Int_t adc);


  /**   Station number   **/
  Int_t GetStationNr() const { return StationNr(GetDetectorId()); }

  /**   Sector number   **/
  Int_t GetSectorNr() const { return SectorNr(GetDetectorId()); }


  /**   Side ( 0 = front, 1 = back )   **/
  Int_t GetSide() const { return Side(GetDetectorId()); }


  /**   Channel number   **/
  Int_t GetChannelNr() const { return ChannelNr(GetDetectorId()); }


  /**   Charge   **/
  Int_t GetAdc() const { return ( (fData >> fgkCharShift) & fgkCharMask ); }


  /**   Time   **/
  Int_t GetTime() const { return ( (fData >> fgkTimeShift) & fgkTimeMask ); }


  /**   Unique detector ID   **/
  Int_t GetDetectorId() const { 
    return ( kSTS | ( (fData & fgkAddrMask) << 4) );
  }



 private:


  /**   Data members  **/
  Long64_t fData;


  /**   Modifiers   **/
  void SetAdc(Int_t charge);


  /**   Static members for bit operations   **/
  static const Int_t fgkAddrBits;
  static const Int_t fgkCharBits;
  static const Int_t fgkTimeBits;
  static const Int_t fgkCharShift;
  static const Int_t fgkTimeShift;
  static const Long64_t fgkAddrMask;
  static const Long64_t fgkCharMask;
  static const Long64_t fgkTimeMask;



  ClassDef(CbmStsDigi,2);

};


#endif
