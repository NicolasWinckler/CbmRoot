//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmTrdDigiMatch source file                -----
// -----                  Created 27/02/09  by F. Uhlig                -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiMatch.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default onstructor   --------------------------------------------
CbmTrdDigiMatch::CbmTrdDigiMatch() 
:TObject(),
 fRefIndex(0) 
{ 

};

// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdDigiMatch::CbmTrdDigiMatch(Int_t iPoint) 
:TObject(),
 fRefIndex(0) 
{ 
  Int_t n = fRefIndex.GetSize();
  fRefIndex.Set(n+1);
  fRefIndex.AddAt(iPoint, n);
};
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdDigiMatch::~CbmTrdDigiMatch() { };
// -------------------------------------------------------------------------



// -----   Public method AddPoint   ----------------------------------------
Int_t CbmTrdDigiMatch::AddPoint(Int_t iPoint) 
{
  Int_t n = fRefIndex.GetSize();
  fRefIndex.Set(n+1);
  fRefIndex.AddAt(iPoint, n);
  return n+1;
}
// -------------------------------------------------------------------------

  

// -----   Public method GetRefIndex   -------------------------------------
Int_t CbmTrdDigiMatch::GetRefIndex(Int_t i) const 
{
  if ( i<0 || i> fRefIndex.GetSize() ) {
    cout << "-W- CbmTrdDigiMatch::GetRefIndex: Illegal index number "
	 << i << endl;
    return -1;
  }
  return fRefIndex.At(i);
}
// -------------------------------------------------------------------------



ClassImp(CbmTrdDigiMatch)
