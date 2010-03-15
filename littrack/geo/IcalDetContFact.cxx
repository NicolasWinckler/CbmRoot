#include "IcalDetContFact.h"

#include "IcalDetGeoPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(IcalDetContFact)

static IcalDetContFact gIcalDetContFact;

IcalDetContFact::IcalDetContFact() {
  /** Constructor (called when the library is loaded) */
  fName="IcalDetContFact";
  fTitle="Factory for parameter containers in libIcal";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void IcalDetContFact::setAllContainers() {
  /** Creates the Container objects with all accepted 
      contexts and adds them to
      the list of containers for the Tutorial1 library.
  */

  FairContainer* p= new FairContainer("IcalDetGeoPar",
				    "IcalDet Geometry Parameters",
				    "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
}

FairParSet* IcalDetContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not 
      the default context
      of this container, the name is concatinated with the context. 
  */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"IcalDetGeoPar")==0) {
    p=new IcalDetGeoPar(c->getConcatName().Data(),
			       c->GetTitle(),c->getContext());
  }
  return p;
}
