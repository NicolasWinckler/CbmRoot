//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoMagnet
//
// Class for the geometry of detector part Magnet
//
/////////////////////////////////////////////////////////////

#include "CbmGeoMagnet.h"

ClassImp(CbmGeoMagnet)

CbmGeoMagnet::CbmGeoMagnet() {
  // Constructor
  fName="magnet";
  maxSectors=0;
  maxModules=1;
  strcpy(modName,"m");
  strcpy(eleName,"m");
}
