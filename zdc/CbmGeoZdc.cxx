//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTrd
//
// Class for geometry of ECAL
//
/////////////////////////////////////////////////////////////

#include "CbmGeoZdc.h"

ClassImp(CbmGeoZdc)

CbmGeoZdc::CbmGeoZdc() {
  // Constructor
  fName="zdc";
  maxSectors=0;
  maxModules=10;
}

const char* CbmGeoZdc::getModuleName(Int_t m) {
  // Returns the module name of ecal number m
  sprintf(modName,"zdc%i",m+1);
  return modName;
}

const char* CbmGeoZdc::getEleName(Int_t m) {
  // Returns the element name of zdc number m
  sprintf(eleName,"zdcd%i",m+1);
  return eleName;
}
