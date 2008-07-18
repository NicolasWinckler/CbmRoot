//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/10/2004

/////////////////////////////////////////////////////////////
//
//  CbmTofContFact
//
//  Factory for the parameter containers in libTof
//
/////////////////////////////////////////////////////////////

#include "CbmTofContFact.h"

#include "CbmGeoTofPar.h"

#include "CbmRuntimeDb.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(CbmTofContFact)

static CbmTofContFact gCbmTofContFact;

CbmTofContFact::CbmTofContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmTofContFact";
  fTitle="Factory for parameter containers in libTof";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmTofContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    CbmContainer* p= new CbmContainer("CbmGeoTofPar",
                                          "Tof Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* CbmTofContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  CbmParSet* p=0;
  if (strcmp(name,"CbmGeoTofPar")==0) {
    p=new CbmGeoTofPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

