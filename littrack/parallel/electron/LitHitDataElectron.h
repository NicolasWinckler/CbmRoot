/** LitHitDataElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** Class for accessing the hits in the track reconstruction.
 **/

#ifndef LITHITDATAELECTRON_H_
#define LITHITDATAELECTRON_H_

#include "../LitHit.h"
#include "LitDetectorGeometryElectron.h"
#include "../LitComparators.h"

#include <algorithm>

namespace lit {
namespace parallel {

template<class T>
class LitHitDataElectron
{
public:
   /* Constructor */
   LitHitDataElectron():
      fMaxErr(),
      fHits(),
      fLayout() {};

   /* Destructor */
   virtual ~LitHitDataElectron() {};

   /* Sets the detector layout for which the hits are arranged
    *@param layout Detector layout
    */
   void SetDetectorLayout(
      const LitDetectorLayoutElectron<T>& layout) {
      fLayout = layout;
      int nofGroups = layout.GetNofStationGroups();
      fHits.resize(nofGroups);
      fMaxErr.resize(nofGroups);
      for(int i = 0; i < nofGroups; i++) {
         int nofStations = layout.GetNofStations(i);
         fHits[i].resize(nofStations);
         fMaxErr[i].resize(nofStations);
         for(int j = 0; j < nofStations; j++) {
            fHits[i][j].reserve(1500);
         }
      }
   }

   /* Adds the hit using station group, station and substation indices
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@param hit Pointer to the hit to be added
    */
   void AddHit(
      int stationGroup,
      int station,
      LitScalPixelHit* hit) {
      fHits[stationGroup][station].push_back(hit);
      if (fMaxErr[stationGroup][station] < hit->Dx) {
         fMaxErr[stationGroup][station] = hit->Dx;
      }
   }

   /*Adds the hit using absolute detector plane (station) index in the detector
    *@param planeId Index of the detector plane (station) in the detector
    *@param hit Pointer to the hit to be added
    */
   void AddHit(
      unsigned char planeId,
      LitScalPixelHit* hit) {
      unsigned char stationGroup;
      unsigned char station;
      StationByPlaneId(planeId, stationGroup, station);
      AddHit(stationGroup, station, hit);
   }

   /* Returns the hit using station group and station indices
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@param hitId Hit index in the array of hits for the specified station
    *@return Hit pointer
    */
   const LitScalPixelHit* GetHit(
      int stationGroup,
      int station,
      int hitId) const {
      return fHits[stationGroup][station][hitId];
   }

   /* Returns hit iterators using station group and station indices
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@return Hit iterators
    */
   const std::vector<LitScalPixelHit*>& GetHits(
      int stationGroup,
      int station) {
      return fHits[stationGroup][station];
   }

   /* Returns number of hits for the specified station
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@return Number of hits
    */
   unsigned int GetNofHits(
      int stationGroup,
      int station) const {
      return fHits[stationGroup][station].size();
   }

   /* Returns maximum hit error in [cm] and the name of the coordinate
    * ("X", "Y", "U")for the specified station.
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@return Pair of hit error and and coordinate ID
    */
   fscal GetMaxErr(
      int stationGroup,
      int station) const {
      return fMaxErr[stationGroup][station];
   }

   /* Clears the hit arrays */
   void Clear() {
      for(unsigned int i = 0; i < fHits.size(); i++) {
         for(unsigned int j = 0; j < fHits[i].size(); j++) {
            fHits[i][j].clear();
            fHits[i][j].reserve(1500);
            fMaxErr[i][j] = 0.;
         }
      }
   }


   /* Sorts hits in each substation by X coordinate */
   void SortHits() {
      for (int i = 0; i < fLayout.GetNofStationGroups(); i++) {
         for (int j = 0; j < fLayout.GetNofStations(i); j++) {
               std::vector<LitScalPixelHit*>& shits = fHits[i][j];
               std::sort(shits.begin(), shits.end(), ComparePixelHitXLess());
   //                std::cout << "station group " << i << " station " << j << std::endl;
   //                for(unsigned int i = 0; i < nh; i++)
   //                   std::cout << *shits[i];
   //          }
         }
      }
   }

   /* Returns std::string representation for the class */
   std::string ToString() const {
      std::string str = "HitDataElectron:\n";
      for(int i = 0; i < fLayout.GetNofStationGroups(); i++) {
         str += " station group " + lit::parallel::ToString<int>(i) + "\n";
         for(int j = 0; j < fLayout.GetNofStations(i); j++) {
            str += "   station " + lit::parallel::ToString<int>(j)
                  + ": " + lit::parallel::ToString<int>(GetNofHits(i, j)) + " hits, "
                  + "max err=" + lit::parallel::ToString<T>(GetMaxErr(i, j)) + "\n";
         }
      }
      return str;
   }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitHitDataElectron& hitData) {
      strm << hitData.ToString();
      return strm;
   }

private:
   /* Calculates station group and station indices using the
    * detector plane number.
    *@param planeId [in] Detector plane index
    *@param stationGroup [out] Index of the station group in the detector
    *@param station [out] Index of the station in the station group
    */
   void StationByPlaneId(
      unsigned char planeId,
      unsigned char& stationGroup,
      unsigned char& station) const {
      unsigned char counter = 0;
      for(unsigned char i = 0; i < fLayout.GetNofStationGroups(); i++) {
         counter += fLayout.GetNofStations(i);
         if (counter > planeId) {
            stationGroup = i;
            station = fLayout.GetNofStations(i) - (counter - planeId);
            return;
         }
      }
   }

   // Arrays with hits
   std::vector<std::vector<PixelHitArray> > fHits;
   // Arrays with maximum hit position errors for each station
   std::vector<std::vector<fscal> > fMaxErr;
   // Detector layout
   LitDetectorLayoutElectron<T> fLayout;
} _fvecalignment;

typedef LitHitDataElectron<fscal> LitHitDataElectronScal;
typedef LitHitDataElectron<fvec> LitHitDataElectronVec;

} // namespace parallel
} // namespace lit
#endif /* LITHITDATAELECTRON_H_ */
