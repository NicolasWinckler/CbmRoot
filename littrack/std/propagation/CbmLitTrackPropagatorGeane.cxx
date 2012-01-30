/** CbmTrackPropagatorGeane.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/
#include "propagation/CbmLitTrackPropagatorGeane.h"

#include "data/CbmLitTrackParam.h"
#include "utils/CbmLitConverter.h"
#include "cbm/CbmPropagatorGeane.h"

#include "FairTrackParam.h"
#include "FairTrackParP.h"

#include "TVector3.h"

#include <cmath>

CbmLitTrackPropagatorGeane::CbmLitTrackPropagatorGeane()
{
   fPropagator = new CbmPropagatorGeane();
}

CbmLitTrackPropagatorGeane::~CbmLitTrackPropagatorGeane()
{
   if (fPropagator) { delete fPropagator; }
}

LitStatus CbmLitTrackPropagatorGeane::Propagate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   litfloat zOut,
   int pdg,
   std::vector<litfloat>* F)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg, F);
}

LitStatus CbmLitTrackPropagatorGeane::Propagate(
   CbmLitTrackParam* par,
   litfloat zOut,
   int pdg,
   std::vector<litfloat>* F)
{
   FairTrackParam param;
   StatusCode result;

   CbmLitConverter::LitTrackParamToTrackParam(par, &param);

   if (std::abs(pdg) == 13) { pdg = (par->GetQp() < 0) ? 13 : -13; }
   if (std::abs(pdg) == 11) { pdg = (par->GetQp() < 0) ? 11 : -11; }

   result = fPropagator->Propagate(&param, zOut, pdg);

   if (result == kCBMSUCCESS) {
      CbmLitConverter::TrackParamToLitTrackParam(&param, par);
      return kLITSUCCESS;
   } else {
      return kLITERROR;
   }
}
