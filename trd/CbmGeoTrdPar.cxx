
#include "CbmGeoTrdPar.h"

#include "CbmParamList.h"

#include "TObjArray.h"

ClassImp(CbmGeoTrdPar)

CbmGeoTrdPar::CbmGeoTrdPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoTrdPar::~CbmGeoTrdPar(void) {
}

void CbmGeoTrdPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoTrdPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoTrdPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
