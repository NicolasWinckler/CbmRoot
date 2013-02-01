#include "CbmTofDigiPar.h"

#include "FairParamList.h"

#include <iostream>

ClassImp(CbmTofDigiPar)

CbmTofDigiPar ::CbmTofDigiPar(const char* name,
    const char* title,
    const char* context)
  : FairParGenericSet(name,title,context)
{
}

CbmTofDigiPar::~CbmTofDigiPar(void)
{
}

void CbmTofDigiPar::clear(void)
{
}

void CbmTofDigiPar::putParams(FairParamList* l)
{
  if (!l) { return; }
}

Bool_t CbmTofDigiPar::getParams(FairParamList* l)
{
  if (!l) { return kFALSE; }
  return kTRUE;
}
