/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Author(s):   Gläß / Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - reads some parameters from a configuration file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-21 13:20:11 $
// $Revision: 1.24 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../DataRootObjectLIB/include/tables.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../../LutGeneratorLIB/include/lutGenerator.h"
#include "../../MiscLIB/include/memoryDef.h"
#include "../../AnalysisLIB/include/analysisDef.h"
#include "../include/inf.h"


/*
 * default filename
 */
#define defFileName "CbmHoughStsTrackFinder.ini"

/*
 * true if a default configuration-file should be written.
 */
#define writeDefaultHeader false

/*
 * default values for each parameter
 */
#define detectorGammaOpeningAngle                                0.46764	// atan((planeMaxY + planeMaxY / 100) / planeZ)
#define detectorThetaOpeningAngle                                0.46764	// atan((planeMaxX + planeMaxX / 100) / planeZ)
#define momentumCutSmallerThan                                   1.0		// [GeV]

#define defValInputFileName                                      "./input/*.root"
#define defValInputFileFormat                                    ROOTFILEFORMAT
#define defValInputMinTracks                                     0
#define defValInputMaxTracks                                     0
#define defValInputDetectorMask                                  0
#define defValInputFileNameMagneticField                         "./input/FieldActive.root"
#define defValInputMagneticFieldIsRootFile                       true
#define defValInputMapNameMagneticField                          "NewMap"
#define defValInputMagneticFieldIntegrationStepwidthPerStation   10
#define defValInputMagneticFieldIntegrationFactor                1.0
#define defValInputDisableAutomaticMagneticField                 false
#define defValInputFileNameDetector                              "./geometry/*.dat"
#define defValInputNumberOfVolumesInfrontOfSTS                   10
#define defValInputDisableAutomaticDetector                      false
#define defValInputCodingTableMode                               FILETABLE
#define defValInputGradingPTableMode                             FILETABLE
#define defValInputGradingRTableMode                             FILETABLE
#define defValInputCodingTableFileName                           "./parameters/htrack/codingTable.txt"
#define defValInputGradingPTableFileName                         "./parameters/htrack/gradingPTable.txt"
#define defValInputGradingRTableFileName                         "./parameters/htrack/gradingRTable.txt"
#define defValInputCodingTableWrite                              false
#define defValInputGradingPTableWrite                            false
#define defValInputGradingRTableWrite                            false
#define defValTrackfinderHitProducer                             NONEHITPRODUCER
#define defValTrackfinderReadPointsFromFile                      true
#define defValTrackfinderReadHitsFromFile                        false
#define defValTrackfinderReadMapsHits                            false
#define defValTrackfinderReadHybridHits                          true
#define defValTrackfinderReadStripHits                           true
#define defValTrackfinderLutsVersion                             ANALYTICFORMULALUT
#define defValTrackfinderPrelutFileName                          "./parameters/htrack/prelut.txt"
#define defValTrackfinderLutFileName                             "./parameters/htrack/lut.txt"
#define defValTrackfinderGammaMin                                -detectorGammaOpeningAngle
#define defValTrackfinderGammaMax                                +detectorGammaOpeningAngle
#ifndef CALLOC_HISTOGRAM_RUNTIME
	#define defValTrackfinderGammaStep                           maxDimGamma
#else
	#define defValTrackfinderGammaStep                           191
#endif
#define defValTrackfinderThetaMin                                -detectorThetaOpeningAngle
#define defValTrackfinderThetaMax                                +detectorThetaOpeningAngle
#ifndef CALLOC_HISTOGRAM_RUNTIME
	#define defValTrackfinderThetaStep                           maxDimTheta
#else
	#define defValTrackfinderThetaStep                           383
#endif
#define defValTrackfinderPrelutRadiusMin                         -0.25
#define defValTrackfinderPrelutRadiusMax                         +0.25
#define defValTrackfinderLutRadiusMin                            -(1 / (momentumCutSmallerThan - momentumCutSmallerThan / 10))
#define defValTrackfinderLutRadiusMax                            +(1 / (momentumCutSmallerThan - momentumCutSmallerThan / 10))
#ifndef CALLOC_HISTOGRAM_RUNTIME
	#define defValTrackfinderLutRadiusStep                       maxDimRadius
#else
	#define defValTrackfinderLutRadiusStep                       127
#endif
#define defValTrackfinderMinClassCoding                          1
#define defValTrackfinderMinClassGradingP                        1
#define defValTrackfinderMinClassGradingR                        1
#define defValTrackfinderFirstFilterNeighborhoodDim1ClearRadius  4
#define defValTrackfinderFirstFilterNeighborhoodDim2ClearRadius  1
#define defValTrackfinderSecondFilterNeighborhoodDim1ClearRadius 1
#define defValTrackfinderSecondFilterNeighborhoodDim2ClearRadius 1
#define defValTrackfinderSecondFilterNeighborhoodDim3ClearRadius 1
#define defValTrackfinderWriteTracksToFile                       false
#define defValAnalysisThresholdForP                              momentumCutSmallerThan
#define defValAnalysisInitConfiguration                          false
#define defValAnalysisInitDetector                               false
#define defValAnalysisInitMemory                                 false
#define defValAnalysisInitEvent                                  true
#define defValAnalysisInitClassPriority                          false
#define defValAnalysisInitQualityEFGCEventAbsolute               false
#define defValAnalysisInitQualityEFGCEventRelative               false
#define defValAnalysisInitQualityEFGCTotalAbsolute               false
#define defValAnalysisInitQualityEFGCTotalRelative               true
#define defValAnalysisInitMomentumEFGCEventPzEFGC                false
#define defValAnalysisInitMomentumEFGCEventPtEFGC                false
#define defValAnalysisInitMomentumEFGCTotalPzEFGC                false
#define defValAnalysisInitMomentumEFGCTotalPtEFGC                false
#define defValAnalysisInitProjectionEFGCNEvent12EFGCN            false
#define defValAnalysisInitProjectionEFGCNEvent13EFGCN            false
#define defValAnalysisInitProjectionEFGCNEvent32EFGCN            false
#define defValAnalysisInitProjectionEFGCNTotal12EFGCN            false
#define defValAnalysisInitProjectionEFGCNTotal13EFGCN            false
#define defValAnalysisInitProjectionEFGCNTotal32EFGCN            false
#define defValAnalysisInitMomentumEvent                          false
#define defValAnalysisInitMomentumTotal                          false
#define defValAnalysisInitMomentumDisplay                        false
#define defValAnalysisInitMomentumToRoot                         false
#define defValAnalysisInitProjectionEvent                        false
#define defValAnalysisInitProjectionTotal                        false
#define defValAnalysisInitProjectionDisplay                      false
#define defValAnalysisInitProjectionToRoot                       false
#define defValAnalysisInitMagnetfieldX                           false
#define defValAnalysisInitMagnetfieldY                           false
#define defValAnalysisInitMagnetfieldZ                           false
#define defValAnalysisInitMagnetfieldDisplay                     false
#define defValAnalysisInitMagnetfieldToRoot                      false
#define defValAnalysisInitMagnetfieldConstantForEachEvent        false
#define defValAnalysisInitWeightedMagnetfieldConstant            false
#define defValAnalysisInitMagnetfieldConstantDisplay             false
#define defValAnalysisMagnetfieldConstantDisplayMask             0
#define defValAnalysisInitMagnetfieldConstantToRoot              false
#define defValAnalysisInitMagnetfieldVSConstants                 false
#define defValAnalysisInitPrelutGoodness                         false
#define defValAnalysisInitLutGoodness                            false
#define defValAnalysisInitHoughTransformGoodness                 false
#define defValAnalysisInitQuantizationGoodness                   false
#define defValAnalysisInitCreatedHistogramToRoot                 false
#define defValAnalysisInitEncodedHistogramToRoot                 false
#define defValAnalysisInitFilteredHistogramToRoot                false
#define defValAnalysisInitJustOneCreatedHistogramToRoot          false
#define defValAnalysisInitJustOneEncodedHistogramToRoot          false
#define defValAnalysisInitJustOneFilteredHistogramToRoot         false
#define defValAnalysisInitCreatedHistogramToShow                 false
#define defValAnalysisInitEncodedHistogramToShow                 false
#define defValAnalysisInitFilteredHistogramToShow                false
#define defValAnalysisInitHistogramLayer                         100
#define defValAnalysisInitNumberOfTracksPerColumn                false
#define defValAnalysisInitNumberOfTracksPerLayer                 false
#define defValAnalysisInitTotalAnalysis                          false
#define defValAnalysisInitTotalAnalysisDisplay                   false
#define defValAnalysisInitPercentageOfHitsInSignature            70
#define defValAnalysisInitPercentageOfTracksForSignature         75
#define defValAnalysisInitAnalysisResultWarnings                 (NUMBEROFTRACKSWHICHCANNOTBEFOUND)
#define defValAnalysisInitAnalysisResultDisplays                 (TRACKWITHMOMENTAERRORDISTRIBUTION | TRACKSPERCOLUMNDISTRIBUTION | TRACKSPERLAYERDISTRIBUTION)
#define defValInitStatus                                         true
#define defValAnalysisOutputFileName                      "analysisOutput.root"

/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdInputFileName                                      "inputFileName"
#define stringCmdInputFileFormat                                    "inputFileFormat"
#define stringCmdInputMinTracks                                     "inputMinTracks"
#define stringCmdInputMaxTracks                                     "inputMaxTracks"
#define stringCmdInputDetectorMask                                  "inputDetectorMask"
#define stringCmdInputFileNameMagneticField                         "inputFileNameMagneticField"
#define stringCmdInputMagneticFieldIsRootFile                       "inputMagneticFieldIsRootFile"
#define stringCmdInputMapNameMagneticField                          "inputMapNameMagneticField"
#define stringCmdInputMagneticFieldIntegrationStepwidthPerStation   "inputMagneticFieldIntegrationStepwidthPerStation"
#define stringCmdInputMagneticFieldIntegrationFactor                "inputMagneticFieldIntegrationFactor"
#define stringCmdInputDisableAutomaticMagneticField                 "inputDisableAutomaticMagneticField"
#define stringCmdInputFileNameDetector                              "inputFileNameDetector"
#define stringCmdInputNumberOfVolumesInfrontOfSTS                   "inputNumberOfVolumesInfrontOfSTS"
#define stringCmdInputDisableAutomaticDetector                      "inputDisableAutomaticDetector"
#define stringCmdInputCodingTableMode                               "inputCodingTableMode"
#define stringCmdInputGradingPTableMode                             "inputGradingPTableMode"
#define stringCmdInputGradingRTableMode                             "inputGradingRTableMode"
#define stringCmdInputCodingTableFileName                           "inputCodingTableFileName"
#define stringCmdInputGradingPTableFileName                         "inputGradingPTableFileName"
#define stringCmdInputGradingRTableFileName                         "inputGradingRTableFileName"
#define stringCmdInputCodingTableWrite                              "inputCodingTableWrite"
#define stringCmdInputGradingPTableWrite                            "inputGradingPTableWrite"
#define stringCmdInputGradingRTableWrite                            "inputGradingRTableWrite"
#define stringCmdTrackfinderHitProducer                             "trackfinderHitProducer"
#define stringCmdTrackfinderReadPointsFromFile                      "trackfinderReadPointsFromFile"
#define stringCmdTrackfinderReadHitsFromFile                        "trackfinderReadHitsFromFile"
#define stringCmdTrackfinderReadMapsHits                            "trackfinderReadMapsHits"
#define stringCmdTrackfinderReadHybridHits                          "trackfinderReadHybridHits"
#define stringCmdTrackfinderReadStripHits                           "trackfinderReadStripHits"
#define stringCmdTrackfinderLutsVersion                             "trackfinderLutsVersion"
#define stringCmdTrackfinderPrelutFileName                          "trackfinderPrelutFileName"
#define stringCmdTrackfinderLutFileName                             "trackfinderLutFileName"
#define stringCmdTrackfinderGammaMin                                "trackfinderGammaMin"
#define stringCmdTrackfinderGammaMax                                "trackfinderGammaMax"
#define stringCmdTrackfinderGammaStep                               "trackfinderGammaStep"
#define stringCmdTrackfinderThetaMin                                "trackfinderThetaMin"
#define stringCmdTrackfinderThetaMax                                "trackfinderThetaMax"
#define stringCmdTrackfinderThetaStep                               "trackfinderThetaStep"
#define stringCmdTrackfinderPrelutRadiusMin                         "trackfinderPrelutRadiusMin"
#define stringCmdTrackfinderPrelutRadiusMax                         "trackfinderPrelutRadiusMax"
#define stringCmdTrackfinderLutRadiusMin                            "trackfinderLutRadiusMin"
#define stringCmdTrackfinderLutRadiusMax                            "trackfinderLutRadiusMax"
#define stringCmdTrackfinderLutRadiusStep                           "trackfinderLutRadiusStep"
#define stringCmdTrackfinderMinClassCoding                          "trackfinderMinClassCoding"
#define stringCmdTrackfinderMinClassGradingP                        "trackfinderMinClassGradingP"
#define stringCmdTrackfinderMinClassGradingR                        "trackfinderMinClassGradingR"
#define stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius  "trackfinderFirstFilterNeighborhoodDim1ClearRadius"
#define stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius  "trackfinderFirstFilterNeighborhoodDim2ClearRadius"
#define stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius "trackfinderSecondFilterNeighborhoodDim1ClearRadius"
#define stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius "trackfinderSecondFilterNeighborhoodDim2ClearRadius"
#define stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius "trackfinderSecondFilterNeighborhoodDim3ClearRadius"
#define stringCmdTrackfinderWriteTracksToFile                       "trackfinderWriteTracksToFile"
#define stringCmdAnalysisOutputFileName                             "analysisOutputFileName"
#define stringCmdAnalysisThresholdForP                              "analysisThresholdForP"
#define stringCmdAnalysisInitConfiguration                          "analysisInitConfiguration"
#define stringCmdAnalysisInitDetector                               "analysisInitDetector"
#define stringCmdAnalysisInitMemory                                 "analysisInitMemory"
#define stringCmdAnalysisInitEvent                                  "analysisInitEvent"
#define stringCmdAnalysisInitClassPriority                          "analysisInitClassPriority"
#define stringCmdAnalysisInitQualityEFGCEventAbsolute               "analysisInitQualityEFGCEventAbsolute"
#define stringCmdAnalysisInitQualityEFGCEventRelative               "analysisInitQualityEFGCEventRelative"
#define stringCmdAnalysisInitQualityEFGCTotalAbsolute               "analysisInitQualityEFGCTotalAbsolute"
#define stringCmdAnalysisInitQualityEFGCTotalRelative               "analysisInitQualityEFGCTotalRelative"
#define stringCmdAnalysisInitMomentumEFGCEventPzEFGC                "analysisInitMomentumEFGCEventPzEFGC"
#define stringCmdAnalysisInitMomentumEFGCEventPtEFGC                "analysisInitMomentumEFGCEventPtEFGC"
#define stringCmdAnalysisInitMomentumEFGCTotalPzEFGC                "analysisInitMomentumEFGCTotalPzEFGC"
#define stringCmdAnalysisInitMomentumEFGCTotalPtEFGC                "analysisInitMomentumEFGCTotalPtEFGC"
#define stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN            "analysisInitProjectionEFGCNEvent12EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN            "analysisInitProjectionEFGCNEvent13EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN            "analysisInitProjectionEFGCNEvent32EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN            "analysisInitProjectionEFGCNTotal12EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN            "analysisInitProjectionEFGCNTotal13EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN            "analysisInitProjectionEFGCNTotal32EFGCN"
#define stringCmdAnalysisInitMomentumEvent                          "analysisInitMomentumEvent"
#define stringCmdAnalysisInitMomentumTotal                          "analysisInitMomentumTotal"
#define stringCmdAnalysisInitMomentumDisplay                        "analysisInitMomentumDisplay"
#define stringCmdAnalysisInitMomentumToRoot                         "analysisInitMomentumToRoot"
#define stringCmdAnalysisInitProjectionEvent                        "analysisInitProjectionEvent"
#define stringCmdAnalysisInitProjectionTotal                        "analysisInitProjectionTotal"
#define stringCmdAnalysisInitProjectionDisplay                      "analysisInitProjectionDisplay"
#define stringCmdAnalysisInitProjectionToRoot                       "analysisInitProjectionToRoot"
#define stringCmdAnalysisInitMagnetfieldX                           "analysisInitMagnetfieldX"
#define stringCmdAnalysisInitMagnetfieldY                           "analysisInitMagnetfieldY"
#define stringCmdAnalysisInitMagnetfieldZ                           "analysisInitMagnetfieldZ"
#define stringCmdAnalysisInitMagnetfieldDisplay                     "analysisInitMagnetfieldDisplay"
#define stringCmdAnalysisInitMagnetfieldToRoot                      "analysisInitMagnetfieldToRoot"
#define stringCmdAnalysisInitMagnetfieldConstantForEachEvent        "analysisInitMagnetfieldConstantForEachEvent"
#define stringCmdAnalysisInitWeightedMagnetfieldConstant            "analysisInitWeightedMagnetfieldConstant"
#define stringCmdAnalysisInitMagnetfieldConstantDisplay             "analysisInitMagnetfieldConstantDisplay"
#define stringCmdAnalysisMagnetfieldConstantDisplayMask             "analysisMagnetfieldConstantDisplayMask"
#define stringCmdAnalysisInitMagnetfieldConstantToRoot              "analysisInitMagnetfieldConstantToRoot"
#define stringCmdAnalysisInitMagnetfieldVSConstants                 "analysisInitMagnetfieldVSConstants"
#define stringCmdAnalysisInitPrelutGoodness                         "analysisInitPrelutGoodness"
#define stringCmdAnalysisInitLutGoodness                            "analysisInitLutGoodness"
#define stringCmdAnalysisInitHoughTransformGoodness                 "analysisInitHoughTransformGoodness"
#define stringCmdAnalysisInitQuantizationGoodness                   "analysisInitQuantizationGoodness"
#define stringCmdAnalysisInitCreatedHistogramToRoot                 "analysisInitCreatedHistogramToRoot"
#define stringCmdAnalysisInitEncodedHistogramToRoot                 "analysisInitEncodedHistogramToRoot"
#define stringCmdAnalysisInitFilteredHistogramToRoot                "analysisInitFilteredHistogramToRoot"
#define stringCmdAnalysisInitJustOneCreatedHistogramToRoot          "analysisInitJustOneCreatedHistogramToRoot"
#define stringCmdAnalysisInitJustOneEncodedHistogramToRoot          "analysisInitJustOneEncodedHistogramToRoot"
#define stringCmdAnalysisInitJustOneFilteredHistogramToRoot         "analysisInitJustOneFilteredHistogramToRoot"
#define stringCmdAnalysisInitCreatedHistogramToShow                 "analysisInitCreatedHistogramToShow"
#define stringCmdAnalysisInitEncodedHistogramToShow                 "analysisInitEncodedHistogramToShow"
#define stringCmdAnalysisInitFilteredHistogramToShow                "analysisInitFilteredHistogramToShow"
#define stringCmdAnalysisInitHistogramLayer                         "analysisInitHistogramLayer"
#define stringCmdAnalysisInitNumberOfTracksPerColumn				"analysisInitNumberOfTracksPerColumn"
#define stringCmdAnalysisInitNumberOfTracksPerLayer                 "analysisInitNumberOfTracksPerLayer"
#define stringCmdAnalysisInitTotalAnalysis                          "analysisInitTotalAnalysis"
#define stringCmdAnalysisInitTotalAnalysisDisplay                   "analysisInitTotalAnalysisDisplay"
#define stringCmdAnalysisInitPercentageOfHitsInSignature            "analysisInitPercentageOfHitsInSignature"
#define stringCmdAnalysisInitPercentageOfTracksForSignature         "analysisInitPercentageOfTracksForSignature"
#define stringCmdAnalysisInitAnalysisResultWarnings                 "analysisInitAnalysisResultWarnings"
#define stringCmdAnalysisInitAnalysisResultDisplays                 "analysisInitAnalysisResultDisplays"
#define stringCmdInitStatus                                         "initStatus"

/*
 * index of each parameter for the commandID table
 */
#define idCmdInputFileName                                       0
#define idCmdInputFileFormat                                     1
#define idCmdInputMinTracks                                      2
#define idCmdInputMaxTracks                                      3
#define idCmdInputDetectorMask                                   4
#define idCmdInputFileNameMagneticField                          5
#define idCmdInputMagneticFieldIsRootFile                        6
#define idCmdInputMapNameMagneticField                           7
#define idCmdInputMagneticFieldIntegrationStepwidthPerStation    8
#define idCmdInputMagneticFieldIntegrationFactor                 9
#define idCmdInputDisableAutomaticMagneticField                 10
#define idCmdInputFileNameDetector                              11
#define idCmdInputNumberOfVolumesInfrontOfSTS                   12
#define idCmdInputDisableAutomaticDetector                      13
#define idCmdInputCodingTableMode                               14
#define idCmdInputGradingPTableMode                             15
#define idCmdInputGradingRTableMode                             16
#define idCmdInputCodingTableFileName                           17
#define idCmdInputGradingPTableFileName                         18
#define idCmdInputGradingRTableFileName                         19
#define idCmdInputCodingTableWrite                              20
#define idCmdInputGradingPTableWrite                            21
#define idCmdInputGradingRTableWrite                            22
#define idCmdTrackfinderHitProducer                             23
#define idCmdTrackfinderReadPointsFromFile                      24
#define idCmdTrackfinderReadHitsFromFile                        25
#define idCmdTrackfinderReadMapsHits                            26
#define idCmdTrackfinderReadHybridHits                          27
#define idCmdTrackfinderReadStripHits                           28
#define idCmdTrackfinderLutsVersion                             29
#define idCmdTrackfinderPrelutFileName                          30
#define idCmdTrackfinderLutFileName                             31
#define idCmdTrackfinderGammaMin                                32
#define idCmdTrackfinderGammaMax                                33
#define idCmdTrackfinderGammaStep                               34
#define idCmdTrackfinderThetaMin                                35
#define idCmdTrackfinderThetaMax                                36
#define idCmdTrackfinderThetaStep                               37
#define idCmdTrackfinderPrelutRadiusMin                         38
#define idCmdTrackfinderPrelutRadiusMax                         39
#define idCmdTrackfinderLutRadiusMin                            40
#define idCmdTrackfinderLutRadiusMax                            41
#define idCmdTrackfinderLutRadiusStep                           42
#define idCmdTrackfinderMinClassCoding                          43
#define idCmdTrackfinderMinClassGradingP                        44
#define idCmdTrackfinderMinClassGradingR                        45
#define idCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius  46
#define idCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius  47
#define idCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius 48
#define idCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius 49
#define idCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius 50
#define idCmdTrackfinderWriteTracksToFile                       51
#define idCmdAnalysisOutputFileName                             52
#define idCmdAnalysisThresholdForP                              53
#define idCmdAnalysisInitConfiguration                          54
#define idCmdAnalysisInitDetector                               55
#define idCmdAnalysisInitMemory                                 56
#define idCmdAnalysisInitEvent                                  57
#define idCmdAnalysisInitClassPriority                          58
#define idCmdAnalysisInitQualityEFGCEventAbsolute               59
#define idCmdAnalysisInitQualityEFGCEventRelative               60
#define idCmdAnalysisInitQualityEFGCTotalAbsolute               61
#define idCmdAnalysisInitQualityEFGCTotalRelative               62
#define idCmdAnalysisInitMomentumEFGCEventPzEFGC                63
#define idCmdAnalysisInitMomentumEFGCEventPtEFGC                64
#define idCmdAnalysisInitMomentumEFGCTotalPzEFGC                65
#define idCmdAnalysisInitMomentumEFGCTotalPtEFGC                66
#define idCmdAnalysisInitProjectionEFGCNEvent12EFGCN            67
#define idCmdAnalysisInitProjectionEFGCNEvent13EFGCN            68
#define idCmdAnalysisInitProjectionEFGCNEvent32EFGCN            69
#define idCmdAnalysisInitProjectionEFGCNTotal12EFGCN            70
#define idCmdAnalysisInitProjectionEFGCNTotal13EFGCN            71
#define idCmdAnalysisInitProjectionEFGCNTotal32EFGCN            72
#define idCmdAnalysisInitMomentumEvent                          73
#define idCmdAnalysisInitMomentumTotal                          74
#define idCmdAnalysisInitMomentumDisplay                        75
#define idCmdAnalysisInitMomentumToRoot                         76
#define idCmdAnalysisInitProjectionEvent                        77
#define idCmdAnalysisInitProjectionTotal                        78
#define idCmdAnalysisInitProjectionDisplay                      79
#define idCmdAnalysisInitProjectionToRoot                       80
#define idCmdAnalysisInitMagnetfieldX                           81
#define idCmdAnalysisInitMagnetfieldY                           82
#define idCmdAnalysisInitMagnetfieldZ                           83
#define idCmdAnalysisInitMagnetfieldDisplay                     84
#define idCmdAnalysisInitMagnetfieldToRoot                      85
#define idCmdAnalysisInitMagnetfieldConstantForEachEvent        86
#define idCmdAnalysisInitWeightedMagnetfieldConstant            87
#define idCmdAnalysisInitMagnetfieldConstantDisplay             88
#define idCmdAnalysisMagnetfieldConstantDisplayMask             89
#define idCmdAnalysisInitMagnetfieldConstantToRoot              90
#define idCmdAnalysisInitMagnetfieldVSConstants                 91
#define idCmdAnalysisInitPrelutGoodness                         92
#define idCmdAnalysisInitLutGoodness                            93
#define idCmdAnalysisInitHoughTransformGoodness                 94
#define idCmdAnalysisInitQuantizationGoodness                   95
#define idCmdAnalysisInitCreatedHistogramToRoot                 96
#define idCmdAnalysisInitEncodedHistogramToRoot                 97
#define idCmdAnalysisInitFilteredHistogramToRoot                98
#define idCmdAnalysisInitJustOneCreatedHistogramToRoot          99
#define idCmdAnalysisInitJustOneEncodedHistogramToRoot         100
#define idCmdAnalysisInitJustOneFilteredHistogramToRoot        101
#define idCmdAnalysisInitCreatedHistogramToShow                102
#define idCmdAnalysisInitEncodedHistogramToShow                103
#define idCmdAnalysisInitFilteredHistogramToShow               104
#define idCmdAnalysisInitHistogramLayer                        105
#define idCmdAnalysisInitNumberOfTracksPerColumn               106
#define idCmdAnalysisInitNumberOfTracksPerLayer                107
#define idCmdAnalysisInitTotalAnalysis                         108
#define idCmdAnalysisInitTotalAnalysisDisplay                  109
#define idCmdAnalysisInitPercentageOfHitsInSignature           110
#define idCmdAnalysisInitPercentageOfTracksForSignature        111
#define idCmdAnalysisInitAnalysisResultWarnings                112
#define idCmdAnalysisInitAnalysisResultDisplays                113
#define idCmdInitStatus                                        114


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getInputHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdInputFileName) == 0) {
		if (!(commandID[idCmdInputFileName])) {
			config.inputFileName = value;
			commandID[idCmdInputFileName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputFileFormat) == 0) {
		if (!(commandID[idCmdInputFileFormat])) {
			config.inputFileFormat = stoi((char*)value.c_str(), 10);
			commandID[idCmdInputFileFormat] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMinTracks) == 0) {
		if (!(commandID[idCmdInputMinTracks])) {
			config.inputMinTracks = stoi((char*)value.c_str(), 10);
			commandID[idCmdInputMinTracks] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMaxTracks) == 0) {
		if (!(commandID[idCmdInputMaxTracks])) {
			config.inputMaxTracks = stoi((char*)value.c_str(), 10);
			commandID[idCmdInputMaxTracks] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputDetectorMask) == 0) {
		if (!(commandID[idCmdInputDetectorMask])) {
			config.inputDetectorMask = bitArray(value);
			commandID[idCmdInputDetectorMask] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputFileNameDetector) == 0) {
		if (!(commandID[idCmdInputFileNameDetector])) {
			config.inputFileNameDetector = value;
			commandID[idCmdInputFileNameDetector] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputNumberOfVolumesInfrontOfSTS) == 0) {
		if (!(commandID[idCmdInputNumberOfVolumesInfrontOfSTS])) {
			config.inputNumberOfVolumesInfrontOfSTS = stous((char*)value.c_str(), 10);
			commandID[idCmdInputNumberOfVolumesInfrontOfSTS] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputDisableAutomaticDetector) == 0) {
		if (!(commandID[idCmdInputDisableAutomaticDetector])) {
			config.inputDisableAutomaticDetector = bstob((char*)value.c_str());
			if (!config.inputDisableAutomaticDetector)
				config.inputDisableAutomaticDetector = dstob(*((char*)value.c_str()));
			commandID[idCmdInputDisableAutomaticDetector] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputCodingTableMode) == 0) {
		if (!(commandID[idCmdInputCodingTableMode])) {
			config.inputCodingTableMode = stoi((char*)value.c_str(), 10);
			commandID[idCmdInputCodingTableMode] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingPTableMode) == 0) {
		if (!(commandID[idCmdInputGradingPTableMode])) {
			config.inputGradingPTableMode = stoi((char*)value.c_str(), 10);
			commandID[idCmdInputGradingPTableMode] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingRTableMode) == 0) {
		if (!(commandID[idCmdInputGradingRTableMode])) {
			config.inputGradingRTableMode = stoi((char*)value.c_str(), 10);
			commandID[idCmdInputGradingRTableMode] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputCodingTableFileName) == 0) {
		if (!(commandID[idCmdInputCodingTableFileName])) {
			config.inputCodingTableFileName = value;
			commandID[idCmdInputCodingTableFileName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingPTableFileName) == 0) {
		if (!(commandID[idCmdInputGradingPTableFileName])) {
			config.inputGradingPTableFileName = value;
			commandID[idCmdInputGradingPTableFileName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingRTableFileName) == 0) {
		if (!(commandID[idCmdInputGradingRTableFileName])) {
			config.inputGradingRTableFileName = value;
			commandID[idCmdInputGradingRTableFileName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputCodingTableWrite) == 0) {
		if (!(commandID[idCmdInputCodingTableWrite])) {
			config.inputCodingTableWrite = bstob((char*)value.c_str());
			if (!config.inputCodingTableWrite)
				config.inputCodingTableWrite = dstob(*((char*)value.c_str()));
			commandID[idCmdInputCodingTableWrite] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingPTableWrite) == 0) {
		if (!(commandID[idCmdInputGradingPTableWrite])) {
			config.inputGradingPTableWrite = bstob((char*)value.c_str());
			if (!config.inputGradingPTableWrite)
				config.inputGradingPTableWrite = dstob(*((char*)value.c_str()));
			commandID[idCmdInputGradingPTableWrite] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingRTableWrite) == 0) {
		if (!(commandID[idCmdInputGradingRTableWrite])) {
			config.inputGradingRTableWrite = bstob((char*)value.c_str());
			if (!config.inputGradingRTableWrite)
				config.inputGradingRTableWrite = dstob(*((char*)value.c_str()));
			commandID[idCmdInputGradingRTableWrite] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getTrackfinderHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdTrackfinderHitProducer) == 0) {
		if (!(commandID[idCmdTrackfinderHitProducer])) {
			config.trackfinderHitProducer = stoi((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderHitProducer] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadPointsFromFile) == 0) {
		if (!(commandID[idCmdTrackfinderReadPointsFromFile])) {
			config.trackfinderReadPointsFromFile = bstob((char*)value.c_str());
			if (!config.trackfinderReadPointsFromFile)
				config.trackfinderReadPointsFromFile = dstob(*((char*)value.c_str()));
			commandID[idCmdTrackfinderReadPointsFromFile] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadHitsFromFile) == 0) {
		if (!(commandID[idCmdTrackfinderReadHitsFromFile])) {
			config.trackfinderReadHitsFromFile = bstob((char*)value.c_str());
			if (!config.trackfinderReadHitsFromFile)
				config.trackfinderReadHitsFromFile = dstob(*((char*)value.c_str()));
			commandID[idCmdTrackfinderReadHitsFromFile] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadMapsHits) == 0) {
		if (!(commandID[idCmdTrackfinderReadMapsHits])) {
			config.trackfinderReadMapsHits = bstob((char*)value.c_str());
			if (!config.trackfinderReadMapsHits)
				config.trackfinderReadMapsHits = dstob(*((char*)value.c_str()));
			commandID[idCmdTrackfinderReadMapsHits] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadHybridHits) == 0) {
		if (!(commandID[idCmdTrackfinderReadHybridHits])) {
			config.trackfinderReadHybridHits = bstob((char*)value.c_str());
			if (!config.trackfinderReadHybridHits)
				config.trackfinderReadHybridHits = dstob(*((char*)value.c_str()));
			commandID[idCmdTrackfinderReadHybridHits] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadStripHits) == 0) {
		if (!(commandID[idCmdTrackfinderReadStripHits])) {
			config.trackfinderReadStripHits = bstob((char*)value.c_str());
			if (!config.trackfinderReadStripHits)
				config.trackfinderReadStripHits = dstob(*((char*)value.c_str()));
			commandID[idCmdTrackfinderReadStripHits] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutsVersion) == 0) {
		if (!(commandID[idCmdTrackfinderLutsVersion])) {
			config.trackfinderLutsVersion = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderLutsVersion] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderPrelutFileName) == 0) {
		if (!(commandID[idCmdTrackfinderPrelutFileName])) {
			config.trackfinderPrelutFileName = value;
			commandID[idCmdTrackfinderPrelutFileName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutFileName) == 0) {
		if (!(commandID[idCmdTrackfinderLutFileName])) {
			config.trackfinderLutFileName = value;
			commandID[idCmdTrackfinderLutFileName] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderGammaMin) == 0) {
		if (!(commandID[idCmdTrackfinderGammaMin])) {
			config.trackfinderGammaMin = stod((char*)value.c_str());
			commandID[idCmdTrackfinderGammaMin] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderGammaMax) == 0) {
		if (!(commandID[idCmdTrackfinderGammaMax])) {
			config.trackfinderGammaMax = stod((char*)value.c_str());
			commandID[idCmdTrackfinderGammaMax] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderGammaStep) == 0) {
		if (!(commandID[idCmdTrackfinderGammaStep])) {
			config.trackfinderGammaStep = stoi((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderGammaStep] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderThetaMin) == 0) {
		if (!(commandID[idCmdTrackfinderThetaMin])) {
			config.trackfinderThetaMin = stod((char*)value.c_str());
			commandID[idCmdTrackfinderThetaMin] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderThetaMax) == 0) {
		if (!(commandID[idCmdTrackfinderThetaMax])) {
			config.trackfinderThetaMax = stod((char*)value.c_str());
			commandID[idCmdTrackfinderThetaMax] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderThetaStep) == 0) {
		if (!(commandID[idCmdTrackfinderThetaStep])) {
			config.trackfinderThetaStep = stoi((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderThetaStep] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderPrelutRadiusMin) == 0) {
		if (!(commandID[idCmdTrackfinderPrelutRadiusMin])) {
			config.trackfinderPrelutRadiusMin = stod((char*)value.c_str());
			commandID[idCmdTrackfinderPrelutRadiusMin] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderPrelutRadiusMax) == 0) {
		if (!(commandID[idCmdTrackfinderPrelutRadiusMax])) {
			config.trackfinderPrelutRadiusMax = stod((char*)value.c_str());
			commandID[idCmdTrackfinderPrelutRadiusMax] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutRadiusMin) == 0) {
		if (!(commandID[idCmdTrackfinderLutRadiusMin])) {
			config.trackfinderLutRadiusMin = stod((char*)value.c_str());
			commandID[idCmdTrackfinderLutRadiusMin] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutRadiusMax) == 0) {
		if (!(commandID[idCmdTrackfinderLutRadiusMax])) {
			config.trackfinderLutRadiusMax = stod((char*)value.c_str());
			commandID[idCmdTrackfinderLutRadiusMax] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutRadiusStep) == 0) {
		if (!(commandID[idCmdTrackfinderLutRadiusStep])) {
			config.trackfinderLutRadiusStep = stoi((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderLutRadiusStep] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderMinClassCoding) == 0) {
		if (!(commandID[idCmdTrackfinderMinClassCoding])) {
			config.trackfinderMinClassCoding = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderMinClassCoding] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderMinClassGradingP) == 0) {
		if (!(commandID[idCmdTrackfinderMinClassGradingP])) {
			config.trackfinderMinClassGradingP = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderMinClassGradingP] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderMinClassGradingR) == 0) {
		if (!(commandID[idCmdTrackfinderMinClassGradingR])) {
			config.trackfinderMinClassGradingR = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderMinClassGradingR] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius) == 0) {
		if (!(commandID[idCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius])) {
			config.trackfinderFirstFilterNeighborhoodDim1ClearRadius = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius) == 0) {
		if (!(commandID[idCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius])) {
			config.trackfinderFirstFilterNeighborhoodDim2ClearRadius = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius) == 0) {
		if (!(commandID[idCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius])) {
			config.trackfinderSecondFilterNeighborhoodDim1ClearRadius = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius) == 0) {
		if (!(commandID[idCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius])) {
			config.trackfinderSecondFilterNeighborhoodDim2ClearRadius = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius) == 0) {
		if (!(commandID[idCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius])) {
			config.trackfinderSecondFilterNeighborhoodDim3ClearRadius = stous((char*)value.c_str(), 10);
			commandID[idCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderWriteTracksToFile) == 0) {
		if (!(commandID[idCmdTrackfinderWriteTracksToFile])) {
			config.trackfinderWriteTracksToFile = bstob((char*)value.c_str());
			if (!config.trackfinderWriteTracksToFile)
				config.trackfinderWriteTracksToFile = dstob(*((char*)value.c_str()));
			commandID[idCmdTrackfinderWriteTracksToFile] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getInitialisationHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisThresholdForP) == 0) {
		if (!(commandID[idCmdAnalysisThresholdForP])) {
			config.analysisThresholdForP = stod((char*)value.c_str());
			commandID[idCmdAnalysisThresholdForP] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitConfiguration) == 0) {
		if (!(commandID[idCmdAnalysisInitConfiguration])) {
			config.analysisInitConfiguration = bstob((char*)value.c_str());
			if (!config.analysisInitConfiguration)
				config.analysisInitConfiguration = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitConfiguration] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitDetector) == 0) {
		if (!(commandID[idCmdAnalysisInitDetector])) {
			config.analysisInitDetector = bstob((char*)value.c_str());
			if (!config.analysisInitDetector)
				config.analysisInitDetector = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitDetector] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMemory) == 0) {
		if (!(commandID[idCmdAnalysisInitMemory])) {
			config.analysisInitMemory = bstob((char*)value.c_str());
			if (!config.analysisInitMemory)
				config.analysisInitMemory = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMemory] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitEvent) == 0) {
		if (!(commandID[idCmdAnalysisInitEvent])) {
			config.analysisInitEvent = bstob((char*)value.c_str());
			if (!config.analysisInitEvent)
				config.analysisInitEvent = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitEvent] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitClassPriority) == 0) {
		if (!(commandID[idCmdAnalysisInitClassPriority])) {
			config.analysisInitClassPriority = bstob((char*)value.c_str());
			if (!config.analysisInitClassPriority)
				config.analysisInitClassPriority = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitClassPriority] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInitStatus) == 0) {
		if (!(commandID[idCmdInitStatus])) {
			config.initStatus = bstob((char*)value.c_str());
			if (!config.initStatus)
				config.initStatus = dstob(*((char*)value.c_str()));
			commandID[idCmdInitStatus] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getQualityHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisInitQualityEFGCEventAbsolute) == 0) {
		if (!(commandID[idCmdAnalysisInitQualityEFGCEventAbsolute])) {
			config.analysisInitQualityEFGCEventAbsolute = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCEventAbsolute)
				config.analysisInitQualityEFGCEventAbsolute = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitQualityEFGCEventAbsolute] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQualityEFGCEventRelative) == 0) {
		if (!(commandID[idCmdAnalysisInitQualityEFGCEventRelative])) {
			config.analysisInitQualityEFGCEventRelative = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCEventRelative)
				config.analysisInitQualityEFGCEventRelative = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitQualityEFGCEventRelative] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQualityEFGCTotalAbsolute) == 0) {
		if (!(commandID[idCmdAnalysisInitQualityEFGCTotalAbsolute])) {
			config.analysisInitQualityEFGCTotalAbsolute = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCTotalAbsolute)
				config.analysisInitQualityEFGCTotalAbsolute = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitQualityEFGCTotalAbsolute] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQualityEFGCTotalRelative) == 0) {
		if (!(commandID[idCmdAnalysisInitQualityEFGCTotalRelative])) {
			config.analysisInitQualityEFGCTotalRelative = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCTotalRelative)
				config.analysisInitQualityEFGCTotalRelative = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitQualityEFGCTotalRelative] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getMomentumHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisInitMomentumEFGCEventPzEFGC) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumEFGCEventPzEFGC])) {
			config.analysisInitMomentumEFGCEventPzEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCEventPzEFGC)
				config.analysisInitMomentumEFGCEventPzEFGC = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumEFGCEventPzEFGC] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEFGCEventPtEFGC) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumEFGCEventPtEFGC])) {
			config.analysisInitMomentumEFGCEventPtEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCEventPtEFGC)
				config.analysisInitMomentumEFGCEventPtEFGC = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumEFGCEventPtEFGC] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEFGCTotalPzEFGC) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumEFGCTotalPzEFGC])) {
			config.analysisInitMomentumEFGCTotalPzEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCTotalPzEFGC)
				config.analysisInitMomentumEFGCTotalPzEFGC = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumEFGCTotalPzEFGC] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEFGCTotalPtEFGC) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumEFGCTotalPtEFGC])) {
			config.analysisInitMomentumEFGCTotalPtEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCTotalPtEFGC)
				config.analysisInitMomentumEFGCTotalPtEFGC = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumEFGCTotalPtEFGC] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEvent) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumEvent])) {
			config.analysisInitMomentumEvent = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEvent)
				config.analysisInitMomentumEvent = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumEvent] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumTotal) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumTotal])) {
			config.analysisInitMomentumTotal = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumTotal)
				config.analysisInitMomentumTotal = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumTotal] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumDisplay) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumDisplay])) {
			config.analysisInitMomentumDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumDisplay)
				config.analysisInitMomentumDisplay = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumDisplay] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitMomentumToRoot])) {
			config.analysisInitMomentumToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumToRoot)
				config.analysisInitMomentumToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMomentumToRoot] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getProjectionHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEFGCNEvent12EFGCN])) {
			config.analysisInitProjectionEFGCNEvent12EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNEvent12EFGCN)
				config.analysisInitProjectionEFGCNEvent12EFGCN = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEFGCNEvent12EFGCN] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEFGCNEvent13EFGCN])) {
			config.analysisInitProjectionEFGCNEvent13EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNEvent13EFGCN)
				config.analysisInitProjectionEFGCNEvent13EFGCN = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEFGCNEvent13EFGCN] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEFGCNEvent32EFGCN])) {
			config.analysisInitProjectionEFGCNEvent32EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNEvent32EFGCN)
				config.analysisInitProjectionEFGCNEvent32EFGCN = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEFGCNEvent32EFGCN] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEFGCNTotal12EFGCN])) {
			config.analysisInitProjectionEFGCNTotal12EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNTotal12EFGCN)
				config.analysisInitProjectionEFGCNTotal12EFGCN = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEFGCNTotal12EFGCN] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEFGCNTotal13EFGCN])) {
			config.analysisInitProjectionEFGCNTotal13EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNTotal13EFGCN)
				config.analysisInitProjectionEFGCNTotal13EFGCN = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEFGCNTotal13EFGCN] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEFGCNTotal32EFGCN])) {
			config.analysisInitProjectionEFGCNTotal32EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNTotal32EFGCN)
				config.analysisInitProjectionEFGCNTotal32EFGCN = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEFGCNTotal32EFGCN] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEvent) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionEvent])) {
			config.analysisInitProjectionEvent = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEvent)
				config.analysisInitProjectionEvent = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionEvent] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionTotal) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionTotal])) {
			config.analysisInitProjectionTotal = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionTotal)
				config.analysisInitProjectionTotal = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionTotal] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionDisplay) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionDisplay])) {
			config.analysisInitProjectionDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionDisplay)
				config.analysisInitProjectionDisplay = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionDisplay] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitProjectionToRoot])) {
			config.analysisInitProjectionToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionToRoot)
				config.analysisInitProjectionToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitProjectionToRoot] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getMagnetfieldHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdInputFileNameMagneticField) == 0) {
		if (!(commandID[idCmdInputFileNameMagneticField])) {
			config.inputFileNameMagneticField = value;
			commandID[idCmdInputFileNameMagneticField] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMagneticFieldIsRootFile) == 0) {
		if (!(commandID[idCmdInputMagneticFieldIsRootFile])) {
			config.inputMagneticFieldIsRootFile = bstob((char*)value.c_str());
			if (!config.inputMagneticFieldIsRootFile)
				config.inputMagneticFieldIsRootFile = dstob(*((char*)value.c_str()));
			commandID[idCmdInputMagneticFieldIsRootFile] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMapNameMagneticField) == 0) {
		if (!(commandID[idCmdInputMapNameMagneticField])) {
			config.inputMapNameMagneticField = value;
			commandID[idCmdInputMapNameMagneticField] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMagneticFieldIntegrationStepwidthPerStation) == 0) {
		if (!(commandID[idCmdInputMagneticFieldIntegrationStepwidthPerStation])) {
			config.inputMagneticFieldIntegrationStepwidthPerStation = stous((char*)value.c_str(), 10);
			commandID[idCmdInputMagneticFieldIntegrationStepwidthPerStation] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMagneticFieldIntegrationFactor) == 0) {
		if (!(commandID[idCmdInputMagneticFieldIntegrationFactor])) {
			config.inputMagneticFieldIntegrationFactor = stod((char*)value.c_str());
			commandID[idCmdInputMagneticFieldIntegrationFactor] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputDisableAutomaticMagneticField) == 0) {
		if (!(commandID[idCmdInputDisableAutomaticMagneticField])) {
			config.inputDisableAutomaticMagneticField = bstob((char*)value.c_str());
			if (!config.inputDisableAutomaticMagneticField)
				config.inputDisableAutomaticMagneticField = dstob(*((char*)value.c_str()));
			commandID[idCmdInputDisableAutomaticMagneticField] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldX) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldX])) {
			config.analysisInitMagnetfieldX = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldX)
				config.analysisInitMagnetfieldX = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldX] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldY) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldY])) {
			config.analysisInitMagnetfieldY = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldY)
				config.analysisInitMagnetfieldY = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldY] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldZ) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldZ])) {
			config.analysisInitMagnetfieldZ = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldZ)
				config.analysisInitMagnetfieldZ = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldZ] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldDisplay) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldDisplay])) {
			config.analysisInitMagnetfieldDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldDisplay)
				config.analysisInitMagnetfieldDisplay = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldDisplay] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldToRoot])) {
			config.analysisInitMagnetfieldToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldToRoot)
				config.analysisInitMagnetfieldToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldConstantForEachEvent) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldConstantForEachEvent])) {
			config.analysisInitMagnetfieldConstantForEachEvent = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldConstantForEachEvent)
				config.analysisInitMagnetfieldConstantForEachEvent = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldConstantForEachEvent] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitWeightedMagnetfieldConstant) == 0) {
		if (!(commandID[idCmdAnalysisInitWeightedMagnetfieldConstant])) {
			config.analysisInitWeightedMagnetfieldConstant = bstob((char*)value.c_str());
			if (!config.analysisInitWeightedMagnetfieldConstant)
				config.analysisInitWeightedMagnetfieldConstant = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitWeightedMagnetfieldConstant] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldConstantDisplay) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldConstantDisplay])) {
			config.analysisInitMagnetfieldConstantDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldConstantDisplay)
				config.analysisInitMagnetfieldConstantDisplay = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldConstantDisplay] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisMagnetfieldConstantDisplayMask) == 0) {
		if (!(commandID[idCmdAnalysisMagnetfieldConstantDisplayMask])) {
			config.analysisMagnetfieldConstantDisplayMask = bitArray(value);
			commandID[idCmdAnalysisMagnetfieldConstantDisplayMask] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldConstantToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldConstantToRoot])) {
			config.analysisInitMagnetfieldConstantToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldConstantToRoot)
				config.analysisInitMagnetfieldConstantToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldConstantToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldVSConstants) == 0) {
		if (!(commandID[idCmdAnalysisInitMagnetfieldVSConstants])) {
			config.analysisInitMagnetfieldVSConstants = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldVSConstants)
				config.analysisInitMagnetfieldVSConstants = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitMagnetfieldVSConstants] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getGoodnessHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitPrelutGoodness) == 0) {
		if (!(commandID[idCmdAnalysisInitPrelutGoodness])) {
			config.analysisInitPrelutGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitPrelutGoodness)
				config.analysisInitPrelutGoodness = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitPrelutGoodness] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitLutGoodness) == 0) {
		if (!(commandID[idCmdAnalysisInitLutGoodness])) {
			config.analysisInitLutGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitLutGoodness)
				config.analysisInitLutGoodness = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitLutGoodness] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitHoughTransformGoodness) == 0) {
		if (!(commandID[idCmdAnalysisInitHoughTransformGoodness])) {
			config.analysisInitHoughTransformGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitHoughTransformGoodness)
				config.analysisInitHoughTransformGoodness = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitHoughTransformGoodness] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQuantizationGoodness) == 0) {
		if (!(commandID[idCmdAnalysisInitQuantizationGoodness])) {
			config.analysisInitQuantizationGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitQuantizationGoodness)
				config.analysisInitQuantizationGoodness = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitQuantizationGoodness] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPercentageOfHitsInSignature) == 0) {
		if (!(commandID[idCmdAnalysisInitPercentageOfHitsInSignature])) {
			config.analysisInitPercentageOfHitsInSignature = stoul((char*)value.c_str(), 10);
			if (config.analysisInitPercentageOfHitsInSignature > 100)
				config.analysisInitPercentageOfHitsInSignature = 100;
			commandID[idCmdAnalysisInitPercentageOfHitsInSignature] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPercentageOfTracksForSignature) == 0) {
		if (!(commandID[idCmdAnalysisInitPercentageOfTracksForSignature])) {
			config.analysisInitPercentageOfTracksForSignature = stoul((char*)value.c_str(), 10);
			if (config.analysisInitPercentageOfTracksForSignature > 100)
				config.analysisInitPercentageOfTracksForSignature = 100;
			commandID[idCmdAnalysisInitPercentageOfTracksForSignature] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitAnalysisResultWarnings) == 0) {
		if (!(commandID[idCmdAnalysisInitAnalysisResultWarnings])) {
			config.analysisInitAnalysisResultWarnings = stoul((char*)value.c_str(), 10);
			commandID[idCmdAnalysisInitAnalysisResultWarnings] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitAnalysisResultDisplays) == 0) {
		if (!(commandID[idCmdAnalysisInitAnalysisResultDisplays])) {
			config.analysisInitAnalysisResultDisplays = stoul((char*)value.c_str(), 10);
			commandID[idCmdAnalysisInitAnalysisResultDisplays] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getHistogramHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitCreatedHistogramToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitCreatedHistogramToRoot])) {
			config.analysisInitCreatedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitCreatedHistogramToRoot)
				config.analysisInitCreatedHistogramToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitCreatedHistogramToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitEncodedHistogramToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitEncodedHistogramToRoot])) {
			config.analysisInitEncodedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitEncodedHistogramToRoot)
				config.analysisInitEncodedHistogramToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitEncodedHistogramToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitFilteredHistogramToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitFilteredHistogramToRoot])) {
			config.analysisInitFilteredHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitFilteredHistogramToRoot)
				config.analysisInitFilteredHistogramToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitFilteredHistogramToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitJustOneCreatedHistogramToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitJustOneCreatedHistogramToRoot])) {
			config.analysisInitJustOneCreatedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitJustOneCreatedHistogramToRoot)
				config.analysisInitJustOneCreatedHistogramToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitJustOneCreatedHistogramToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitJustOneEncodedHistogramToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitJustOneEncodedHistogramToRoot])) {
			config.analysisInitJustOneEncodedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitJustOneEncodedHistogramToRoot)
				config.analysisInitJustOneEncodedHistogramToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitJustOneEncodedHistogramToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitJustOneFilteredHistogramToRoot) == 0) {
		if (!(commandID[idCmdAnalysisInitJustOneFilteredHistogramToRoot])) {
			config.analysisInitJustOneFilteredHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitJustOneFilteredHistogramToRoot)
				config.analysisInitJustOneFilteredHistogramToRoot = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitJustOneFilteredHistogramToRoot] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitCreatedHistogramToShow) == 0) {
		if (!(commandID[idCmdAnalysisInitCreatedHistogramToShow])) {
			config.analysisInitCreatedHistogramToShow = bstob((char*)value.c_str());
			if (!config.analysisInitCreatedHistogramToShow)
				config.analysisInitCreatedHistogramToShow = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitCreatedHistogramToShow] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitEncodedHistogramToShow) == 0) {
		if (!(commandID[idCmdAnalysisInitEncodedHistogramToShow])) {
			config.analysisInitEncodedHistogramToShow = bstob((char*)value.c_str());
			if (!config.analysisInitEncodedHistogramToShow)
				config.analysisInitEncodedHistogramToShow = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitEncodedHistogramToShow] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitFilteredHistogramToShow) == 0) {
		if (!(commandID[idCmdAnalysisInitFilteredHistogramToShow])) {
			config.analysisInitFilteredHistogramToShow = bstob((char*)value.c_str());
			if (!config.analysisInitFilteredHistogramToShow)
				config.analysisInitFilteredHistogramToShow = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitFilteredHistogramToShow] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitHistogramLayer) == 0) {
		if (!(commandID[idCmdAnalysisInitHistogramLayer])) {
			config.analysisInitHistogramLayer = stous((char*)value.c_str(), 10);
			commandID[idCmdAnalysisInitHistogramLayer] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getHardwareHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitNumberOfTracksPerColumn) == 0) {
		if (!(commandID[idCmdAnalysisInitNumberOfTracksPerColumn])) {
			config.analysisInitNumberOfTracksPerColumn = bstob((char*)value.c_str());
			if (!config.analysisInitNumberOfTracksPerColumn)
				config.analysisInitNumberOfTracksPerColumn = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitNumberOfTracksPerColumn] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitNumberOfTracksPerLayer) == 0) {
		if (!(commandID[idCmdAnalysisInitNumberOfTracksPerLayer])) {
			config.analysisInitNumberOfTracksPerLayer = bstob((char*)value.c_str());
			if (!config.analysisInitNumberOfTracksPerLayer)
				config.analysisInitNumberOfTracksPerLayer = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitNumberOfTracksPerLayer] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getTotalAnalysisHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitTotalAnalysis) == 0) {
		if (!(commandID[idCmdAnalysisInitTotalAnalysis])) {
			config.analysisInitTotalAnalysis = bstob((char*)value.c_str());
			if (!config.analysisInitTotalAnalysis)
				config.analysisInitTotalAnalysis = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitTotalAnalysis] = true;
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTotalAnalysisDisplay) == 0) {
		if (!(commandID[idCmdAnalysisInitTotalAnalysisDisplay])) {
			config.analysisInitTotalAnalysisDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitTotalAnalysisDisplay)
				config.analysisInitTotalAnalysisDisplay = dstob(*((char*)value.c_str()));
			commandID[idCmdAnalysisInitTotalAnalysisDisplay] = true;
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inf::inf() : configuration() {

	for (unsigned int i = 0; i < numberOfCmds; i++)
		commandID[i] = 0;

	init(getDefFileName());

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inf::inf(InfData& setup) : configuration() {

/********************************************************/
/* make code changes for a different configuration here */
	config.inputFileName                                      = setup.inputFileName;
	config.inputFileFormat                                    = setup.inputFileFormat;
	config.inputMinTracks                                     = setup.inputMinTracks;
	config.inputMaxTracks                                     = setup.inputMaxTracks;
	config.inputDetectorMask                                  = setup.inputDetectorMask;
	config.inputFileNameMagneticField                         = setup.inputFileNameMagneticField;
	config.inputMagneticFieldIsRootFile                       = setup.inputMagneticFieldIsRootFile;
	config.inputMapNameMagneticField                          = setup.inputMapNameMagneticField;
	config.inputMagneticFieldIntegrationStepwidthPerStation   = setup.inputMagneticFieldIntegrationStepwidthPerStation;
	config.inputMagneticFieldIntegrationFactor                = setup.inputMagneticFieldIntegrationFactor;
	config.inputDisableAutomaticMagneticField                 = setup.inputDisableAutomaticMagneticField;
	config.inputFileNameDetector                              = setup.inputFileNameDetector;
	config.inputNumberOfVolumesInfrontOfSTS                   = setup.inputNumberOfVolumesInfrontOfSTS;
	config.inputDisableAutomaticDetector                      = setup.inputDisableAutomaticDetector;
	config.inputCodingTableMode                               = setup.inputCodingTableMode;
	config.inputGradingPTableMode                             = setup.inputGradingPTableMode;
	config.inputGradingRTableMode                             = setup.inputGradingRTableMode;
	config.inputCodingTableFileName                           = setup.inputCodingTableFileName;
	config.inputGradingPTableFileName                         = setup.inputGradingPTableFileName;
	config.inputGradingRTableFileName                         = setup.inputGradingRTableFileName;
	config.inputCodingTableWrite                              = setup.inputCodingTableWrite;
	config.inputGradingPTableWrite                            = setup.inputGradingPTableWrite;
	config.inputGradingRTableWrite                            = setup.inputGradingRTableWrite;
	config.trackfinderHitProducer                             = setup.trackfinderHitProducer;
	config.trackfinderReadPointsFromFile                      = setup.trackfinderReadPointsFromFile;
	config.trackfinderReadHitsFromFile                        = setup.trackfinderReadHitsFromFile;
	config.trackfinderReadMapsHits                            = setup.trackfinderReadMapsHits;
	config.trackfinderReadHybridHits                          = setup.trackfinderReadHybridHits;
	config.trackfinderReadStripHits                           = setup.trackfinderReadStripHits;
	config.trackfinderLutsVersion                             = setup.trackfinderLutsVersion;
	config.trackfinderPrelutFileName                          = setup.trackfinderPrelutFileName;
	config.trackfinderLutFileName                             = setup.trackfinderLutFileName;
	config.trackfinderGammaMin                                = setup.trackfinderGammaMin;
	config.trackfinderGammaMax                                = setup.trackfinderGammaMax;
	config.trackfinderGammaStep                               = setup.trackfinderGammaStep;
	config.trackfinderThetaMin                                = setup.trackfinderThetaMin;
	config.trackfinderThetaMax                                = setup.trackfinderThetaMax;
	config.trackfinderThetaStep                               = setup.trackfinderThetaStep;
	config.trackfinderPrelutRadiusMin                         = setup.trackfinderPrelutRadiusMin;
	config.trackfinderPrelutRadiusMax                         = setup.trackfinderPrelutRadiusMax;
	config.trackfinderLutRadiusMin                            = setup.trackfinderLutRadiusMin;
	config.trackfinderLutRadiusMax                            = setup.trackfinderLutRadiusMax;
	config.trackfinderLutRadiusStep                           = setup.trackfinderLutRadiusStep;
	config.trackfinderMinClassCoding                          = setup.trackfinderMinClassCoding;
	config.trackfinderMinClassGradingP                        = setup.trackfinderMinClassGradingP;
	config.trackfinderMinClassGradingR                        = setup.trackfinderMinClassGradingR;
	config.trackfinderFirstFilterNeighborhoodDim1ClearRadius  = setup.trackfinderFirstFilterNeighborhoodDim1ClearRadius;
	config.trackfinderFirstFilterNeighborhoodDim2ClearRadius  = setup.trackfinderFirstFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim1ClearRadius = setup.trackfinderSecondFilterNeighborhoodDim1ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim2ClearRadius = setup.trackfinderSecondFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim3ClearRadius = setup.trackfinderSecondFilterNeighborhoodDim3ClearRadius;
	config.trackfinderWriteTracksToFile                       = setup.trackfinderWriteTracksToFile;
	config.analysisOutputFileName                             = setup.analysisOutputFileName;
	config.analysisThresholdForP                              = setup.analysisThresholdForP;
	config.analysisInitConfiguration                          = setup.analysisInitConfiguration;
	config.analysisInitDetector                               = setup.analysisInitDetector;
	config.analysisInitMemory                                 = setup.analysisInitMemory;
	config.analysisInitEvent                                  = setup.analysisInitEvent;
	config.analysisInitClassPriority                          = setup.analysisInitClassPriority;
	config.analysisInitQualityEFGCEventAbsolute               = setup.analysisInitQualityEFGCEventAbsolute;
	config.analysisInitQualityEFGCEventRelative               = setup.analysisInitQualityEFGCEventRelative;
	config.analysisInitQualityEFGCTotalAbsolute               = setup.analysisInitQualityEFGCTotalAbsolute;
	config.analysisInitQualityEFGCTotalRelative               = setup.analysisInitQualityEFGCTotalRelative;
	config.analysisInitMomentumEFGCEventPzEFGC                = setup.analysisInitMomentumEFGCEventPzEFGC;
	config.analysisInitMomentumEFGCEventPtEFGC                = setup.analysisInitMomentumEFGCEventPtEFGC;
	config.analysisInitMomentumEFGCTotalPzEFGC                = setup.analysisInitMomentumEFGCTotalPzEFGC;
	config.analysisInitMomentumEFGCTotalPtEFGC                = setup.analysisInitMomentumEFGCTotalPtEFGC;
	config.analysisInitProjectionEFGCNEvent12EFGCN            = setup.analysisInitProjectionEFGCNEvent12EFGCN;
	config.analysisInitProjectionEFGCNEvent13EFGCN            = setup.analysisInitProjectionEFGCNEvent13EFGCN;
	config.analysisInitProjectionEFGCNEvent32EFGCN            = setup.analysisInitProjectionEFGCNEvent32EFGCN;
	config.analysisInitProjectionEFGCNTotal12EFGCN            = setup.analysisInitProjectionEFGCNTotal12EFGCN;
	config.analysisInitProjectionEFGCNTotal13EFGCN            = setup.analysisInitProjectionEFGCNTotal13EFGCN;
	config.analysisInitProjectionEFGCNTotal32EFGCN            = setup.analysisInitProjectionEFGCNTotal32EFGCN;
	config.analysisInitMomentumEvent                          = setup.analysisInitMomentumEvent;
	config.analysisInitMomentumTotal                          = setup.analysisInitMomentumTotal;
	config.analysisInitMomentumDisplay                        = setup.analysisInitMomentumDisplay;
	config.analysisInitMomentumToRoot                         = setup.analysisInitMomentumToRoot;
	config.analysisInitProjectionEvent                        = setup.analysisInitProjectionEvent;
	config.analysisInitProjectionTotal                        = setup.analysisInitProjectionTotal;
	config.analysisInitProjectionDisplay                      = setup.analysisInitProjectionDisplay;
	config.analysisInitProjectionToRoot                       = setup.analysisInitProjectionToRoot;
	config.analysisInitMagnetfieldX                           = setup.analysisInitMagnetfieldX;
	config.analysisInitMagnetfieldY                           = setup.analysisInitMagnetfieldY;
	config.analysisInitMagnetfieldZ                           = setup.analysisInitMagnetfieldZ;
	config.analysisInitMagnetfieldDisplay                     = setup.analysisInitMagnetfieldDisplay;
	config.analysisInitMagnetfieldToRoot                      = setup.analysisInitMagnetfieldToRoot;
	config.analysisInitMagnetfieldConstantForEachEvent        = setup.analysisInitMagnetfieldConstantForEachEvent;
	config.analysisInitWeightedMagnetfieldConstant            = setup.analysisInitWeightedMagnetfieldConstant;
	config.analysisInitMagnetfieldConstantDisplay             = setup.analysisInitMagnetfieldConstantDisplay;
	config.analysisMagnetfieldConstantDisplayMask             = setup.analysisMagnetfieldConstantDisplayMask;
	config.analysisInitMagnetfieldConstantToRoot              = setup.analysisInitMagnetfieldConstantToRoot;
	config.analysisInitMagnetfieldVSConstants                 = setup.analysisInitMagnetfieldVSConstants;
	config.analysisInitPrelutGoodness                         = setup.analysisInitPrelutGoodness;
	config.analysisInitLutGoodness                            = setup.analysisInitLutGoodness;
	config.analysisInitHoughTransformGoodness                 = setup.analysisInitHoughTransformGoodness;
	config.analysisInitQuantizationGoodness                   = setup.analysisInitQuantizationGoodness;
	config.analysisInitCreatedHistogramToRoot                 = setup.analysisInitCreatedHistogramToRoot;
	config.analysisInitEncodedHistogramToRoot                 = setup.analysisInitEncodedHistogramToRoot;
	config.analysisInitFilteredHistogramToRoot                = setup.analysisInitFilteredHistogramToRoot;
	config.analysisInitJustOneCreatedHistogramToRoot          = setup.analysisInitJustOneCreatedHistogramToRoot;
	config.analysisInitJustOneEncodedHistogramToRoot          = setup.analysisInitJustOneEncodedHistogramToRoot;
	config.analysisInitJustOneFilteredHistogramToRoot         = setup.analysisInitJustOneFilteredHistogramToRoot;
	config.analysisInitCreatedHistogramToShow                 = setup.analysisInitCreatedHistogramToShow;
	config.analysisInitEncodedHistogramToShow                 = setup.analysisInitEncodedHistogramToShow;
	config.analysisInitFilteredHistogramToShow                = setup.analysisInitFilteredHistogramToShow;
	config.analysisInitHistogramLayer                         = setup.analysisInitHistogramLayer;
	config.analysisInitNumberOfTracksPerColumn                = setup.analysisInitNumberOfTracksPerColumn;
	config.analysisInitNumberOfTracksPerLayer                 = setup.analysisInitNumberOfTracksPerLayer;
	config.analysisInitTotalAnalysis                          = setup.analysisInitTotalAnalysis;
	config.analysisInitTotalAnalysisDisplay                   = setup.analysisInitTotalAnalysisDisplay;
	config.analysisInitPercentageOfHitsInSignature            = setup.analysisInitPercentageOfHitsInSignature;
	if (config.analysisInitPercentageOfHitsInSignature > 100)
		config.analysisInitPercentageOfHitsInSignature = 100;
	config.analysisInitPercentageOfTracksForSignature         = setup.analysisInitPercentageOfTracksForSignature;
	if (config.analysisInitPercentageOfTracksForSignature > 100)
		config.analysisInitPercentageOfTracksForSignature = 100;
	config.analysisInitAnalysisResultWarnings                 = setup.analysisInitAnalysisResultWarnings;
	config.analysisInitAnalysisResultDisplays                 = setup.analysisInitAnalysisResultDisplays;
	config.initStatus                                         = setup.initStatus;
/********************************************************/

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inf::inf(int numberOfTchars, char** tchars) : configuration() {
	
	for (unsigned int i = 0; i < numberOfCmds; i++)
		commandID[i] = 0;

	init(numberOfTchars, tchars);

}
inf::inf(std::string name) : configuration() {
	
	for (unsigned int i = 0; i < numberOfCmds; i++)
		commandID[i] = 0;

	init(name);

}
inf::inf(char* name) : configuration() {
	
	for (unsigned int i = 0; i < numberOfCmds; i++)
		commandID[i] = 0;

	init(name);

}

/****************************************************************
 * destructor													*
 ****************************************************************/

inf::~inf() {

}

/****************************************************************
 * This method returns the number of accepted commands.			*
 ****************************************************************/

int inf::getNumberOfCmds() {

	return numberOfCmds;

}

/****************************************************************
 * This method is to get the default name of the file.			*
 ****************************************************************/

std::string inf::getDefFileName() {

	return defFileName;

}

/****************************************************************
 * This method must return true if a default file should be		*
 * written.														*
 ****************************************************************/

bool inf::getWriteDefaultHeader() {

	return writeDefaultHeader;

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.													*
 ****************************************************************/

void inf::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	config.inputFileName                                      = defValInputFileName;
	config.inputFileFormat                                    = defValInputFileFormat;
	config.inputMinTracks                                     = defValInputMinTracks;
	config.inputMaxTracks                                     = defValInputMaxTracks;
	config.inputDetectorMask                                  = defValInputDetectorMask;
	config.inputFileNameMagneticField                         = defValInputFileNameMagneticField;
	config.inputMagneticFieldIsRootFile                       = defValInputMagneticFieldIsRootFile;
	config.inputMapNameMagneticField                          = defValInputMapNameMagneticField;
	config.inputMagneticFieldIntegrationStepwidthPerStation   = defValInputMagneticFieldIntegrationStepwidthPerStation;
	config.inputMagneticFieldIntegrationFactor                = defValInputMagneticFieldIntegrationFactor;
	config.inputDisableAutomaticMagneticField                 = defValInputDisableAutomaticMagneticField;
	config.inputFileNameDetector                              = defValInputFileNameDetector;
	config.inputNumberOfVolumesInfrontOfSTS                   = defValInputNumberOfVolumesInfrontOfSTS;
	config.inputDisableAutomaticDetector                      = defValInputDisableAutomaticDetector;
	config.inputCodingTableMode                               = defValInputCodingTableMode;
	config.inputGradingPTableMode                             = defValInputGradingPTableMode;
	config.inputGradingRTableMode                             = defValInputGradingRTableMode;
	config.inputCodingTableFileName                           = defValInputCodingTableFileName;
	config.inputGradingPTableFileName                         = defValInputGradingPTableFileName;
	config.inputGradingRTableFileName                         = defValInputGradingRTableFileName;
	config.inputCodingTableWrite                              = defValInputCodingTableWrite;
	config.inputGradingPTableWrite                            = defValInputGradingPTableWrite;
	config.inputGradingRTableWrite                            = defValInputGradingRTableWrite;
	config.trackfinderHitProducer                             = defValTrackfinderHitProducer;
	config.trackfinderReadPointsFromFile                      = defValTrackfinderReadPointsFromFile;
	config.trackfinderReadHitsFromFile                        = defValTrackfinderReadHitsFromFile;
	config.trackfinderReadMapsHits                            = defValTrackfinderReadMapsHits;
	config.trackfinderReadHybridHits                          = defValTrackfinderReadHybridHits;
	config.trackfinderReadStripHits                           = defValTrackfinderReadStripHits;
	config.trackfinderLutsVersion                             = defValTrackfinderLutsVersion;
	config.trackfinderPrelutFileName                          = defValTrackfinderPrelutFileName;
	config.trackfinderLutFileName                             = defValTrackfinderLutFileName;
	config.trackfinderGammaMin                                = defValTrackfinderGammaMin;
	config.trackfinderGammaMax                                = defValTrackfinderGammaMax;
	config.trackfinderGammaStep                               = defValTrackfinderGammaStep;
	config.trackfinderThetaMin                                = defValTrackfinderThetaMin;
	config.trackfinderThetaMax                                = defValTrackfinderThetaMax;
	config.trackfinderThetaStep                               = defValTrackfinderThetaStep;
	config.trackfinderPrelutRadiusMin                         = defValTrackfinderPrelutRadiusMin;
	config.trackfinderPrelutRadiusMax                         = defValTrackfinderPrelutRadiusMax;
	config.trackfinderLutRadiusMin                            = defValTrackfinderLutRadiusMin;
	config.trackfinderLutRadiusMax                            = defValTrackfinderLutRadiusMax;
	config.trackfinderLutRadiusStep                           = defValTrackfinderLutRadiusStep;
	config.trackfinderMinClassCoding                          = defValTrackfinderMinClassCoding;
	config.trackfinderMinClassGradingP                        = defValTrackfinderMinClassGradingP;
	config.trackfinderMinClassGradingR                        = defValTrackfinderMinClassGradingR;
	config.trackfinderFirstFilterNeighborhoodDim1ClearRadius  = defValTrackfinderFirstFilterNeighborhoodDim1ClearRadius;
	config.trackfinderFirstFilterNeighborhoodDim2ClearRadius  = defValTrackfinderFirstFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim1ClearRadius = defValTrackfinderSecondFilterNeighborhoodDim1ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim2ClearRadius = defValTrackfinderSecondFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim3ClearRadius = defValTrackfinderSecondFilterNeighborhoodDim3ClearRadius;
	config.trackfinderWriteTracksToFile                       = defValTrackfinderWriteTracksToFile;
	config.analysisThresholdForP                              = defValAnalysisThresholdForP;
	config.analysisInitConfiguration                          = defValAnalysisInitConfiguration;
	config.analysisInitDetector                               = defValAnalysisInitDetector;
	config.analysisInitMemory                                 = defValAnalysisInitMemory;
	config.analysisInitEvent                                  = defValAnalysisInitEvent;
	config.analysisInitClassPriority                          = defValAnalysisInitClassPriority;
	config.analysisInitQualityEFGCEventAbsolute               = defValAnalysisInitQualityEFGCEventAbsolute;
	config.analysisInitQualityEFGCEventRelative               = defValAnalysisInitQualityEFGCEventRelative;
	config.analysisInitQualityEFGCTotalAbsolute               = defValAnalysisInitQualityEFGCTotalAbsolute;
	config.analysisInitQualityEFGCTotalRelative               = defValAnalysisInitQualityEFGCTotalRelative;
	config.analysisInitMomentumEFGCEventPzEFGC                = defValAnalysisInitMomentumEFGCEventPzEFGC;
	config.analysisInitMomentumEFGCEventPtEFGC                = defValAnalysisInitMomentumEFGCEventPtEFGC;
	config.analysisInitMomentumEFGCTotalPzEFGC                = defValAnalysisInitMomentumEFGCTotalPzEFGC;
	config.analysisInitMomentumEFGCTotalPtEFGC                = defValAnalysisInitMomentumEFGCTotalPtEFGC;
	config.analysisInitProjectionEFGCNEvent12EFGCN            = defValAnalysisInitProjectionEFGCNEvent12EFGCN;
	config.analysisInitProjectionEFGCNEvent13EFGCN            = defValAnalysisInitProjectionEFGCNEvent13EFGCN;
	config.analysisInitProjectionEFGCNEvent32EFGCN            = defValAnalysisInitProjectionEFGCNEvent32EFGCN;
	config.analysisInitProjectionEFGCNTotal12EFGCN            = defValAnalysisInitProjectionEFGCNTotal12EFGCN;
	config.analysisInitProjectionEFGCNTotal13EFGCN            = defValAnalysisInitProjectionEFGCNTotal13EFGCN;
	config.analysisInitProjectionEFGCNTotal32EFGCN            = defValAnalysisInitProjectionEFGCNTotal32EFGCN;
	config.analysisInitMomentumEvent                          = defValAnalysisInitMomentumEvent;
	config.analysisInitMomentumTotal                          = defValAnalysisInitMomentumTotal;
	config.analysisInitMomentumDisplay                        = defValAnalysisInitMomentumDisplay;
	config.analysisInitMomentumToRoot                         = defValAnalysisInitMomentumToRoot;
	config.analysisInitProjectionEvent                        = defValAnalysisInitProjectionEvent;
	config.analysisInitProjectionTotal                        = defValAnalysisInitProjectionTotal;
	config.analysisInitProjectionDisplay                      = defValAnalysisInitProjectionDisplay;
	config.analysisInitProjectionToRoot                       = defValAnalysisInitProjectionToRoot;
	config.analysisInitMagnetfieldX                           = defValAnalysisInitMagnetfieldX;
	config.analysisInitMagnetfieldY                           = defValAnalysisInitMagnetfieldY;
	config.analysisInitMagnetfieldZ                           = defValAnalysisInitMagnetfieldZ;
	config.analysisInitMagnetfieldDisplay                     = defValAnalysisInitMagnetfieldDisplay;
	config.analysisInitMagnetfieldToRoot                      = defValAnalysisInitMagnetfieldToRoot;
	config.analysisInitMagnetfieldConstantForEachEvent        = defValAnalysisInitMagnetfieldConstantForEachEvent;
	config.analysisInitWeightedMagnetfieldConstant            = defValAnalysisInitWeightedMagnetfieldConstant;
	config.analysisInitMagnetfieldConstantDisplay             = defValAnalysisInitMagnetfieldConstantDisplay;
	config.analysisMagnetfieldConstantDisplayMask             = defValAnalysisMagnetfieldConstantDisplayMask;
	config.analysisInitMagnetfieldConstantToRoot              = defValAnalysisInitMagnetfieldConstantToRoot;
	config.analysisInitMagnetfieldVSConstants                 = defValAnalysisInitMagnetfieldVSConstants;
	config.analysisInitPrelutGoodness                         = defValAnalysisInitPrelutGoodness;
	config.analysisInitLutGoodness                            = defValAnalysisInitLutGoodness;
	config.analysisInitHoughTransformGoodness                 = defValAnalysisInitHoughTransformGoodness;
	config.analysisInitQuantizationGoodness                   = defValAnalysisInitQuantizationGoodness;
	config.analysisInitCreatedHistogramToRoot                 = defValAnalysisInitCreatedHistogramToRoot;
	config.analysisInitEncodedHistogramToRoot                 = defValAnalysisInitEncodedHistogramToRoot;
	config.analysisInitFilteredHistogramToRoot                = defValAnalysisInitFilteredHistogramToRoot;
	config.analysisInitJustOneCreatedHistogramToRoot          = defValAnalysisInitJustOneCreatedHistogramToRoot;
	config.analysisInitJustOneEncodedHistogramToRoot          = defValAnalysisInitJustOneEncodedHistogramToRoot;
	config.analysisInitJustOneFilteredHistogramToRoot         = defValAnalysisInitJustOneFilteredHistogramToRoot;
	config.analysisInitCreatedHistogramToShow                 = defValAnalysisInitCreatedHistogramToShow;
	config.analysisInitEncodedHistogramToShow                 = defValAnalysisInitEncodedHistogramToShow;
	config.analysisInitFilteredHistogramToShow                = defValAnalysisInitFilteredHistogramToShow;
	config.analysisInitHistogramLayer                         = defValAnalysisInitHistogramLayer;
	config.analysisInitNumberOfTracksPerColumn                = defValAnalysisInitNumberOfTracksPerColumn;
	config.analysisInitNumberOfTracksPerLayer                 = defValAnalysisInitNumberOfTracksPerLayer;
	config.analysisInitTotalAnalysis                          = defValAnalysisInitTotalAnalysis;
	config.analysisInitTotalAnalysisDisplay                   = defValAnalysisInitTotalAnalysisDisplay;
	config.analysisInitPercentageOfHitsInSignature            = defValAnalysisInitPercentageOfHitsInSignature;
	if (config.analysisInitPercentageOfHitsInSignature > 100)
		config.analysisInitPercentageOfHitsInSignature = 100;
	config.analysisInitPercentageOfTracksForSignature         = defValAnalysisInitPercentageOfTracksForSignature;
	if (config.analysisInitPercentageOfTracksForSignature > 100)
		config.analysisInitPercentageOfTracksForSignature = 100;
	config.analysisInitAnalysisResultWarnings                 = defValAnalysisInitAnalysisResultWarnings;
	config.analysisInitAnalysisResultDisplays                 = defValAnalysisInitAnalysisResultDisplays;
	config.initStatus                                         = defValInitStatus;
	config.analysisOutputFileName                             = defValAnalysisOutputFileName;
/********************************************************/

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.
 ****************************************************************/

bool inf::getHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

/********************************************************/
/* make code changes for a different configuration here */
	if (!specifierFound)
		specifierFound = getInputHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getTrackfinderHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getInitialisationHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getQualityHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getMomentumHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getProjectionHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getMagnetfieldHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getGoodnessHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getHistogramHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getHardwareHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getTotalAnalysisHeaderValue(specifier, value);
	if (!specifierFound) {
		if (specifier.compare(stringCmdAnalysisOutputFileName) == 0) {
			if (!(commandID[idCmdAnalysisOutputFileName])) {
				config.analysisOutputFileName = value;
				commandID[idCmdAnalysisOutputFileName] = true;
				specifierFound = true;
			}
		}
	}
/********************************************************/

	return specifierFound;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void inf::writeFileHeader(std::ofstream& fileStream) {

/********************************************************/
/* make code changes for a different configuration here */
	char buffer[200];
	char conversionBuffer[intConversionDigits + 1];

	writeComment(fileStream, "[PARAMETERS FOR THE INPUT OBJECT]");

	setHeaderValue(fileStream, stringCmdInputFileName,                                      config.inputFileName,                                             "Filename of the file which consists of the input data");

	strcpy(buffer, "Fileformat of the file which consists of the input data -> (");
	itos(ASCIIFILEFORMAT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":ASCII, ");
	itos(ROOTFILEFORMAT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Standalone ROOT)");

	setHeaderValue(fileStream, stringCmdInputFileFormat,                                    config.inputFileFormat,                                           buffer);
	setHeaderValue(fileStream, stringCmdInputMinTracks,                                     config.inputMinTracks,                                            "Number of minimum tracks to be considered in the program");
	setHeaderValue(fileStream, stringCmdInputMaxTracks,                                     config.inputMaxTracks,                                            "Number of maximum tracks to be considered in the program");
	setHeaderValue(fileStream, stringCmdInputDetectorMask,                                  config.inputDetectorMask.toString().c_str(),                      "Number which inhibits corresponding detector stations from computation");
	setHeaderValue(fileStream, stringCmdInputFileNameMagneticField,                         config.inputFileNameMagneticField,                                "Filename of the file which consists of the magnetic field map");
	setHeaderValue(fileStream, stringCmdInputMagneticFieldIsRootFile,                       config.inputMagneticFieldIsRootFile,                              "True if the file which consists of the magnetic field map is a root-file");
	setHeaderValue(fileStream, stringCmdInputMapNameMagneticField,                          config.inputMapNameMagneticField,                                 "Name of the map in the root-file which consists of the magnetic field map");
	setHeaderValue(fileStream, stringCmdInputMagneticFieldIntegrationStepwidthPerStation,   config.inputMagneticFieldIntegrationStepwidthPerStation,          "Number of points which are inserted between each detector station to integrate the magnetic field");
	setHeaderValue(fileStream, stringCmdInputMagneticFieldIntegrationFactor,                config.inputMagneticFieldIntegrationFactor,                       "Factor to correct the integrated magnetic field");
	setHeaderValue(fileStream, stringCmdInputDisableAutomaticMagneticField,                 config.inputDisableAutomaticMagneticField,                        "True if the magnetic field should be used directly from the framework or not");
	setHeaderValue(fileStream, stringCmdInputFileNameDetector,                              config.inputFileNameDetector,                                     "Filename of the file which consists of the detector information");
	setHeaderValue(fileStream, stringCmdInputNumberOfVolumesInfrontOfSTS,                   config.inputNumberOfVolumesInfrontOfSTS,                          "Number of geometric volumes which are established infront of the detector. Needed to compute the correct volumeID");
	setHeaderValue(fileStream, stringCmdInputDisableAutomaticDetector,                      config.inputDisableAutomaticDetector,                             "False if the detector should be used directly from the framework or not");

	strcpy(buffer, "Version of the used tables -> (");
	itos(NOTABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":NOTABLE, ");
	itos(CROSSFOOTTABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":CROSSFOOTTABLE, ");
	itos(LUTGOODNESSTABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":LUTGOODNESSTABLE, ");
	itos(FILETABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FILETABLE, ");
	itos(ONLINETABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":ONLINETABLE)");

	setHeaderValue(fileStream, stringCmdInputCodingTableMode,                               config.inputCodingTableMode,                                      buffer);
	setHeaderValue(fileStream, stringCmdInputGradingPTableMode,                             config.inputGradingPTableMode,                                    buffer);
	setHeaderValue(fileStream, stringCmdInputGradingRTableMode,                             config.inputGradingRTableMode,                                    buffer);
	setHeaderValue(fileStream, stringCmdInputCodingTableFileName,                           config.inputCodingTableFileName,                                  "Name of the file which consists the coding table if enabled");
	setHeaderValue(fileStream, stringCmdInputGradingPTableFileName,                         config.inputGradingPTableFileName,                                "Name of the file which consists the gradingP table if enabled");
	setHeaderValue(fileStream, stringCmdInputGradingRTableFileName,                         config.inputGradingRTableFileName,                                "Name of the file which consists the gradingR table if enabled");
	setHeaderValue(fileStream, stringCmdInputCodingTableWrite,                              config.inputCodingTableWrite,                                     "Initializes that the coding table is written to file");
	setHeaderValue(fileStream, stringCmdInputGradingPTableWrite,                            config.inputGradingPTableWrite,                                   "Initializes that the gradingP table is written to file");
	setHeaderValue(fileStream, stringCmdInputGradingRTableWrite,                            config.inputGradingRTableWrite,                                   "Initializes that the gradingR table is written to file");

	writeComment(fileStream, "");
	writeComment(fileStream, "[PARAMETERS FOR THE TRACKFINDER OBJECT]");

	strcpy(buffer, "Type of the hitProducer -> (");
	itos(NONEHITPRODUCER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":NONE, ");
	itos(BINNINGHITPRODUCER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Binning, ");
	itos(SMEARINGHITPRODUCER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Smearing)");

	setHeaderValue(fileStream, stringCmdTrackfinderHitProducer,                             config.trackfinderHitProducer,                                    buffer);
	setHeaderValue(fileStream, stringCmdTrackfinderReadPointsFromFile,                      config.trackfinderReadPointsFromFile,                             "Just points are read out of the root file");
	setHeaderValue(fileStream, stringCmdTrackfinderReadHitsFromFile,                        config.trackfinderReadHitsFromFile,                               "The hits are read out of the root file");
	setHeaderValue(fileStream, stringCmdTrackfinderReadMapsHits,                            config.trackfinderReadMapsHits,                                   "Maps hits are read");
	setHeaderValue(fileStream, stringCmdTrackfinderReadHybridHits,                          config.trackfinderReadHybridHits,                                 "Hybrid hits are read");
	setHeaderValue(fileStream, stringCmdTrackfinderReadStripHits,                           config.trackfinderReadStripHits,                                  "Strip Hits are read");

	strcpy(buffer, "Version of the used look-up-tables -> (");
	itos(GENERATERUNGEKUTTALUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Generate Runge-Kutta-Lut, ");
	itos(GENERATEANALYTICFORMULALUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Generate Formula-Lut, ");
	itos(FILELUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Use File-Lut, ");
	itos(ANALYTICFORMULALUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Use Formula)");

	setHeaderValue(fileStream, stringCmdTrackfinderLutsVersion,                             config.trackfinderLutsVersion,                                    buffer);
	setHeaderValue(fileStream, stringCmdTrackfinderPrelutFileName,                          config.trackfinderPrelutFileName,                                 "Name of the file which consists the prelut");
	setHeaderValue(fileStream, stringCmdTrackfinderLutFileName,                             config.trackfinderLutFileName,                                    "Name of the file which consists the lut");
	setHeaderValue(fileStream, stringCmdTrackfinderGammaMin,                                config.trackfinderGammaMin,                                       "Minimal gamma angle value for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderGammaMax,                                config.trackfinderGammaMax,                                       "Maximal gamma angle value for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderGammaStep,                               config.trackfinderGammaStep,                                      "Quantization steps for the gamma angle");
	setHeaderValue(fileStream, stringCmdTrackfinderThetaMin,                                config.trackfinderThetaMin,                                       "Minimal theta angle value for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderThetaMax,                                config.trackfinderThetaMax,                                       "Maximal theta angle value for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderThetaStep,                               config.trackfinderThetaStep,                                      "Quantization steps for the theta angle");
	setHeaderValue(fileStream, stringCmdTrackfinderPrelutRadiusMin,                         config.trackfinderPrelutRadiusMin,                                "Minimal PrelutRadius for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderPrelutRadiusMax,                         config.trackfinderPrelutRadiusMax,                                "Maximal PrelutRadius for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderLutRadiusMin,                            config.trackfinderLutRadiusMin,                                   "Minimal LutRadius for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderLutRadiusMax,                            config.trackfinderLutRadiusMax,                                   "Maximal LutRadius for the hough transformation");
	setHeaderValue(fileStream, stringCmdTrackfinderLutRadiusStep,                           config.trackfinderLutRadiusStep,                                  "Quantization steps for the LutRadius");
	setHeaderValue(fileStream, stringCmdTrackfinderMinClassCoding,                          config.trackfinderMinClassCoding,                                 "Minimal allowed classification in the coding table");
	setHeaderValue(fileStream, stringCmdTrackfinderMinClassGradingP,                        config.trackfinderMinClassGradingP,                               "Minimal allowed classification in the gradingP table");
	setHeaderValue(fileStream, stringCmdTrackfinderMinClassGradingR,                        config.trackfinderMinClassGradingR,                               "Minimal allowed classification in the gradingR table");
	setHeaderValue(fileStream, stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius,  config.trackfinderFirstFilterNeighborhoodDim1ClearRadius,         "FirstFilter's Dim1-radius which is cleared around a peak");
	setHeaderValue(fileStream, stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius,  config.trackfinderFirstFilterNeighborhoodDim2ClearRadius,         "FirstFilter's Dim2-radius which is cleared around a peak");
	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius, config.trackfinderSecondFilterNeighborhoodDim1ClearRadius,        "SecondFilter's Dim1-radius which is cleared around a peak");
	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius, config.trackfinderSecondFilterNeighborhoodDim2ClearRadius,        "SecondFilter's Dim2-radius which is cleared around a peak");
	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius, config.trackfinderSecondFilterNeighborhoodDim3ClearRadius,        "SecondFilter's Dim3-radius which is cleared around a peak");
	setHeaderValue(fileStream, stringCmdTrackfinderWriteTracksToFile,                       config.trackfinderWriteTracksToFile,                              "False if the output should be placed directly into the framework interface");

	writeComment(fileStream, "");
	writeComment(fileStream, "[PARAMETERS FOR THE ANALYSIS OBJECT]");

	setHeaderValue(fileStream, stringCmdAnalysisOutputFileName,                             config.analysisOutputFileName,                                    "Filename of the file which consists of the analyis graphic output data");
	setHeaderValue(fileStream, stringCmdAnalysisThresholdForP,                              config.analysisThresholdForP,                                     "Threshold for P to be a track");
	setHeaderValue(fileStream, stringCmdAnalysisInitConfiguration,                          config.analysisInitConfiguration,                                 "Initialize analysis for the configuration");
	setHeaderValue(fileStream, stringCmdAnalysisInitDetector,                               config.analysisInitDetector,                                      "Initialize analysis for the detector");
	setHeaderValue(fileStream, stringCmdAnalysisInitMemory,                                 config.analysisInitMemory,                                        "Initialize analysis for the memory");
	setHeaderValue(fileStream, stringCmdAnalysisInitEvent,                                  config.analysisInitEvent,                                         "Initialize analysis for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitClassPriority,                          config.analysisInitClassPriority,                                 "Initialize analysis for the classPriority");
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCEventAbsolute,               config.analysisInitQualityEFGCEventAbsolute,                      "Initialize analysis for the absolute EFGC-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCEventRelative,               config.analysisInitQualityEFGCEventRelative,                      "Initialize analysis for the relative EFGC-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCTotalAbsolute,               config.analysisInitQualityEFGCTotalAbsolute,                      "Initialize analysis for the absolute EFGC-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCTotalRelative,               config.analysisInitQualityEFGCTotalRelative,                      "Initialize analysis for the relative EFGC-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCEventPzEFGC,                config.analysisInitMomentumEFGCEventPzEFGC,                       "Initialize analysis for the EFGC-Pz-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCEventPtEFGC,                config.analysisInitMomentumEFGCEventPtEFGC,                       "Initialize analysis for the EFGC-Pt-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCTotalPzEFGC,                config.analysisInitMomentumEFGCTotalPzEFGC,                       "Initialize analysis for the EFGC-Pz-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCTotalPtEFGC,                config.analysisInitMomentumEFGCTotalPtEFGC,                       "Initialize analysis for the EFGC-Pt-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN,            config.analysisInitProjectionEFGCNEvent12EFGCN,                   "Initialize analysis for the EFGCN-12-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN,            config.analysisInitProjectionEFGCNEvent13EFGCN,                   "Initialize analysis for the EFGCN-13-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN,            config.analysisInitProjectionEFGCNEvent32EFGCN,                   "Initialize analysis for the EFGCN-32-quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN,            config.analysisInitProjectionEFGCNTotal12EFGCN,                   "Initialize analysis for the EFGCN-12-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN,            config.analysisInitProjectionEFGCNTotal13EFGCN,                   "Initialize analysis for the EFGCN-13-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN,            config.analysisInitProjectionEFGCNTotal32EFGCN,                   "Initialize analysis for the EFGCN-32-quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEvent,                          config.analysisInitMomentumEvent,                                 "Initialize analysis for the graphic quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumTotal,                          config.analysisInitMomentumTotal,                                 "Initialize analysis for the graphic quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumDisplay,                        config.analysisInitMomentumDisplay,                               "Initialize analysis for the graphic display");
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumToRoot,                         config.analysisInitMomentumToRoot,                                "Initialize analysis for the graphic-to-root-file functionality");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEvent,                        config.analysisInitProjectionEvent,                               "Initialize analysis for the graphic quality for the event");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionTotal,                        config.analysisInitProjectionTotal,                               "Initialize analysis for the graphic quality for all events");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionDisplay,                      config.analysisInitProjectionDisplay,                             "Initialize analysis for the graphic display");
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionToRoot,                       config.analysisInitProjectionToRoot,                              "Initialize analysis for the graphic-to-root-file functionality");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldX,                           config.analysisInitMagnetfieldX,                                  "Initialize analysis for the magnetfield in the x dimension");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldY,                           config.analysisInitMagnetfieldY,                                  "Initialize analysis for the magnetfield in the y dimension");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldZ,                           config.analysisInitMagnetfieldZ,                                  "Initialize analysis for the magnetfield in the z dimension");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldDisplay,                     config.analysisInitMagnetfieldDisplay,                            "Initialize analysis for the magnetfield display");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldToRoot,                      config.analysisInitMagnetfieldToRoot,                             "Initialize analysis for the magnetfield-to-root-file functionality");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldConstantForEachEvent,        config.analysisInitMagnetfieldConstantForEachEvent,               "Initialize analysis for the magnetfield factors for each event");
	setHeaderValue(fileStream, stringCmdAnalysisInitWeightedMagnetfieldConstant,            config.analysisInitWeightedMagnetfieldConstant,                   "Initialize analysis for the magnetfield factors to be weighted for the events");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldConstantDisplay,             config.analysisInitMagnetfieldConstantDisplay,                    "Initialize analysis for the magnetfield constant display");
	setHeaderValue(fileStream, stringCmdAnalysisMagnetfieldConstantDisplayMask,             config.analysisMagnetfieldConstantDisplayMask.toString().c_str(), "Number which inhibits corresponding magnetfield constants from drawing");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldConstantToRoot,              config.analysisInitMagnetfieldConstantToRoot,                     "Initialize analysis for the magnetfield-constant-to-root-file functionality");
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldVSConstants,                 config.analysisInitMagnetfieldVSConstants,                        "Initialize analysis for the comparisson between the magnetfield and the constants");
	setHeaderValue(fileStream, stringCmdAnalysisInitPrelutGoodness,                         config.analysisInitPrelutGoodness,                                "Initialize analysis for the prelut");
	setHeaderValue(fileStream, stringCmdAnalysisInitLutGoodness,                            config.analysisInitLutGoodness,                                   "Initialize analysis for the lut");
	setHeaderValue(fileStream, stringCmdAnalysisInitHoughTransformGoodness,                 config.analysisInitHoughTransformGoodness,                        "Initialize analysis for the Hough transform");
	setHeaderValue(fileStream, stringCmdAnalysisInitQuantizationGoodness,                   config.analysisInitQuantizationGoodness,                          "Initialize analysis for the quantization");
	setHeaderValue(fileStream, stringCmdAnalysisInitCreatedHistogramToRoot,                 config.analysisInitCreatedHistogramToRoot,                        "Initialize analysis for the created histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitEncodedHistogramToRoot,                 config.analysisInitEncodedHistogramToRoot,                        "Initialize analysis for the encoded histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitFilteredHistogramToRoot,                config.analysisInitFilteredHistogramToRoot,                       "Initialize analysis for the filtered histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitJustOneCreatedHistogramToRoot,          config.analysisInitJustOneCreatedHistogramToRoot,                 "Initialize analysis for just one created histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitJustOneEncodedHistogramToRoot,          config.analysisInitJustOneEncodedHistogramToRoot,                 "Initialize analysis for just one encoded histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitJustOneFilteredHistogramToRoot,         config.analysisInitJustOneFilteredHistogramToRoot,                "Initialize analysis for just one filtered histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitCreatedHistogramToShow,                 config.analysisInitCreatedHistogramToShow,                        "Initialize analysis for showing the created histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitEncodedHistogramToShow,                 config.analysisInitEncodedHistogramToShow,                        "Initialize analysis for showing the encoded histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitFilteredHistogramToShow,                config.analysisInitFilteredHistogramToShow,                       "Initialize analysis for showing the filtered histogram");
	setHeaderValue(fileStream, stringCmdAnalysisInitHistogramLayer,                         config.analysisInitHistogramLayer,                                "Initialize the index of the layer to make just one histogram accessible for showing and/or writing");
	setHeaderValue(fileStream, stringCmdAnalysisInitNumberOfTracksPerColumn,                config.analysisInitNumberOfTracksPerColumn,                       "Initialize analysis for evaluating the number of tracks per column in all histogram layers");
	setHeaderValue(fileStream, stringCmdAnalysisInitNumberOfTracksPerLayer,                 config.analysisInitNumberOfTracksPerLayer,                        "Initialize analysis for evaluating the number of tracks per histogram layer");
	setHeaderValue(fileStream, stringCmdAnalysisInitTotalAnalysis,                          config.analysisInitTotalAnalysis,                                 "Initialize the total analysis package");
	setHeaderValue(fileStream, stringCmdAnalysisInitTotalAnalysisDisplay,                   config.analysisInitTotalAnalysisDisplay,                          "Initialize the total analysis display for the arrays");
	setHeaderValue(fileStream, stringCmdAnalysisInitPercentageOfHitsInSignature,            config.analysisInitPercentageOfHitsInSignature,                   "Initialize the percentage of the number of hits which must be found to accept the signature");
	setHeaderValue(fileStream, stringCmdAnalysisInitPercentageOfTracksForSignature,         config.analysisInitPercentageOfTracksForSignature,                "Initialize the percentage of the real tracks which must be found with the accepted signatures");
	setHeaderValue(fileStream, stringCmdAnalysisInitAnalysisResultWarnings,                 config.analysisInitAnalysisResultWarnings,                        "Initialize the printing of the special analysis results to standard output");
	setHeaderValue(fileStream, stringCmdAnalysisInitAnalysisResultDisplays,                 config.analysisInitAnalysisResultDisplays,                        "Initialize the displays of the special analysis results in graphical mode");

	writeComment(fileStream, "");
	writeComment(fileStream, "[GLOBAL PARAMETERS]");

	setHeaderValue(fileStream, stringCmdInitStatus,                                         config.initStatus,                                                "Initialize the status to be shown");
/********************************************************/

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

InfData& inf::getDataReference() {

	return config;

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string inf::getInfo() {

	std::string message;
	char buffer[max(max(intConversionDigits, doubleConversionDigits), 6)+1]; /* 6 = sizeof(false+'/0')*/

/********************************************************/
/* make code changes for a different information here	*/
	message += "Setup for the Trackfinder Testbench:\n";
	message += "------------------------------------\n";
	message += "CommandID";
	message += "\t\t: ";
	for (int i = 0; i < numberOfCmds; i++) {
		btods(commandID[i], buffer);
		message += buffer;
	}
	message += "\n\n";
	message += "[PARAMETERS FOR THE INPUT OBJECT]";
	message += "\n";
	message += stringCmdInputFileName;
	message += "\t\t\t\t: ";
	message += config.inputFileName;
	message += "\n";
	message += stringCmdInputFileFormat;
	message += "\t\t\t\t: ";
	itos(config.inputFileFormat, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputMinTracks;
	message += "\t\t\t\t: ";
	itos(config.inputMinTracks, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputMaxTracks;
	message += "\t\t\t\t: ";
	itos(config.inputMaxTracks, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputDetectorMask;
	message += "\t\t\t: ";
	message += config.inputDetectorMask.toString();
	message += "\n";
	message += stringCmdInputFileNameMagneticField;
	message += "\t\t: ";
	message += config.inputFileNameMagneticField;
	message += "\n";
	message += stringCmdInputMagneticFieldIsRootFile;
	message += "\t\t: ";
	btobs(config.inputMagneticFieldIsRootFile, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdInputMapNameMagneticField;
	message += "\t\t: ";
	message += config.inputMapNameMagneticField;
	message += "\n";
	message += stringCmdInputMagneticFieldIntegrationStepwidthPerStation;
	message += "\t: ";
	ustos(config.inputMagneticFieldIntegrationStepwidthPerStation, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputMagneticFieldIntegrationFactor;
	message += "\t: ";
	dtos(config.inputMagneticFieldIntegrationFactor, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputDisableAutomaticMagneticField;
	message += "\t\t: ";
	message += config.inputDisableAutomaticMagneticField;
	message += "\n";
	message += stringCmdInputFileNameDetector;
	message += "\t\t\t: ";
	message += config.inputFileNameDetector;
	message += "\n";
	message += stringCmdInputNumberOfVolumesInfrontOfSTS;
	message += "\t: ";
	itos(config.inputNumberOfVolumesInfrontOfSTS, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputDisableAutomaticDetector;
	message += "\t\t: ";
	message += config.inputDisableAutomaticDetector;
	message += "\n";
	message += stringCmdInputCodingTableMode;
	message += "\t\t\t: ";
	itos(config.inputCodingTableMode, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingPTableMode;
	message += "\t\t\t: ";
	itos(config.inputGradingPTableMode, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingRTableMode;
	message += "\t\t\t: ";
	itos(config.inputGradingRTableMode, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputCodingTableFileName;
	message += "\t\t: ";
	message += config.inputCodingTableFileName;
	message += "\n";
	message += stringCmdInputGradingPTableFileName;
	message += "\t\t: ";
	message += config.inputGradingPTableFileName;
	message += "\n";
	message += stringCmdInputGradingRTableFileName;
	message += "\t\t: ";
	message += config.inputGradingRTableFileName;
	message += "\n";
	message += stringCmdInputCodingTableWrite;
	message += "\t\t\t: ";
	btobs(config.inputCodingTableWrite, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingPTableWrite;
	message += "\t\t\t: ";
	btobs(config.inputGradingPTableWrite, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingRTableWrite;
	message += "\t\t\t: ";
	btobs(config.inputGradingRTableWrite, buffer);
	message += buffer;
	message += "\n\n";
	message += "[PARAMETERS FOR THE TRACKFINDER OBJECT]";
	message += "\n";
	message += stringCmdTrackfinderHitProducer;
	message += "\t\t\t: ";
	itos(config.trackfinderHitProducer, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadPointsFromFile;
	message += "\t\t: ";
	btobs(config.trackfinderReadPointsFromFile, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadHitsFromFile;
	message += "\t\t: ";
	btobs(config.trackfinderReadHitsFromFile, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadMapsHits;
	message += "\t\t\t: ";
	btobs(config.trackfinderReadMapsHits, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadHybridHits;
	message += "\t\t: ";
	btobs(config.trackfinderReadHybridHits, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadStripHits;
	message += "\t\t: ";
	btobs(config.trackfinderReadStripHits, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutsVersion;
	message += "\t\t\t: ";
	itos(config.trackfinderLutsVersion, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderPrelutFileName;
	message += "\t\t: ";
	message += config.trackfinderPrelutFileName;
	message += "\n";
	message += stringCmdTrackfinderLutFileName;
	message += "\t\t\t: ";
	message += config.trackfinderLutFileName;
	message += "\n";
	message += stringCmdTrackfinderGammaMin;
	message += "\t\t\t: ";
	dtos(config.trackfinderGammaMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderGammaMax;
	message += "\t\t\t: ";
	dtos(config.trackfinderThetaMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderGammaStep;
	message += "\t\t\t: ";
	itos(config.trackfinderGammaStep, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderThetaMin;
	message += "\t\t\t: ";
	dtos(config.trackfinderThetaMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderThetaMax;
	message += "\t\t\t: ";
	dtos(config.trackfinderThetaMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderThetaStep;
	message += "\t\t\t: ";
	itos(config.trackfinderThetaStep, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderPrelutRadiusMin;
	message += "\t\t: ";
	dtos(config.trackfinderPrelutRadiusMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderPrelutRadiusMax;
	message += "\t\t: ";
	dtos(config.trackfinderPrelutRadiusMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutRadiusMin;
	message += "\t\t\t: ";
	dtos(config.trackfinderLutRadiusMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutRadiusMax;
	message += "\t\t\t: ";
	dtos(config.trackfinderLutRadiusMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutRadiusStep;
	message += "\t\t: ";
	itos(config.trackfinderLutRadiusStep, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderMinClassCoding;
	message += "\t\t\t: ";
	ustos(config.trackfinderMinClassCoding, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderMinClassGradingP;
	message += "\t\t\t: ";
	ustos(config.trackfinderMinClassGradingP, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderMinClassGradingR;
	message += "\t\t\t: ";
	ustos(config.trackfinderMinClassGradingR, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderFirstFilterNeighborhoodDim1ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderFirstFilterNeighborhoodDim2ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderSecondFilterNeighborhoodDim1ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderSecondFilterNeighborhoodDim2ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderSecondFilterNeighborhoodDim3ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderWriteTracksToFile;
	message += "\t\t: ";
	btobs(config.trackfinderWriteTracksToFile, buffer);
	message += buffer;
	message += "\n\n";
	message += "[PARAMETERS FOR THE ANALYSIS OBJECT]";
	message += "\n";
	message += stringCmdAnalysisOutputFileName;
	message += "\t\t: ";
	message += config.analysisOutputFileName;
	message += "\n";
	message += stringCmdAnalysisThresholdForP;
	message += "\t\t\t\t: ";
	dtos(config.analysisThresholdForP, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitConfiguration;
	message += "\t\t\t: ";
	btobs(config.analysisInitConfiguration, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitDetector;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitDetector, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMemory;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitMemory, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitEvent;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitClassPriority;
	message += "\t\t\t: ";
	btobs(config.analysisInitClassPriority, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCEventAbsolute;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCEventAbsolute, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCEventRelative;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCEventRelative, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCTotalAbsolute;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCTotalAbsolute, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCTotalRelative;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCTotalRelative, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCEventPzEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCEventPzEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCEventPtEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCEventPtEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCTotalPzEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCTotalPzEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCTotalPtEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCTotalPtEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNEvent12EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNEvent13EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNEvent32EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNTotal12EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNTotal13EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNTotal32EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEvent;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumTotal;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumTotal, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEvent;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionTotal;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionTotal, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldX;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldX, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldY;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldY, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldZ;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldZ, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldConstantForEachEvent;
	message += "\t: ";
	btobs(config.analysisInitMagnetfieldConstantForEachEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitWeightedMagnetfieldConstant;
	message += "\t\t: ";
	btobs(config.analysisInitWeightedMagnetfieldConstant, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldConstantDisplay;
	message += "\t\t: ";
	btobs(config.analysisInitMagnetfieldConstantDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisMagnetfieldConstantDisplayMask;
	message += "\t\t: ";
	message += config.analysisMagnetfieldConstantDisplayMask.toString();
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldConstantToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitMagnetfieldConstantToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldVSConstants;
	message += "\t\t: ";
	btobs(config.analysisInitMagnetfieldVSConstants, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPrelutGoodness;
	message += "\t\t\t: ";
	btobs(config.analysisInitPrelutGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitLutGoodness;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitLutGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitHoughTransformGoodness;
	message += "\t\t: ";
	btobs(config.analysisInitHoughTransformGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQuantizationGoodness;
	message += "\t\t: ";
	btobs(config.analysisInitQuantizationGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitCreatedHistogramToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitCreatedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitEncodedHistogramToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitEncodedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitFilteredHistogramToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitFilteredHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitJustOneCreatedHistogramToRoot;
	message += "\t: ";
	btobs(config.analysisInitJustOneCreatedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitJustOneEncodedHistogramToRoot;
	message += "\t: ";
	btobs(config.analysisInitJustOneEncodedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitJustOneFilteredHistogramToRoot;
	message += "\t: ";
	btobs(config.analysisInitJustOneFilteredHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitCreatedHistogramToShow;
	message += "\t\t: ";
	btobs(config.analysisInitCreatedHistogramToShow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitEncodedHistogramToShow;
	message += "\t\t: ";
	btobs(config.analysisInitEncodedHistogramToShow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitFilteredHistogramToShow;
	message += "\t\t: ";
	btobs(config.analysisInitFilteredHistogramToShow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitHistogramLayer;
	message += "\t\t\t: ";
	ustos(config.analysisInitHistogramLayer, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitNumberOfTracksPerColumn;
	message += "\t\t: ";
	btobs(config.analysisInitNumberOfTracksPerColumn, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitNumberOfTracksPerLayer;
	message += "\t\t: ";
	btobs(config.analysisInitNumberOfTracksPerLayer, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTotalAnalysis;
	message += "\t\t\t: ";
	btobs(config.analysisInitTotalAnalysis, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTotalAnalysisDisplay;
	message += "\t\t: ";
	btobs(config.analysisInitTotalAnalysisDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPercentageOfHitsInSignature;
	message += "\t\t: ";
	ustos(config.analysisInitPercentageOfHitsInSignature, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPercentageOfTracksForSignature;
	message += "\t: ";
	ustos(config.analysisInitPercentageOfTracksForSignature, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitAnalysisResultWarnings;
	message += "\t\t: ";
	ultos(config.analysisInitAnalysisResultWarnings, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitAnalysisResultDisplays;
	message += "\t\t: ";
	ultos(config.analysisInitAnalysisResultDisplays, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n\n";
	message += "[GLOBAL PARAMETERS]";
	message += "\n";
	message += stringCmdInitStatus;
	message += "\t\t\t\t\t: ";
	btobs(config.initStatus, buffer);
	message += buffer;
/********************************************************/

	return message;

}
