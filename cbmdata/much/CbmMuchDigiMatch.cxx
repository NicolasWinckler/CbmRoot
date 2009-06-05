/** CbmMuchDigiMatch.cxx
 **@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   23.03.07
 **@version 1.0
 **
 ** Data class for matching CbmMuchDigi with CbmMuchPoint.
 ** The indizes of CbmMuchDigi and CbmMuchDigiMatch correspond in their
 ** respective arrays.
 ** The class holds the indices of all MuchPoints corresponding to
 ** the MuchDigi.
 **/


#include "CbmMuchDigiMatch.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default  constructor   ------------------------------------------
CbmMuchDigiMatch::CbmMuchDigiMatch() {
  fRefIndex.Set(0);
  fCharge.Set(0);
};
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchDigiMatch::CbmMuchDigiMatch(CbmMuchDigiMatch* match){
   if(!match){
      cout << "-W- CbmMuchDigiMatch: NULL CbmMuchDigiMatch object " << endl;
   }
   else {
      fRefIndex.Set(match->GetNPoints());
      fCharge.Set(match->GetNPoints());
      for(Int_t i=0; i<fRefIndex.GetSize();i++){
         fRefIndex.AddAt(match->GetRefIndex(i), i);
         fCharge.AddAt(match->GetCharge(i), i);
      }
   }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigiMatch::~CbmMuchDigiMatch() { };
// -------------------------------------------------------------------------



// -----   Public method AddPoint   ----------------------------------------
Int_t CbmMuchDigiMatch::AddPoint(Int_t iPoint) {
  if ( iPoint < 0 ) {
    cout << "-W- CbmMuchDigiMatch::AddPoint: Illegal MuchPoint index "
	 << iPoint << endl;
    return 0;
  }
  Int_t n = fRefIndex.GetSize();
  fRefIndex.Set(n+1);
  fRefIndex.AddAt(iPoint, n);
  return n+1;
}
// -------------------------------------------------------------------------

// -----   Public method GetRefIndex   -------------------------------------
Int_t CbmMuchDigiMatch::GetRefIndex(Int_t i) const {
  if ( i<0 || i>GetNPoints()-1 ) {
    cout << "-W- CbmMuchDigiMatch::GetRefIndex: Illegal index number "
	 << i << endl;
    return -1;
  }
  return fRefIndex.At(i);
}
// -------------------------------------------------------------------------

// -----   Public method AddCharge  ----------------------------------------
UInt_t CbmMuchDigiMatch::AddCharge(UInt_t iCharge) {
  if ( iCharge < 0 ) {
    cout << "-W- CbmMuchDigiMatch::AddCharge: Illegal charge value "
	 << iCharge << endl;
    return 0;
  }
  Int_t n = fCharge.GetSize();
  fCharge.Set(n+1);
  fCharge.AddAt(iCharge, n);
  return n+1;
}
// -------------------------------------------------------------------------

// -----   Public method GetCharge  ----------------------------------------
UInt_t CbmMuchDigiMatch::GetCharge(Int_t i) const {
  if ( i<0 || i>GetNPoints()-1 ) {
    cout << "-W- CbmMuchDigiMatch::GetCharge: Illegal index number "
	 << i << endl;
    return 0;
  }
  return fCharge.At(i);
}
// -------------------------------------------------------------------------

// -----   Public method GetTotalCharge  -----------------------------------
UInt_t CbmMuchDigiMatch::GetTotalCharge() const {
  UInt_t totCharge = 0;
  for(Int_t iRef = 0; iRef < GetNPoints(); ++iRef){
    totCharge += fCharge.At(iRef);
  }
  return totCharge;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchDigiMatch)
