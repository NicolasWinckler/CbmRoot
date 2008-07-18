 
 
// -------------------------------------------------------------------------
// -----                  CbmLitRectMaterial source file               -----
// -----                  Created 27/08/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 

#include "CbmLitRectMaterial.h"



//constructors
CbmLitRectMaterial::CbmLitRectMaterial() {
  fInnerHeight = 0.;
  fInnerWidth  = 0.;
  fOuterHeight = 0.;
  fOuterWidth  = 0.;
  fXCenter     = 0.;
  fYCenter     = 0.;
}


//Destructor
CbmLitRectMaterial::~CbmLitRectMaterial() {
   //
}


Bool_t CbmLitRectMaterial::IsTraversed(Double_t xPos, Double_t yPos)
{
   Bool_t isOutInnerRect = ((fXCenter + fInnerWidth/2.0) > xPos) && ((fXCenter - fInnerWidth/2.0) > xPos) &&
                           ((fYCenter + fInnerHeight/2.0) > yPos) && ((fYCenter - fInnerHeight/2.0) > yPos); 
                           
   Bool_t isInOuterRect = ((fXCenter + fOuterWidth/2.0) < xPos) && ((fXCenter - fOuterWidth/2.0) < xPos) &&
                          ((fYCenter + fOuterHeight/2.0) < yPos) && ((fYCenter - fOuterHeight/2.0) < yPos); 
    
   return  isOutInnerRect && isInOuterRect;
}

ClassImp(CbmLitRectMaterial)
