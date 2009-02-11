#ifndef CBMMUCHPARROOTFILEIO_H
#define CBMMUCHPARROOTFILEIO_H

#include "FairDetParRootFileIo.h"
#include "TFile.h"
#include "TArrayI.h"

class FairParRootFile;
class FairParSet;
class CbmMuchDigiPar;

class CbmMuchParRootFileIo : public FairDetParRootFileIo {
public:
  CbmMuchParRootFileIo(FairParRootFile* f);
  ~CbmMuchParRootFileIo();
  Bool_t init(FairParSet*,Int_t*);
  Bool_t read(CbmMuchDigiPar*,Int_t*);
  ClassDef(CbmMuchParRootFileIo,0) // Class for Much parameter I/O from ROOT file
};

#endif  /* !CBMMUCHPARROOTFILEIO_H */










