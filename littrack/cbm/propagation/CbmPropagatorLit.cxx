#include "cbm/propagation/CbmPropagatorLit.h"

#include "base/CbmLitToolFactory.h"
#include "interface/CbmLitTrackPropagator.h"
#include "utils/CbmLitConverter.h"

#include "FairTrackParam.h"

CbmPropagatorLit::CbmPropagatorLit(
   const std::string& type)
{
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fPropagator = factory->CreateTrackPropagator(type);
}

CbmPropagatorLit::~CbmPropagatorLit()
{
// if (fPropagator) delete fPropagator;
}

StatusCode CbmPropagatorLit::Propagate(
   const FairTrackParam* parIn,
   FairTrackParam* parOut,
   Double_t zOut,
   Int_t pdg)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg);
}

StatusCode CbmPropagatorLit::Propagate(
   FairTrackParam* par,
   Double_t zOut,
   Int_t pdg)
{
   CbmLitTrackParam param;
   LitStatus result;

   CbmLitConverter::TrackParamToLitTrackParam(par, &param);

   result = fPropagator->Propagate(&param, zOut, pdg);

   if (result == kLITSUCCESS) {
      CbmLitConverter::LitTrackParamToTrackParam(&param, par);
      return kCBMSUCCESS;
   } else {
      return kCBMERROR;
   }
}

void CbmPropagatorLit::TransportMatrix(
   std::vector<Double_t>& F)
{
// fPropagator->TransportMatrix(F);
}

void CbmPropagatorLit::TransportMatrix(
   TMatrixD& F)
{
// fPropagator->TransportMatrix(F);
}
