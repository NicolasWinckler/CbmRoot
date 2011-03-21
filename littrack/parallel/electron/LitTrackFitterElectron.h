/** LitTrackFitterElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Functions perform kalman filter based track fit for
 * 'electron' setup of CBM.
 **/

#ifndef LITTRACKFITTERELECTRON_H_
#define LITTRACKFITTERELECTRON_H_

#include "LitDetectorGeometryElectron.h"
#include "parallel/LitTypes.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitExtrapolation.h"
#include "parallel/LitFiltration.h"
#include "parallel/LitAddMaterial.h"

/* Performs SIMDized/scalar fast track fit.
 * Number of hits must be equal to the number of stations.
 */
template<class T>
inline void LitTrackFitterElectron(
   LitTrack<T>& track,
   const LitDetectorLayoutElectron<T>& layout)
{
   //   track.paramLast = track.paramFirst;
   LitTrackParam<T>& par = track.paramLast;
   unsigned char ihit = 0;

   for (unsigned char ivp = 0; ivp < layout.GetNofVirtualPlanes()-1; ivp++) {
      const LitVirtualPlaneElectron<T>& vp1 = layout.virtualPlanes[ivp];
      const LitVirtualPlaneElectron<T>& vp2 = layout.virtualPlanes[ivp+1];

      LitRK4ExtrapolationElectron(par, vp2.Z, vp1.fieldSlice, vp1.fieldSliceMid, vp2.fieldSlice);
      LitAddMaterialElectron(par, vp2.material);
   }

   for (unsigned char isg = 0; isg < layout.GetNofStationGroups(); isg++) {
      const LitStationGroupElectron<T>& stationGroup = layout.stationGroups[isg];

      for (unsigned char ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         const LitStationElectron<T>& station = stationGroup.stations[ist];

         LitLineExtrapolation(par, station.Z);

         for (unsigned char im = 0; im < station.GetNofMaterialsBefore(); im++) {
            LitAddMaterialElectron(par, station.materialsBefore[im]);
         }

         LitPixelHit<T>* hit = track.hits[ihit];
         LitFiltration(par, *hit);
         ihit++;

         for (unsigned char im = 0; im < station.GetNofMaterialsAfter(); im++) {
            LitAddMaterialElectron(par, station.materialsAfter[im]);
         }
      }
   }
}

#endif /* LITTRACKFITTERELECTRON_H_ */
