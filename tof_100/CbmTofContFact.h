#ifndef CBMTOFCONTFACT_H
#define CBMTOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmTofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmTofContFact();
  ~CbmTofContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmTofContFact,0) // Factory for all TOF parameter containers
};

#endif  /* !CBMTOFCONTFACT_H */
