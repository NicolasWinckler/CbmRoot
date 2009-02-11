#ifndef MABGNET_H
#define MABGNET_H

#include "FairDetector.h"
#include "FairModule.h"

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"


class CbmMagnet : public FairModule {
  public:
    CbmMagnet(const char * name, const char *Title="CBM Magnet");
    CbmMagnet();
    virtual ~CbmMagnet();
    virtual void ConstructGeometry();
    ClassDef(CbmMagnet,1) //CBMMagnet

};

#endif //MABGNET_H

