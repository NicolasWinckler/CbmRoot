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

#include "FairParamList.h"

#include "TObjArray.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(CbmGeoMuchPar)

CbmGeoMuchPar::CbmGeoMuchPar(const char* name,const char* title,const char* context)
             : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoMuchPar::~CbmGeoMuchPar(void) {
}

void CbmGeoMuchPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoMuchPar::putParams(FairParamList* l) {
  cout<<"\n SG=> CbmGeoMuchPar::putParams() called\n"<<endl;
  if (!l) return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoMuchPar::getParams(FairParamList* l) {
  cout<<"\n SG=> CbmGeoMuchPar::getParams() called\n"<<endl;
  if (!l) return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;
  
  return kTRUE;
}
