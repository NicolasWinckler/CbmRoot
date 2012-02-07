// -------------------------------------------------------------------------
// -----                        CbmDigi source file                    -----
// -----                  Created 09/05/07  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmDigi.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmDigi::CbmDigi() 
  : TObject(),
    fDetectorId(0),
    fChannelNr(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmDigi::CbmDigi(Int_t iSystem, Int_t iChannel) 
  : TObject(),
    fDetectorId(0),
    fChannelNr(0)
{

  // Check range for system ID
  if ( ! ( iSystem >=0 && iSystem <=31 ) ) {
    cout << "-E- CbmDigi: Illegal system identifier " << iSystem << endl;
    Fatal("", "Illegal system identifier");
  }

  fDetectorId = iSystem;            // system on bits 0-3
  fChannelNr  = iChannel;
  
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmDigi::~CbmDigi() { }
// -------------------------------------------------------------------------



ClassImp(CbmDigi)
