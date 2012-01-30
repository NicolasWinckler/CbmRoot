/** CbmLitTrackFitterWeight.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2008
 *
 * Class implements iterative Kalman filter track fit
 * with calculation of weights and simulated annealing procedure.
 **/

#ifndef CBMLITTRACKFITTERROBUST_H_
#define CBMLITTRACKFITTERROBUST_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackFitter.h"

class CbmLitTrackParam;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitHit;
class CbmLitTrack;

class CbmLitTrackFitterWeight : public CbmLitTrackFitter
{
public:
   /* Constructor
    *@param propagator Track propagation tool
    *@param update Track update tool */
   CbmLitTrackFitterWeight(
      TrackFitterPtr fitter,
      TrackFitterPtr smoother);

   /* Destructor */
   virtual ~CbmLitTrackFitterWeight();

   /* inherited from CbmLitTrackFitter */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = true);

private:
   LitStatus CreateEffectiveTrack(
      CbmLitTrack* track,
      int iter,
      CbmLitTrack* etrack) const;

   LitStatus CreateEffectiveHit(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      const CbmLitTrackParam* par,
      int iter,
      CbmLitHit* hit) const;

   LitStatus CalculateWeights(
      const CbmLitTrackParam* par,
      HitPtrIterator itBegin,
      HitPtrIterator itEnd,
      int iter) const;

   bool AreAllOutliers(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd) const;

   bool MarkOutliers(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd) const;

   LitStatus CheckEffectiveTrack(
      const CbmLitTrack* track) const;

   LitStatus CreateOutputTrack(
      CbmLitTrack* track);

   TrackFitterPtr fFitter;
   TrackFitterPtr fSmoother;

   WeightedHitCalculatorPtr fWeightedHitCalculator;
   WeightCalculatorPtr fSimpleWeightCalculator;
// WeightCalculatorPtr fGaussWeightCalculator;
   WeightCalculatorPtr fTukeyWeightCalculator;

   int fNofIterations;
   std::vector<litfloat> fAnnealing;
   litfloat fOutlierCut;
};

#endif /*CBMLITTRACKFITTERROBUST_H_*/
