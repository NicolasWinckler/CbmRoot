# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/passive
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PASSIVE_SRCS
CbmCave.cxx            
CbmGeoCave.cxx         
CbmGeoMagnet.cxx       
CbmGeoPassivePar.cxx   
CbmGeoPipe.cxx         
CbmGeoStart.cxx        
CbmGeoTarget.cxx       
CbmMagnet.cxx          
CbmPassiveContFact.cxx 
CbmPipe.cxx   
CbmShield.cxx
CbmShieldGeo.cxx         
CbmTarget.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PASSIVE_SRCS}" "${INCLUDE_DIRECTORIES}" PASSIVE_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PASSIVE_HEADERS "${PASSIVE_SRCS}")

set(PASSIVE_LINKDEF  PassiveLinkDef.h)
set(PASSIVE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmPassiveDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PASSIVE_HEADERS}" "${PASSIVE_LINKDEF}" "${PASSIVE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(PASSIVE_SRCS ${PASSIVE_SRCS} ${PASSIVE_DICTIONARY})


add_library(Passive SHARED ${PASSIVE_SRCS})
target_link_libraries(Passive ${ROOT_LIBRARIES})
set_target_properties(Passive PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Passive DESTINATION ${CMAKE_BINARY_DIR}/lib)
