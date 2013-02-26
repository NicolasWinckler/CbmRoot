# Create a library called "libTrd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${Boost_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmbase/draw
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/littrack

${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/trd/TrdMC
${CBMROOT_SOURCE_DIR}/trd/TrdDigi
${CBMROOT_SOURCE_DIR}/trd/TrdTools
${CBMROOT_SOURCE_DIR}/trd/TrdParam
${CBMROOT_SOURCE_DIR}/trd/TrdReco
${CBMROOT_SOURCE_DIR}/trd/TrdQa
${CBMROOT_SOURCE_DIR}/trd/TrdTools
${CBMROOT_SOURCE_DIR}/trd/TrdData
${CBMROOT_SOURCE_DIR}/trd/TrdAna

${CBMROOT_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})


#FILE(GLOB TRD_HEADERS ${CMAKE_HOME_DIRECTORY}/trd *.h)

set(TRD_SRCS
TrdMC/CbmGeoTrd.cxx 
TrdMC/CbmTrd.cxx 		 

TrdParam/CbmGeoTrdPar.cxx 
TrdParam/CbmTrdContFact.cxx   
TrdParam/CbmTrdDigiPar.cxx
TrdParam/CbmTrdCreateDigiPar.cxx

TrdDigi/CbmTrdDigitizerMathieson.cxx 
TrdDigi/CbmTrdDigitizerPRF.cxx
TrdDigi/CbmTrdDigitizer.cxx
#TrdDigi/CbmTrdDigiWriteoutBuffer.cxx

TrdQa/CbmTrdHitProducerQa.cxx  
TrdQa/CbmTrdTracksPidQa.cxx
TrdQa/CbmTrdQa.cxx

TrdReco/CbmTrdHitProducerIdeal.cxx    
TrdReco/CbmTrdHitProducerSmearing.cxx
TrdReco/CbmTrdHitProducerDigi.cxx
TrdReco/CbmTrdClusterFinder.cxx
TrdReco/CbmTrdClusterFinderFast.cxx
TrdReco/CbmTrdHitProducerCluster.cxx
TrdReco/CbmTrdSimpleHitProducerCluster.cxx

TrdTools/CbmTrdElectronsTrainAnn.cxx
TrdTools/CbmTrdGeoHandler.cxx

#TrdData/CbmTrdCluster.cxx 
TrdData/CbmTrdRadiator.cxx  
TrdData/CbmTrdGas.cxx
TrdData/CbmTrdModule.cxx
#TrdData/CbmTrdDetectorId.cxx

TrdAna/CbmTrdHitRateTest.cxx
TrdAna/CbmTrdHitDensityTest.cxx
#TrdAna/CbmTrdPhotonAnalysis.cxx
TrdAna/CbmTrdOccupancy.cxx

CbmTrdFindTracks.cxx  
CbmTrdTrackFinderIdeal.cxx  
CbmTrdFitTracks.cxx  
CbmTrdTrackFitterIdeal.cxx  
CbmTrdMatchTracks.cxx
CbmTrdSetTracksPidWkn.cxx       
CbmTrdSetTracksPidLike.cxx     
CbmTrdSetTracksPidANN.cxx       
CbmTrdCreatePidLike.cxx 
#CbmTrdDigiSorterTask.cxx
#CbmTrdDigiRingSorter.cxx

)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TRD_SRCS}" "${INCLUDE_DIRECTORIES}" TRD_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TRD_HEADERS "${TRD_SRCS}")

set(TRD_LINKDEF TrdLinkDef.h)
set(TRD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmTrdDict.cxx) 

#ROOT_GENERATE_DICTIONARY("${TRD_HEADERS}" "${TRD_LINKDEF}" "${TRD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

CHANGE_FILE_EXTENSION(*.cxx *.h HDRS "${TRD_SRCS}")
set(LINKDEF TrdLinkDef.h)
set(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmTrdDict.cxx) 
set(DICTIONARY CbmTrdDict.cxx) 
ROOT_GENERATE_DICTIONARY()

set(TRD_SRCS ${TRD_SRCS} ${TRD_DICTIONARY})

add_library(Trd SHARED ${TRD_SRCS})
target_link_libraries(Trd ${ROOT_LIBRARIES})
set_target_properties(Trd PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Trd  DESTINATION ${CMAKE_BINARY_DIR}/lib)
