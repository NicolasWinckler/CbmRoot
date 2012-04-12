# Create a library called "libCbmGenerators" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmgenerators
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
#${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(CBMGENERATORS_SRCS
CbmDirectPhotonGenerator.cxx
CbmEcalGenerator.cxx
CbmHsdLvmGenerator.cxx
CbmHsdGenerator.cxx
CbmShieldGenerator.cxx
CbmUnigenGenerator.cxx
CbmUrqmdGenerator.cxx
URun.cxx
UEvent.cxx
UParticle.cxx
CbmIonGenerator.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CBMGENERATORS_SRCS}" "${INCLUDE_DIRECTORIES}" CBMGENERATORS_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h CBMGENERATORS_HEADERS "${CBMGENERATORS_SRCS}")

set(CBMGENERATORS_LINKDEF  CbmGeneratorsLinkDef.h)
set(CBMGENERATORS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmGeneratorsDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CBMGENERATORS_HEADERS}" "${CBMGENERATORS_LINKDEF}" "${CBMGENERATORS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(CBMGENERATORS_SRCS ${CBMGENERATORS_SRCS} ${CBMGENERATORS_DICTIONARY})

add_library(CbmGenerators SHARED ${CBMGENERATORS_SRCS})
target_link_libraries(CbmGenerators ${ROOT_LIBRARIES})
set_target_properties(CbmGenerators PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS CbmGenerators DESTINATION ${CMAKE_BINARY_DIR}/lib)
