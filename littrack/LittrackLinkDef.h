#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//ana
#pragma link C++ class  CbmLitPropagationAnalysis+;
#pragma link C++ class  CbmLitElossAna+;
#pragma link C++ class  CbmLitRobustFitterAnalysis+;

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
#pragma link C++ class  CbmLitTrackBranch+;
#pragma link C++ class  CbmLitFitNode+;

// finder
#pragma link C++ class  CbmLitTrackFinderWeight+;
#pragma link C++ class  CbmLitTrdTrackFinderBranch+;
#pragma link C++ class  CbmLitTrdTrackFinderStandalone+;
#pragma link C++ class  CbmLitMuchTrackFinderBranch+;
#pragma link C++ class  CbmLitMuchTrackFinderWeight+;
#pragma link C++ class  CbmLitTrdTrackFinderWeight+;
#pragma link C++ class  CbmLitTrackFinderBase+;
#pragma link C++ class  CbmLitTrackFinderBranch+;
#pragma link C++ class  CbmLitStsBasedTrackFinder+;
#pragma link C++ class  CbmLitTrackFinderNN+;
#pragma link C++ class  CbmLitMuchTrackFinderNN+;
#pragma link C++ class  CbmLitTrdTrackFinderNN+;
//#pragma link C++ class  CbmLitMuchTrackFinderTrigger+;

//fitter
#pragma link C++ class  CbmLitTrackFitterImp+;
#pragma link C++ class  CbmLitKalmanFilter+;
#pragma link C++ class  CbmLitTrackFitterRobust+;
#pragma link C++ class  CbmLitTrackFitterIter+;
#pragma link C++ class  CbmLitKalmanSmoother+;

// interface
#pragma link C++ class  CbmLitMaterialEffects+;
#pragma link C++ class  CbmLitGeoNavigator+;
#pragma link C++ class  CbmLitTrackFitter+;
#pragma link C++ class  CbmLitTrackExtrapolator+;
#pragma link C++ class  CbmLitTrackPropagator+;
#pragma link C++ class  CbmLitTrackUpdate+;
#pragma link C++ class  CbmLitTrackSelection+;
#pragma link C++ class  CbmLitTrackFinder+;
#pragma link C++ class  CbmLitWeightCalculator+;
#pragma link C++ class  CbmLitWeightedHitCalculator+;

// propagation
#pragma link C++ class  CbmLitTrackPropagatorImp+;
#pragma link C++ class  CbmLitRK4TrackExtrapolator+;
#pragma link C++ class  CbmLitLineTrackExtrapolator+;
#pragma link C++ class  CbmLitTrackPropagatorGeane+;
#pragma link C++ class  CbmLitGeoNavigatorImp+;
#pragma link C++ class  CbmLitMaterialInfo+;
#pragma link C++ class  CbmLitMaterialEffectsImp+;
#pragma link C++ class  CbmLitCleverTrackExtrapolator+;

//qa
#pragma link C++ class  CbmLitRecQa+;

//robust
#pragma link C++ class  CbmLitWeightedHitCalculatorImp+;
#pragma link C++ class  CbmLitWeightCalculatorSimple+;
#pragma link C++ class  CbmLitWeightCalculatorGauss+;

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
