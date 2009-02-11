#include "CbmGeoStt.h"
//#include "FairGeoNode.h"

ClassImp(CbmGeoStt)

// -----   Default constructor   -------------------------------------------
CbmGeoStt::CbmGeoStt() {
  // Constructor
  fName="stt";
  maxSectors=0;
  maxModules=10;
}

// -------------------------------------------------------------------------

const char* CbmGeoStt::getModuleName(Int_t m) {

  // Returns the module name of MyDet number m

  // Setting MyDet here means that all modules names in the ASCII file should start with MyDet otherwise they will not be constructed


  sprintf(modName,"stt%i",m+1); 
  return modName;
}

const char* CbmGeoStt::getEleName(Int_t m) {
  // Returns the element name of Det number m
  sprintf(eleName,"stt%i",m+1);
  return eleName;
}
