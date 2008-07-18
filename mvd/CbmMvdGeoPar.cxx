// -------------------------------------------------------------------------
// -----                     CbmMvdGeoPar source file                  -----
// -----                  Created 07/11/06  by V. Friese               -----
// -----             Copied from CbmGeostsPar.cxx (D. Bertini)         -----
// -------------------------------------------------------------------------

#include "CbmMvdGeoPar.h"

#include "CbmParamList.h"

#include "TObjArray.h"

#include <iostream>
#include <iomanip>

// -----   Constructor   ---------------------------------------------------
CbmMvdGeoPar::CbmMvdGeoPar(const char* name, const char* title,
			   const char* context)
  : CbmParGenericSet(name, title, context) {
  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdGeoPar::~CbmMvdGeoPar(void) {
  if ( fGeoSensNodes ) {
    fGeoSensNodes->Delete();
    delete fGeoSensNodes;
  }
  if ( fGeoPassNodes ) {
    fGeoPassNodes->Delete();
    delete fGeoPassNodes;
  }
}
// -------------------------------------------------------------------------



// -----   Virtual public method clear   -----------------------------------
void CbmMvdGeoPar::clear() {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}
// -------------------------------------------------------------------------



// -----   Virtual public method putParams   -------------------------------
void CbmMvdGeoPar::putParams(CbmParamList* parList) {
  if ( ! parList ) return;
  parList->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
  parList->addBinary("CbmGeoNodes Passive List",   fGeoPassNodes);
}
// -------------------------------------------------------------------------



// -----   Virtual public method getParams   -------------------------------
Bool_t CbmMvdGeoPar::getParams(CbmParamList* parList) {
  if ( ! parList ) return kFALSE;
  if ( ! parList->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) 
    return kFALSE;
  if ( ! parList->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) 
    return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------




ClassImp(CbmMvdGeoPar)
