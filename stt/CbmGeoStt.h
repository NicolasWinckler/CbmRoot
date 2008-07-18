#ifndef CBMGEOSTT_H
#define CBMGEOSTT_H

#include "CbmGeoSet.h"

class  CbmGeoStt : public CbmGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  CbmGeoStt();
  ~CbmGeoStt() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoStt,1)
};

inline Int_t CbmGeoStt::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1; // 
}

#endif  /* !CBMSTTDETGEO_H */
