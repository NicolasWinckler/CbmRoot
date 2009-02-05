/** CbmMuchStationDigiPar.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   14.03.07
 **
 ** Parameter container for the digitization of a MuCh station.
 ** Holds and manages an array of CbmMuchSectorDigiPar objects.
 **/


#include "CbmMuchStationDigiPar.h"

#include "TMath.h"

// -----   Default constructor   -------------------------------------------
CbmMuchStationDigiPar::CbmMuchStationDigiPar() {
  fStationNr   = -1;
  fRotation    =  0.;
  fSectors = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchStationDigiPar::CbmMuchStationDigiPar(Int_t iStation, Double_t rotation){
  fStationNr = iStation;
  fRotation  = rotation * TMath::Pi() / 180.;
  fSectors   = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchStationDigiPar::~CbmMuchStationDigiPar() {
  if ( fSectors) {
    fSectors->Delete();
    delete fSectors;
  }
}
// -------------------------------------------------------------------------




ClassImp(CbmMuchStationDigiPar)
