# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
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
CbmEcalCell.cxx CbmEcalModule.cxx CbmEcalStructure.cxx 
CbmEcalHitProducerFastMC.cxx 
CbmEcalRecParticle.cxx CbmEcalReconstructionFastMC.cxx 
CbmEcalClusterFinder.cxx 
CbmEcalPhotonCalibrator.cxx CbmEcalShowerCalibrator.cxx 
CbmEcalUrqmdCalibrator.cxx CbmEcalSCurveLib.cxx 
CbmEcalSCurveLibRecord.cxx CbmEcalOShowerCalibrator.cxx 
CbmEcalShowerLib.cxx CbmEcalTrackExtrapolation.cxx
CbmEcalTracking.cxx CbmEcalStructureFiller.cxx
CbmEcalTrackExport.cxx CbmEcalStructureExport.cxx
CbmEcalRecoSimple.cxx CbmEcalClusterV1.cxx
CbmEcalQualityCheck.cxx CbmEcalClusterFinderV1.cxx
CbmEcalTrackImport.cxx CbmEcalCalibration.cxx
CbmEcalParam.cxx CbmEcalRecoSlow.cxx CbmEcalMatching.cxx
CbmEcalAnalysisPi0.cxx CbmEcalAnalysisPair.cxx CbmEcalAnalysisMaterial.cxx
CbmEcalAnalysisNeutron.cxx CbmEcalTimeDigitizer.cxx
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
