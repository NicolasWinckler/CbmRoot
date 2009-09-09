/** CbmLitMaterialEffects.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **
 ** Base class for LIT material effects calculation (multiple scattering and energy loss) algorithms.
 **/

#ifndef CBMLITMATERIALEFFECTS_H_
#define CBMLITMATERIALEFFECTS_H_

#include "CbmLitTool.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

class CbmLitMaterialEffects : public CbmLitTool
{
public:
	/* Constructor */
	CbmLitMaterialEffects() {};

	/* Destructor */
	virtual ~CbmLitMaterialEffects() {};

	/* Updates track parameters with material effects.
	 * @param par Input/Output track parameters.
	 * @param mat Material information.
	 * @param pdg PDG code
	 * @param downstream Propagation direction. If true than downstream from the target, else upstream from the target.
	 * @return Update status.
	 */
	virtual LitStatus Update(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat,
	        int pdg,
	        bool downstream) = 0;
};

#endif /*CBMLITMATERIALEFFECTS_H_*/
