/**
 * \file CbmLitSimpleGeoQa.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#include "qa/CbmLitSimpleGeoQa.h"

#include "base/CbmLitSimpleGeometryConstructor.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

CbmLitSimpleGeoQa::CbmLitSimpleGeoQa()
{
}

CbmLitSimpleGeoQa::~CbmLitSimpleGeoQa()
{
}

InitStatus CbmLitSimpleGeoQa::Init()
{
   fGeoConstructor = CbmLitSimpleGeometryConstructor::Instance();
   fGeoConstructor->Draw();
   return kSUCCESS;
}

void CbmLitSimpleGeoQa::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("CbmGeoMuchPar");
   rtdb->getContainer("CbmStsDigiPar");
   rtdb->getContainer("CbmGeoStsPar");
}

void CbmLitSimpleGeoQa::Exec(
   Option_t* opt)
{

}

void CbmLitSimpleGeoQa::Finish()
{
}

ClassImp(CbmLitSimpleGeoQa);
