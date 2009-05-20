#include "CbmLitMyGeoNavigator.h"

CbmLitMyGeoNavigator::CbmLitMyGeoNavigator()
{
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
			Double_t zOut,
			std::vector<CbmLitMaterialInfo>& inter)
{

}

ClassImp(CbmLitMyGeoNavigator);
