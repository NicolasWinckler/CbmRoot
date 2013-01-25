#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class CbmStsSimulationQa+;

#pragma link C++ class CbmLitPolarizedGenerator+;

#pragma link C++ class CbmLitGlobalElectronId+;
#pragma link C++ class CbmLitFindGlobalTracks+;
#pragma link C++ class CbmLitFindMvdTracks+;
#pragma link C++ class CbmLitFindGlobalTracksIdeal+;

#pragma link C++ class CbmLitDetContFact;
#pragma link C++ class CbmLitDetGeo;
#pragma link C++ class CbmLitDetGeoPar;
#pragma link C++ class CbmLitDet+;
#pragma link C++ class CbmLitDetPoint+;

#ifdef BOOST_FOUND
#pragma link C++ class CbmLitTrackingQa+;
#pragma link C++ class CbmLitClusteringQa+;
#pragma link C++ class CbmLitFieldQa+;
#pragma link C++ class CbmLitFieldApproximationQa+;
#pragma link C++ class CbmLitFitQa+;
#pragma link C++ class CbmLitRadLengthQa+;
#endif

#pragma link C++ class CbmLitPropagationQa+;
#pragma link C++ class CbmLitTrackingGeoQa+;
#pragma link C++ class CbmLitCheckEnergyLossMuons+;
#pragma link C++ class CbmLitCheckMultipleScattering+;
#pragma link C++ class CbmLitCheckBrem+;
#pragma link C++ class CbmLitCreateStandaloneData+;
#pragma link C++ class CbmLitTestMatrixMath+;

#pragma link C++ class CbmLitTrackingQaReport+;
#pragma link C++ class CbmLitTrackingQaStudyReport+;
#pragma link C++ class CbmLitClusteringQaReport+;
#pragma link C++ class CbmLitClusteringQaStudyReport+;
#pragma link C++ class CbmLitFitQaReport+;
#pragma link C++ class CbmLitFieldQaReport+;
#pragma link C++ class CbmLitFieldApproximationQaReport+;
#pragma link C++ class CbmLitRadLengthQaReport+;

#endif
