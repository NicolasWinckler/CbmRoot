/**
 * \file LitHitDataElectron.h
 *
 * \brief Class for accessing hits in track reconstruction.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 **/

#ifndef LITHITDATAELECTRON_H_
#define LITHITDATAELECTRON_H_

#include "LitDetectorGeometryElectron.h"
#include "../LitHit.h"
#include "../LitComparators.h"

#include <algorithm>

namespace lit {
namespace parallel {

/**
 * \class LitHitDataElectron
 *
 * \brief Class for accessing hits in track reconstruction.
 *
 * Hits are arranged by stations and sorted on each
 * station by X coordinate in acceding order.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitHitDataElectron
{
public:
   /**
    * \brief Constructor.
    */
   LitHitDataElectron():
      fMaxErr(),
      fHits(),
      fLayout() {};

   /**
    * \brief Destructor.
    */
   virtual ~LitHitDataElectron() {};

   /**
    * \brief Set detector layout for which hits are arranged.
    * \param[in] layout Detector layout.
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

   /**
    * \brief Add hit using station group, station and substation indices.
    * \param[in] stationGroup Station group index in detector layout.
    * \param[in] station Station index in station group.
    * \param[in] hit Pointer to hit to be added.
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

   /**
    * Add hit using absolute detector station index in detector layout.
    * \param[in] planeId Station index in detector layout.
    * \param[in] hit Pointer to hit to be added.
    */
   void AddHit(
      unsigned char planeId,
      LitScalPixelHit* hit) {
      unsigned char stationGroup;
      unsigned char station;
      StationByPlaneId(planeId, stationGroup, station);
      AddHit(stationGroup, station, hit);
   }

   /**
    * \brief Return hit using station group and station indices.
    * \param[in] stationGroup Station group index in detector layout.
    * \param[in] station Station index in station group.
    * \param[in] hitId Hit index in array of hits for specified station.
    * \return Hit pointer.
    */
   const LitScalPixelHit* GetHit(
      int stationGroup,
      int station,
      int hitId) const {
      return fHits[stationGroup][station][hitId];
   }

   /**
    * \brief Return vector of hits using station group and station indices.
    * \param[in] stationGroup Station group index in detector layout.
    * \param[in] station Station index in station group.
    * \return Vector of hits.
    */
   const std::vector<LitScalPixelHit*>& GetHits(
      int stationGroup,
      int station) {
      return fHits[stationGroup][station];
   }

   /**
    * \brief Return number of hits for specified station.
    * \param[in] stationGroup Station group index in detector layout.
    * \param[in] station Station index in station group.
    * \return Number of hits in station.
    */
   unsigned int GetNofHits(
      int stationGroup,
      int station) const {
      return fHits[stationGroup][station].size();
   }

   /**
    * \brief Return maximum hit error in [cm] for specified station.
    * \param[in] stationGroup Station group index in detector layout.
    * \param[in] station Station index in station group.
    * \return Hit error.
    */
   fscal GetMaxErr(
      int stationGroup,
      int station) const {
      return fMaxErr[stationGroup][station];
   }

   /**
    * \brief Clear hit arrays.
    */
   void Clear() {
      for(unsigned int i = 0; i < fHits.size(); i++) {
         for(unsigned int j = 0; j < fHits[i].size(); j++) {
            fHits[i][j].clear();
            fHits[i][j].reserve(1500);
            fMaxErr[i][j] = 0.;
         }
      }
   }


   /**
    * \brief Sort hits in each station by X coordinate.
    */
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

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
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

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitHitDataElectron& hitData) {
      strm << hitData.ToString();
      return strm;
   }

private:
   /**
    * \brief Calculate station group and station indices using the absolute detector plane number.
    * \param[in] planeId Absolute station index in detector layout.
    * \param[out] stationGroup Station group index in detector layout.
    * \param[out] station Station index in station group.
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

/**
 * \typedef LitHitDataElectron<fscal> LitHitDataElectronScal
 * \brief Scalar version of LitHitDataElectron.
 */
typedef LitHitDataElectron<fscal> LitHitDataElectronScal;

/**
 * \typedef LitHitDataElectron<fvec> LitHitDataElectronVec
 * \brief Vector version of LitHitDataElectron.
 */
typedef LitHitDataElectron<fvec> LitHitDataElectronVec;

} // namespace parallel
} // namespace lit
#endif /* LITHITDATAELECTRON_H_ */
