// -------------------------------------------------------------------------
// -----                         CbmDigi header file                   -----
// -----                  Created 09/05/07  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmDigi.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 09.05.07
 **@version 1.0
 **
 ** Base class for digital information
 ** Data level: RAW
 **
 ** The four first bits of detectorId encode the system (subdetector).
 ** The other bits can be used to denote internal structures of the
 ** system (stations, sectors, modules etc.)
 **/


#ifndef CBMDIGI_H
#define CBMDIGI_H 1


#include "TObject.h"



class CbmDigi : public TObject
{

 public:

  /** Default constructor **/
  CbmDigi();


  /** Constructor from system ID and channel number 
   **@param iSystem   System identifier (1-15)
   **@param iChannel  channel number
   **/
  CbmDigi(Int_t iSystem, Int_t iChannel);


  /** Destructor **/
  virtual ~CbmDigi();


  /** Accessors **/
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetChannelNr()  const { return fChannelNr; }
  Int_t GetSystemId()   const { return ( fDetectorId & 31 ); }
  

 protected:

  Int_t fDetectorId;       // Detector unique identifier
  Int_t fChannelNr;        // Channel number in detector


  ClassDef(CbmDigi,1);

};

#endif
