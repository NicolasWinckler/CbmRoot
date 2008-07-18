 
// -------------------------------------------------------------------------
// -----                  CbmLitCircleMaterial header file               -----
// -----                  Created 27/08/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitCircleMaterial.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 


#ifndef CbmLitCircleMaterialAAA
#define CbmLitCircleMaterialAAA

#include "CbmLitMaterial.h"


class CbmLitCircleMaterial: public CbmLitMaterial {

public:
   
   //constructors
   CbmLitCircleMaterial();
    
   // destructors
   virtual ~CbmLitCircleMaterial();

   Double_t GetInnerRadius() const {return fInnerRadius;};
   Double_t GetOuterRadius() const {return fOuterRadius;};
   Double_t GetXCenter() const {return fXCenter;};
   Double_t GetYCenter() const {return fYCenter;};  
      
   void SetInnerRadius(Double_t innerRadius) { fInnerRadius = innerRadius;};
   void SetOuterRadius(Double_t outerRadius) { fOuterRadius = outerRadius;};
   void SetXCenter(Double_t xCenter) { fXCenter = xCenter;};
   void SetYCenter(Double_t yCenter) { fYCenter = yCenter;};
   
   Bool_t IsTraversed(Double_t xPos, Double_t yPos);
   
   void Print();
private:
   
   Double_t fInnerRadius;
   Double_t fOuterRadius;
   Double_t fXCenter;
   Double_t fYCenter;

   ClassDef(CbmLitCircleMaterial,1);
};

#endif //CbmLitCircleMaterial


