#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//ana
#pragma link C++ class  CbmLitMuchAna+;
#pragma link C++ class  CbmLitElossAna+;

// base
#pragma link C++ class  CbmTool+;
#pragma link C++ class  CbmPropertyType+;
#pragma link C++ class  CbmProperty<int>+;
#pragma link C++ class  CbmProperty<double>+;
#pragma link C++ class  CbmProperty<bool>+;
#pragma link C++ class  CbmPropertyMgr+;

// data
#pragma link C++ class  CbmLitHit+;
#pragma link C++ class  CbmLitTrackParam+;
#pragma link C++ class  CbmLitTrack+;

// finder
#pragma link C++ class  CbmLitTrackFinderImp+;
#pragma link C++ class  CbmLitTrackFinderBranch+;
#pragma link C++ class  CbmLitTrackFinderRobust+;
#pragma link C++ class  CbmLitTrdTrackFinderSts+;
#pragma link C++ class  CbmLitTrdTrackFinderS+;
#pragma link C++ class  CbmLitMuchTrackFinderBranch+;
#pragma link C++ class  CbmLitMuchTrackFinderRobust+;
//#pragma link C++ class  CbmLitMuchTrackFinderTrigger+;

//fitter
#pragma link C++ class  CbmLitTrackFitterImp+;
#pragma link C++ class  CbmLitKalmanFilter+;
#pragma link C++ class  CbmLitTrackFitterRobust+;

// interface
#pragma link C++ class  CbmLitTrackFitter+;
#pragma link C++ class  CbmLitTrackExtrapolator+;
#pragma link C++ class  CbmLitTrackPropagator+;
#pragma link C++ class  CbmLitTrackUpdate+;
#pragma link C++ class  CbmLitTrackSelection+;
#pragma link C++ class  CbmLitTrackFinder+;
#pragma link C++ class  CbmLitWeightCalculator+;
#pragma link C++ class  CbmLitEffHitCalculator+;

// propagation
#pragma link C++ class  CbmLitTrackPropagatorImp+;
#pragma link C++ class  CbmLitRK4TrackExtrapolator+;
#pragma link C++ class  CbmLitLineTrackExtrapolator+;
#pragma link C++ class  CbmLitParabolicTrackExtrapolator+;
#pragma link C++ class  CbmTrackPropagatorGeane+;

//qa
#pragma link C++ class  CbmLitMuchRecQa+;
#pragma link C++ class  CbmLitTrdRecQa+;

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

// utils
#pragma link C++ class  CbmLitConverter+;

// 
#pragma link C++ class  CbmLitMaterial+;
#pragma link C++ class  CbmLitCircleMaterial+;
#pragma link C++ class  CbmLitRectMaterial+;
#pragma link C++ class  CbmLitEnvironment+;
#pragma link C++ class  CbmLitToolFactory+;
#pragma link C++ class  CbmLitDetectorLayout+;


#endif
