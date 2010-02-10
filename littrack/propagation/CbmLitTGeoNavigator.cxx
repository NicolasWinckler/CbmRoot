/** CbmLitTGeoNavigator.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **/

#include "CbmLitTGeoNavigator.h"

#include "CbmLitTrackParam.h"
#include "CbmLitDefaultSettings.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoVolume.h"

#include <iostream>
#include <cmath>

CbmLitTGeoNavigator::CbmLitTGeoNavigator()
{
	fGeo = gGeoManager;
}

CbmLitTGeoNavigator::~CbmLitTGeoNavigator()
{
}

LitStatus CbmLitTGeoNavigator::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitTGeoNavigator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTGeoNavigator::FindIntersections(
		const CbmLitTrackParam* par,
		myf zOut,
		std::vector<CbmLitMaterialInfo>& inter)
{
	InitTrack(par);
	if(fGeo->IsOutside()) return kLITERROR;

	CbmLitMaterialInfo stepInfo;
	bool last = false;

	std::cout.precision(7);

	do {
		fGeo->PushPoint();
		stepInfo = MakeStep();
		if(fGeo->IsOutside()) {
			fGeo->PopDummy();
			return kLITERROR;
		}
		if ((stepInfo.GetZpos() >= zOut)){
			fGeo->PopPoint();
			myf l = CalcLength(zOut);
			stepInfo.SetLength(l);
			stepInfo.SetZpos(zOut);
			last = true;
		} else {
			fGeo->PopDummy();
		}
		inter.push_back(stepInfo);
	} while (!last);
	return kLITSUCCESS;
}

void CbmLitTGeoNavigator::InitTrack(
		const CbmLitTrackParam* par) const
{
	myf nx, ny, nz;
	par->GetDirCos(nx, ny, nz);
	fGeo->InitTrack(par->GetX(), par->GetY(), par->GetZ(), nx, ny, nz);
}

CbmLitMaterialInfo CbmLitTGeoNavigator::MakeStep(
		myf step) const
{
	// fill current material information and then make a step
	CbmLitMaterialInfo matInfo;
	TGeoMaterial* mat = fGeo->GetCurrentNode()->GetMedium()->GetMaterial();
	matInfo.SetRL(mat->GetRadLen());
	matInfo.SetRho(mat->GetDensity());
	matInfo.SetZ(mat->GetZ());
	matInfo.SetA(mat->GetA());

	if (step == 0.) {
		fGeo->FindNextBoundaryAndStep(lit::MAXIMUM_TGEO_NAVIGATION_DISTANCE);
	} else {
		fGeo->SetStep(step);
		fGeo->Step(kFALSE);
	}

	matInfo.SetLength(fGeo->GetStep());
	matInfo.SetZpos(fGeo->GetCurrentPoint()[2]);

	return matInfo;
}

myf CbmLitTGeoNavigator::CalcLength(
		myf zOut) const
{
	//find intersection point of straight line with plane
	myf nx = fGeo->GetCurrentDirection()[0];
	myf ny = fGeo->GetCurrentDirection()[1];
	myf nz = fGeo->GetCurrentDirection()[2];
	myf z = fGeo->GetCurrentPoint()[2];

	myf t0 = (zOut - z) / nz;

	myf dx = nx * t0;
	myf dy = ny * t0;
	myf dz = (zOut - z);//nz * t0;

	//calculate distance between two points
	return std::sqrt( dx * dx + dy * dy + dz * dz);
}
