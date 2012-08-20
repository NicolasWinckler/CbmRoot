/**
 * \file CbmStsSimulationQaCalculator.h
 * \brief Performance calculator for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQACALCULATOR_H_
#define CBMSTSSIMULATIONQACALCULATOR_H_

#include "TObject.h"
class CbmHistManager;
class TClonesArray;

/**
 * \class CbmStsSimulationQaCalculator
 * \brief Performance calculator for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
class CbmStsSimulationQaCalculator
{
public:
   /**
    * \brief Constructor
    */
   CbmStsSimulationQaCalculator(
         CbmHistManager* histManager);

   /**
    * \brief Destructor.
    */
   virtual ~CbmStsSimulationQaCalculator();

   /**
    * \brief Initialization. Has to be called in FairTask::Init.
    */
   void Init();

   /**
    * \brief Execution. Has to be called in FairTask::Exec.
    */
   void Exec();

   /**
    * \brief Finish. Has to be called in FairTask::Finish.
    */
   void Finish();

private:

   /**
    * \brief Test for point in a given station.
    * \param[in] hitPattern Hit pattern of track (bitfield).
    * \param[in] iStation Number of station.
    * \return kTRUE if point in station.
    */
   Bool_t HasPointInStation(Int_t hitPattern, Int_t iStation) {
     return ( hitPattern & ( 1 << iStation ) );
   }

   /**
    * \brief Number of stations with a point.
    * \param hitPattern Hit pattern of track (bitfield).
    * \return Number of stations.
    */
   Int_t NofStationsForTrack(Int_t hitPattern);

   /**
    * \brief Calculate statistics for hits.
    */
   void ProcessHits();

   /**
    *
    */
   void IncreaseCounters();

   Int_t fNofStations;         // Number of stations
   TClonesArray* fMCTracks;    // Array of MCTracks
   TClonesArray* fStsPoints;   // Array of StsPoints

   CbmHistManager* fHM; // Pointer to histogram manager
};

#endif /* CBMLITCLUSTERINGQACALCULATOR_H_ */
