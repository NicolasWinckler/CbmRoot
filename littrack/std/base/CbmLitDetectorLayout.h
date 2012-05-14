/**
 * \file CbmLitDetectorLayout.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector layout used in track reconstruction.
 **/

#ifndef CBMLITDETECTORLAYOUT_H_
#define CBMLITDETECTORLAYOUT_H_

#include "base/CbmLitStationGroup.h"
#include "base/CbmLitStation.h"
#include "base/CbmLitSubstation.h"
#include "base/CbmLitEnums.h"

#include "TObject.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

class TGeoManager;
class CbmLitTrackParam;

/**
 * \class CbmLitDetectorLayout
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector layout used in track reconstruction.
 *
 * Detector layout consists of station groups.
 * Each station group consists of stations.
 * Each station consists of substations.
 **/
class CbmLitDetectorLayout
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitDetectorLayout();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitDetectorLayout();

   /**
    * \brief Set vector of station groups.
    * \param[in] stationGroups Vector of station groups.
    */
   void SetStationGroups(
      const std::vector<CbmLitStationGroup>& stationGroups) {
      fStationGroups = stationGroups;
   }

   /**
    * \brief Add station group to vector.
    * \param[in] stationGroup Station group to be added.
    */
   void AddStationGroup(
      const CbmLitStationGroup& stationGroup) {
      fStationGroups.push_back(stationGroup);
   }

   /**
    * \brief Return number of station groups in detector.
    * \return Number of station groups in detector.
    */
   Int_t GetNofStationGroups() const {
      return fStationGroups.size();
   }

   /**
    * \brief Return i-th station group.
    * \param[in] i Index of station group in detector.
    * \return Station group.
    */
   const CbmLitStationGroup& GetStationGroup(Int_t i) const {
      return fStationGroups[i];
   }

   /**
    * \brief Return number of station in concrete station group.
    * \param[in] stationGroup Index of station group.
    * \return Number of station groups in concrete station group.
    */
   Int_t GetNofStations(Int_t stationGroup) const {
      return GetStationGroup(stationGroup).GetNofStations();
   }

   /**
    * \brief Return station.
    * \param[in] stationGroup Index of station group.
    * \param[in] station Index of station in station group.
    * \return Station.
    */
   const CbmLitStation& GetStation(Int_t stationGroup, Int_t station) const {
      return GetStationGroup(stationGroup).GetStation(station);
   }

   /**
    * \brief Return number of substations in station.
    * \param[in] stationGroup Index of station group.
    * \param[in] station Index of station in station group.
    * \return Number of substations in station.
    */
   Int_t GetNofSubstations(Int_t stationGroup, Int_t station) const {
      return GetStationGroup(stationGroup).GetStation(station).GetNofSubstations();
   }

   /**
    * \brief Return substations in station.
    * \param[in] stationGroup Index of station group.
    * \param[in] station Index of station in station group.
    * \param[in] substation Index of substation in station.
    * \return Substation in station.
    */
   const CbmLitSubstation& GetSubstation(Int_t stationGroup, Int_t station, Int_t substation) const {
      return GetStationGroup(stationGroup).GetStation(station).GetSubstation(substation);
   }

   /**
    * \brief Return number of module rotations.
    * \param[in] stationGroup Index of station group.
    * \param[in] station Index of station in station group.
    * \param[in] substation Index of substation in station.
    * \return Number of module rotations.
    */
   Int_t GetNofModuleRotations(Int_t stationGroup, Int_t station, Int_t substation) const {
      return GetStationGroup(stationGroup).GetStation(station).GetSubstation(substation).GetNofModuleRotations();
   }

   /**
    * \brief Return total number of substations (detector planes) in detector.
    * \return Total number of substations (detector planes) in detector.
    */
   Int_t GetNofPlanes() const {
      Int_t nofPlanes = 0;
      for (Int_t i = 0; i < GetNofStationGroups(); i++) {
         nofPlanes += GetNofPlanes(i);
      }
      return nofPlanes;
   }

   /**
    * \brief Return total number of substations (detector planes) in station group.
    * \param[in] stationGroup Index of station group.
    * \return Total number of substations (detector planes) in station group.
    */
   Int_t GetNofPlanes(Int_t stationGroup) const {
      Int_t counter = 0;
      for (Int_t i = 0; i < GetNofStations(stationGroup); i++) {
         counter += GetNofSubstations(stationGroup, i);
      }
      return counter;
   }

   /**
    * \brief Set TGeoManager based tracking geometry.
    * \param[in] geo Pointer to TGeoManager.
    */
   void SetGeo(TGeoManager* geo) { fGeo = geo; }

   /**
    * \brief Return TGeoManager.
    */
   TGeoManager* GetGeo() const { return fGeo; }

   LitStatus FindBoundary(
      const CbmLitTrackParam* par,
      Int_t stationGroup,
      Int_t station,
      Int_t substation,
      bool downstream,
      litfloat& zOut,
      Int_t& moduleId) const;

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   std::string ToString() const;

private:
   void InitTrack(
      const CbmLitTrackParam* par,
      Bool_t downstream) const;

   Bool_t CheckGeoFailure() const;

   std::vector<CbmLitStationGroup> fStationGroups; // Vector of station groups
   TGeoManager* fGeo; // Tracking geometry based on TGeo
};

#endif /*CBMLITDETECTORLAYOUT_H_*/
