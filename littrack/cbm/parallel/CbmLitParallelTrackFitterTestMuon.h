/**
 * \file CbmLitParallelTrackFitterTestMuon.h
 * \brief Parallel track fitter used to test propagation and track fit.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#ifndef CBMLITPARALLELTRACKFITTERTESTMUON_H_
#define CBMLITPARALLELTRACKFITTERTESTMUON_H_

#include "std/interface/CbmLitTrackFitter.h"
#include "std/data/CbmLitPixelHit.h"
#include "parallel/LitHit.h"
#include "parallel/muon/LitDetectorLayoutMuon.h"

class CbmLitTrack;
class CbmLitTrackParam;

/**
 * \class CbmLitParallelTrackFitterTestMuon
 * \brief Parallel track fitter used to test propagation and track fit.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
class CbmLitParallelTrackFitterTestMuon : public CbmLitTrackFitter
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitParallelTrackFitterTestMuon();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitParallelTrackFitterTestMuon();

   /**
    * \brief Inherited from CbmLitTrackFitter.
    */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = true);

private:

   unsigned char PlaneId(
      unsigned char stationGroup,
      unsigned char station,
      unsigned char substation,
      const lit::parallel::LitDetectorLayoutMuon<fscal>& layout) const;

   bool CheckHit(
      unsigned char stationGroup,
      unsigned char station,
      unsigned char substation,
      const lit::parallel::LitDetectorLayoutMuon<fscal>& layout,
      CbmLitTrack* track) const;

   void SerialHitToParallel(
      const CbmLitPixelHit& hit,
      lit::parallel::LitPixelHit<fscal>& lhit) const;

   lit::parallel::LitDetectorLayoutMuon<fscal> fLayout;
};

#endif /* CBMLITPARALLELTRACKFITTERTESTMUON_H_ */
