# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/sts 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(STS_SRCS
CbmGeoSts.cxx 
CbmGeoStsPar.cxx 
CbmSts.cxx 
CbmStsContFact.cxx 
CbmStsDigiPar.cxx 
CbmStsDigiScheme.cxx 
CbmStsDigitize.cxx 
CbmStsRealDigitize.cxx 
CbmStsFindHits.cxx 
CbmStsRealClusterFinder.cxx 
CbmStsRealFindHits.cxx 
CbmStsFindTracks.cxx 
CbmStsFindTracksQa.cxx 
CbmStsFitTracks.cxx 
CbmStsHitProducerIdeal.cxx 
CbmStsMatchHits.cxx 
CbmStsRealMatchHits.cxx 
CbmStsMatchTracks.cxx 
CbmStsParAsciiFileIo.cxx 
CbmStsParRootFileIo.cxx 
CbmStsSensor.cxx 
CbmStsSensorDigiPar.cxx 
CbmStsSector.cxx 
CbmStsSectorDigiPar.cxx 
CbmStsStation.cxx 
CbmStsStationDigiPar.cxx 
CbmStsTrackFinder.cxx 
CbmStsTrackFinderIdeal.cxx 
CbmStsTrackFitter.cxx 
CbmStsTrackFitterIdeal.cxx 
CbmStsSimulationQa.cxx 
CbmStsFindHitsQa.cxx
CbmStsReconstructionQa.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STS_HEADERS "${STS_SRCS}")

set(STS_LINKDEF  StsLinkDef.h)
set(STS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmStsDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STS_HEADERS}" "${STS_LINKDEF}" "${STS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(STS_SRCS ${STS_SRCS}  ${STS_DICTIONARY})


add_library(Sts SHARED ${STS_SRCS})
target_link_libraries(Sts ${ROOT_LIBRARIES})
set_target_properties(Sts PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Sts DESTINATION ${CMAKE_BINARY_DIR}/lib)
