#ifndef CBMLITGEONAVIGATOR_H_
#define CBMLITGEONAVIGATOR_H_

#include "CbmLitTool.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitFloat.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitGeoNavigator : public CbmLitTool
{
public:
	CbmLitGeoNavigator();
	virtual ~CbmLitGeoNavigator();

	virtual LitStatus FindIntersections(
			const CbmLitTrackParam* par,
			myf zOut,
			std::vector<CbmLitMaterialInfo>& inter) = 0;
};

#endif /*CBMLITGEONAVIGATOR_H_*/
