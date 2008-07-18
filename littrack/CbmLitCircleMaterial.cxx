 
// -------------------------------------------------------------------------
// -----                  CbmLitCircleMaterial source file               -----
// -----                  Created 27/08/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 

#include "CbmLitCircleMaterial.h"
#include "TMath.h"

// Constructor
CbmLitCircleMaterial::CbmLitCircleMaterial(): 
   fInnerRadius(0.),
   fOuterRadius(0.),
   fXCenter(0.),
   fYCenter(0.) 
{
   //
}

//Destructor
CbmLitCircleMaterial::~CbmLitCircleMaterial() {
   //
}

Bool_t CbmLitCircleMaterial::IsTraversed(Double_t xPos, Double_t yPos)
{
   Double_t pos = TMath::Sqrt( (xPos - fXCenter) * (xPos - fXCenter) + 
                               (yPos - fYCenter) * (yPos - fYCenter));
   return (pos > fInnerRadius && pos < fOuterRadius);
}


void CbmLitCircleMaterial::Print()
{
	  printf("name=%s",fName.Data());
	  printf(" z=%6.2f",fZ);
	  printf(" lz=%6.2f",fThickness);
	  printf(" rl=%f",fRadLength);
	  printf(" zeff=%6.2f",fZeff);
	  printf(" aeff=%6.2f",fAeff);
	  printf(" rho=%f",fDensity);
	  printf(" rmin=%6.2f",fInnerRadius);
	  printf(" rmax=%6.2f",fOuterRadius);
	  printf(" x0=%6.2f",fXCenter);
	  printf(" y0=%6.2f",fYCenter);
	  printf("\n");
}

ClassImp(CbmLitCircleMaterial)
