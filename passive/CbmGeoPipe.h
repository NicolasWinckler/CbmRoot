#ifndef CBMGEOPIPE_H
#define CBMGEOPIPE_H

#include "FairGeoSet.h"

class  CbmGeoPipe : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  CbmGeoPipe();
  virtual ~CbmGeoPipe();
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  Bool_t create(FairGeoBuilder*);
  ClassDef(CbmGeoPipe,0) // Class for geometry of beam pipe
};

#endif  /* !CBMGEOPIPE_H */
