//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/10/2004

/////////////////////////////////////////////////////////////
//
//  CbmZdcContFact
//
//  Factory for the parameter containers in libZdc
//
/////////////////////////////////////////////////////////////

#include "CbmZdcContFact.h"

#include "CbmGeoZdcPar.h"

#include "FairRuntimeDb.h"

ClassImp(CbmZdcContFact)

static CbmZdcContFact gCbmZdcContFact;

CbmZdcContFact::CbmZdcContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmZdcContFact";
  fTitle="Factory for parameter containers in libZdc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmZdcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    FairContainer* p= new FairContainer("CbmGeoZdcPar",
                                          "Zdc Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* CbmZdcContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
//  cout << " -I conatiner name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmGeoZdcPar")==0) {
      p=new CbmGeoZdcPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

