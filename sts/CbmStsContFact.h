#ifndef CBMSTSCONTFACT_H
#define CBMSTSCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmStsContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmStsContFact();
  ~CbmStsContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  void  activateParIo(CbmParIo* io);
  ClassDef( CbmStsContFact,0) // Factory for all STS parameter containers
};

#endif  /* !CBMSTSCONTFACT_H */
