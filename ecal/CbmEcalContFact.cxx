//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  CbmEcalContFact
//
//  Factory for the parameter containers in libEcal
//
/////////////////////////////////////////////////////////////
#include "CbmEcalContFact.h"

#include "CbmGeoEcalPar.h"

#include "CbmRuntimeDb.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"

#include <iostream>
#include <iomanip>

ClassImp(CbmEcalContFact)

static CbmEcalContFact gCbmEcalContFact;

CbmEcalContFact::CbmEcalContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmEcalContFact";
  fTitle="Factory for parameter containers in libEcal";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmEcalContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    CbmContainer* p2= new CbmContainer("CbmGeoEcalPar",
                                          "Ecal Geometry Parameters",
                                          "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");

    containers->Add(p2);
}

CbmParSet* CbmEcalContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=0;
  if (strcmp(name,"CbmGeoEcalPar")==0) {
    p=new CbmGeoEcalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

void  CbmEcalContFact::activateParIo(CbmParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Ecal
return;
}

