#ifndef CBMLITPTRTYPES_H_
#define CBMLITPTRTYPES_H_

#include "CbmLitTrackExtrapolator.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackSelection.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitWeightedHitCalculator.h"
#include "CbmLitWeightCalculator.h"
#include "CbmLitGeoNavigator.h"
#include "CbmLitMaterialEffects.h"
#include "CbmLitTrackFinder.h"
#include "CbmLitHitToTrackMerger.h"

#ifndef __CINT__
#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<CbmLitTrackExtrapolator> TrackExtrapolatorPtr;
typedef boost::shared_ptr<CbmLitTrackPropagator> TrackPropagatorPtr;
typedef boost::shared_ptr<CbmLitTrackFitter> TrackFitterPtr;
typedef boost::shared_ptr<CbmLitTrackSelection> TrackSelectionPtr;
typedef boost::shared_ptr<CbmLitTrackUpdate> TrackUpdatePtr;
typedef boost::shared_ptr<CbmLitWeightedHitCalculator> WeightedHitCalculatorPtr;
typedef boost::shared_ptr<CbmLitWeightCalculator> WeightCalculatorPtr;
typedef boost::shared_ptr<CbmLitGeoNavigator> GeoNavigatorPtr;
typedef boost::shared_ptr<CbmLitMaterialEffects> MaterialEffectsPtr;
typedef boost::shared_ptr<CbmLitTrackFinder> TrackFinderPtr;
typedef boost::shared_ptr<CbmLitHitToTrackMerger> HitToTrackMergerPtr;
#else
template <class T> class shared_ptr;
typedef shared_ptr<CbmLitTrackExtrapolator> TrackExtrapolatorPtr;
typedef shared_ptr<CbmLitTrackPropagator> TrackPropagatorPtr;
typedef shared_ptr<CbmLitTrackFitter> TrackFitterPtr;
typedef shared_ptr<CbmLitTrackSelection> TrackSelectionPtr;
typedef shared_ptr<CbmLitTrackUpdate> TrackUpdatePtr;
typedef shared_ptr<CbmLitWeightedHitCalculator> WeightedHitCalculatorPtr;
typedef shared_ptr<CbmLitWeightCalculator> WeightCalculatorPtr;
typedef shared_ptr<CbmLitGeoNavigator> GeoNavigatorPtr;
typedef shared_ptr<CbmLitMaterialEffects> MaterialEffectsPtr;
typedef shared_ptr<CbmLitTrackFinder> TrackFinderPtr;
typedef shared_ptr<CbmLitHitToTrackMerger> HitToTrackMergerPtr;
#endif

#endif
