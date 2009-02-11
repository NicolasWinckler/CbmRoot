#ifndef CBMTRDCONTFACT_H
#define CBMTRDCONTFACT_H

#include "FairContFact.h"

class FairContainer;
class FairParSet;

class CbmTrdContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmTrdContFact();
  ~CbmTrdContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmTrdContFact,0) // Factory for all TRD parameter containers
};

#endif  /* !CBMTRDCONTFACT_H */
