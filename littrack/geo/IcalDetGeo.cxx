#include "IcalDetGeo.h"
#include "FairGeoNode.h"

// -----   Default constructor   -------------------------------------------
IcalDetGeo::IcalDetGeo() {
  // Constructor
  fName="ical";
  maxSectors=0;
  maxModules=10;
}

// -------------------------------------------------------------------------

const char* IcalDetGeo::getModuleName(Int_t m) {
  /** Returns the module name of TutorialDet number m
      Setting MyDet here means that all modules names in the 
      ASCII file should start with TutorialDet otherwise they will 
      not be constructed
  */
  sprintf(modName,"ical%i",m+1);
  return modName;
}

const char* IcalDetGeo::getEleName(Int_t m) {
  /** Returns the element name of Det number m */
  sprintf(eleName,"ical%i",m+1);
  return eleName;
}

ClassImp(IcalDetGeo);
