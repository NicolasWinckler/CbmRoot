// -------------------------------------------------------------------------
// -----                    CbmFieldContFact source file               -----
// -----                   Created 20/02/06  by V. Friese              -----
// -------------------------------------------------------------------------
#include "CbmFieldContFact.h"

#include "CbmFieldPar.h"

#include "CbmParSet.h"
#include "CbmRuntimeDb.h"

#include <iostream>

using std::cout;
using std::endl;


static CbmFieldContFact gCbmFieldContFact;


// -----   Constructor   ---------------------------------------------------
CbmFieldContFact::CbmFieldContFact() {
  fName = "CbmFieldContFact";
  fTitle = "Factory for field parameter containers";
  SetAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmFieldContFact::~CbmFieldContFact() { }
// -------------------------------------------------------------------------



// -----   Create containers   ---------------------------------------------
CbmParSet* CbmFieldContFact::createContainer(CbmContainer* container) {


  const char* name = container->GetName();
  cout << "create CbmFieldPar container " << name << endl;
  CbmParSet* set = NULL;
  if ( strcmp(name, "CbmFieldPar") == 0 ) 
    set = new CbmFieldPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  return set;
}
// -------------------------------------------------------------------------



// -----   Set all containers (private)   ----------------------------------
void CbmFieldContFact::SetAllContainers() {
  CbmContainer* container = new CbmContainer("CbmFieldPar",
					     "Field parameter container",
					     "Default field");
  containers->Add(container);
}
// -------------------------------------------------------------------------


ClassImp(CbmFieldContFact)
