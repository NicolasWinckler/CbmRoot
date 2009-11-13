#ifndef CbmLitDetContFact_H
#define CbmLitDetContFact_H

#include "FairContFact.h"

class FairContainer;

class CbmLitDetContFact : public FairContFact {
 private:
  void setAllContainers();
 public:
  CbmLitDetContFact();
  ~CbmLitDetContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmLitDetContFact,0) // Factory for all MyDet parameter containers
};

#endif  /* !CbmLitDetContFact_H */
