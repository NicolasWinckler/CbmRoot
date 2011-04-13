# Create a library called "libStt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
#put here all directories where header files are located
${CBMROOT_SOURCE_DIR}/stt
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/geobase
${CBMROOT_SOURCE_DIR}/parbase
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/stt
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(STT_SRCS

#Put here your sourcefiles
CbmStt.cxx
CbmSttContFact.cxx
CbmGeoStt.cxx
CbmGeoSttPar.cxx
CbmSttHitProducer.cxx
CbmSttMatchTracks.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${STT_SRCS}" "${INCLUDE_DIRECTORIES}" STT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STT_HEADERS "${STT_SRCS}")

set(STT_LINKDEF SttLinkDef.h)
set(STT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmSttDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STT_HEADERS}" "${STT_LINKDEF}" "${STT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(STT_SRCS ${STT_SRCS} ${STT_DICTIONARY})

add_library(Stt SHARED ${STT_SRCS})
target_link_libraries(Stt ${ROOT_LIBRARIES})
set_target_properties(Stt PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Stt DESTINATION ${CMAKE_BINARY_DIR}/lib)
