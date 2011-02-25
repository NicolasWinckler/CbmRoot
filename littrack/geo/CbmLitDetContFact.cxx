#include "geo/CbmLitDetContFact.h"

#include "geo/CbmLitDetGeoPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(CbmLitDetContFact)

static CbmLitDetContFact gCbmLitDetContFact;

CbmLitDetContFact::CbmLitDetContFact() {
  /** Constructor (called when the library is loaded) */
  fName="CbmLitDetContFact";
  fTitle="Factory for parameter containers in libLittrack";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmLitDetContFact::setAllContainers() {
  /** Creates the Container objects with all accepted 
      contexts and adds them to
      the list of containers for the Tutorial1 library.
  */

  FairContainer* p= new FairContainer("CbmLitDetGeoPar",
				    "CbmLitDet Geometry Parameters",
				    "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
}

FairParSet* CbmLitDetContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not 
      the default context
      of this container, the name is concatinated with the context. 
  */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"CbmLitDetGeoPar")==0) {
    p=new CbmLitDetGeoPar(c->getConcatName().Data(),
			       c->GetTitle(),c->getContext());
  }
  return p;
}
