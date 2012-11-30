/** CbmPsdDigi.h
 **@author Alla Maevskaya <alla@inr.ru>
 **@31 October 2012
 **@version 1.0
 **
 ** Data class for digital PSD information
 **
 ** energy deposition in 1  channel for 6 scintillators 
 ** Ideal case - Edep in GeV, idel  detector response
 **
 ** The time is relative to the event time. 
 **/


#ifndef CBMPSDDIGI_H
#define CBMPSDDIGI_H 1


#include "CbmDetectorList.h"
#include "TObject.h"

#include <vector>

class CbmPsdDigi : public TObject
{

 public:

  /**   Default constructor   **/
  CbmPsdDigi() ;
  
  CbmPsdDigi(Int_t section, Int_t module, Float_t edep) ;
  
  
  /**   Destructor   **/
  virtual ~CbmPsdDigi();
     
  /**   Setter - Getter   **/
  
  Float_t GetEdep(Int_t module, Int_t section) const { return fEdep[module][section]; }
  void SetEdep(Float_t edep, Int_t module, Int_t section) {fEdep[module][section]=edep;}

  Int_t GetModuleID() const { return fModuleID; }
  void SetModuleID(Int_t mod) {  fModuleID = mod; }

  Int_t GetSectionID() const { return fSectionID; }
  void SetSectionID(Int_t sec) {  fSectionID = sec; }
  
   void Print();
  
 private:


  /**   Data members  **/
 
  Int_t fModuleID;
  Int_t fSectionID;
  Float_t fEdep[10][44];

  
  ClassDef(CbmPsdDigi,1);

};


#endif
