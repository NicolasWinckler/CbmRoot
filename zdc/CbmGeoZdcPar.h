#ifndef CBMGEOZDCPAR_H
#define CBMGEOZDCPAR_H


#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class CbmGeoZdcPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoZdcPar(const char* name="CbmGeoZdcPar",
             const char* title="Zdc Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoZdcPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoZdcPar,1)
};

#endif /* !CBMGEOZDCPAR_H */
