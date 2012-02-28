# Create a library called "libEcal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts 
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/ecal 
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/KF

${CBMROOT_SOURCE_DIR}/ecal
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(ECAL_SRCS_RULECHECKER
CbmEcal.cxx CbmEcalDetailed.cxx 	
CbmEcalHitProducer.cxx 
CbmGeoEcalPar.cxx CbmEcalDrawer.cxx 
CbmEcalContFact.cxx CbmGeoEcal.cxx  
CbmEcalAnalysisDS.cxx CbmEcalAnalysis.cxx 
CbmEcalModule.cxx CbmEcalStructure.cxx 
CbmEcalHitProducerFastMC.cxx  CbmEcalCellMC.cxx
CbmEcalReconstructionFastMC.cxx
CbmEcalPhotonCalibrator.cxx CbmEcalShowerCalibrator.cxx 
CbmEcalUrqmdCalibrator.cxx CbmEcalSCurveLib.cxx 
CbmEcalSCurveLibRecord.cxx CbmEcalOShowerCalibrator.cxx 
CbmEcalTrackExtrapolation.cxx
CbmEcalTracking.cxx CbmEcalStructureFiller.cxx
CbmEcalTrackExport.cxx CbmEcalStructureExport.cxx
CbmEcalQualityCheck.cxx CbmEcalClusterFinderV2.cxx
CbmEcalTrackImport.cxx CbmEcalCalibration.cxx
CbmEcalParam.cxx CbmEcalMatching.cxx
CbmEcalAnalysisPi0.cxx CbmEcalAnalysisPair.cxx CbmEcalAnalysisMaterial.cxx
CbmEcalAnalysisNeutron.cxx CbmEcalTimeDigitizer.cxx
CbmEcalLightMap.cxx CbmEcalDrawer2.cxx
CbmEcalAnalysisCluster.cxx CbmEcalAnalysisUniformity.cxx
CbmEcalAnalysisDst.cxx CbmEcalAnalysisBSh.cxx
CbmEcalCalibrationV2.cxx CbmEcalAnalysisSimple.cxx CbmEcalIdentification.cxx
CbmEcalMaximumLocator.cxx CbmEcalAnalysisJPsi.cxx
CbmEcalAnalysisSShape.cxx CbmEcalAnalysisShape.cxx
CbmEcalShLibRecord.cxx CbmEcalShLib.cxx CbmEcalShLibTable.cxx
CbmEcalClusterFinder.cxx CbmEcalReco.cxx CbmEcalShLibNN.cxx
CbmEcalAnalysisEres.cxx CbmEcalPosLib.cxx CbmEcalReco2.cxx
CbmEcalShLibCorr.cxx CbmEcalShLibCorrTable.cxx
CbmEcalShLibCorrRecord.cxx CbmEcalRecoCorr.cxx
CbmEcalDigi.cxx CbmEcalPrepare.cxx
)

SET(ECAL_SRCS ${ECAL_SRCS_RULECHECKER} CbmEcalInf.cxx)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${ECAL_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" ECAL_RULES)
endif(RULE_CHECKER_FOUND)
 
# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h ECAL_HEADERS "${ECAL_SRCS}")

set(ECAL_LINKDEF  EcalLinkDef.h)
set(ECAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmEcalDict.cxx) 

ROOT_GENERATE_DICTIONARY("${ECAL_HEADERS}" "${ECAL_LINKDEF}" "${ECAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(ECAL_SRCS ${ECAL_SRCS} ${ECAL_DICTIONARY})


add_library(Ecal SHARED ${ECAL_SRCS})
target_link_libraries(Ecal ${ROOT_LIBRARIES})
set_target_properties(Ecal PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Ecal DESTINATION ${CMAKE_BINARY_DIR}/lib)
