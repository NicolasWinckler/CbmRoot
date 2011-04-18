# Create a library called "libGlobal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/global
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(GLOBAL_SRCS
CbmEcalTrackingMuch.cxx
CbmFindGlobalTracks.cxx    
CbmFindGlobalTracksQa.cxx  
CbmFindPrimaryVertex.cxx   
CbmFitGlobalTracks.cxx 
CbmFitGlobalTracksQa.cxx 
CbmGlobalTrackFitterIdeal.cxx 
CbmPVFinderIdeal.cxx       
CbmTrackMergerIdeal.cxx    
CbmTofMergerIdeal.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GLOBAL_SRCS}" "${INCLUDE_DIRECTORIES}" GLOBAL_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GLOBAL_HEADERS "${GLOBAL_SRCS}")

set(GLOBAL_LINKDEF GlobalLinkDef.h)
set(GLOBAL_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/GlobalDict.cxx) 

ROOT_GENERATE_DICTIONARY("${GLOBAL_HEADERS}" "${GLOBAL_LINKDEF}" "${GLOBAL_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(GLOBAL_SRCS ${GLOBAL_SRCS} ${GLOBAL_DICTIONARY})


add_library(Global SHARED ${GLOBAL_SRCS})
target_link_libraries(Global ${ROOT_LIBRARIES})
set_target_properties(Global PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Global DESTINATION ${CMAKE_BINARY_DIR}/lib)
