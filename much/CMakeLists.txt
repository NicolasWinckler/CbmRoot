# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed
# here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/much/reco
${CBMROOT_SOURCE_DIR}/much/tracking
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MUCHRULECHECKER_SRCS
geo/CbmGeoMuch.cxx  
geo/CbmGeoMuchPar.cxx 
geo/CbmMuch.cxx
geo/CbmMuchContFact.cxx  
geo/CbmMuchGeoScheme.cxx
geo/CbmMuchLayer.cxx
geo/CbmMuchLayerSide.cxx
geo/CbmMuchModule.cxx
#geo/CbmMuchModuleGem.cxx
geo/CbmMuchModuleStraws.cxx
geo/CbmMuchPad.cxx
geo/CbmMuchSector.cxx
geo/CbmMuchSegmentAuto.cxx
geo/CbmMuchSegmentManual.cxx
geo/CbmMuchSegmentSector.cxx
geo/CbmMuchStation.cxx
geo/CbmMuchRadialSector.cxx

reco/CbmMuchDigitizeGem.cxx
reco/CbmMuchDigitizeGem.cxx
reco/CbmMuchDigitizeStraws.cxx
reco/CbmMuchFindHitsGem.cxx
reco/CbmMuchFindHitsGem.cxx
reco/CbmMuchFindHitsStraws.cxx
reco/CbmMuchHitFinderQa.cxx
reco/CbmMuchHitProducerIdeal.cxx
reco/CbmMuchPointInfo.cxx
reco/CbmMuchStrawHitFinderQa.cxx
reco/CbmMuchClusterAnalysis.cxx

tracking/CbmMuchFindTracks.cxx
tracking/CbmMuchMatchTracks.cxx 
tracking/CbmMuchTrackFinderIdeal.cxx
)
set(MUCH_SRCS
    ${MUCHRULECHECKER_SRCS}
    geo/CbmMuchModuleGem.cxx
    geo/CbmMuchModuleSector.cxx
   )

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MUCHRULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MUCH_RULES)
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
