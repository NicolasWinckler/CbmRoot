#ifndef CBMGEOTARGET_H
#define CBMGEOTARGET_H

#include "CbmGeoSet.h"

class  CbmGeoTarget : public CbmGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  CbmGeoTarget();
  ~CbmGeoTarget() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  ClassDef(CbmGeoTarget,0) // Class for geometry of Target
};

#endif  /* !CBMGEOTARGET_H */
