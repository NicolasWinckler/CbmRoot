//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTarget
//
// Class for geometry of Target
//
/////////////////////////////////////////////////////////////

#include "CbmGeoTarget.h"

ClassImp(CbmGeoTarget)

CbmGeoTarget::CbmGeoTarget() {
  // Constructor
  fName="target";
  strcpy(modName,"t");
  strcpy(eleName,"t");
  maxSectors=0;
  maxModules=1;
}
