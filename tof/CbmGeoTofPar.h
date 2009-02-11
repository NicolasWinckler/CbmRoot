#ifndef CBMGEOTOFPAR_H
#define CBMGEOTOFPAR_H

#include "FairParGenericSet.h"

#include "TH1F.h"

class CbmGeoTofPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoTofPar(const char* name="CbmGeoTofPar",
             const char* title="Tof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoTofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoTofPar,1)
};

#endif /* !CBMGEOTOFPAR_H */
