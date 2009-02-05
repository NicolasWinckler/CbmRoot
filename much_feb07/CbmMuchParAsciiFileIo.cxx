//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005


///////////////////////////////////////////////////////////////
// CbmMuchParAsciiFileIo
//
// Class for MuCh parameter input/output from/into Ascii file
//
///////////////////////////////////////////////////////////////
#include "CbmMuchParAsciiFileIo.h"

#include "CbmMuchDigiPar.h"

#include <iostream>

using std::cout;
using std::endl;
using std::cerr;

ClassImp(CbmMuchParAsciiFileIo)

CbmMuchParAsciiFileIo::CbmMuchParAsciiFileIo(fstream* f) : CbmDetParAsciiFileIo(f) {
  // constructor calls the base class constructor
  fName="CbmMuchParIo";
}

Bool_t CbmMuchParAsciiFileIo::init(CbmParSet* pPar) {
  // calls the appropriate read function for the container
  const  Text_t* name=pPar->GetName();
   cout << "-I- Ascii Io init() " << pPar->GetName() <<   endl;

  if (pFile) {
    if (!strcmp(name,"CbmMuchDigiPar")) return read((CbmMuchDigiPar*)pPar,0,kTRUE);
    cerr<<"initialization of "<<name<<" not possible from file!"<<endl;
    return kFALSE;
  }
  cerr<<"no input file open"<<endl;
  return kFALSE;
}

Int_t CbmMuchParAsciiFileIo::write(CbmParSet* pPar) {
  // calls the appropriate write function for the container
  if (pFile) {
    const  Text_t* name=pPar->GetName();
    if (!strcmp(name,"CbmMuchDigiPar")) return writeFile2((CbmMuchDigiPar*)pPar);
    //problem with container name
    cerr<<name<<" could not be written to Ascii file"<<endl;
    return -1;
  }
  cerr<<"no output file open"<<endl;
  return -1;
}

template<class T> Bool_t CbmMuchParAsciiFileIo::read(T* pPar, Int_t* set,
                                                    Bool_t needsClear) {
  // template function for all parameter containers
  // searches the container in the file, reads the data line by line and
  //   called the member function readline(...) of the container class
  //  cout << "-I- Read Ascii IO " << endl;
  //    return kTRUE;

  
  const  Text_t* name=pPar->GetName();
  if (!findContainer(name)) return kFALSE;
  if (needsClear) pPar->clear();
  const Int_t maxbuf=155;
  Text_t buf[maxbuf];
  while (!pFile->eof()) {
    pFile->getline(buf, maxbuf);
    if (buf[0]=='#') break;
    if (buf[0]!='/' && buf[0]!='\0')
      //  printf("-I- CbmMuchParAsciiFileIo container name: %s",
      //         pPar->GetName());
        pPar->readline(buf,set,pFile);
  }
  pPar->setInputVersion(1,inputNumber);
  pPar->setChanged();
  Bool_t allFound=kTRUE;
   // if (allFound) printf("%s initialized from Ascii file\n",name);
   printf("%s initialized from Ascii file\n",name);

  return allFound;
}


template<class T> Int_t CbmMuchParAsciiFileIo::writeFile2(T* pPar) {
  // template function for all parameter containers with 2 levels
  // writes the header, loops over the container and calls its member
  //   function writeline(...)
  pPar->putAsciiHeader(fHeader);
  writeHeader(pPar->GetName());
  //  Text_t buf[155];
/*
  Int_t n0 = (*pPar).getSize();
  for(Int_t i0=0; i0<n0; i0++) {
    Int_t n1 = (*pPar)[i0].getSize();
    for(Int_t i1=0; i1<n1; i1++) {
      if (pPar->writeline(buf,i0,i1))
        pFile->write(buf,strlen(buf));
    }
  }
  pPar->setChanged(kFALSE);
  pFile->write(sepLine,strlen(sepLine));
 */
  return 1;
}
