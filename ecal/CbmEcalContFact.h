#ifndef CBMECALCONTFACT_H
#define CBMECALCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmEcalContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmEcalContFact();
  ~CbmEcalContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  void  activateParIo(CbmParIo* io);
  ClassDef( CbmEcalContFact,0) // Factory for all ECAL parameter containers
};

#endif  /* !CBMECALCONTFACT_H */
