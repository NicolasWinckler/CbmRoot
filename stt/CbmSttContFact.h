#ifndef CBMSTTCONTFACT_H
#define CBMSTTCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmSttContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmSttContFact();
  ~CbmSttContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmSttContFact,0) // Factory for all SttDet parameter containers
};

#endif  /* !CbmSttContFact_H */
