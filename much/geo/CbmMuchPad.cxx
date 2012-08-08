#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"

// -------------------------------------------------------------------------
CbmMuchPad::CbmMuchPad() 
  : fDetectorId(0),
    fChannelId(0),
    fX(0.), 
    fY(0.),
    fDx(0.),
    fDy(0.),
    fNeighbours(),
    fDigiIndex(-1),
    fDigi(NULL),
    fMatch(NULL)
{
  fNeighbours.resize(20);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchPad::CbmMuchPad (Int_t detId, Long64_t channelId, Double_t x, Double_t y, Double_t dx, Double_t dy) 
  : fDetectorId(detId),
    fChannelId(channelId),
    fX(x),
    fY(y),
    fDx(dx),
    fDy(dy),
    fNeighbours(),
    fDigiIndex(-1),
    fDigi(new CbmMuchDigi(fDetectorId,fChannelId,0,0,0)),
    fMatch(new CbmMuchDigiMatch())
{
  fNeighbours.resize(20);
}
// -------------------------------------------------------------------------

