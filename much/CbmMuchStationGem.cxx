/** CbmMuchStationGem.cxx
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   18.03.09
 **
** This class holds the transport geometry parameters
 ** of one MuCh GEM/micromegas tracking station.
 ** Derived from base CbmMuchStation class
  **/

#include "CbmMuchStationGem.h"



// -----   Default constructor   -------------------------------------------
CbmMuchStationGem::CbmMuchStationGem():CbmMuchStation() {
  fSigmaXmin=0.;        // Minimum sigma in X [mm]
  fSigmaYmin=0.;        // Minimum sigma in Y [mm]
  fSigmaXmax=0.;        // Maximum sigma in X [mm]
  fSigmaYmax=0.;        // Maximum sigma in Y [mm]
  fOccupancyMax=0.;     // Maximum occupancy
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchStationGem::CbmMuchStationGem(Int_t iStation, Double_t z)
:CbmMuchStation(iStation,z){
  fSigmaXmin=0.;        // Minimum sigma in X [mm]
  fSigmaYmin=0.;        // Minimum sigma in Y [mm]
  fSigmaXmax=0.;        // Maximum sigma in X [mm]
  fSigmaYmax=0.;        // Maximum sigma in Y [mm]
  fOccupancyMax=0.;     // Maximum occupancy
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchStationGem::~CbmMuchStationGem() {
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchStationGem)
