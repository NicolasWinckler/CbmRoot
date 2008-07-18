#ifndef CBMSTSPARROOTFILEIO_H
#define CBMSTSPARROOTFILEIO_H

#include "CbmDetParRootFileIo.h"

#include "TFile.h"
#include "TArrayI.h"

class CbmParRootFile;
class CbmParSet;
class CbmStsDigiPar;

class CbmStsParRootFileIo : public CbmDetParRootFileIo {
public:
  CbmStsParRootFileIo(CbmParRootFile* f);
  ~CbmStsParRootFileIo();
  Bool_t init(CbmParSet*,Int_t*);
  Bool_t read(CbmStsDigiPar*,Int_t*);
  ClassDef(CbmStsParRootFileIo,0) // Class for STS parameter I/O from ROOT file
};

#endif  /* !CBMSTSPARROOTFILEIO_H */










