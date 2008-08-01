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

class CbmField;
class TGeoNode;
class TGeoMaterial;

class CbmLitEnvironment: public TObject {

public:
   virtual ~CbmLitEnvironment();  
   
   static CbmLitEnvironment* Instance();
   
   CbmField* GetField();
   CbmLitDetectorLayout GetMuchLayout();
   CbmLitDetectorLayout GetTrdLayout();
      
protected:    
   CbmLitEnvironment();
 
private:
   static CbmLitEnvironment* fInstance;
   
   CbmLitDetectorLayout fMuchLayout;
   CbmLitDetectorLayout fTrdLayout;
   
   CbmField *fField;
   
   void DetermineLayout(CbmLitDetectorLayout& layout);
   
   ClassDef(CbmLitEnvironment,1)
}; 

#endif // CBMLITENVIRONMENT_H_
