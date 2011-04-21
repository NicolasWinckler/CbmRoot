/** CbmLitParallelTrackFitterTest.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#ifndef CBMLITPARALLELTRACKFITTERTEST_H_
#define CBMLITPARALLELTRACKFITTERTEST_H_

#include "std/interface/CbmLitTrackFitter.h"
#include "std/data/CbmLitPixelHit.h"
#include "parallel/LitHit.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"

class CbmLitTrack;
class CbmLitTrackParam;

class CbmLitParallelTrackFitterTest : public CbmLitTrackFitter
{
public:
   CbmLitParallelTrackFitterTest();
   virtual ~CbmLitParallelTrackFitterTest();

   virtual LitStatus Initialize();
   virtual LitStatus Finalize();

   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = true);

private:

   unsigned char PlaneId(
      unsigned char stationGroup,
      unsigned char station,
      unsigned char substation,
      lit::parallel::LitDetectorLayoutMuon<fscal>& layout) const;

   bool CheckHit(
      unsigned char stationGroup,
      unsigned char station,
      unsigned char substation,
      lit::parallel::LitDetectorLayoutMuon<fscal>& layout,
      CbmLitTrack* track);

   void SerialHitToParallel(
      const CbmLitPixelHit& hit,
      lit::parallel::LitPixelHit<fscal>& lhit);

   lit::parallel::LitDetectorLayoutMuon<fscal> fLayout;
};

#endif /* CBMLITPARALLELTRACKFITTERTEST_H_ */
