#ifndef MABGNET_H
#define MABGNET_H

#include "CbmDetector.h"
#include "CbmModule.h"

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"


class CbmMagnet : public CbmModule {
  public:
    CbmMagnet(const char * name, const char *Title="CBM Magnet");
    CbmMagnet();
    virtual ~CbmMagnet();
    virtual void ConstructGeometry();
    ClassDef(CbmMagnet,1) //CBMMagnet

};

#endif //MABGNET_H

