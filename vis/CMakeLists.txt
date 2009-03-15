set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/vis
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/cbmdata/sts 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(VIS_SRCS
CbmVisMuch.cxx
CbmVisPoint.cxx
CbmVisHit.cxx
CbmVisMuchStationFrame.cxx  
CbmVisMuchCluster.cxx 
CbmVisMuchModuleFrame.cxx 
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h VIS_HEADERS "${VIS_SRCS}")

set(VIS_LINKDEF  VisLinkDef.h)
set(VIS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmVisDict.cxx)

ROOT_GENERATE_DICTIONARY("${VIS_HEADERS}" "${VIS_LINKDEF}" "${VIS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(VIS_SRCS ${VIS_SRCS}  ${VIS_DICTIONARY})


add_library(Vis SHARED ${VIS_SRCS})
target_link_libraries(Vis ${ROOT_LIBRARIES})
set_target_properties(Vis PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Vis DESTINATION ${CMAKE_BINARY_DIR}/lib)
