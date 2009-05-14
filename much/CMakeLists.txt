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
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/much
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MUCH_SRCS
CbmGeoMuch.cxx  
CbmGeoMuchPar.cxx 
CbmMuch.cxx
CbmMuchContFact.cxx  
CbmMuchDigitizeAdvancedGem.cxx
CbmMuchDigitizeSimpleGem.cxx
CbmMuchDigitizeStraws.cxx
CbmMuchFindHitsAdvancedGem.cxx
CbmMuchFindHitsSimpleGem.cxx
CbmMuchFindTracks.cxx
CbmMuchGeoScheme.cxx
CbmMuchHitFinderQa.cxx
CbmMuchHitProducerIdeal.cxx
CbmMuchLayer.cxx
CbmMuchLayerSide.cxx
CbmMuchMatchTracks.cxx 
CbmMuchModule.cxx
CbmMuchModuleGem.cxx
CbmMuchModuleStraws.cxx
CbmMuchPad.cxx
CbmMuchPointInfo.cxx
#CbmMuchPhysicsQa.cxx
CbmMuchSector.cxx
CbmMuchSegmentAuto.cxx
CbmMuchSegmentManual.cxx
CbmMuchStation.cxx
CbmMuchStationGem.cxx
CbmMuchStationStraw.cxx
CbmMuchTrackFinderIdeal.cxx
CbmMuchFindHitsStraws.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MUCH_SRCS}" "${INCLUDE_DIRECTORIES}" MUCH_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MUCH_HEADERS "${MUCH_SRCS}")

set(MUCH_LINKDEF MuchLinkDef.h)
set(MUCH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmMuchDict.cxx) 

ROOT_GENERATE_DICTIONARY("${MUCH_HEADERS}" "${MUCH_LINKDEF}" "${MUCH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(MUCH_SRCS ${MUCH_SRCS} ${MUCH_DICTIONARY})

add_library(Much SHARED ${MUCH_SRCS})
target_link_libraries(Much ${ROOT_LIBRARIES})
set_target_properties(Much PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Much DESTINATION ${CMAKE_BINARY_DIR}/lib)
