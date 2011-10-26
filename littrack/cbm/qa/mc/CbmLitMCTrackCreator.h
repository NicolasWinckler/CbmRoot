/**
 * \file CbmLitMCTrackCreator.h
 *
 * \brief Creates CbmLitMCTrack objects.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef CBMLITMCTRACKCREATOR_H_
#define CBMLITMCTRACKCREATOR_H_

#include "CbmLitMCTrack.h"
#include <map>

class TClonesArray;
class FairMCPoint;

/**
 * \class CbmLitMCTrackCreator.h
 *
 * \brief Creates CbmLitMCTrack objects.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class CbmLitMCTrackCreator {
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

   /*
    * \brief Creates array of CbmLitMCTracks for current event.
    * Has to be executed in Exec() function of the task.
    */
   void Create();

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
         int refId);

   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fStsPoints; // CbmStsPoint array
   TClonesArray* fTrdPoints; // CbmTrdPoint array
   TClonesArray* fMuchPoints; // CbmMuchPoint array
   TClonesArray* fTofPoints; // CbmTofPoint array

   // Stores created CbmLitMCTrack objects.
   // std::map<MC track index, CbmLitMCTrack object>.
   std::map<int, CbmLitMCTrack> fLitMCTracks;
};

#endif /* CBMLITMCTRACKCREATOR_H_ */
