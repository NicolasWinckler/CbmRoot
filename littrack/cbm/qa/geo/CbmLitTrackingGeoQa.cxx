/**
 * \file CbmLitTrackingGeoQa.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#include "qa/geo/CbmLitTrackingGeoQa.h"

#include "base/CbmLitTrackingGeometryConstructor.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

CbmLitTrackingGeoQa::CbmLitTrackingGeoQa()
{
}

CbmLitTrackingGeoQa::~CbmLitTrackingGeoQa()
{
}

InitStatus CbmLitTrackingGeoQa::Init()
{
  // fGeoConstructor = CbmLitTrackingGeometryConstructor::Instance();
  // fGeoConstructor->Draw();
   return kSUCCESS;
}

void CbmLitTrackingGeoQa::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("CbmGeoMuchPar");
   rtdb->getContainer("CbmStsDigiPar");
   rtdb->getContainer("CbmGeoStsPar");
}

void CbmLitTrackingGeoQa::Exec(
   Option_t* opt)
{

}

void CbmLitTrackingGeoQa::Finish()
{
}

ClassImp(CbmLitTrackingGeoQa);
