# Create a library called "libCbmBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(CBMBASE_SRCS
CbmDigi.cxx 
CbmDigiManager.cxx
CbmStack.cxx 
CbmTrack.cxx
CbmTrkHit.cxx
CbmPropagator.cxx
CbmMCTrack.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CBMBASE_SRCS}" "${INCLUDE_DIRECTORIES}" CBMBASE_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h CBMBASE_HEADERS "${CBMBASE_SRCS}")

SET(CBMBASE_HEADERS ${CBMBASE_HEADERS} CbmDetectorList.h)

set(CBMBASE_LINKDEF  CbmBaseLinkDef.h)
set(CBMBASE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmBaseDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CBMBASE_HEADERS}" "${CBMBASE_LINKDEF}" "${CBMBASE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(CBMBASE_SRCS ${CBMBASE_SRCS} ${CBMBASE_DICTIONARY})

add_library(CbmBase SHARED ${CBMBASE_SRCS})
target_link_libraries(CbmBase ${ROOT_LIBRARIES})
set_target_properties(CbmBase PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS CbmBase DESTINATION ${CMAKE_BINARY_DIR}/lib)
