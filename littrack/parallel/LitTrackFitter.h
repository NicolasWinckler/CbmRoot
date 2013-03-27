/**
 * \file LitTrackFitter.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Kalman filter based track fit.
 **/

#ifndef LITTRACKFITTER_H_
#define LITTRACKFITTER_H_

#include "LitDetectorLayout.h"
#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitExtrapolation.h"
#include "LitFiltration.h"
#include "LitAddMaterial.h"
#include "LitScalPixelHit.h"

namespace lit {
namespace parallel {

/**
 * \brief Fast scalar track fit.
 */
inline void LitTrackFitter(
   LitScalTrack& track,
   const LitDetectorLayoutScal& layout)
{
   LitTrackParamScal par = track.GetParamFirst();

   unsigned char nofVirtualStations = layout.GetNofVirtualStations();
   unsigned char nofSteps = (nofVirtualStations - 1) / 2;
   for (unsigned char iStep = 0; iStep < nofSteps; iStep++) {
      const LitStationScal& vsFront = layout.GetVirtualStation(2 * iStep + 0);
      const LitStationScal& vsMiddle = layout.GetVirtualStation(2 * iStep + 1);
      const LitStationScal& vsBack = layout.GetVirtualStation(2 * iStep + 2);
      if (vsFront.GetField().IsEmpty() || vsMiddle.GetField().IsEmpty() || vsBack.GetField().IsEmpty()) {
         LitLineExtrapolation(par, vsBack.GetZ());
      } else {
         LitRK4Extrapolation(par, vsBack.GetZ(), vsFront.GetField(), vsMiddle.GetField(), vsBack.GetField());
      }

      if (!vsFront.GetMaterial().IsEmpty()) {
         LitAddMaterial<fscal>(par, vsFront.GetMaterial().GetMaterial(par.X, par.Y));
      }

      if (!vsMiddle.GetMaterial().IsEmpty()) {
         LitAddMaterial<fscal>(par, vsMiddle.GetMaterial().GetMaterial(par.X, par.Y));
      }

      if (!vsBack.GetMaterial().IsEmpty()) {
         LitAddMaterial<fscal>(par, vsBack.GetMaterial().GetMaterial(par.X, par.Y));
      }
   }

   fscal chiSq = 0.;
   unsigned char prevStationId = -1; // start with -1, i.e. there where no previous station id
   unsigned short nofHits = track.GetNofHits();
   for (unsigned short iHit = 0; iHit < nofHits; iHit++) {
      const LitScalPixelHit* hit = track.GetHit(iHit);

      LitLineExtrapolation(par, hit->Z);

      // Add material taking into account missing hits
      for (unsigned char iStation = prevStationId + 1; iStation <= hit->stationId; iStation++) {
         const LitStationScal& station = layout.GetStation(iStation);
         if (!station.GetMaterial().IsEmpty()) {
            LitAddMaterial<fscal>(par, station.GetMaterial().GetMaterial(par.X, par.Y));
         }
      }
      prevStationId = hit->stationId;

      LitFiltration(par, *hit, chiSq);

      if (iHit == 0) track.SetParamFirst(par);
   }
   track.SetParamLast(par);
}

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFITTER_H_ */