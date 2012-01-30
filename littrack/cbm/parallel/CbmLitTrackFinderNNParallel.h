/**
 * \file CbmLitTrackFinderNNParallel.h
 * \brief Implementation of the CbmLitTrackFinder interface for the parallel nearest neighbor tracking.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef CBMLITTRACKFINDERNNPARALLEL_H_
#define CBMLITTRACKFINDERNNPARALLEL_H_

#include "std/interface/CbmLitTrackFinder.h"

#include "parallel/LitHit.h"
#include "parallel/LitTrack.h"
#include "parallel/muon/LitTrackFinderNNVecMuon.h"
#include "parallel/electron/LitTrackFinderNNVecElectron.h"

#include <vector>

/**
 * \class CbmLitTrackFinderNNParallel
 * \brief Implementation of the CbmLitTrackFinder interface for the parallel nearest neighbor tracking.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitTrackFinderNNParallel : public CbmLitTrackFinder
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackFinderNNParallel(
         const std::string& trackingType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackFinderNNParallel();

   /**
    * \brief Inherited from CbmLitTrackFinder.
    */
   virtual LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks);

private:
   void ConvertHits(
      HitPtrVector& hits,
      std::vector<lit::parallel::LitScalPixelHit*>& lhits);

   void ConvertSeeds(
      TrackPtrVector& seeds,
      std::vector<lit::parallel::LitScalTrack*>& lseeds);

   void ConvertTracks(
      const std::vector<lit::parallel::LitScalTrack*>& ltracks,
      TrackPtrVector& tracks);

   // Parallel NN track finder for muon setup.
   lit::parallel::LitTrackFinderNNVecMuon* fTFParallelMuon;
   // Parallel NN track finder for electron setup.
   lit::parallel::LitTrackFinderNNVecElectron* fTFVecElectron;

   // For time calculation
   double fTime;
   // Event number
   int fEventNo;
   // Type of the parallel tracking to be used
   std::string fTrackingType;
};

#endif /* CBMLITTRACKFINDERNNPARALLEL_H_ */
