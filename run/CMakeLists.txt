# Create a library called "libRun" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/generators
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmgenerators
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/psd
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/trackbase
${CBMROOT_SOURCE_DIR}/mvd
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/rich
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/tof
${CBMROOT_SOURCE_DIR}/ecal
${CBMROOT_SOURCE_DIR}/run
${CMAKE_SOURCE_DIR}/geane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(RUN_SRCS

CbmDaq.cxx
CbmDaqBuffer.cxx
CbmMCBuffer.cxx
CbmMCTimeSim.cxx
CbmTimeSlice.cxx

CbmMCStreamer.cxx
CbmRunAna.cxx
CbmMCStreamerQa.cxx

)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RUN_SRCS}" "${INCLUDE_DIRECTORIES}" RUN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RUN_HEADERS "${RUN_SRCS}")

set(RUN_LINKDEF  RunLinkDef.h)
set(RUN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmRunDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RUN_HEADERS}" "${RUN_LINKDEF}" "${RUN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(RUN_SRCS ${RUN_SRCS} ${RUN_DICTIONARY})

add_library(Run SHARED ${RUN_SRCS})
target_link_libraries(Run ${ROOT_LIBRARIES})
set_target_properties(Run PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Run DESTINATION ${CMAKE_BINARY_DIR}/lib)
