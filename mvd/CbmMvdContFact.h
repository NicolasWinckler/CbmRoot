#ifndef CBMMVDCONTFACT_H
#define CBMMVDCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;
class CbmParIo;

class CbmMvdContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmMvdContFact();
  ~CbmMvdContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmMvdContFact,0) // Factory for all STS parameter containers
};

#endif  /* !CBMSTSCONTFACT_H */
