/** CbmMuchLayer.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters 
 ** of one MuCh tracking layer.
 **/

#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchGeoScheme.h"

// -----   Default constructor   -------------------------------------------
CbmMuchLayer::CbmMuchLayer() {
  fDetectorId = 0;
  fZ = 0.;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchLayer::CbmMuchLayer(Long64_t detId, Double_t z):
  fDetectorId(detId),
  fZ(z){
  Int_t stId = CbmMuchGeoScheme::GetStationIndex(detId);
  Int_t layerId = CbmMuchGeoScheme::GetLayerIndex(detId); 
  fSideF=CbmMuchLayerSide(stId,layerId,0,z);
  fSideB=CbmMuchLayerSide(stId,layerId,1,z);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchLayer::CbmMuchLayer(Int_t iStation, Int_t iLayer, Double_t z)  {
  fDetectorId = CbmMuchGeoScheme::GetDetectorId(iStation, iLayer);
  fZ          = z;
  
  fSideF=CbmMuchLayerSide(iStation,iLayer,0,z);
  fSideB=CbmMuchLayerSide(iStation,iLayer,1,z);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchLayer::~CbmMuchLayer() {
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchLayer)
