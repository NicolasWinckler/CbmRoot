//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "CbmGeoRichPar.h"

#include "CbmParamList.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(CbmGeoRichPar)

CbmGeoRichPar::CbmGeoRichPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoRichPar::~CbmGeoRichPar(void) {
}

void CbmGeoRichPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoRichPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoRichPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
