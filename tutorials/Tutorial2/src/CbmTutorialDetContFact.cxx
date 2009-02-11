#include "CbmTutorialDetContFact.h"

#include "CbmTutorialDetGeoPar.h"
#include "CbmTutorialDetDigiPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(CbmTutorialDetContFact)

static CbmTutorialDetContFact gCbmTutorialDetContFact;

CbmTutorialDetContFact::CbmTutorialDetContFact() {
  /** Constructor (called when the library is loaded) */
  fName="CbmTutorialDetContFact";
  fTitle="Factory for parameter containers in libTutorial1";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmTutorialDetContFact::setAllContainers() {
  /** Creates the Container objects with all accepted 
      contexts and adds them to
      the list of containers for the Tutorial1 library.
  */

  FairContainer* p1= new FairContainer("CbmTutorialDetGeoPar",
				     "CbmTutorialDet Geometry Parameters",
				     "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");
  
  containers->Add(p1);

  FairContainer* p2= new FairContainer("CbmTutorialDetDigiPar",
				     "Tutorial Det Digi Parameters",
				     "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");

  containers->Add(p2);
}

FairParSet* CbmTutorialDetContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not 
      the default context
      of this container, the name is concatinated with the context. 
  */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"CbmTutorialDetGeoPar")==0) {
    p=new CbmTutorialDetGeoPar(c->getConcatName().Data(),
			       c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmTutorialDetDigiPar")==0) {
    p=new CbmTutorialDetDigiPar(c->getConcatName().Data(),
			       c->GetTitle(),c->getContext());
  }
  return p;
}
