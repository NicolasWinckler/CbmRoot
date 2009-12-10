# Create a library called "libTrd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/eventdisplay

${CBMROOT_SOURCE_DIR}/cbmdisplay 
${CBMROOT_SOURCE_DIR}/cbmdata/trd

${CBMROOT_SOURCE_DIR}/trd
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(CBMDISPLAY_SRCS
CbmTrdDigiDraw.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CBMDISPLAY_SRCS}" "${INCLUDE_DIRECTORIES}" CBMDISPLAY_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h CBMDISPLAY_HEADERS "${CBMDISPLAY_SRCS}")

set(CBMDISPLAY_LINKDEF CbmDisplayLinkDef.h)
set(CBMDISPLAY_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmDisplayDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CBMDISPLAY_HEADERS}" "${CBMDISPLAY_LINKDEF}" "${CBMDISPLAY_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(CBMDISPLAY_SRCS ${CBMDISPLAY_SRCS} ${CBMDISPLAY_DICTIONARY})

add_library(CbmDisplay SHARED ${CBMDISPLAY_SRCS})
target_link_libraries(CbmDisplay ${ROOT_LIBRARIES})
set_target_properties(CbmDisplay PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS CbmDisplay DESTINATION ${CMAKE_BINARY_DIR}/lib)
