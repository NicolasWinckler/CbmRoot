/** CbmLitMyGeoNavigator.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2009
 **/

#include "propagation/CbmLitMyGeoNavigator.h"

#include "../cbm/base/CbmLitSimpleGeometryConstructor.h"
#include "data/CbmLitTrackParam.h"
#include "propagation/CbmLitMaterialInfo.h"
#include "utils/CbmLitComparators.h"

#include <algorithm>

CbmLitMyGeoNavigator::CbmLitMyGeoNavigator()
{
   CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
   fMyGeoNodes = geoConstructor->GetMyGeoNodes();
}

CbmLitMyGeoNavigator::~CbmLitMyGeoNavigator()
{
}

LitStatus CbmLitMyGeoNavigator::FindIntersections(
   const CbmLitTrackParam* par,
   litfloat zOut,
   std::vector<CbmLitMaterialInfo>& inter)
{
   litfloat zIn = par->GetZ();
   std::vector<CbmLitMaterialInfo>::iterator it_min, it_max;
   CbmLitMaterialInfo material;
   material.SetZpos(zIn);
   it_min = std::lower_bound(fMyGeoNodes.begin(), fMyGeoNodes.end(), material, CompareMaterialInfoZLess());
   material.SetZpos(zOut);
   it_max = std::lower_bound(fMyGeoNodes.begin(), fMyGeoNodes.end(), material, CompareMaterialInfoZLess());
   inter.assign(it_min, it_max);
   return kLITSUCCESS;
}
