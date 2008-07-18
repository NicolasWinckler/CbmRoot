//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  CbmStsContFact
//
//  Factory for the parameter containers in libSts
//
/////////////////////////////////////////////////////////////

#include "CbmMvdContFact.h"

#include "CbmMvdGeoPar.h"

#include "CbmRuntimeDb.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include "CbmParIo.h"
#include "CbmParGenericSet.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(CbmMvdContFact)

static CbmMvdContFact gCbmMvdContFact;

CbmMvdContFact::CbmMvdContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmMvdContFact";
  fTitle="Factory for parameter containers in libMvd";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmMvdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
    CbmContainer* p= new CbmContainer("CbmMvdGeoPar",
                                          "Mvd Geometry Parameters",
                                          "Default");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* CbmMvdContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  CbmParSet* p=0;
  if (strcmp(name,"CbmMvdGeoPar")==0) {
    p=new CbmMvdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
