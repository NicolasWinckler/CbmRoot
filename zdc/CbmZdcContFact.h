#ifndef CBMZDCCONTFACT_H
#define CBMZDCCONTFACT_H

#include "FairContFact.h"

class FairContainer;
class FairParSet;

class CbmZdcContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmZdcContFact();
  ~CbmZdcContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmZdcContFact,0) // Factory for all ZDC parameter containers
};

#endif  /* !CBMZDCCONTFACT_H */
