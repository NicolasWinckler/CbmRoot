/** CbmLitCleverTrackExtrapolator.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2009
 **/

#include "propagation/CbmLitCleverTrackExtrapolator.h"

#include "base/CbmLitEnums.h"
#include "base/CbmLitDefaultSettings.h"
#include "../cbm/base/CbmLitToolFactory.h"
#include "data/CbmLitTrackParam.h"
#include "interface/CbmLitTrackExtrapolator.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitMatrixMath.h"

CbmLitCleverTrackExtrapolator::CbmLitCleverTrackExtrapolator(
   const std::string& type)
{
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fLineExtrapolator = factory->CreateTrackExtrapolator("line");
   if (type == "myfield") { fRK4Extrapolator = factory->CreateTrackExtrapolator("rk4myfield"); }
   else if (type == "") { fRK4Extrapolator = factory->CreateTrackExtrapolator("rk4"); }
}

CbmLitCleverTrackExtrapolator::~CbmLitCleverTrackExtrapolator()
{
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   myf zOut,
   std::vector<myf>* F)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut, F);
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
   CbmLitTrackParam* par,
   myf zOut,
   std::vector<myf>* F)
{
   myf zIn = par->GetZ();
   myf zStart = lit::LINE_EXTRAPOLATION_START_Z;

   if (zIn >= zStart && zOut >= zStart) {
      return fLineExtrapolator->Extrapolate(par, zOut, F);
   } else if (zIn < zStart && zOut < zStart) {
      return fRK4Extrapolator->Extrapolate(par, zOut, F);
   } else if (zOut > zIn && zIn < zStart && zOut > zStart) {
      std::vector<myf> F1(25), F2(25);
      LitStatus result;
      if (F != NULL) { result = fRK4Extrapolator->Extrapolate(par, zStart, &F1); }
      else { result = fRK4Extrapolator->Extrapolate(par, zStart, NULL); }
      if (result == kLITERROR) {
         return result;
      } else {
         LitStatus result1;
         if (F != NULL) { result1 = fLineExtrapolator->Extrapolate(par, zOut, &F2); }
         else { result1 = fLineExtrapolator->Extrapolate(par, zOut, NULL); }
         if (F != NULL && result1 == kLITSUCCESS) { Mult25(F2, F1, *F); }
         return result1;
      };
   } else if (zOut < zIn && zIn > zStart && zOut < zStart) {
      std::vector<myf> F1(25), F2(25);
      LitStatus result;
      if (F != NULL) { result = fLineExtrapolator->Extrapolate(par, zStart, &F1); }
      else { result = fLineExtrapolator->Extrapolate(par, zStart, NULL); }
      if (result == kLITERROR) {
         return result;
      } else {
         LitStatus result1;
         if (F != NULL) { result1 = fRK4Extrapolator->Extrapolate(par, zOut, &F2); }
         else { result1 = fRK4Extrapolator->Extrapolate(par, zOut, NULL); }
         if (F != NULL && result1 == kLITSUCCESS) { Mult25(F2, F1, *F); }
         return result1;
      }
   }

   return kLITSUCCESS;
}
