/** CbmMuchStation.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking station.
 **/

#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchGeoScheme.h"

#include "TObjArray.h"


// -----   Default constructor   -------------------------------------------
CbmMuchStation::CbmMuchStation() {
  fDetectorId = 0;
  fZ = 0.;
  fRmin=0.;             // Minimum radius of the station [cm]
  fRmax=0.;             // Maximum radius of the station [cm]
  fModuleDesign=0;      // No detailed module design
  fTubeRmin=0.;         // Station tube Rmin
  fTubeRmax=0.;         // Station tube Rmax
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchStation::CbmMuchStation(Int_t iStation, Double_t z){
  fDetectorId = CbmMuchGeoScheme::GetDetectorId(iStation);
  fZ          = z;
  fRmin=0.;             // Minimum radius of the station [cm]
  fRmax=0.;             // Maximum radius of the station [cm]
  fModuleDesign=0;      // No detailed module design
  fTubeRmin=0.;         // Station tube Rmin
  fTubeRmax=0.;         // Station tube Rmax
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchStation::~CbmMuchStation() {
}
// -------------------------------------------------------------------------

// -----   Public method AddSector   ---------------------------------------
void CbmMuchStation::AddLayer(CbmMuchLayer* layer) {
  fLayers.Add(layer);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchStation::GetTubeDz(){
  Double_t dzmax=0;

  for (Int_t l=0; l<GetNLayers(); l++){
    CbmMuchLayer* layer = GetLayer(l);
    Double_t ldz = layer->GetDz();
    Double_t z   = layer->GetZ();
    Double_t dz  = TMath::Abs(z-fZ) + ldz;
    if (dz>dzmax) dzmax = dz;
  }

  return dzmax;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchStation)
