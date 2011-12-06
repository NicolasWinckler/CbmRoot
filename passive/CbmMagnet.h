#ifndef CBMMAGNET_H
#define CBMMAGNET_H

#include "CbmModule.h"

class CbmMagnet : public CbmModule {
  public:
    CbmMagnet(const char *name, const char *title="CBM Magnet", 
              Double_t px=0., Double_t py=0., Double_t pz=0.,
              Double_t rx=0., Double_t ry=0., Double_t rz=0.);
    CbmMagnet();
    virtual ~CbmMagnet();
    virtual void ConstructGeometry();
    virtual void ConstructASCIIGeometry();

 private:
    TGeoRotation* frot;      //!
    TGeoCombiTrans* fposrot; //!
    
    ClassDef(CbmMagnet,2) //CBMMagnet
};

#endif // CBMMAGNET_H
