#ifndef CBMGEORICHPAR_H
#define CBMGEORICHPAR_H

#include "CbmParGenericSet.h"

#include "TObjArray.h"

class CbmGeoRichPar : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for sensitive  volumes */

  CbmGeoRichPar(const char* name="CbmGeoRichPar",
             const char* title="Rich Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoRichPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoRichPar,1)
};

#endif /* !CBMGEORICHPAR_H */
