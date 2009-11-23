#include "CbmLitDetGeo.h"
#include "FairGeoNode.h"

ClassImp(CbmLitDetGeo)

// -----   Default constructor   -------------------------------------------
CbmLitDetGeo::CbmLitDetGeo() {
  // Constructor
  fName="litdet";
  maxSectors=0;
  maxModules=10;
}

// -------------------------------------------------------------------------

const char* CbmLitDetGeo::getModuleName(Int_t m) {
  /** Returns the module name of TutorialDet number m
      Setting MyDet here means that all modules names in the 
      ASCII file should start with TutorialDet otherwise they will 
      not be constructed
  */
  sprintf(modName,"litdet%i",m+1);
  return modName;
}

const char* CbmLitDetGeo::getEleName(Int_t m) {
  /** Returns the element name of Det number m */
  sprintf(eleName,"litdet%i",m+1);
  return eleName;
}
