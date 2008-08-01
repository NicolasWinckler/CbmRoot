#include "CbmLitGeoNavigatorImp.h"

#include "CbmLitTrackParam.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoVolume.h"

#include <iostream>

CbmLitGeoNavigatorImp::CbmLitGeoNavigatorImp()
{
}

CbmLitGeoNavigatorImp::~CbmLitGeoNavigatorImp()
{
}

LitStatus CbmLitGeoNavigatorImp::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitGeoNavigatorImp::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitGeoNavigatorImp::FindIntersections(
		const CbmLitTrackParam* par,
		Double_t zOut,
		std::vector<CbmLitMaterialInfo>& inter)
{
	InitTrack(par);
	if(gGeoManager->IsOutside()) return kLITERROR;
	
	CbmLitMaterialInfo stepInfo;
	Bool_t last = false;
	
	while (!last) {
		gGeoManager->PushPoint();
		stepInfo = MakeStep();
		if(gGeoManager->IsOutside()) {
			gGeoManager->PopDummy();
			return kLITERROR;
		}
		if (stepInfo.GetZpos() > zOut) {
			gGeoManager->PopPoint();
			Double_t l = CalcLength(zOut);
			stepInfo = MakeStep(l);
			last = true;
		} else {
			gGeoManager->PopDummy();
		}
		inter.push_back(stepInfo);
	}	
	return kLITSUCCESS;
}

void CbmLitGeoNavigatorImp::InitTrack(
		const CbmLitTrackParam* par) const
{
	Double_t nx, ny, nz;
	par->GetDirCos(nx, ny, nz);
	gGeoManager->InitTrack(par->GetX(), par->GetY(), par->GetZ(), 
			               nx, ny, nz);
}

CbmLitMaterialInfo CbmLitGeoNavigatorImp::MakeStep(
		Double_t step) const
{
	// fill current material information and then make a step
	CbmLitMaterialInfo matInfo;
	TGeoMaterial* mat = gGeoManager->GetCurrentNode()->GetMedium()->GetMaterial();
	matInfo.SetRL(mat->GetRadLen());
	matInfo.SetRho(mat->GetDensity());
	matInfo.SetZ(mat->GetZ());
	matInfo.SetA(mat->GetA());
	
	if (step == 0.) {
		gGeoManager->FindNextBoundaryAndStep(15.);
	} else {
		gGeoManager->SetStep(step);
		gGeoManager->Step(kFALSE);
	}
	
	matInfo.SetLength(gGeoManager->GetStep());
	matInfo.SetZpos(gGeoManager->GetCurrentPoint()[2]);

	return matInfo;	 
}

Double_t CbmLitGeoNavigatorImp::CalcLength(
		Double_t zOut) const
{
	//find intersection point of straight line with plane
	Double_t nx = gGeoManager->GetCurrentDirection()[0];
	Double_t ny = gGeoManager->GetCurrentDirection()[1];
	Double_t nz = gGeoManager->GetCurrentDirection()[2];
	Double_t z = gGeoManager->GetCurrentPoint()[2];
	
	Double_t t0 = (zOut - z) / nz;
	
	Double_t dx = nx * t0;
	Double_t dy = ny * t0;
	Double_t dz = (zOut - z);//nz * t0;
	
	//calculate distance between two points
	return std::sqrt( dx * dx + dy * dy + dz * dz);
}

ClassImp(CbmLitGeoNavigatorImp)
