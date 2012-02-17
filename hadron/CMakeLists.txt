# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/rich
${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/tof
${CBMROOT_SOURCE_DIR}/hadron 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(HADRON_SRCS
CbmHadron.cxx
CbmProduceDst.cxx
CbmFillM2.cxx
CbmFitM2.cxx
CbmHadronSpectra.cxx
CbmHadronYields.cxx
CbmMixedEventsProducer.cxx
CbmEbefluctPYR.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${HADRON_SRCS}" "${INCLUDE_DIRECTORIES}" HADRON_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HADRON_HEADERS "${HADRON_SRCS}")

set(HADRON_LINKDEF  HadronLinkDef.h)
set(HADRON_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmHadronDict.cxx) 

ROOT_GENERATE_DICTIONARY("${HADRON_HEADERS}" "${HADRON_LINKDEF}" "${HADRON_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(HADRON_SRCS ${HADRON_SRCS}  ${HADRON_DICTIONARY})


add_library(Hadron SHARED ${HADRON_SRCS})
target_link_libraries(Hadron ${ROOT_LIBRARIES})
set_target_properties(Hadron PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Hadron DESTINATION ${CMAKE_BINARY_DIR}/lib)
