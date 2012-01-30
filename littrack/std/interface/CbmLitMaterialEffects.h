/** CbmLitMaterialEffects.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **
 ** Interface for material effects calculation
 ** (multiple scattering and energy loss) algorithms.
 **/

#ifndef CBMLITMATERIALEFFECTS_H_
#define CBMLITMATERIALEFFECTS_H_

#include "base/CbmLitEnums.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

class CbmLitMaterialEffects
{
public:
   /* Constructor */
   CbmLitMaterialEffects() {}

   /* Destructor */
   virtual ~CbmLitMaterialEffects() {}

   /* Main function to be implemented for concrete material effects calculation algorithms.
    * @param par Input/Output track parameters.
    * @param mat Material information.
    * @param pdg PDG code
    * @param downstream Propagation direction. If true than downstream from the target, else upstream from the target.
    * @return Status code. */
   virtual LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat,
      int pdg,
      bool downstream) = 0;
};

#endif /*CBMLITMATERIALEFFECTS_H_*/
