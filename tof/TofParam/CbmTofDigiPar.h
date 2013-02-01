#ifndef CBMTOFDIGIPAR_H
#define CBMTOFDIGIPAR_H

#include "FairParGenericSet.h"

class FairParamList;

class CbmTofDigiPar : public FairParGenericSet
{
  public:

    CbmTofDigiPar(const char* name="CbmTofDigiPar",
                          const char* title="Digitization parameters for the TOF detector",
                          const char* context="TestDefaultContext");
    ~CbmTofDigiPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);

  private:
    CbmTofDigiPar(const CbmTofDigiPar&);
    CbmTofDigiPar& operator=(const CbmTofDigiPar&);

    ClassDef(CbmTofDigiPar,1)
};

#endif
