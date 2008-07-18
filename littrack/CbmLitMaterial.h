// -------------------------------------------------------------------------
// -----                  CbmLitMaterial header file               -----
// -----                  Created 14/08/06  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitMaterial.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 
 
#ifndef CbmLitMaterialAAA
#define CbmLitMaterialAAA

#include "TObject.h"
#include "TString.h"

#include <iostream>

class CbmLitMaterial: public TObject {

public:
   
   //constructors
   CbmLitMaterial();
    
   // destructors
   virtual ~CbmLitMaterial();
   
   virtual Bool_t IsTraversed(Double_t xPos, Double_t yPos) {return false;};
      
   TString &GetName() { return fName; };
   Double_t GetZ() const { return fZ; };
   Double_t GetThickness() const { return fThickness; };
   Double_t GetRadLength() const { return fRadLength; };
   Double_t GetZeff() const { return fZeff; };
   Double_t GetAeff() const { return fAeff; };
   Double_t GetDensity() const { return fDensity; };  
   
   void SetName(TString &name) { fName = name; };
   void SetZ(Double_t Z) { fZ = Z; };
   void SetThickness(Double_t Thickness) { fThickness = Thickness;};
   void SetRadLength(Double_t RadLength) { fRadLength = RadLength;};
   void SetZeff(Double_t Zeff) { fZeff = Zeff;};
   void SetAeff(Double_t Aeff) { fAeff = Aeff;};
   void SetDensity(Double_t Density) { fDensity = Density;}; 
   
   virtual void Print(){ std::cout << "Print not implemented" << std::endl;}; 
   
protected:
   
   TString fName;
   Double_t fZ;
   Double_t fThickness;
   Double_t fRadLength ;
   Double_t fZeff;
   Double_t fAeff;
   Double_t fDensity;


   ClassDef(CbmLitMaterial,1);
};

#endif //CbmLItMaterial


