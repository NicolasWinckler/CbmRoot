/** CbmGeoMuchPar class
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

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(CbmGeoMuchPar)

CbmGeoMuchPar::CbmGeoMuchPar(const char* name,const char* title,const char* context)
             : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoMuchPar::~CbmGeoMuchPar(void) {
}

void CbmGeoMuchPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoMuchPar::putParams(CbmParamList* l) {
  cout<<"\n SG=> CbmGeoMuchPar::putParams() called\n"<<endl;
  if (!l) return;
  l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
  l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoMuchPar::getParams(CbmParamList* l) {
  cout<<"\n SG=> CbmGeoMuchPar::getParams() called\n"<<endl;
  if (!l) return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;
  
  return kTRUE;
}
