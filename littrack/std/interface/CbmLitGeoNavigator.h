/** CbmLitGeoNavigator.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 **
 ** Interface for geometry navigation algorithms.
 **/

#ifndef CBMLITGEONAVIGATOR_H_
#define CBMLITGEONAVIGATOR_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"
#include "propagation/CbmLitMaterialInfo.h"
#include <vector>

class CbmLitTrackParam;

class CbmLitGeoNavigator
{
public:
   /* Constructor */
   CbmLitGeoNavigator() {}

   /* Destructor */
   virtual ~CbmLitGeoNavigator() {}

   /* Finds intersection points with detector materials on a certain interval.
    * @param par Input track parameters. Defines initial direction and Z position.
    * @param zOut Output Z position [cm].
    * @param inter Output vector with crossed material information.
    * @return Status code.
    */
   virtual LitStatus FindIntersections(
      const CbmLitTrackParam* par,
      myf zOut,
      std::vector<CbmLitMaterialInfo>& inter) = 0;
};

#endif /*CBMLITGEONAVIGATOR_H_*/
