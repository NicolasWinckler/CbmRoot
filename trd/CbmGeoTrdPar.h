#ifndef CBMGEOTRDPAR_H
#define CBMGEOTRDPAR_H


#include "CbmParGenericSet.h"

class TObjArray;
class CbmParamList;

class CbmGeoTrdPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  CbmGeoTrdPar(const char* name="CbmGeoTrdPar",
             const char* title="Trd Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoTrdPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoTrdPar,1)
};

#endif /* !CBMGEOTRDPAR_H */
