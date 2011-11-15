/**
 * \file CbmLitMCTrack.h
 *
 * \brief Monte-Carlo track.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef CBMLITMCTRACK_H_
#define CBMLITMCTRACK_H_

#include "CbmLitMCPoint.h"
#include "CbmDetectorList.h"
#include <map>
#include <vector>

/**
 * \class CbmLitMCTrack
 *
 * \brief Monte-Carlo track.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class CbmLitMCTrack {
public:
   /**
    * \brief Constructor.
    */
   CbmLitMCTrack() {
      // Initialize all maps
      fPoints[kMVD];
      fPoints[kSTS];
      fPoints[kTRD];
      fPoints[kMUCH];
      fPoints[kTOF];
      fStationPoints[kMVD];
      fStationPoints[kSTS];
      fStationPoints[kTRD];
      fStationPoints[kMUCH];
      fStationPoints[kTOF];
   }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitMCTrack() {};

   /**
    * \brief Add point to track.
    * \param[in] detId Detector identificator.
    * \param[in] point MC point to be added.
    */
   void AddPoint(DetectorId detId, const CbmLitMCPoint& point) {
      fPoints[detId].push_back(point);
      fStationPoints[detId][point.GetStationId()].push_back(point);
   }

   /**
    * \brief Return vector of MC point for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Array of MC points.
    */
   const std::vector<CbmLitMCPoint>& GetPoints(DetectorId detId) const {
      return fPoints.find(detId)->second;
   }

   /**
    * \brief Return MC point for specified detector id and point index.
    * \param[in] detId Detector identificator.
    * \param[in] index Index of MC point.
    * \return MC point.
    */
   const CbmLitMCPoint& GetPoint(DetectorId detId, int index) const {
      return fPoints.find(detId)->second[index];
   }

   /**
    * \brief Return number of MC points for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
   unsigned int GetNofPoints(DetectorId detId) const {
      return fPoints.find(detId)->second.size();
   }

   /**
    * \brief Return MC point for specified detector id and point index.
    *
    * Before accessing points with this function one has to check
    * the number of points on this station!
    *
    * \param[in] detId Detector identificator.
    * \param[in] stationId Station id.
    * \param[in] index Index of MC point.
    * \return MC point.
    */
   const CbmLitMCPoint& GetPointAtStation(
         DetectorId detId,
         int stationId,
         int index) const {
      return fStationPoints.find(detId)->second.find(stationId)->second[index];
   }

   /**
    * \brief Return number of MC points for specified detector ID and station ID.
    * \param[in] detId Detector identificator.
    *  \param[in] stationId Station id.
    * \return Number of MC points.
    */
   unsigned int GetNofPointsAtStation(
         DetectorId detId,
         int stationId) const {
      if (fStationPoints.find(detId)->second.count(stationId) > 0) {
         return fStationPoints.find(detId)->second.find(stationId)->second.size();
      } else return 0;
   }

private:
   // std::map<detector id, std::vector of MC points>
   std::map<int, std::vector<CbmLitMCPoint> > fPoints;

   // std::map<detector id, std::map<station id, std::vector of MC points> >
   std::map<int, std::map<int, std::vector<CbmLitMCPoint> > > fStationPoints;

private:

   std::string PointsToString(
         DetectorId detId,
         const std::string& detName) const {
      std::stringstream ss;
      ss << detName << " npoints=" << GetNofPoints(detId) << " points=(";
      for (int i = 0; i < GetNofPoints(detId); i++) {
         //ss << GetPoint(detId, i).GetRefId() << ",";
         ss << GetPoint(detId, i);
      }
      ss << ") ";
      return ss.str();
   }

public:

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "MCTrack: ";
      ss << PointsToString(kMVD, "MVD") << "|";
      ss << PointsToString(kSTS, "STS") << "|";
      ss << PointsToString(kTRD, "TRD") << "|";
      ss << PointsToString(kMUCH, "MUCH") << "|";
      ss << PointsToString(kTOF, "TOF") << "|";
      ss << std::endl;
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const CbmLitMCTrack& track) {
      strm << track.ToString();
      return strm;
   }
};

#endif /* CBMLITMCTRACK_H_ */
