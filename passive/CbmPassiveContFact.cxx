//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

/////////////////////////////////////////////////////////////
//
//  CbmPassiveContFact
//
//  Factory for the parameter containers in libPassive
//
/////////////////////////////////////////////////////////////

#include "CbmPassiveContFact.h"

#include "CbmGeoPassivePar.h"

#include "CbmRuntimeDb.h"

#include <iostream>
#include <iomanip>

ClassImp(CbmPassiveContFact)

static CbmPassiveContFact gCbmPassiveContFact;

CbmPassiveContFact::CbmPassiveContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmPassiveContFact";
  fTitle="Factory for parameter containers in libPassive";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmPassiveContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    CbmContainer* p= new CbmContainer("CbmGeoPassivePar",
                                          "Passive Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* CbmPassiveContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet *p=NULL;
  if (strcmp(name,"CbmGeoPassivePar")==0) {
    p=new CbmGeoPassivePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

