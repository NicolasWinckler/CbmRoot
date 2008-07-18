#ifndef CBMZDCCONTFACT_H
#define CBMZDCCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;
class CbmParSet;

class CbmZdcContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmZdcContFact();
  ~CbmZdcContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmZdcContFact,0) // Factory for all ZDC parameter containers
};

#endif  /* !CBMZDCCONTFACT_H */
