# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/much_feb07
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MUCHFEB07_HEADERS
CbmGeoMuch.h  
CbmGeoMuchPar.h 
CbmMuchContFact.h  
CbmMuchDigi.h
CbmMuchDigiMatch.h
CbmMuchDigiPar.h
CbmMuchDigiScheme.h
CbmMuchDigitize.h
CbmMuchFindHits.h
CbmMuch.h  
CbmMuchHit.h 
CbmMuchHitProducerIdeal.h
CbmMuchParAsciiFileIo.h
CbmMuchParRootFileIo.h
#CbmMuchPhysicsQa.h
CbmMuchPoint.h 
CbmMuchSectorDigiPar.h
CbmMuchSector.h
CbmMuchSegmentation.h
#CbmMuchSimulationQa.h
CbmMuchStationDigiPar.h
CbmMuchStation.h
CbmMuchTrack.h 
#CbmMuchClusterFinder.h
CbmMuchPad.h
CbmMuchCluster.h
CbmMuchHitProducerQa.h
CbmMuchFindTracks.h
CbmMuchTrackFinder.h
CbmMuchTrackMatch.h 
CbmMuchMatchTracks.h
CbmMuchTrackFinderIdeal.h
)

set(MUCHFEB07_LINKDEF MuchFeb07LinkDef.h)
set(MUCHFEB07_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmMuchFeb07Dict.cxx) 

ROOT_GENERATE_DICTIONARY("${MUCHFEB07_HEADERS}" "${MUCHFEB07_LINKDEF}" "${MUCHFEB07_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(MUCHFEB07_SRCS
CbmGeoMuch.cxx  
CbmGeoMuchPar.cxx 
CbmMuchContFact.cxx  
CbmMuch.cxx  
CbmMuchPoint.cxx 
CbmMuchTrack.cxx 
CbmMuchHit.cxx 
CbmMuchHitProducerIdeal.cxx
CbmMuchDigiPar.cxx
CbmMuchSectorDigiPar.cxx
CbmMuchStationDigiPar.cxx
CbmMuchParAsciiFileIo.cxx
CbmMuchParRootFileIo.cxx
#CbmMuchPhysicsQa.cxx
CbmMuchDigitize.cxx
CbmMuchDigi.cxx
CbmMuchDigiScheme.cxx
CbmMuchSector.cxx
CbmMuchStation.cxx
CbmMuchFindHits.cxx
CbmMuchDigiMatch.cxx
CbmMuchSegmentation.cxx
#CbmMuchClusterFinder.cxx
CbmMuchPad.cxx
CbmMuchCluster.cxx
CbmMuchHitProducerQa.cxx
CbmMuchFindTracks.cxx
CbmMuchTrackFinder.cxx
CbmMuchTrackMatch.cxx
CbmMuchMatchTracks.cxx 
CbmMuchTrackFinderIdeal.cxx
${MUCHFEB07_DICTIONARY} )

add_library(MuchFeb07 SHARED ${MUCHFEB07_SRCS})
target_link_libraries(MuchFeb07 ${ROOT_LIBRARIES})
set_target_properties(MuchFeb07 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES} )

################ install ###################
install(TARGETS MuchFeb07 DESTINATION ${CMAKE_BINARY_DIR}/lib)
