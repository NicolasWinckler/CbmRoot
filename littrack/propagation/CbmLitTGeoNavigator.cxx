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
		Double_t zOut,
		std::vector<CbmLitMaterialInfo>& inter)
{
	InitTrack(par);
	if(fGeo->IsOutside()) return kLITERROR;

	CbmLitMaterialInfo stepInfo;
	Bool_t last = false;

	while (!last) {
		fGeo->PushPoint();
		stepInfo = MakeStep();
		if(fGeo->IsOutside()) {
			fGeo->PopDummy();
			return kLITERROR;
		}
		if (stepInfo.GetZpos() > zOut) {
			fGeo->PopPoint();
			Double_t l = CalcLength(zOut);
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
	Double_t nx, ny, nz;
	par->GetDirCos(nx, ny, nz);
	fGeo->InitTrack(par->GetX(), par->GetY(), par->GetZ(),
			               nx, ny, nz);
}

CbmLitMaterialInfo CbmLitTGeoNavigator::MakeStep(
		Double_t step) const
{
	// fill current material information and then make a step
	CbmLitMaterialInfo matInfo;
	TGeoMaterial* mat = fGeo->GetCurrentNode()->GetMedium()->GetMaterial();
	matInfo.SetRL(mat->GetRadLen());
	matInfo.SetRho(mat->GetDensity());
	matInfo.SetZ(mat->GetZ());
	matInfo.SetA(mat->GetA());

	if (step == 0.) {
		fGeo->FindNextBoundaryAndStep(15.);
	} else {
		fGeo->SetStep(step);
		fGeo->Step(kFALSE);
	}

	matInfo.SetLength(fGeo->GetStep());
	matInfo.SetZpos(fGeo->GetCurrentPoint()[2]);

	return matInfo;
}

Double_t CbmLitTGeoNavigator::CalcLength(
		Double_t zOut) const
{
	//find intersection point of straight line with plane
	Double_t nx = fGeo->GetCurrentDirection()[0];
	Double_t ny = fGeo->GetCurrentDirection()[1];
	Double_t nz = fGeo->GetCurrentDirection()[2];
	Double_t z = fGeo->GetCurrentPoint()[2];

	Double_t t0 = (zOut - z) / nz;

	Double_t dx = nx * t0;
	Double_t dy = ny * t0;
	Double_t dz = (zOut - z);//nz * t0;

	//calculate distance between two points
	return std::sqrt( dx * dx + dy * dy + dz * dz);
}

ClassImp(CbmLitTGeoNavigator)
