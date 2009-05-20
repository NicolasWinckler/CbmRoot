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
			Double_t zOut,
			std::vector<CbmLitMaterialInfo>& inter);

private:
	void InitTrack(
			const CbmLitTrackParam* par) const;

	CbmLitMaterialInfo MakeStep(
			Double_t step = 0.) const;

	Double_t CalcLength(
			Double_t zOut) const;

	TGeoManager* fGeo;

	ClassDef(CbmLitTGeoNavigator,1);
};

#endif /*CBMLITTGEONAVIGATOR_H_*/
