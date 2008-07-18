/** CbmMuchContFact.cxx
 *
 * @author  M. Ryzhinskiy <m.ryzhinskiy@gsi.de>
 * @version 0.0
 * @since   15.03.07
 * 
 * Factory for the parameter containers for MUon CHambers detector
 *
 */
#include "CbmMuchContFact.h"

#include "CbmGeoMuchPar.h"
// #include "CbmMuchParRootFileIo.h"
// #include "CbmMuchParAsciiFileIo.h"

#include "CbmRuntimeDb.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(CbmMuchContFact)

static CbmMuchContFact gCbmMuchContFact;

CbmMuchContFact::CbmMuchContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmMuchContFact";
  fTitle="Factory for parameter containers in libMuch";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmMuchContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MuCh library.*/
//   CbmContainer* p1= new CbmContainer("CbmMuchDigiPar",
// 				     "Much Digitization Parameters",
// 				     "TestDefaultContext");
//   p1->addContext("TestNonDefaultContext");
  
  CbmContainer* p2= new CbmContainer("CbmGeoMuchPar",
				     "Much Geometry Parameters",
				     "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  
  //  containers->Add(p1);
  containers->Add(p2);

}

CbmParSet* CbmMuchContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  CbmParSet* p=0;
  if (strcmp(name,"CbmGeoMuchPar")==0) {
    p=new CbmGeoMuchPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;

}

void  CbmMuchContFact::activateParIo(CbmParIo* io) {
  // activates the input/output class for the parameters
  // needed by the MuCh
//   if (strcmp(io->IsA()->GetName(),"CbmParRootFileIo")==0) {
//     CbmMuchParRootFileIo* p=new CbmMuchParRootFileIo(((CbmParRootFileIo*)io)->getParRootFile());
//     io->setDetParIo(p);
//   }
//   else if (strcmp(io->IsA()->GetName(),"CbmParAsciiFileIo")==0) {
//     CbmMuchParAsciiFileIo* p=new CbmMuchParAsciiFileIo(((CbmParAsciiFileIo*)io)->getFile());
//     io->setDetParIo(p);
//   }
}

