#ifndef CBMLITTGEONAVIGATOR_H_
#define CBMLITTGEONAVIGATOR_H_

#include "CbmLitGeoNavigator.h"
#include "CbmLitMaterialInfo.h"

#include <vector>

class CbmLitTrackParam;
class TGeoManager;

class CbmLitTGeoNavigator : public CbmLitGeoNavigator
{
public:
	CbmLitTGeoNavigator();
	virtual ~CbmLitTGeoNavigator();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	LitStatus FindIntersections(
			const CbmLitTrackParam* par,
			myf zOut,
			std::vector<CbmLitMaterialInfo>& inter);

private:
	void InitTrack(
			const CbmLitTrackParam* par) const;

	CbmLitMaterialInfo MakeStep(
			myf step = 0.) const;

	myf CalcLength(
			myf zOut) const;

	TGeoManager* fGeo;
};

#endif /*CBMLITTGEONAVIGATOR_H_*/
