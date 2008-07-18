#ifndef CBMPARTEST_H
#define CBMPARTEST_H

#include "CbmParGenericSet.h"

#include "TH1F.h"

class CbmParTest : public CbmParGenericSet {
public:
  Float_t p1;
  Int_t ai[5000];
  TH1F* histo1;

  CbmParTest(const char* name="CbmParTest",
             const char* title="Test class for parameter io",
             const char* context="TestDefaultContext");
  ~CbmParTest(void);
  void clear(void);
  virtual Bool_t init(CbmParIo*){return kTRUE;}
  virtual Int_t write(CbmParIo*){return 0;}

  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  ClassDef(CbmParTest,1)
};

#endif /* !CBMPARTEST_H */
