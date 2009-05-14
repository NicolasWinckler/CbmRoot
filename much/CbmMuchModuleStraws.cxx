/** CbmMuchModuleStraws.cxx
 **/

#include "CbmMuchModuleStraws.h"

// -----   Default constructor   -------------------------------------------
CbmMuchModuleStraws::CbmMuchModuleStraws(): CbmMuchModule() {
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModuleStraws::CbmMuchModuleStraws(Int_t detId, TVector3 position, TVector3 size,
    Double_t cutRadius)
: CbmMuchModule(detId, position, size, cutRadius) {
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModuleStraws::CbmMuchModuleStraws(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
:  CbmMuchModule(iStation, iLayer, iSide, iModule, position, size, cutRadius) {
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchModuleStraws::~CbmMuchModuleStraws() {
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchModuleStraws)
