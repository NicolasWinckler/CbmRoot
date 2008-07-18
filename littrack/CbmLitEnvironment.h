// -------------------------------------------------------------------------
// -----                  CbmLitEnvironment header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitEnvironment.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** 
 **/ 


#ifndef CBMLITENVIRONMENT_H_
#define CBMLITENVIRONMENT_H_

#include "CbmLitDetectorLayout.h"
#include "TObject.h"
#include <vector>

class CbmLitMaterial;
class CbmField;
class TGeoNode;
class TGeoMaterial;

class CbmLitEnvironment: public TObject {

public:
   virtual ~CbmLitEnvironment();  
   
   static CbmLitEnvironment* Instance();
   
   CbmField* GetField();
   
   const std::vector<CbmLitMaterial*> & GetMaterials();
   const std::vector<CbmLitMaterial*> & GetMaterialsSimple();
   
   CbmLitDetectorLayout GetMuchLayout();
   
   CbmLitDetectorLayout GetTrdLayout();
   
   void DetermineLayout(CbmLitDetectorLayout& layout);
    
protected:
    
   CbmLitEnvironment();
 
private:

   static CbmLitEnvironment* fInstance;
   
   std::vector<CbmLitMaterial*> fvMaterials;
   std::vector<TGeoNode*> fvGeoNodes;

   Double_t fThickWall;
   
   CbmField *fField;
   
   void ReadFromGeoManager();
   void FillMaterialInfo(TGeoMaterial* material, CbmLitMaterial* mat);
   
   ClassDef(CbmLitEnvironment,1)
}; 

#endif // CBMLITENVIRONMENT_H_

