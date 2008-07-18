#ifndef CBMTRDCONTFACT_H
#define CBMTRDCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;
class CbmParSet;

class CbmTrdContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmTrdContFact();
  ~CbmTrdContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( CbmTrdContFact,0) // Factory for all TRD parameter containers
};

#endif  /* !CBMTRDCONTFACT_H */
