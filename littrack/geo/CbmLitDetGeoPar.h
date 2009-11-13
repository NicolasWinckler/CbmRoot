#ifndef CbmLitDetGeoPar_H
#define CbmLitDetGeoPar_H

#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class CbmLitDetGeoPar       : public FairParGenericSet {
public:
 
  /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoSensNodes; 

  /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; 

  CbmLitDetGeoPar(const char* name="CbmLitDetGeoPar",
		       const char* title="CbmLitDet Geometry Parameters",
		       const char* context="TestDefaultContext");
  ~CbmLitDetGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray* GetGeoSensitiveNodes() {return fGeoSensNodes;}
  TObjArray* GetGeoPassiveNodes()   {return fGeoPassNodes;}

  ClassDef(CbmLitDetGeoPar,1)
};

#endif /* CbmLitDetGeoPar_H */
