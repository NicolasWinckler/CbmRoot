/** LitHitDataMuon.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class for accessing hits in the track reconstruction.
 **/

#ifndef LITHITDATAMUON_H_
#define LITHITDATAMUON_H_

#include "LitDetectorGeometryMuon.h"
#include "../LitHit.h"
#include "../LitComparators.h"

#include <vector>
#include <algorithm>

namespace lit {
namespace parallel {

template<class T>
class LitHitDataMuon
{
public:
   /* Constructor */
   LitHitDataMuon():
      fMaxErr(),
      fHits(),
      fLayout() {};

   /* Destructor */
   virtual ~LitHitDataMuon() {};

   /* Sets detector layout for which hits are arranged
    * @param layout Detector layout */
   void SetDetectorLayout(
      const LitDetectorLayoutMuon<T>& layout) {
      fLayout = layout;
      int nofGroups = layout.GetNofStationGroups();
      fHits.resize(nofGroups);
      fMaxErr.resize(nofGroups);
      for(int i = 0; i < nofGroups; i++) {
         int nofStations = layout.GetNofStations(i);
         fHits[i].resize(nofStations);
         fMaxErr[i].resize(nofStations);
         for(int j = 0; j < nofStations; j++) {
            int nofSubstations = layout.GetNofSubstations(i, j);
            fHits[i][j].resize(nofSubstations);
            fMaxErr[i][j].resize(nofSubstations);
            for(int k = 0; k < nofSubstations; k++) {
               fHits[i][j][k].reserve(1500);
            }
         }
      }
   }

   /* Adds hit using station group, station and substation indices
    * @param stationGroup Index of the station group in the detector
    * @param station Index of the station in the station group
    * @param substation Index of the substation in the station
    * @param hit Pointer to the hit to be added */
   void AddHit(
      int stationGroup,
      int station,
      int substation,
      LitScalPixelHit* hit) {
      fHits[stationGroup][station][substation].push_back(hit);
      if (fMaxErr[stationGroup][station][substation] < hit->Dx) {
         fMaxErr[stationGroup][station][substation] = hit->Dx;
      }
   }

   /* Adds hit using absolute detector plane (substation) index in the detector
    * @param planeId Index of the detector plane (substation) in the detector
    * @param hit Pointer to the hit to be added */
   void AddHit(
      unsigned char planeId,
      LitScalPixelHit* hit) {
      unsigned char stationGroup;
      unsigned char station;
      unsigned char substation;
      StationByPlaneId(planeId, stationGroup, station, substation);
      AddHit(stationGroup, station, substation, hit);
   }

   /* Returns hit using station group, station and substation indices
    * @param stationGroup Index of the station group in the detector
    * @param station Index of the station in the station group
    * @param substation Index of the substation in the station
    * @param hitId Hit index in the array of hits for the specified substation
    * @return Hit pointer */
   const LitScalPixelHit* GetHit(
      int stationGroup,
      int station,
      int substation,
      int hitId) const {
      return fHits[stationGroup][station][substation][hitId];
   }

   /* Returns hit vector using station group, station and substation indices
    * @param stationGroup Index of the station group in the detector
    * @param station Index of the station in the station group
    * @param substation Index of the substation in the station
    * @return Hit vector */
   const std::vector<LitScalPixelHit*>& GetHits(
      int stationGroup,
      int station,
      int substation) {
      return fHits[stationGroup][station][substation];
   }

   /* Returns number of hits for the specified substation
    * @param stationGroup Index of the station group in the detector
    * @param station Index of the station in the station group
    * @param substation Index of the substation in the station
    * @return Number of hits */
   unsigned int GetNofHits(
      int stationGroup,
      int station,
      int substation) const {
      return fHits[stationGroup][station][substation].size();
   }

   /* Returns maximum hit error in [cm] for the specified substation.
    * @param stationGroup Index of the station group in the detector
    * @param station Index of the station in the station group
    * @param substation Index of the substation in the station
    * @return Maximum hit error */
   fscal GetMaxErr(
      int stationGroup,
      int station,
      int substation) const {
      return fMaxErr[stationGroup][station][substation];
   }

   /* Clears hit arrays */
   void Clear() {
      for(unsigned int i = 0; i < fHits.size(); i++) {
         for(unsigned int j = 0; j < fHits[i].size(); j++) {
            for(unsigned int k = 0; k < fHits[i][j].size(); k++) {
               fHits[i][j][k].clear();
               fHits[i][j][k].reserve(1500);
               fMaxErr[i][j][k] = 0.;
            }
         }
      }
   }

   /* Sorts hits in each substation by X coordinate */
   void SortHits() {
      // TODO : add threads here
      for (int i = 0; i < fLayout.GetNofStationGroups(); i++) {
         for (int j = 0; j < fLayout.GetNofStations(i); j++) {
            for (int k = 0; k < fLayout.GetNofSubstations(i, j); k++) {
               std::vector<LitScalPixelHit*>& shits = fHits[i][j][k];
#ifdef LIT_USE_TBB
               tbb::parallel_sort(shits.begin(), shits.end(), ComparePixelHitXLess());
#else
               std::sort(shits.begin(), shits.end(), ComparePixelHitXLess());
#endif
   //                std::cout << "station group " << i << " station " << j
   //                   << " substation " << k << std::endl;
   //                for(unsigned int i = 0; i < nh; i++)
   //                   std::cout << *shits[i];
            }
   //          }
         }
      }
   }

public:
   /* Calculates station group, station and substation indices using the
    * detector plane number.
    * @param planeId [in] Detector plane index
    * @param stationGroup [out] Index of the station group in the detector
    * @param station [out] Index of the station in the station group
    * @param subsattion [out] Index of the substation in the station */
   void StationByPlaneId(
      unsigned char planeId,
      unsigned char& stationGroup,
      unsigned char& station,
      unsigned char& substation) const {
      unsigned char counter = 0;
      for(unsigned char i = 0; i < fLayout.GetNofStationGroups(); i++) {
         for(unsigned char j = 0; j < fLayout.GetNofStations(i); j++) {
            counter += fLayout.GetNofSubstations(i, j);
            if (counter > planeId) {
               stationGroup = i;
               station = j;
               substation = fLayout.GetNofSubstations(i, j) - (counter - planeId);
               return;
            }
         }
      }
   }

   // Arrays with hits
   std::vector<std::vector<std::vector<PixelHitArray> > > fHits;
   // Arrays with maximum hit position errors for each substation
   std::vector<std::vector<std::vector<fscal> > > fMaxErr;
   // Detector layout
   LitDetectorLayoutMuon<T> fLayout;

   friend std::ostream& operator<<(std::ostream& strm, const LitHitDataMuon& hitData) {
      strm << "HitDataMuon:" << std::endl;
      for(int i = 0; i < hitData.fLayout.GetNofStationGroups(); i++) {
         strm << " station group " << i << std::endl;
         for(int j = 0; j < hitData.fLayout.GetNofStations(i); j++) {
            strm << "  station " << j << std::endl;
            for(int k = 0; k < hitData.fLayout.GetNofSubstations(i, j); k++) {
               strm << "   substation " << k << ": " << hitData.GetNofHits(i, j, k) << " hits, "
                    << "max err=" << hitData.GetMaxErr(i, j, k) << std::endl;
            }
         }
      }
      return strm;
   }

} _fvecalignment;

} // namespace parallel
} // namespace lit
#endif /* LITHITDATAMUON_H_ */
