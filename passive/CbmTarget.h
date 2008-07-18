#ifndef Target_H
#define Target_H

#include "CbmDetector.h"
#include "CbmModule.h"

class CbmTarget : public CbmModule {
  public:
    CbmTarget(const char * name, const char *Title="CBM Target");
    CbmTarget();
    virtual ~CbmTarget();
    virtual void ConstructGeometry();
    ClassDef(CbmTarget,1) 
  
};

#endif //Target_H

