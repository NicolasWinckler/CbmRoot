#ifndef CBMSHIELD_H
#define CBMSHIELD_H 1

#include "CbmDetector.h"
#include "CbmModule.h"

class CbmShield : public CbmModule {
  public:
    CbmShield(const char * name, const char *Title="CBM Shield");
    CbmShield();
    virtual ~CbmShield();
    virtual void ConstructGeometry();
    ClassDef(CbmShield,1) 
  
};

#endif

