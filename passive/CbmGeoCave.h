#ifndef CBMGEOCAVE_H
#define CBMGEOCAVE_H

#include "CbmGeoSet.h"
#include "CbmGeoMedia.h"

#include "TString.h"

class  CbmGeoCave : public CbmGeoSet {
protected:
  TString name;
public:
  CbmGeoCave();
  ~CbmGeoCave() {}
  const char* getModuleName(Int_t) {return name.Data();}  
  Bool_t read(fstream&,CbmGeoMedia*);
  void addRefNodes();
  void write(fstream&);
  void print();
  ClassDef(CbmGeoCave,0) // Class for the geometry of CAVE
};

#endif  /* !CBMGEOCAVE_H */
