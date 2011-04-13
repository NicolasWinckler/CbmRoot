# Create a library called "libDilept" which includes the source files
# given in the array . The extension is already found.  Any number of
# sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base

${CMAKE_SOURCE_DIR}/cbmbase
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/cbmdata
${CMAKE_SOURCE_DIR}/cbmdata/mvd
${CMAKE_SOURCE_DIR}/cbmdata/sts
${CMAKE_SOURCE_DIR}/cbmdata/rich
${CMAKE_SOURCE_DIR}/cbmdata/trd
${CMAKE_SOURCE_DIR}/cbmdata/tof
${CMAKE_SOURCE_DIR}/cbmdata/global

${CMAKE_SOURCE_DIR}/global
${CMAKE_SOURCE_DIR}/KF
${CMAKE_SOURCE_DIR}/KF/Interface
${CMAKE_SOURCE_DIR}/sts
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/dilept
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR} 
)
 
link_directories( ${LINK_DIRECTORIES})


set(DILEPT_SRCS
CbmDileptonTrack.cxx
CbmDileptonTrackReal.cxx
CbmDileptonTrackRealCollection.cxx
CbmDileptonAssignPid.cxx
CbmDileptonTrackSim.cxx
CbmDileptonTrackSimCollection.cxx
CbmDileptonAssignMCid.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${DILEPT_SRCS}" "${INCLUDE_DIRECTORIES}" DILEPT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h DILEPT_HEADERS "${DILEPT_SRCS}")

SET(DILEPT_LINKDEF DileptonLinkDef.h)

set(DILEPT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmDileptDict.cxx) 
ROOT_GENERATE_DICTIONARY("${DILEPT_HEADERS}" "${DILEPT_LINKDEF}" "${DILEPT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


# add the dictionary to the list of source files
SET(DILEPT_SRCS ${DILEPT_SRCS} ${DILEPT_DICTIONARY}) 


add_library(Dilept SHARED ${DILEPT_SRCS})
target_link_libraries(Dilept ${ROOT_LIBRARIES})
set_target_properties(Dilept PROPERTIES VERSION 0.0.1 SOVERSION 0 )

################ install ###################
install(TARGETS Dilept DESTINATION ${CMAKE_BINARY_DIR}/lib)
