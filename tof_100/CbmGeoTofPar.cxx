#include "CbmGeoTofPar.h"

#include "FairParamList.h"

#include "TObjArray.h"


#include <iostream>
using std::cout;
using std::endl;

ClassImp(CbmGeoTofPar)

CbmGeoTofPar::CbmGeoTofPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoTofPar::~CbmGeoTofPar(void) {
}

void CbmGeoTofPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoTofPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
   cout << "CbmGeoTofPar::putParams stored list at l" << endl;
   
}

Bool_t CbmGeoTofPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
