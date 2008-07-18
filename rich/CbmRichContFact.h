#ifndef CBMRICHCONTFACT_H
#define CBMRICHCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmRichContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmRichContFact();
  ~CbmRichContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmRichContFact,0) // Factory for all RICH parameter containers
};

#endif  /* !CBMRICHCONTFACT_H */
