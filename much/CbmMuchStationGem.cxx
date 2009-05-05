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
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchStationGem::CbmMuchStationGem(Int_t iStation, Double_t z)
:CbmMuchStation(iStation,z){
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchStationGem::~CbmMuchStationGem() {
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchStationGem)
