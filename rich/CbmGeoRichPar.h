#ifndef CBMGEORICHPAR_H
#define CBMGEORICHPAR_H

#include "FairParGenericSet.h"

#include "TObjArray.h"

class CbmGeoRichPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for sensitive  volumes */

  CbmGeoRichPar(const char* name="CbmGeoRichPar",
             const char* title="Rich Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoRichPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoRichPar,1)
};

#endif /* !CBMGEORICHPAR_H */
