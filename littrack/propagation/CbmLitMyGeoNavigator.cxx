#include "CbmLitMyGeoNavigator.h"

#include "CbmLitSimpleGeometryConstructor.h"
#include "CbmLitTrackParam.h"
#include "CbmLitMaterialInfo.h"
#include "CbmLitComparators.h"

#include <algorithm>

CbmLitMyGeoNavigator::CbmLitMyGeoNavigator()
{
	CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
	fMyGeoNodes = geoConstructor->GetMyGeoNodes();
}

CbmLitMyGeoNavigator::~CbmLitMyGeoNavigator()
{
}

LitStatus CbmLitMyGeoNavigator::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitMyGeoNavigator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitMyGeoNavigator::FindIntersections(
			const CbmLitTrackParam* par,
			myf zOut,
			std::vector<CbmLitMaterialInfo>& inter)
{
	myf zIn = par->GetZ();
	std::vector<CbmLitMaterialInfo>::iterator it_min, it_max;
	CbmLitMaterialInfo material;
	material.SetZpos(zIn);
	it_min = std::lower_bound(fMyGeoNodes.begin(), fMyGeoNodes.end(), material, CompareMaterialInfoZLess());
	material.SetZpos(zOut);
	it_max = std::lower_bound(fMyGeoNodes.begin(), fMyGeoNodes.end(), material, CompareMaterialInfoZLess());
	inter.assign(it_min, it_max);
	return kLITSUCCESS;
}
