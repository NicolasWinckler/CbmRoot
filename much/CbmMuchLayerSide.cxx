/** CbmMuchLayerSide.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking layer side.
 **/

#include "CbmMuchLayerSide.h"
#include "CbmMuchLayer.h"
#include "CbmMuchModule.h"
#include "CbmMuchSector.h"

// -----   Default constructor   -------------------------------------------
CbmMuchLayerSide::CbmMuchLayerSide() {
  fDetectorId = 0;
  fZ = 0.;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchLayerSide::CbmMuchLayerSide(Int_t detId, Double_t z) {
  fDetectorId = detId;
  fZ          = z;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchLayerSide::CbmMuchLayerSide(Int_t iStation, Int_t iLayer, Bool_t iSide, Double_t z) {
  fDetectorId = CbmMuchGeoScheme::GetDetectorId(iStation, iLayer, iSide);
  fZ          = z;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchLayerSide::~CbmMuchLayerSide() {
}
// -------------------------------------------------------------------------

// -----   Public method AddModule   ---------------------------------------
void CbmMuchLayerSide::AddModule(CbmMuchModule* module) {
  fModules.Add(module);
}
// -------------------------------------------------------------------------

void CbmMuchLayerSide::DrawModules(Color_t color, Bool_t modulesVisible, Bool_t sectorsVisible){
  for (Int_t m=0;m<GetNModules();m++){
    CbmMuchModule* module = GetModule(m);
    if (modulesVisible) {
      module->SetFillColor(color);
      module->SetFillStyle(3001);
      module->Draw("f");
    }
    module->DrawModule(color);
  }
}
ClassImp(CbmMuchLayerSide)
