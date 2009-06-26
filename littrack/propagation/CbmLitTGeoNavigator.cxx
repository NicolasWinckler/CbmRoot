#include "CbmLitTGeoNavigator.h"

#include "CbmLitTrackParam.h"

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
		double zOut,
		std::vector<CbmLitMaterialInfo>& inter)
{
	InitTrack(par);
	if(fGeo->IsOutside()) return kLITERROR;

	CbmLitMaterialInfo stepInfo;
	bool last = false;

	while (!last) {
		fGeo->PushPoint();
		stepInfo = MakeStep();
		if(fGeo->IsOutside()) {
			fGeo->PopDummy();
			return kLITERROR;
		}
		if (stepInfo.GetZpos() > zOut) {
			fGeo->PopPoint();
			double l = CalcLength(zOut);
			stepInfo = MakeStep(l);
			last = true;
		} else {
			fGeo->PopDummy();
		}
		inter.push_back(stepInfo);
	}
	return kLITSUCCESS;
}

void CbmLitTGeoNavigator::InitTrack(
		const CbmLitTrackParam* par) const
{
	double nx, ny, nz;
	par->GetDirCos(nx, ny, nz);
	fGeo->InitTrack(par->GetX(), par->GetY(), par->GetZ(),
			               nx, ny, nz);
}

CbmLitMaterialInfo CbmLitTGeoNavigator::MakeStep(
		double step) const
{
	// fill current material information and then make a step
	CbmLitMaterialInfo matInfo;
	TGeoMaterial* mat = fGeo->GetCurrentNode()->GetMedium()->GetMaterial();
	matInfo.SetRL(mat->GetRadLen());
	matInfo.SetRho(mat->GetDensity());
	matInfo.SetZ(mat->GetZ());
	matInfo.SetA(mat->GetA());

	if (step == 0.) {
		fGeo->FindNextBoundaryAndStep(30.);
	} else {
		fGeo->SetStep(step);
		fGeo->Step(kFALSE);
	}

	matInfo.SetLength(fGeo->GetStep());
	matInfo.SetZpos(fGeo->GetCurrentPoint()[2]);

	return matInfo;
}

double CbmLitTGeoNavigator::CalcLength(
		double zOut) const
{
	//find intersection point of straight line with plane
	double nx = fGeo->GetCurrentDirection()[0];
	double ny = fGeo->GetCurrentDirection()[1];
	double nz = fGeo->GetCurrentDirection()[2];
	double z = fGeo->GetCurrentPoint()[2];

	double t0 = (zOut - z) / nz;

	double dx = nx * t0;
	double dy = ny * t0;
	double dz = (zOut - z);//nz * t0;

	//calculate distance between two points
	return std::sqrt( dx * dx + dy * dy + dz * dz);
}
