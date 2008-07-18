
/////////////////////////////////////////////////////////////
// CbmShieldGeo
//
// Class for geometry of shield
//
/////////////////////////////////////////////////////////////

#include "CbmShieldGeo.h"

ClassImp(CbmShieldGeo)

CbmShieldGeo::CbmShieldGeo() {
  // Constructor
  fName="shield";
  strcpy(modName,"m");
  strcpy(eleName,"m");
  maxSectors=0;
  maxModules=1;
}
