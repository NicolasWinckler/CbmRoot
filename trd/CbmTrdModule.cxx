#include "CbmTrdModule.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdModule::CbmTrdModule() {
  fDetectorId = 0;
  fX        = -666.;
  fY        = -666.;
  fZ        = -666.;
  fSizex    = -666.;
  fSizey    = -666.;
  fPadSizex = -666.;
  fPadSizey = -666.;
  fIsRotated= kFALSE;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmTrdModule::CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
                           Double_t sizex, Double_t sizey, 
                           Double_t padsizex, Double_t padsizey, 
                           Bool_t rotated) {
  fDetectorId = detId;
  fX          = x;
  fY          = y;
  fZ          = z;
  fSizex      = sizex;
  fSizey      = sizey;
  fPadSizex   = padsizex;
  fPadSizey   = padsizey;
  fIsRotated  = rotated;
}
// -------------------------------------------------------------------------
CbmTrdModule::~CbmTrdModule() {

}
// -------------------------------------------------------------------------
ClassImp(CbmTrdModule)
