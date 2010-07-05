#ifndef Target_H
#define Target_H

#include "FairModule.h"

class CbmTarget : public FairModule {
  public:
    CbmTarget(const char * name, const char *Title="CBM Target");
    CbmTarget();
    virtual ~CbmTarget();
    virtual void ConstructGeometry();
    ClassDef(CbmTarget,1) 
  
};

#endif //Target_H

