#ifndef CBMGEOECAL_H
#define CBMGEOECAL_H

#include "CbmGeoSet.h"

class  CbmGeoEcal : public CbmGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  CbmGeoEcal();
  ~CbmGeoEcal() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoEcal,0) // Class for Trd
};

inline Int_t CbmGeoEcal::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !CBMGEOECAL_H */
