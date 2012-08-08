# Create a library called "libClustering".

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${GEANT3_INCLUDE_DIR}
${Boost_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/littrack/clustering
${CBMROOT_SOURCE_DIR}/geobase
${CBMROOT_SOURCE_DIR}/parbase
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/field
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/passive
${CBMROOT_SOURCE_DIR}/mvd
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(CLUSTERING_SRCS
CbmMuchGeoCl.cxx
CbmMuchClustering.cxx
CbmClusteringA1.cxx
CbmClusteringSL.cxx
CbmClusteringWard.cxx
CbmMuchTest.cxx
CbmMuchClusteringQa.cxx
CbmMvdGeoCl.cxx
CbmMvdClustering.cxx
)

set(CLUSTERING_HEADERS
CbmMuchGeoCl.h
CbmMuchClustering.h
CbmClusteringA1.h
CbmClusteringSL.h
CbmClusteringWard.h
CbmMuchTest.h
CbmMuchClusteringQa.h
CbmMvdGeoCl.h
CbmMvdClustering.h
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CLUSTERING_SRCS}" "${INCLUDE_DIRECTORIES}" CLUSTERING_RULES)
endif(RULE_CHECKER_FOUND)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${CLUSTERING_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3 -w")
  Message(STATUS "Clustering will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Clustering will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${CLUSTERING_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3 -w")
ENDIF (SSE_FOUND)

set(LINKDEF ClusteringLinkDef.h)
set(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmClusteringDict.cxx) 
set(HDRS ${CLUSTERING_HEADERS})

ROOT_GENERATE_DICTIONARY()

SET(CLUSTERING_SRCS ${CLUSTERING_SRCS} ${DICTIONARY})

add_library(Clustering SHARED ${CLUSTERING_SRCS})
target_link_libraries(Clustering ${ROOT_LIBRARIES})
set_target_properties(Clustering PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

install(TARGETS Clustering DESTINATION ${CMAKE_BINARY_DIR}/lib)
