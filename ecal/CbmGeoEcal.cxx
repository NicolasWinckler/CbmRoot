//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTrd
//
// Class for geometry of ECAL
//
/////////////////////////////////////////////////////////////

#include "CbmGeoEcal.h"

#include "FairGeoNode.h"

ClassImp(CbmGeoEcal)

CbmGeoEcal::CbmGeoEcal() {
  // Constructor
  fName="ecal";
  maxSectors=0;
  maxModules=10;
}

const char* CbmGeoEcal::getModuleName(Int_t m) {
  // Returns the module name of ecal number m
  sprintf(modName,"ecal%i",m+1);
  return modName;
}

const char* CbmGeoEcal::getEleName(Int_t m) {
  // Returns the element name of ecal number m
  sprintf(eleName,"ecald%i",m+1);
  return eleName;
}
