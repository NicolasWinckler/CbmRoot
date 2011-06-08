// $Id: EcalLinkDef.h,v 1.12 2006/09/18 07:58:04 prokudin Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
 
#pragma link C++ class  CbmEcal;
#pragma link C++ class  CbmEcalDetailed;
#pragma link C++ class  CbmEcalCell+;
#pragma link C++ class  CbmEcalModule+;
#pragma link C++ class  CbmEcalStructure+;
#pragma link C++ class  CbmEcalInf+;
#pragma link C++ class  CbmGeoEcalPar;
#pragma link C++ class  CbmEcalContFact;
#pragma link C++ class  CbmEcalDrawer+;
#pragma link C++ class  CbmEcalDrawer2;
#pragma link C++ class  CbmEcalAnalysis+;
#pragma link C++ class  CbmEcalAnalysisDS+;
#pragma link C++ class  CbmEcalAnalysisCluster;
#pragma link C++ class  CbmEcalHitProducer+;
#pragma link C++ class  CbmEcalHitProducerFastMC+;
#pragma link C++ class  CbmGeoEcal+;
#pragma link C++ class  CbmEcalRecParticle+;
#pragma link C++ class  CbmEcalIdParticle+;
#pragma link C++ class  CbmEcalReconstructionFastMC+;
#pragma link C++ class  CbmEcalClusterFinder+;
#pragma link C++ class  CbmEcalDrawer+;
#pragma link C++ class  CbmEcalPhotonCalibrator+;
#pragma link C++ class  CbmEcalShowerCalibrator+;
#pragma link C++ class  CbmEcalOShowerCalibrator+;
#pragma link C++ class  CbmEcalUrqmdCalibrator+;
#pragma link C++ class  CbmEcalSCurveLib;
#pragma link C++ class  CbmEcalSCurveLibRecord;
#pragma link C++ class  CbmEcalShowerLib;
#pragma link C++ class  CbmEcalTracking;
#pragma link C++ class  CbmEcalTrackExtrapolation;
#pragma link C++ class  CbmEcalStructureFiller;
#pragma link C++ class  CbmEcalTrackExport;
#pragma link C++ class  CbmEcalStructureExport;
#pragma link C++ class  CbmEcalRecoSimple;
#pragma link C++ class  CbmEcalRecoSlow;
#pragma link C++ class  CbmEcalClusterV1+;
#pragma link C++ class  CbmEcalClusterFinderV1;
#pragma link C++ class  CbmEcalClusterFinderV2;
#pragma link C++ class  CbmEcalClusterFinderV3;
#pragma link C++ class  CbmEcalTrackImport;
#pragma link C++ class  CbmEcalCalibration;
#pragma link C++ class  CbmEcalCalibrationV2;
#pragma link C++ class  CbmEcalParam+;
#pragma link C++ class  CbmEcalMatching;
#pragma link C++ class  CbmEcalAnalysisPi0;
#pragma link C++ class  CbmEcalAnalysisPair;
#pragma link C++ class  CbmEcalAnalysisNeutron;
#pragma link C++ class  CbmEcalAnalysisMaterial;
#pragma link C++ class  CbmEcalTimeDigitizer;
#pragma link C++ class  CbmEcalIdentification;
#pragma link C++ class  CbmEcalIdentification2;
#pragma link C++ class  CbmEcalLightMap;
#pragma link C++ class  CbmEcalAnalysisUniformity;
#pragma link C++ class  CbmEcalAnalysisSimple;
#pragma link C++ class  CbmEcalAnalysisShape;
#pragma link C++ class  CbmEcalAnalysisJPsi;
#pragma link C++ class  CbmEcalAnalysisDst;
#pragma link C++ class  CbmEcalAnalysisBSh;
#pragma link C++ class  CbmEcalAnalysisEres;
#pragma link C++ class  CbmEcalMaximum;
#pragma link C++ class  CbmEcalMaximumLocator;
#pragma link C++ class  CbmEcalAnalysisSShape;
#pragma link C++ class  CbmEcalShLibRecord;
#pragma link C++ class  CbmEcalShLib;
#pragma link C++ class  CbmEcalShLibTable;
#pragma link C++ class  CbmEcalRecoSlow2;

#ifdef WITH_TASIMAGE
  #pragma link C++ class CbmEcalQualityCheck;
#endif
         
#endif
