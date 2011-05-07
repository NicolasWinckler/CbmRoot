/** CbmMuchDigiMatch.cxx
 **@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   23.03.07
 **@version 1.0
 **
 ** Data class for matching CbmMuchDigi with CbmMuchPoint.
 ** The indices of CbmMuchDigi and CbmMuchDigiMatch correspond in their
 ** respective arrays.
 ** The class holds the indices of all MuchPoints corresponding to
 ** the MuchDigi.
 **/


#include "CbmMuchDigiMatch.h"
#include "CbmMuchPoint.h"
#include "TClonesArray.h"

#include <iostream>
#include <limits>
#include <cassert>
#include <map>
using std::cout;
using std::endl;
using std::numeric_limits;
using std::map;
using std::pair;

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
  assert(fCharge.At(n) == iCharge);
  return n+1;
}
// -------------------------------------------------------------------------


// -----   Public method AddCharge  ----------------------------------------
UInt_t CbmMuchDigiMatch::AddCharge(Int_t iPoint, UInt_t iCharge) {
  for (Int_t i=0;i<fRefIndex.GetSize();i++){
    if (fRefIndex[i]!=iPoint) continue;
    fCharge[i]+=iCharge;
    return 0;
  }
  AddPoint(iPoint);
  return AddCharge(iCharge);
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
  for(Int_t iRef=0; iRef<GetNPoints(); ++iRef){
    UInt_t first = totCharge;
    totCharge += GetCharge(iRef);
    if(totCharge < first || totCharge < GetCharge(iRef)) totCharge = numeric_limits<Int_t>::max();
  }
  return totCharge;
}
// -------------------------------------------------------------------------

void CbmMuchDigiMatch::SortPointsInTime(TClonesArray* points) {
  map<Double_t,Int_t> time_vs_index;
  map<Double_t,Int_t> time_vs_charge;
  map<Double_t,Int_t>::iterator it;
  assert(points);
  for (Int_t i=0;i<fRefIndex.GetSize();i++){
    assert(fRefIndex[i]<points->GetEntriesFast());
    CbmMuchPoint* point = (CbmMuchPoint*) points->At(fRefIndex[i]);
    assert(point);
    Double_t time = point->GetTime(); 
    time_vs_index.insert(pair<Double_t,Int_t>(time,fRefIndex[i]));
    time_vs_charge.insert(pair<Double_t,Int_t>(time,fCharge[i]));
  }
  Int_t i=0;
  for (it=time_vs_index.begin();it!=time_vs_index.end();++it) fRefIndex[i++] = it->second;
  i=0;
  for (it=time_vs_charge.begin();it!=time_vs_charge.end();++it) fCharge[i++] = it->second;
  time_vs_index.clear();
  time_vs_charge.clear();
}


// -------------------------------------------------------------------------

void CbmMuchDigiMatch::Reset() {
  fRefIndex.Reset();
  fCharge.Reset();
  fRefIndex.Set(0);
  fCharge.Set(0);
}

ClassImp(CbmMuchDigiMatch)
