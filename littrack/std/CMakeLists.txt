
set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
#${SIMPATH}/basics/boost/include
#${CBMROOT_SOURCE_DIR}/littrack
#${CBMROOT_SOURCE_DIR}/littrack/utils
${ROOT_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/littrack/std
)

include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(STD_DIR "std/")
#set(STD_DIR "")

set(LITTRACKSTD_SRCS_RULECHECKER

${STD_DIR}base/CbmLitDetectorLayout.cxx
${STD_DIR}base/CbmLitHitData.cxx

${STD_DIR}finder/CbmLitTrackFinderBase.cxx
${STD_DIR}finder/CbmLitTrackFinderBranch.cxx
${STD_DIR}finder/CbmLitTrackFinderNN.cxx
${STD_DIR}finder/CbmLitGating.cxx
${STD_DIR}finder/CbmLitNearestHitTofMerger.cxx

${STD_DIR}fitter/CbmLitTrackFitterImp.cxx 
${STD_DIR}fitter/CbmLitKalmanFilter.cxx
${STD_DIR}fitter/CbmLitKalmanSmoother.cxx
${STD_DIR}fitter/CbmLitTrackFitterIter.cxx 

${STD_DIR}propagation/CbmLitTGeoTrackPropagator.cxx 
${STD_DIR}propagation/CbmLitRK4TrackExtrapolator.cxx
${STD_DIR}propagation/CbmLitLineTrackExtrapolator.cxx
#${STD_DIR}propagation/CbmLitTrackPropagatorGeane.cxx
${STD_DIR}propagation/CbmLitTGeoNavigator.cxx
##${STD_DIR}propagation/CbmLitMaterialEffectsImp.cxx
${STD_DIR}propagation/CbmLitCleverTrackExtrapolator.cxx

${STD_DIR}selection/CbmLitQualitySort.cxx
${STD_DIR}selection/CbmLitTrackSelectionCuts.cxx
${STD_DIR}selection/CbmLitTrackSelectionEmpty.cxx
${STD_DIR}selection/CbmLitTrackSelectionMuch.cxx
${STD_DIR}selection/CbmLitTrackSelectionSameSeed.cxx
${STD_DIR}selection/CbmLitTrackSelectionSharedHits.cxx 
${STD_DIR}selection/CbmLitTrackSelectionShortTracks.cxx 
${STD_DIR}selection/CbmLitTrackSelectionTrd.cxx

${STD_DIR}utils/CbmLitMath.cxx 
${STD_DIR}utils/CbmLitUtils.cxx
${STD_DIR}utils/CbmLitMatrixMath.cxx 
)

set(LITTRACKSTD_HEADERS

)

SET(LITTRACKSTD_SRCS 
${LITTRACKSTD_SRCS_RULECHECKER}
${STD_DIR}propagation/CbmLitMaterialEffectsImp.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LITTRACKSTD_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" LITTRACKSTD_RULES)
endif(RULE_CHECKER_FOUND)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${LITTRACKSTD_SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3 ")
  Message(STATUS "Littrackstd will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Littrackstd will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${LITTRACKSTD_SRCS} PROPERTIES COMPILE_FLAGS "-O3 ")
ENDIF (SSE_FOUND)

#add_library(Littrackstd SHARED ${LITTRACKSTD_SRCS})
#target_link_libraries(Littrackstd ${ROOT_LIBRARIES})
#set_target_properties(Littrackstd PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
#install(TARGETS Littrackstd DESTINATION ${CMAKE_BINARY_DIR}/lib)
