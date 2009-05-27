#include "CbmLitCheckSimpleGeo.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

CbmLitCheckSimpleGeo::CbmLitCheckSimpleGeo()
{
}

CbmLitCheckSimpleGeo::~CbmLitCheckSimpleGeo()
{
}

InitStatus CbmLitCheckSimpleGeo::Init()
{
	fGeoConstructor.ConstructGeometry();
	fGeoConstructor.Draw();
}

void CbmLitCheckSimpleGeo::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    rtdb->getContainer("CbmGeoMuchPar");
    rtdb->getContainer("CbmStsDigiPar");
    rtdb->getContainer("CbmGeoStsPar");
}

void CbmLitCheckSimpleGeo::Exec(
		Option_t* opt)
{

}

void CbmLitCheckSimpleGeo::Finish()
{
}

ClassImp(CbmLitCheckSimpleGeo);
