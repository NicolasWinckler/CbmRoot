#ifndef CBMSTTCONTFACT_H
#define CBMSTTCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmSttContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmSttContFact();
  ~CbmSttContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmSttContFact,0) // Factory for all SttDet parameter containers
};

#endif  /* !CbmSttContFact_H */
