#ifndef Cave_H
#define Cave_H

#include "CbmDetector.h"
#include "CbmModule.h"
#include "CbmModule.h"


class CbmCave : public CbmModule {
  public:
    CbmCave(const char * name, const char *Title="Exp Cave");
    CbmCave();
    virtual ~CbmCave();
    virtual void ConstructGeometry();
    

private:
     Double_t world[3];
     ClassDef(CbmCave,1) //CBMCaveSD
};

#endif //Cave_H

