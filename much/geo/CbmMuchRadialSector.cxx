/** CbmMuchRadialSector.cxx
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@since   6.02.12
 *@version 1.0
 **
 ** This class describes the digitization scheme for a ring of pads
 **
 **/


#include "CbmMuchRadialSector.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchRadialPad.h"
#include "CbmMuchModuleSector.h"

#include "TMath.h"

// -----   Default constructor   -------------------------------------------
CbmMuchRadialSector::CbmMuchRadialSector():TCrown(){
  fDetectorId = 0;
  fSectorIndex = 0;
  fNChannels = 0;
  fNeighbours.Set(0);
  fPadAngle = 0;
}
// -------------------------------------------------------------------------


// -----  Standard constructor  --------------------------------------------
CbmMuchRadialSector::CbmMuchRadialSector(Int_t detId, Int_t iSector, 
    Double_t rMin, Double_t rMax, Double_t phi0, Double_t pad_angle, Int_t nPads):TCrown()
{
  fDetectorId  = detId;
  fSectorIndex = iSector;
  fNChannels   = nPads;
  fNeighbours.Set(0);
  fR1 = rMin;
  fR2 = rMax;
  fPhimin = phi0-pad_angle*nPads/2;
  fPhimax = phi0+pad_angle*nPads/2;
  fX1 = 0;
  fY1 = 0;
  fPadAngle = pad_angle;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMuchRadialSector::~CbmMuchRadialSector() { };
// -------------------------------------------------------------------------


// -----   Public method GetChannel   --------------------------------------
Int_t CbmMuchRadialSector::GetChannel(Double_t x, Double_t y) {
  return 0;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchRadialSector::AddPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchRadialPad* pad = new CbmMuchRadialPad(this, iChannel);
     fPads.Add(pad);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchRadialSector::DrawPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchRadialPad* pad = (CbmMuchRadialPad*) fPads.At(iChannel);
     pad->Draw("f");
     pad->Draw();
  }
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchRadialSector)
