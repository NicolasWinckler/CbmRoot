/** CbmLitMyGeoNavigator.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **
 ** The class performs geometry navigation in the simplified geometry.
 **/

#ifndef CBMLITMYGEONAVIGATOR_H_
#define CBMLITMYGEONAVIGATOR_H_

#include "interface/CbmLitGeoNavigator.h"
#include "propagation/CbmLitMaterialInfo.h"

#include <vector>

class CbmLitMyGeoNavigator : public CbmLitGeoNavigator
{
public:
   /* Constructor */
   CbmLitMyGeoNavigator();

   /* Destructor */
   virtual ~CbmLitMyGeoNavigator();

   /* Inherited from CbmLitGeoNavigator */
   LitStatus FindIntersections(
      const CbmLitTrackParam* par,
      litfloat zOut,
      std::vector<CbmLitMaterialInfo>& inter);
private:
   // vector with nodes of the simplified geometry
   std::vector<CbmLitMaterialInfo> fMyGeoNodes;
};

#endif /* CBMLITMYGEONAVIGATOR_H_ */
