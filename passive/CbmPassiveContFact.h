#ifndef CBMPASSIVECONTFACT_H
#define CBMPASSIVECONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmPassiveContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmPassiveContFact();
  ~CbmPassiveContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmPassiveContFact,0) // Factory for all Passive parameter containers
};

#endif  /* !CBMPASSIVECONTFACT_H */
