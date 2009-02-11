//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "CbmGeoRichPar.h"

#include "FairParamList.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(CbmGeoRichPar)

CbmGeoRichPar::CbmGeoRichPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoRichPar::~CbmGeoRichPar(void) {
}

void CbmGeoRichPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoRichPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoRichPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
