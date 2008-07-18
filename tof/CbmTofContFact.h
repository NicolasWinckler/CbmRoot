#ifndef CBMTOFCONTFACT_H
#define CBMTOFCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmTofContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmTofContFact();
  ~CbmTofContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmTofContFact,0) // Factory for all TOF parameter containers
};

#endif  /* !CBMTOFCONTFACT_H */
