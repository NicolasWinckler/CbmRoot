//*-- AUTHOR : Denis Bertini
//*-- Modified : 06/10/2005 by Denis Bertini

/////////////////////////////////////////////////////////////
//  CbmMuchParRootFileIo
//
//  Class for Start parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////

#include "CbmMuchParRootFileIo.h"

#include "CbmMuchDigiPar.h"

#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include <iostream> 

using std::cout;
using std::cerr;
using std::endl;

ClassImp(CbmMuchParRootFileIo)

CbmMuchParRootFileIo::CbmMuchParRootFileIo(FairParRootFile* f) : FairDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "CbmMuchParIo"
  fName="CbmMuchParIo";

}


CbmMuchParRootFileIo::~CbmMuchParRootFileIo() {
}

Bool_t CbmMuchParRootFileIo::init(FairParSet* pPar,Int_t* set) {
  // Initializes a container called by name, but only the modules defined
  //   in the array 'set.
  // Calls the special read function for this container.
  // If it is called the first time, it reads the setup found in the file.
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (!strcmp(name,"MuchDigiPar")) return read((CbmMuchDigiPar*)pPar,set);
  }
  cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<endl;
  return kFALSE;
}


Bool_t CbmMuchParRootFileIo::read(CbmMuchDigiPar* pPar,Int_t* set) {
  // Not used yet
  cout << "-I- CbmMuchParRootFileIo : reading " << endl;
  return kTRUE;
}

