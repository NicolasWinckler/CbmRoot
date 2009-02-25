#ifndef CBMLITGEONAVIGATOR_H_
#define CBMLITGEONAVIGATOR_H_

#include "CbmLitTool.h"
#include "CbmLitMaterialInfo.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitGeoNavigator : public CbmLitTool
{
public:
	CbmLitGeoNavigator();
	virtual ~CbmLitGeoNavigator();

	virtual LitStatus FindIntersections(
			const CbmLitTrackParam* par,
			Double_t zOut,
			std::vector<CbmLitMaterialInfo>& inter) = 0;

	ClassDef(CbmLitGeoNavigator,1);
};

#endif /*CBMLITGEONAVIGATOR_H_*/
