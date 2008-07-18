#include "CbmSttContFact.h"

#include "CbmGeoSttPar.h"

#include "CbmRuntimeDb.h"

//#include <iostream>
//#include <iomanip>

ClassImp(CbmSttContFact)

static CbmSttContFact gCbmSttContFact;

CbmSttContFact::CbmSttContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmSttContFact";
  fTitle="Factory for parameter containers in libStt";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmSttContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the SttDet library.*/

    CbmContainer* p= new CbmContainer("CbmGeoSttPar",
                                          "Stt Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* CbmSttContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"CbmGeoSttPar")==0) {
    p=new CbmGeoSttPar (c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
