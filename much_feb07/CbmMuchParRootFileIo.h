#ifndef CBMMUCHPARROOTFILEIO_H
#define CBMMUCHPARROOTFILEIO_H

#include "CbmDetParRootFileIo.h"
#include "TFile.h"
#include "TArrayI.h"

class CbmParRootFile;
class CbmParSet;
class CbmMuchDigiPar;

class CbmMuchParRootFileIo : public CbmDetParRootFileIo {
public:
  CbmMuchParRootFileIo(CbmParRootFile* f);
  ~CbmMuchParRootFileIo();
  Bool_t init(CbmParSet*,Int_t*);
  Bool_t read(CbmMuchDigiPar*,Int_t*);
  ClassDef(CbmMuchParRootFileIo,0) // Class for Much parameter I/O from ROOT file
};

#endif  /* !CBMMUCHPARROOTFILEIO_H */










