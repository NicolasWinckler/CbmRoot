// -------------------------------------------------------------------------
// -----                  CbmLitMaterial source file               -----
// -----                  Created 14/08/06  by A. Lebedev               -----
// -------------------------------------------------------------------------
 

#include "CbmLitMaterial.h"


//constructors
CbmLitMaterial::CbmLitMaterial():
   fName(""),
   fZ(0.),
   fThickness(0.),
   fRadLength(0.),
   fZeff(0.),
   fAeff(0.),
   fDensity(0.)
{
   //
}

//Destructor
CbmLitMaterial::~CbmLitMaterial() {
   //
}



ClassImp(CbmLitMaterial)
