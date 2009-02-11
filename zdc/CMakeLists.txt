# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/zdc
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/cbmbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(ZDC_SRCS
CbmGeoZdc.cxx 
CbmGeoZdcPar.cxx 
CbmZdc.cxx  
CbmZdcPoint.cxx  
CbmZdcv1.cxx 
CbmZdcEvent.cxx
CbmZdcContFact.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${ZDC_SRCS}" "${INCLUDE_DIRECTORIES}" ZDC_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h ZDC_HEADERS "${ZDC_SRCS}")

set(ZDC_LINKDEF ZdcLinkDef.h)
set(ZDC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmZdcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${ZDC_HEADERS}" "${ZDC_LINKDEF}" "${ZDC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(ZDC_SRCS ${ZDC_SRCS} ${ZDC_DICTIONARY})

add_library(Zdc SHARED ${ZDC_SRCS})
target_link_libraries(Zdc ${ROOT_LIBRARIES})
set_target_properties(Zdc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Zdc DESTINATION ${CMAKE_BINARY_DIR}/lib)
