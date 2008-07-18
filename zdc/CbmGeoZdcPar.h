#ifndef CBMGEOZDCPAR_H
#define CBMGEOZDCPAR_H


#include "CbmParGenericSet.h"

class TObjArray;
class CbmParamList;

class CbmGeoZdcPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  CbmGeoZdcPar(const char* name="CbmGeoZdcPar",
             const char* title="Zdc Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoZdcPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoZdcPar,1)
};

#endif /* !CBMGEOZDCPAR_H */
