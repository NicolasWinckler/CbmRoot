/**
 * \file CbmLitMCTrackCreator.h
 * \brief Creates CbmLitMCTrack objects.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef CBMLITMCTRACKCREATOR_H_
#define CBMLITMCTRACKCREATOR_H_

#include "CbmLitMCTrack.h"
#include <map>
#include <iostream>
#include <assert.h>

class TClonesArray;
class FairMCPoint;

/**
 * \class CbmLitMCTrackCreator
 * \brief Creates CbmLitMCTrack objects.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class CbmLitMCTrackCreator
{
private:
   /**
    * \brief Constructor.
    */
   CbmLitMCTrackCreator();

public:
   /**
    * \brief Destructor.
    */
   virtual ~CbmLitMCTrackCreator();

   /**
    * \brief Singleton instance.
    */
   static CbmLitMCTrackCreator* Instance();

   /**
    * \brief Creates array of CbmLitMCTracks for current event.
    * Has to be executed in Exec() function of the task.
    */
   void Create();

   /**
    * \brief Check whether a track exists in the array.
    * \param[in] mcId MC index of track.
    * \return true if track exists in array.
    */
   bool TrackExists(
         int mcId) const {
      return (fLitMCTracks.count(mcId) > 0) ? true : false;
   }

   /**
    * \brief Return CbmLitMCTrack by its index.
    * \param[in] mcId MC index of track.
    * \return MC track.
    */
   const CbmLitMCTrack& GetTrack(
         int mcId) const {
      assert(TrackExists(mcId));
      return fLitMCTracks.find(mcId)->second;
   }

private:

   /**
    * \brief Read data branches.
    */
   void ReadDataBranches();

   /**
    * \brief Add MC points from a certain detector.
    * \param[in] detId Detector identificator.
    * \param[in] array Array of FairMCPoint.
    */
   void AddPoints(
         DetectorId detId,
         const TClonesArray* array);

   /**
    * \brief Convert FairMCPoint to CbmLitMCPoint.
    * \param[in] fairPoint Pointer to FairMCPoint to be converted.
    * \param[out] litPoint Pointer to output CbmLitMCPoint.
    * \param[in] refId Reference index of the MC point.
    */
   void FairMCPointToLitMCPoint(
         const FairMCPoint* fairPoint,
         CbmLitMCPoint* litPoint,
         int refId,
         int stationId);

   /**
    * \brief Fill maps for MC points to station id.
    */
   void FillStationMaps();

   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fStsPoints; // CbmStsPoint array
   TClonesArray* fTrdPoints; // CbmTrdPoint array
   TClonesArray* fMuchPoints; // CbmMuchPoint array
   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fRichPoints; // CbmRichPoint array

   // Stores created CbmLitMCTrack objects.
   // std::map<MC track index, CbmLitMCTrack object>.
   std::map<int, CbmLitMCTrack> fLitMCTracks;

   // Map <MC point index, station index>
   std::map<int, int>fMvdStationsMap; // for MVD
   std::map<int, int>fStsStationsMap; // for STS
   std::map<int, int>fTrdStationsMap; // for TRD
   std::map<int, int>fMuchStationsMap; // for MUCH
};

#endif /* CBMLITMCTRACKCREATOR_H_ */
