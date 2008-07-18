#ifndef CBMGEOSTTPAR_H
#define CBMGEOSTTPAR_H

#include "CbmParGenericSet.h"

class TObjArray;
class CbmParamList;

class CbmGeoSttPar       : public CbmParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of CbmGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of CbmGeoNodes for sensitive  volumes */

  CbmGeoSttPar      (const char* name="CbmGeoSttPar",
             const char* title="Stt Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoSttPar      (void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoSttPar,1)
};

#endif /* !CBMGEOSTTPAR_H */
