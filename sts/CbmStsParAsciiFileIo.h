#ifndef CBMSTSPARASCIIFILEIO_H
#define CBMSTSPARASCIIFILEIO_H

#include "CbmDetParAsciiFileIo.h"

#include "TObject.h"
#include "TArrayI.h"

#include <fstream> 

class CbmParSet;

class CbmStsParAsciiFileIo : public CbmDetParAsciiFileIo {
public:
  CbmStsParAsciiFileIo(fstream*);
  ~CbmStsParAsciiFileIo() {}
  Bool_t init(CbmParSet*);
  Int_t write(CbmParSet*);
  template<class T> Bool_t read(T*, Int_t*, Bool_t needsClear=kFALSE);
  template<class T> Int_t writeFile2(T*);
  ClassDef(CbmStsParAsciiFileIo,0) // Class for STS parameter I/O from Ascii files
};

#endif  /* !CBMSTSPARASCIIFILEIO_H */








