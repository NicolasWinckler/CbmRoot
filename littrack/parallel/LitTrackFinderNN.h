/**
 * \file LitTrackFinderNN.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Parallel implementation of the nearest neighbor tracking algorithm.
 */

#ifndef LITTRACKFINDERNN_H_
#define LITTRACKFINDERNN_H_

#include "LitDetectorLayout.h"
#include <vector>

using std::vector;

#include "LitScalPixelHit.h"
#include "LitScalTrack.h"

namespace lit {
namespace parallel {

class LitTrackFinderNN {
public:
   /**
    * \brief Constructor.
    */
   LitTrackFinderNN();

   /**
    * \brief Destructor.
    */
   virtual ~LitTrackFinderNN();

   /**
    * \brief Main function for track reconstruction.
    * \param[in] Array of hits.
    * \param[in] Array of track seeds.
    * \param[out] Output array of reconstructed tracks.
    */
   void DoFind(
      const vector<lit::parallel::LitScalPixelHit*>& hits,
      const vector<lit::parallel::LitScalTrack*>& trackSeeds,
      vector<lit::parallel::LitScalTrack*>& tracks);

   /*
    * \brief Set detector layout.
    * \param[in] layout Detector layout to be set.
    */
   void SetDetectorLayout(
         const LitDetectorLayoutVec& layout) {
   }
};

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDERNN_H_ */
