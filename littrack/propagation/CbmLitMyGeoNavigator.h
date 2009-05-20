#ifndef CBMLITMYGEONAVIGATOR_H_
#define CBMLITMYGEONAVIGATOR_H_

#include "CbmLitGeoNavigator.h"
#include "CbmLitMaterialInfo.h"

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
			Double_t zOut,
			std::vector<CbmLitMaterialInfo>& inter);

	ClassDef(CbmLitMyGeoNavigator, 1);
};

#endif /* CBMLITMYGEONAVIGATOR_H_ */
