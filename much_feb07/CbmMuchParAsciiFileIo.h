#ifndef CBMMUCHPARASCIIFILEIO_H
#define CBMMUCHPARASCIIFILEIO_H


#include "CbmDetParAsciiFileIo.h"

#include "TObject.h"
#include "TArrayI.h"

#include <fstream> 

class CbmParSet;

class CbmMuchParAsciiFileIo : public CbmDetParAsciiFileIo {
public:
  CbmMuchParAsciiFileIo(fstream*);
  ~CbmMuchParAsciiFileIo() {}
  Bool_t init(CbmParSet*);
  Int_t write(CbmParSet*);
  template<class T> Bool_t read(T*, Int_t*, Bool_t needsClear=kFALSE);
  template<class T> Int_t writeFile2(T*);
  ClassDef(CbmMuchParAsciiFileIo,0) // Class for MuCh parameter I/O from Ascii files
};

#endif  /* !CBMMUCHPARASCIIFILEIO_H */







