/** CbmLitTrackFinderNNParallel.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 **/

#ifndef CBMLITTRACKFINDERNNPARALLEL_H_
#define CBMLITTRACKFINDERNNPARALLEL_H_

#include "std/interface/CbmLitTrackFinder.h"

#include "parallel/LitHit.h"
#include "parallel/LitTrack.h"
#include "parallel/muon/LitTrackFinderNNVecMuon.h"
#include "parallel/electron/LitTrackFinderNNVecElectron.h"

#include <vector>

//class LitTrackFinderNNVecMuon;
//class LitTrackFinderNNScalarElectron;
//class LitTrackFinderNNVecElectron;
//class LitScalPixelHit;
//class LitScalTrack;

class CbmLitTrackFinderNNParallel : public CbmLitTrackFinder
{
public:
   /* Constructor */
   CbmLitTrackFinderNNParallel(const std::string& trackingType);

   /* Destructor */
   virtual ~CbmLitTrackFinderNNParallel();

   /* Inherited from CbmLitTrackFinder */
   virtual LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks);

   /* Inherited from CbmLitTool */
   virtual LitStatus Initialize();

   /* Inherited from CbmLitTool */
   virtual LitStatus Finalize();

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

   lit::parallel::LitTrackFinderNNVecMuon* fTFParallelMuon;

   lit::parallel::LitTrackFinderNNVecElectron* fTFVecElectron;

   double fTime;

   int fEventNo;

   std::string fTrackingType;
};

#endif /* CBMLITTRACKFINDERNNPARALLEL_H_ */
