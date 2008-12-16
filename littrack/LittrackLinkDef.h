#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//ana
#pragma link C++ class  CbmLitPropAna+;
#pragma link C++ class  CbmLitElossAna+;
#pragma link C++ class  CbmLitRobustAna+;

// base
#pragma link C++ class  CbmLitTool+;
#pragma link C++ class  CbmLitPDG+;
#pragma link C++ class  CbmLitDetectorLayout+;
#pragma link C++ class  CbmLitStation+;
#pragma link C++ class  CbmLitStationGroup+;
#pragma link C++ class  CbmLitSubstation+;
#pragma link C++ class  CbmLitEnvironment+;
#pragma link C++ class  CbmLitToolFactory+;
#pragma link C++ class  CbmLitHitData+;

//cbm
#pragma link C++ class  CbmPropagatorGeane+;
#pragma link C++ class  CbmPropagatorLit+;

// data
#pragma link C++ class  CbmLitHit+;
#pragma link C++ class  CbmLitPixelHit+;
#pragma link C++ class  CbmLitStripHit+;
#pragma link C++ class  CbmLitTrackParam+;
#pragma link C++ class  CbmLitTrack+;
#pragma link C++ class  CbmLitFitNode+;
   
// finder
#pragma link C++ class  CbmLitTrackFinderRobust+;
#pragma link C++ class  CbmLitTrdTrackFinderSts+;
#pragma link C++ class  CbmLitTrdTrackFinderS+;
#pragma link C++ class  CbmLitMuchTrackFinderBranch+;
#pragma link C++ class  CbmLitMuchTrackFinderRobust+;
#pragma link C++ class  CbmLitTrdTrackFinderRobust+;
#pragma link C++ class  CbmLitTrackFinderBase+;
#pragma link C++ class  CbmLitTrackFinderBranch+;
//#pragma link C++ class  CbmLitMuchTrackFinderTrigger+;

//fitter
#pragma link C++ class  CbmLitTrackFitterImp+;
#pragma link C++ class  CbmLitKalmanFilter+;
#pragma link C++ class  CbmLitTrackFitterRobust+;
#pragma link C++ class  CbmLitTrackFitterIter+;

// interface
#pragma link C++ class  CbmLitTrackFitter+;
#pragma link C++ class  CbmLitTrackExtrapolator+;
#pragma link C++ class  CbmLitTrackPropagator+;
#pragma link C++ class  CbmLitTrackUpdate+;
#pragma link C++ class  CbmLitTrackSelection+;
#pragma link C++ class  CbmLitTrackFinder+;
#pragma link C++ class  CbmLitWeightCalculator+;
#pragma link C++ class  CbmLitEffHitCalculator+;
#pragma link C++ class  CbmLitKalmanSmoother+;

// propagation
#pragma link C++ class  CbmLitTrackPropagatorImp+;
#pragma link C++ class  CbmLitRK4TrackExtrapolator+;
#pragma link C++ class  CbmLitLineTrackExtrapolator+;
#pragma link C++ class  CbmLitTrackPropagatorGeane+;
#pragma link C++ class  CbmLitGeoNavigatorImp+;
#pragma link C++ class  CbmLitMaterialInfo+;
#pragma link C++ class CbmLitMaterialEffectsImp+;

//qa
#pragma link C++ class  CbmLitRecQa+;

//robust
#pragma link C++ class  CbmLitEffHitCalculatorImp+;
#pragma link C++ class  CbmLitWeightCalculatorSimple+;
#pragma link C++ class  CbmLitRobustSelection+;

//selection
#pragma link C++ class  CbmLitTrackSelectionA+;
#pragma link C++ class  CbmLitTrackSelectionB+;
#pragma link C++ class  CbmLitTrackSelectionC+;
#pragma link C++ class  CbmLitTrackSelectionD+;
#pragma link C++ class  CbmLitTrackSelectionTrd+;
#pragma link C++ class  CbmLitTrackSelectionEmpty+;
#pragma link C++ class  CbmLitTrackSelectionMomentum+;
#pragma link C++ class  CbmLitTrackSelectionMuch+;
#pragma link C++ class  CbmLitTrackSelectionMuchRobust+;
#pragma link C++ class  CbmLitTrackSelectionChiSq+;

// utils
#pragma link C++ class  CbmLitConverter+;


#endif
