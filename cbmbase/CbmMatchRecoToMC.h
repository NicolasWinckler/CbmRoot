/**
 * \file CbmMatchRecoToMC.h
 * \brief FairTask for matching RECO data to MC.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef CBMMATCHRECOTOMC_H_
#define CBMMATCHRECOTOMC_H_

#include "FairTask.h"
class TClonesArray;

class CbmMatchRecoToMC : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmMatchRecoToMC();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMatchRecoToMC();

   /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
       Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

private:
    /**
     * \brief Read and create data branches.
     */
    void ReadAndCreateDataBranches();

    void MatchClusters(
          const TClonesArray* digiMatches,
          const TClonesArray* clusters,
          TClonesArray* clusterMatches);

    void MatchHitsClustering(
          const TClonesArray* clusterMatches,
          const TClonesArray* hits,
          TClonesArray* hitMatches);

    void MatchHitsSmearing(
          const TClonesArray* points,
          const TClonesArray* hits,
          TClonesArray* hitMatches);

    void MatchTracks(
          const TClonesArray* hitMatches,
          const TClonesArray* points,
          const TClonesArray* tracks,
          TClonesArray* trackMatches);

    // Pointers to data arrays
    TClonesArray* fMCTracks;

    // TRD
    TClonesArray* fTrdPoints; // CbmTrdPoint array
    TClonesArray* fTrdDigis; // CbmTrdDigi array
    TClonesArray* fTrdClusters; // CbmTrdCluster array
    TClonesArray* fTrdHits; // CbmTrdHit array
    TClonesArray* fTrdTracks; // CbmTrdTrack array
    TClonesArray* fTrdDigiMatches; // Output CbmMatch array
    TClonesArray* fTrdClusterMatches; // Output CbmMatch array
    TClonesArray* fTrdHitMatches; // Output CbmMatch array
    TClonesArray* fTrdTrackMatches; // Output CbmTrackMatchNew array

    // MUCH
    TClonesArray* fMuchPoints; // CbmMuchPoint array
    TClonesArray* fMuchDigis; // CbmMuchDigi array
    TClonesArray* fMuchClusters; // CbmMuchCluster array
    TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
    TClonesArray* fMuchStrawHits; // CbmMuchStrawHit array
    TClonesArray* fMuchTracks; // CbmMuchTrack array
    TClonesArray* fMuchDigiMatches; // Output CbmMatch array
    TClonesArray* fMuchClusterMatches; // Output CbmMatch array
    TClonesArray* fMuchPixelHitMatches; // Output CbmMatch array
    TClonesArray* fMuchStrawHitMatches; // Output CbmMatch array
    TClonesArray* fMuchTrackMatches; // Output CbmMatch array

    CbmMatchRecoToMC(const CbmMatchRecoToMC&);
    CbmMatchRecoToMC& operator=(const CbmMatchRecoToMC&);

    ClassDef(CbmMatchRecoToMC, 1);
};

#endif /* CBMMATCHRECOTOMC_H_ */
