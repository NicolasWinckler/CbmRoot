# Create a library called "libClustering".

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES}
${GEANT3_INCLUDE_DIR}
${Boost_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/littrack/clustering
${CBMROOT_SOURCE_DIR}/littrack/clustering/qa/clustering
${CBMROOT_SOURCE_DIR}/littrack/std/utils
${CBMROOT_SOURCE_DIR}/geobase
${CBMROOT_SOURCE_DIR}/parbase
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmbase/report
${CBMROOT_SOURCE_DIR}/cbmbase/draw
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
CbmClusteringGeometry.cxx
CbmMuchClustering.cxx
CbmClusteringA1.cxx
CbmClusteringSL.cxx
CbmClusteringWard.cxx
CbmMuchTest.cxx
CbmMvdGeoCl.cxx
CbmMvdClustering.cxx
CbmMvdClusteringNoCharges.cxx
CbmMuchClusteringQa0.cxx
qa/clustering/CbmMuchClusteringQa.cxx
qa/clustering/CbmMuchClusteringQaCalculator.cxx
qa/clustering/CbmMuchClusteringQaDraw.cxx
qa/clustering/CbmMuchClusteringQaHistCreator.cxx
qa/clustering/CbmMuchClusteringQaPTreeCreator.cxx
qa/clustering/CbmMuchClusteringQaReport.cxx
qa/clustering/CbmMuchClusteringQaStudyReport.cxx
)

set(CLUSTERING_HEADERS
#CbmClusteringGeometry.h
CbmMuchClustering.h
#CbmClusteringA1.h
#CbmClusteringSL.h
#CbmClusteringWard.h
CbmMuchTest.h
#CbmMvdGeoCl.h
CbmMvdClustering.h
CbmMvdClusteringNoCharges.h
CbmMuchClusteringQa0.h
qa/clustering/CbmMuchClusteringQa.h
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
If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)
ROOT_GENERATE_DICTIONARY()

SET(CLUSTERING_SRCS ${CLUSTERING_SRCS} ${DICTIONARY})

add_library(Clustering SHARED ${CLUSTERING_SRCS})
target_link_libraries(Clustering ${ROOT_LIBRARIES})
set_target_properties(Clustering PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

install(TARGETS Clustering DESTINATION ${CMAKE_BINARY_DIR}/lib)
