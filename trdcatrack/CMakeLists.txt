## Create a library called "libtrdcatrack" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base

${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/trd/param
${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/L1/

${CBMROOT_SOURCE_DIR}/trdcatrack
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(TRDCATRACK_SRCS
CbmL1CATrdTrackFinderSA.cxx
CbmL1TrdTracklet4.cxx
CbmL1TrdTracklet.cxx
CbmTrdMatchTracksMK.cxx
CbmTrdTestEventsWithMom.cxx
CbmTrdTrackFitterKF_CA.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TRDCATRACK_SRCS}" "${INCLUDE_DIRECTORIES}" TRDCATRACK_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TRDCATRACK_HEADERS "${TRDCATRACK_SRCS}")

set(TRDCATRACK_LINKDEF TrdCaTrackLinkDef.h)
set(TRDCATRACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmTrdCATrackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TRDCATRACK_HEADERS}" "${TRDCATRACK_LINKDEF}" "${TRDCATRACK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(TRDCATRACK_SRCS ${TRDCATRACK_SRCS} ${TRDCATRACK_DICTIONARY})

add_library(trdcatrack SHARED ${TRDCATRACK_SRCS})
target_link_libraries(trdcatrack ${ROOT_LIBRARIES})
set_target_properties(trdcatrack PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS trdcatrack DESTINATION ${CMAKE_BINARY_DIR}/lib)
