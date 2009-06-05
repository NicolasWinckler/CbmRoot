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
CbmTrdDigiMatch::CbmTrdDigiMatch() { 
  fNrRefIndex =0;
  for (Int_t i=0; i<10; i++) fRefIndex[i] = -1;
};

// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdDigiMatch::CbmTrdDigiMatch(Int_t iPoint) { 

    fRefIndex[fNrRefIndex++] = iPoint;
    for (Int_t i=fNrRefIndex; i<10; i++) fRefIndex[i] = -1;
};
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdDigiMatch::~CbmTrdDigiMatch() { };
// -------------------------------------------------------------------------



// -----   Public method AddPoint   ----------------------------------------
Int_t CbmTrdDigiMatch::AddPoint(Int_t iPoint) {

  if(fNrRefIndex<9) {
    fRefIndex[fNrRefIndex++] = iPoint;
    return fNrRefIndex;
  } else {
    return 11;
  }

}
// -------------------------------------------------------------------------

  

// -----   Public method GetRefIndex   -------------------------------------
Int_t CbmTrdDigiMatch::GetRefIndex(Int_t i) const {
  if ( i<0 || i>=10 ) {
    cout << "-W- CbmTrdDigiMatch::GetRefIndex: Illegal index number "
	 << i << endl;
    return -1;
  }
  return fRefIndex[i];
}
// -------------------------------------------------------------------------



ClassImp(CbmTrdDigiMatch)
