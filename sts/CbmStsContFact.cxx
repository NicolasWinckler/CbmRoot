//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  CbmStsContFact
//
//  Factory for the parameter containers in libSts
//
/////////////////////////////////////////////////////////////


#include "CbmStsContFact.h"

#include "CbmStsParRootFileIo.h"
#include "CbmStsParAsciiFileIo.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"

#include "CbmRuntimeDb.h"
#include "CbmParTest.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(CbmStsContFact)

static CbmStsContFact gCbmStsContFact;

CbmStsContFact::CbmStsContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmStsContFact";
  fTitle="Factory for parameter containers in libSts";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmStsContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
    CbmContainer* p1= new CbmContainer("CbmStsDigiPar",
                                          "Sts Digitisation Parameters",
                                          "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");

    CbmContainer* p2= new CbmContainer("CbmGeoStsPar",
                                          "Sts Geometry Parameters",
                                          "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");

    containers->Add(p1);
    containers->Add(p2);
}

CbmParSet* CbmStsContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  CbmParSet* p=0;
  if (strcmp(name,"CbmStsDigiPar")==0) {
    p=new CbmStsDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

void  CbmStsContFact::activateParIo(CbmParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  if (strcmp(io->IsA()->GetName(),"CbmParRootFileIo")==0) {
    CbmStsParRootFileIo* p=new CbmStsParRootFileIo(((CbmParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"CbmParAsciiFileIo")==0) {
    CbmStsParAsciiFileIo* p=new CbmStsParAsciiFileIo(((CbmParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

