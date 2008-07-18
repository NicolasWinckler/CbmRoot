#ifndef CBMGEOTOFPAR_H
#define CBMGEOTOFPAR_H

#include "CbmParGenericSet.h"

#include "TH1F.h"

class CbmGeoTofPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for sensitive volumes

  CbmGeoTofPar(const char* name="CbmGeoTofPar",
             const char* title="Tof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoTofPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoTofPar,1)
};

#endif /* !CBMGEOTOFPAR_H */
