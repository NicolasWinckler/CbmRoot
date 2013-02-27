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

${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/trd/mc
${CBMROOT_SOURCE_DIR}/trd/digi
${CBMROOT_SOURCE_DIR}/trd/tools
${CBMROOT_SOURCE_DIR}/trd/param
${CBMROOT_SOURCE_DIR}/trd/reco
${CBMROOT_SOURCE_DIR}/trd/qa
${CBMROOT_SOURCE_DIR}/trd/tools
${CBMROOT_SOURCE_DIR}/trd/data

${CBMROOT_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})


#FILE(GLOB TRD_HEADERS ${CMAKE_HOME_DIRECTORY}/trd *.h)

set(TRD_SRCS
mc/CbmGeoTrd.cxx 
mc/CbmTrd.cxx 		 

param/CbmGeoTrdPar.cxx 
param/CbmTrdContFact.cxx   
param/CbmTrdDigiPar.cxx
param/CbmTrdCreateDigiPar.cxx

digi/CbmTrdDigitizerMathieson.cxx 
digi/CbmTrdDigitizerPRF.cxx
digi/CbmTrdDigitizer.cxx
#digi/CbmTrdDigiWriteoutBuffer.cxx

qa/CbmTrdHitProducerQa.cxx  
qa/CbmTrdTracksPidQa.cxx
qa/CbmTrdQa.cxx
qa/CbmTrdHitDensityQa.cxx
qa/CbmTrdHitRateQa.cxx
qa/CbmTrdOccupancyQa.cxx

reco/CbmTrdHitProducerIdeal.cxx    
reco/CbmTrdHitProducerSmearing.cxx
reco/CbmTrdHitProducerDigi.cxx
reco/CbmTrdClusterFinder.cxx
reco/CbmTrdClusterFinderFast.cxx
reco/CbmTrdHitProducerCluster.cxx
reco/CbmTrdSimpleHitProducerCluster.cxx

tools/CbmTrdElectronsTrainAnn.cxx
tools/CbmTrdGeoHandler.cxx

data/CbmTrdRadiator.cxx  
data/CbmTrdGas.cxx
data/CbmTrdModule.cxx

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
