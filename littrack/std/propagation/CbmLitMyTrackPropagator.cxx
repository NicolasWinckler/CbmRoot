/** CbmLitMyTrackPropagator.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2009
 **
 **/

#include "propagation/CbmLitMyTrackPropagator.h"

#include "base/CbmLitDefaultSettings.h"
#include "propagation/CbmLitMyGeoNavigator.h"
#include "propagation/CbmLitMaterialEffectsImp.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitMatrixMath.h"

#include <cmath>
#include <iostream>

CbmLitMyTrackPropagator::CbmLitMyTrackPropagator(
   TrackExtrapolatorPtr extrapolator):
   fExtrapolator(extrapolator)
{
   fNavigator = GeoNavigatorPtr(new CbmLitMyGeoNavigator());
   fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());
}

CbmLitMyTrackPropagator::~CbmLitMyTrackPropagator()
{
}

LitStatus CbmLitMyTrackPropagator::Propagate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   litfloat zOut,
   int pdg,
   std::vector<litfloat>* F)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg, F);
}

LitStatus CbmLitMyTrackPropagator::Propagate(
   CbmLitTrackParam* par,
   litfloat zOut,
   int pdg,
   std::vector<litfloat>* F)
{
   litfloat zIn = par->GetZ();
   litfloat dz = zOut - zIn;
   if(std::fabs(dz) < lit::MINIMUM_PROPAGATION_DISTANCE) { return kLITSUCCESS; }

   //Check whether upstream or downstream
   bool downstream = dz > 0;

   if (F != NULL) {
      F->assign(25, 0.);
      (*F)[0] = 1.;
      (*F)[6] = 1.;
      (*F)[12] = 1.;
      (*F)[18] = 1.;
      (*F)[24] = 1.;
   }

   int nofSteps = int(std::abs(dz) / lit::MAXIMUM_PROPAGATION_STEP_SIZE);
   litfloat stepSize;
   if (nofSteps == 0) { stepSize = dz; }
   else { stepSize = lit::MAXIMUM_PROPAGATION_STEP_SIZE; }
   litfloat z = zIn;

   //Loop over steps + additional step to propagate to virtual plane at zOut
   for (int iStep = 0; iStep < nofSteps + 1; iStep++) {
//    if (!IsParCorrect(par)) {
////        std::cout << "-E- CbmLitTGeoTrackPropagator::Propagate: incorrect track parameters" << std::endl;
//       return kLITERROR;
//    }

      // update current z position
      if (iStep != nofSteps) { z += stepSize; }
      else { z = zOut; }

      std::vector<CbmLitMaterialInfo> inter;
      if (fNavigator->FindIntersections(par, z, inter) == kLITERROR) {
         std::cout << "-E- CbmLitMyTrackPropagator::Propagate: navigation failed" << std::endl;
         return kLITERROR;
      }

      for(unsigned int iMat = 0; iMat < inter.size(); iMat++) {
         CbmLitMaterialInfo mat = inter[iMat];

         std::vector<litfloat>* Fnew = NULL;
         if (F != NULL) { Fnew = new std::vector<litfloat>(25, 0.); }
         if (fExtrapolator->Extrapolate(par, mat.GetZpos(), Fnew) == kLITERROR) {
            std::cout << "-E- CbmLitMyTrackPropagator::Propagate extrapolation failed" << std::endl;
            return kLITERROR;
         }

         // update transport matrix
         if (F != NULL) { UpdateF(*F, *Fnew); }
         delete Fnew;

         //scale material length
         litfloat norm = std::sqrt(1. + par->GetTx() * par->GetTx() + par->GetTy() * par->GetTy());
         mat.SetLength(mat.GetLength() * norm);
         // add material effects
         fMaterial->Update(par, &mat, pdg, downstream);
      }
   } // loop over steps

   std::vector<litfloat>* Fnew = NULL;
   if (F != NULL) { Fnew = new std::vector<litfloat>(25, 0.); }
   fExtrapolator->Extrapolate(par, zOut, Fnew);
   if (F != NULL) { UpdateF(*F, *Fnew); }
   delete Fnew;
// par->SetZ(zOut);

   return kLITSUCCESS;
}

void CbmLitMyTrackPropagator::UpdateF(
   std::vector<litfloat>& F,
   const std::vector<litfloat>& newF)
{
   std::vector<litfloat> A(25);
   Mult25(newF, F, A);
   F.assign(A.begin(), A.end());
}
