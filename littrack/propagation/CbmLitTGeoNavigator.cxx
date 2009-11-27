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

	do {
		fGeo->PushPoint();
		stepInfo = MakeStep();
//		std::cout << "FIRST STEP: " << stepInfo.ToString();
		if(fGeo->IsOutside()) {
			fGeo->PopDummy();
			return kLITERROR;
		}
		if (stepInfo.GetZpos() <= zOut + 1e-3 && stepInfo.GetZpos() >= zOut - 1e-3) {
//			std::cout << "3 Current before:" << " " << fGeo->GetCurrentPoint()[0] << " " <<  fGeo->GetCurrentPoint()[1] << " " << fGeo->GetCurrentPoint()[2] << std::endl;
			last = true;
			fGeo->PopDummy();
//			std::cout << "3 Current after:" << " " << fGeo->GetCurrentPoint()[0] << " " <<  fGeo->GetCurrentPoint()[1] << " " << fGeo->GetCurrentPoint()[2] << std::endl;
		} else
		if (stepInfo.GetZpos() > zOut) {
//			std::cout << "1 Current before:" << " " << fGeo->GetCurrentPoint()[0] << " " <<  fGeo->GetCurrentPoint()[1] << " " << fGeo->GetCurrentPoint()[2] << std::endl;
			fGeo->PopPoint();
//			std::cout << "1 Current after:" << " " << fGeo->GetCurrentPoint()[0] << " " <<  fGeo->GetCurrentPoint()[1] << " " << fGeo->GetCurrentPoint()[2] << std::endl;
			myf l = CalcLength(zOut);
			stepInfo = MakeStep(l);
			last = true;
		} else
		if (stepInfo.GetZpos() < zOut){
//			std::cout << "2 Current before:" << " " << fGeo->GetCurrentPoint()[0] << " " <<  fGeo->GetCurrentPoint()[1] << " " << fGeo->GetCurrentPoint()[2] << std::endl;
			fGeo->PopDummy();
//			std::cout << "2 Current after:" << " " << fGeo->GetCurrentPoint()[0] << " " <<  fGeo->GetCurrentPoint()[1] << " " << fGeo->GetCurrentPoint()[2] << std::endl;
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
	fGeo->InitTrack(par->GetX(), par->GetY(), par->GetZ(),
			               nx, ny, nz);
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

//	if (fGeo->GetCurrentPoint()[2] > 100. && fGeo->GetCurrentPoint()[2] < 970.) {
//		std::cout << "step=" << fGeo->GetStep()  << ";" << step << " z=" << fGeo->GetCurrentPoint()[2]
//			<< " rl=" <<  fGeo->GetStep() / matInfo.GetRL() << " " << matInfo.ToString();
//	}

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
