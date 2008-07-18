#ifndef CBMSHIELDGEO_H
#define CBMSHIELDGEO_H 1

#include "CbmGeoSet.h"

class  CbmShieldGeo : public CbmGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  CbmShieldGeo();
  ~CbmShieldGeo() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  ClassDef(CbmShieldGeo,0) // Class for geometry of shield
};

#endif 
