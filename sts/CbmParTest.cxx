
#include "CbmParTest.h"

#include "CbmParamList.h"

#include <iostream>
#include <iomanip>

ClassImp(CbmParTest)

CbmParTest::CbmParTest(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {
  clear();
  histo1=new TH1F("h1","test histogram",100,-3,3);
  histo1->SetDirectory(0);
}

CbmParTest::~CbmParTest(void) {
}

void CbmParTest::clear(void) {
  p1=0.F;
  for (Int_t i=0;i<5000;i++) {
    ai[i]=0;
  }
  if (histo1) histo1->Reset();
}

void CbmParTest::putParams(CbmParamList* l) {
  if (!l) return;
  l->add("p1",p1);
  l->addBinary("ai",ai,5000);
  l->addBinary("histo1",histo1);
}

Bool_t CbmParTest::getParams(CbmParamList* l) {
  if (!l) return kFALSE;
  if (!l->fill("p1",&p1)) return kFALSE;
  if (!l->fillBinary("ai",ai,5000)) return kFALSE;
  if (!l->fillBinary("histo1",histo1)) return kFALSE;
  histo1->SetDirectory(0);
  return kTRUE;
}
