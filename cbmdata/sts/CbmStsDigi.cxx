//* $Id: 

// -------------------------------------------------------------------------
// -----                      CbmStsDigi source file                   -----
// -----                  Created 28/08/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmStsDigi.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmStsDigi::CbmStsDigi() {
  fDetectorId   =  0;
  fChannelNr    =  0;

  fDigiADC      =  0.;
  fDigiTDC      =  0.;  
  fDigiCor      =  0.5;  
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsDigi::CbmStsDigi(Int_t iStation, Int_t iSector, Int_t iSide, 
		       Int_t iChannel) 
  : CbmDigi(kSTS, iChannel) {

  // Check range for station, sector and side
  if ( ! ( iStation >= 0 && iStation <= 255 ) ) {
    cout << "-E- CbmStsDigi: Illegal station number " << iStation << endl;
    Fatal("", "Illegal station number");
  }
  if ( ! ( iSector >= 0 && iSector <= 4095 ) ) {
    cout << "-E- CbmStsDigi: Illegal sector number " << iSector << endl;
    Fatal("", "Illegal sector number");
  }
  if ( ! ( iSide >= 0 && iSide <= 1 ) ) {
    cout << "-E- CbmStsDigi: Illegal side number " << iSide <<  endl;
    Fatal("", "Illegal side number");
  }

  // System id is set by the base class constructor on bits 0-4
  fDetectorId |= (iStation << 16);  // station nr. on bits  5-12
  fDetectorId |= (iSector  <<  4);  // sector  nr. on bits 13-27
  fDetectorId |= (iSide    <<  0);  // side        on bit  28
  // Channel number is set by the base class constructor
 
  fDigiADC      =  0.;
  fDigiTDC      =  0.;  
  fDigiCor      =  0.5;  
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsDigi::CbmStsDigi(Int_t iStation, Int_t iSector, Int_t iSide, 
		       Double_t iChannel, Double_t iADC, Double_t iTDC) 
  : CbmDigi(kSTS, (Int_t)iChannel) {

  // Check range for station, sector and side
  if ( ! ( iStation >= 0 && iStation <= 255 ) ) {
    cout << "-E- CbmStsDigi: Illegal station number " << iStation << endl;
    Fatal("", "Illegal station number");
  }
  if ( ! ( iSector >= 0 && iSector <= 4095 ) ) {
    cout << "-E- CbmStsDigi: Illegal sector number " << iSector << endl;
    Fatal("", "Illegal sector number");
  }
  if ( ! ( iSide >= 0 && iSide <= 1 ) ) {
    cout << "-E- CbmStsDigi: Illegal side number " << iSide <<  endl;
    Fatal("", "Illegal side number");
  }

  // System id is set by the base class constructor on bits 0-4
  fDetectorId |= (iStation << 16);  // station nr. on bits  5-12
  fDetectorId |= (iSector  <<  4);  // sector  nr. on bits 13-27
  fDetectorId |= (iSide    <<  0);  // side        on bit  28
  // Channel number is set by the base class constructor
 
  fDigiADC = iADC;
  fDigiTDC = iTDC;
  fDigiCor = iChannel - (Int_t)iChannel;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsDigi::~CbmStsDigi() { }
// -------------------------------------------------------------------------



ClassImp(CbmStsDigi)
