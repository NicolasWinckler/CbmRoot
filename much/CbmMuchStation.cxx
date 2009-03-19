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
  fDetectorType=1;      // Detector type = GEM by default
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
  fDetectorType=1;      // Detector type = GEM by default
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

ClassImp(CbmMuchStation)
