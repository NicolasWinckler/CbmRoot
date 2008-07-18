 
 
// -------------------------------------------------------------------------
// -----                  CbmLitRectMaterial header file               -----
// -----                  Created 27/08/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitRectangularMaterial.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 


#ifndef CbmLitRectangularMaterialAAA
#define CbmLitRectangularMaterialAAA

#include "CbmLitMaterial.h"


class CbmLitRectMaterial: public CbmLitMaterial {

public:
   
   //constructors
   CbmLitRectMaterial();
    
   // destructors
   virtual ~CbmLitRectMaterial();

   Double_t GetInnerHeight() const {return fInnerHeight;};
   Double_t GetInnerWidth() const {return fInnerWidth;};
   Double_t GetOuterHeight() const {return fOuterHeight;};
   Double_t GetOuterWidth() const {return fOuterWidth;};
   Double_t GetXCenter() const {return fXCenter;};
   Double_t GetYCenter() const {return fYCenter;};  
      
   void SetInnerHeight(Double_t innerHeight) { fInnerHeight = innerHeight;};
   void SetInnerWidth(Double_t innerWidth) { fInnerWidth = innerWidth;};
   void SetOuterHeight(Double_t outerHeight) { fOuterHeight = outerHeight;};
   void SetOuterWidth(Double_t outerWidth) { fOuterWidth = outerWidth;};
   void SetXCenter(Double_t xCenter) { fXCenter = xCenter;};
   void SetYCenter(Double_t yCenter) { fYCenter = yCenter;};
   
   Bool_t IsTraversed(Double_t xPos, Double_t yPos);
   
private:
   
   Double_t fInnerHeight, fInnerWidth;
   Double_t fOuterHeight, fOuterWidth;
   Double_t fXCenter, fYCenter;

   ClassDef(CbmLitRectMaterial,1);
};

#endif //CbmLitRectangularMaterial


