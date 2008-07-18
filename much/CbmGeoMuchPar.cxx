/** CbmGeoMuchPar class
 * @author  E.Kryshen
 * @version 1.0
 * @since   18.02.08
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 * 
 *  Class for geometry parameters of MUon CHambers
 *
 */

#include "CbmGeoMuchPar.h"
#include "CbmParamList.h"
#include "TObjArray.h"

CbmGeoMuchPar::CbmGeoMuchPar(const char* name,const char* title,const char* context)
             : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
               fStations     = new TObjArray();
}

CbmGeoMuchPar::~CbmGeoMuchPar(void) {
}

void CbmGeoMuchPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
    if(fStations) delete fStations;
}

void CbmGeoMuchPar::putParams(CbmParamList* l) {
  if (!l) return;
  l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
  l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
  l->addBinary("CbmMuchStationOlds list",fStations);
}

Bool_t CbmGeoMuchPar::getParams(CbmParamList* l) {
  if (!l) return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;
  if (!l->fillBinary("CbmMuchStationOlds list",fStations)) return kFALSE; 
  return kTRUE;
}

ClassImp(CbmGeoMuchPar)

