#ifndef CBMGEOPASSIVEPAR_H
#define CBMGEOPASSIVEPAR_H

#include "CbmParGenericSet.h"

#include "TH1F.h"

class CbmGeoPassivePar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  CbmGeoPassivePar(const char* name="CbmGeoPassivePar",
             const char* title="Passive Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoPassivePar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoPassivePar,1)
};

#endif /* !CBMGEOPASSIVEPAR_H */
