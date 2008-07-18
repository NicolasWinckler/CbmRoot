#ifndef CBMGEOSTSPAR_H
#define CBMGEOSTSPAR_H

#include "CbmParGenericSet.h"

#include "TH1F.h"

class CbmGeoStsPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  CbmGeoStsPar(const char* name="CbmGeoStsPar",
             const char* title="Sts Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoStsPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoStsPar,1)
};

#endif /* !CBMGEOSTSPAR_H */
