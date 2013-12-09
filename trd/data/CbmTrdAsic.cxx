#include "CbmTrdAsic.h"

#include "CbmTrdPoint.h"
#include "CbmTrdAddress.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TMath.h"

#include <iomanip>

CbmTrdAsic::CbmTrdAsic() 
  : TNamed(),
  fAsicAddress(0),
    fOrientation(0),
  fX(0.),
  fY(0.),
  fZ(0.),
  fSizeX(0.),
  fSizeY(0.),
  fSizeZ(0.)
{
}

CbmTrdAsic::CbmTrdAsic(
      Int_t address, Int_t orientation, Double_t x, Double_t y, Double_t z,
      Double_t sizex, Double_t sizey, Double_t sizez)
  : TNamed(),
  fAsicAddress(address),
  fOrientation(orientation),
  fX(x),
  fY(y),
  fZ(z),
  fSizeX(sizex),
  fSizeY(sizey),
  fSizeZ(sizez)
{

}

CbmTrdAsic::~CbmTrdAsic() 
{
}

ClassImp(CbmTrdAsic)
