#ifndef CBMLITMYGEONAVIGATOR_H_
#define CBMLITMYGEONAVIGATOR_H_

#include "CbmLitGeoNavigator.h"
#include "CbmLitMaterialInfo.h"

#include <vector>

class CbmLitMyGeoNavigator : public CbmLitGeoNavigator
{
public:
	CbmLitMyGeoNavigator();
	virtual ~CbmLitMyGeoNavigator();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	LitStatus FindIntersections(
			const CbmLitTrackParam* par,
			double zOut,
			std::vector<CbmLitMaterialInfo>& inter);
private:

	std::vector<CbmLitMaterialInfo> fMyGeoNodes;
};

#endif /* CBMLITMYGEONAVIGATOR_H_ */
