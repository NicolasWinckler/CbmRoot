# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/tof
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/tof
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(TOF_SRCS
CbmGeoTof.cxx 
CbmGeoTofPar.cxx 
CbmTof.cxx 
CbmTofContFact.cxx 
CbmTofHitProducer.cxx 
CbmTofHitProducerIdeal.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TOF_SRCS}" "${INCLUDE_DIRECTORIES}" TOF_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TOF_HEADERS "${TOF_SRCS}")

set(TOF_LINKDEF TofLinkDef.h)
set(TOF_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmTofDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TOF_HEADERS}" "${TOF_LINKDEF}" "${TOF_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(TOF_SRCS ${TOF_SRCS} ${TOF_DICTIONARY})


add_library(Tof SHARED ${TOF_SRCS})
target_link_libraries(Tof ${ROOT_LIBRARIES})
set_target_properties(Tof PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Tof DESTINATION ${CMAKE_BINARY_DIR}/lib)
