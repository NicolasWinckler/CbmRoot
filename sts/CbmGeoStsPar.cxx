//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "CbmGeoStsPar.h"

#include "CbmParamList.h"

#include "TObjArray.h"

#include <iostream>
#include <iomanip>

ClassImp(CbmGeoStsPar)

CbmGeoStsPar::CbmGeoStsPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoStsPar::~CbmGeoStsPar(void) {
}

void CbmGeoStsPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoStsPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoStsPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
