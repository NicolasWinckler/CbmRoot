# Create a library called "libLittrack" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
#${CLHEP_INCLUDE_DIR}
${SIMPATH}/basics/boost/include
${CBMROOT_SOURCE_DIR}/littrack
${CBMROOT_SOURCE_DIR}/littrack/base
${CBMROOT_SOURCE_DIR}/littrack/cbm
${CBMROOT_SOURCE_DIR}/littrack/data
${CBMROOT_SOURCE_DIR}/littrack/finder
${CBMROOT_SOURCE_DIR}/littrack/fitter
${CBMROOT_SOURCE_DIR}/littrack/interface
${CBMROOT_SOURCE_DIR}/littrack/propagation
${CBMROOT_SOURCE_DIR}/littrack/qa
${CBMROOT_SOURCE_DIR}/littrack/weight
${CBMROOT_SOURCE_DIR}/littrack/selection
${CBMROOT_SOURCE_DIR}/littrack/utils
${CBMROOT_SOURCE_DIR}/littrack/geo
${CBMROOT_SOURCE_DIR}/geobase
${CBMROOT_SOURCE_DIR}/parbase
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/trackbase
${CBMROOT_SOURCE_DIR}/geane
${CBMROOT_SOURCE_DIR}/field
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/sts
#/u/andrey/soft/tbb/tbb22_009oss/include 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(LITTRACK_SRCS_RULECHECKER
base/CbmLitDetectorLayout.cxx
base/CbmLitPDG.cxx
base/CbmLitEnvironment.cxx  
base/CbmLitToolFactory.cxx
base/CbmLitHitData.cxx
base/CbmLitFieldFitter.cxx
base/CbmLitSimpleGeometryConstructor.cxx
base/CbmLitMyField.cxx
cbm/CbmPropagatorGeane.cxx
cbm/CbmPropagatorLit.cxx
data/CbmLitTrackParam.cxx
data/CbmLitTrack.cxx
data/CbmLitTrackBranch.cxx
finder/CbmLitTrackFinderWeight.cxx
finder/CbmLitTrackFinderBase.cxx
finder/CbmLitTrackFinderBranch.cxx
finder/CbmLitTrackFinderNN.cxx
finder/CbmLitNearestHitToTrackMerger.cxx
finder/CbmLitGating.cxx
#finder/CbmLitTrackFinderNNParallel.cxx
#finder/CbmLitTrackFinderNNCuda.cxx
finder_cbm/CbmLitTrdTrackFinderBranch.cxx
finder_cbm/CbmLitTrdTrackFinderNN.cxx
finder_cbm/CbmLitTrdTrackFinderWeight.cxx
finder_cbm/CbmLitTrdTrackFinderStandalone.cxx
#finder_cbm/CbmLitMuchTrackFinderBranch.cxx
#finder_cbm/CbmLitMuchTrackFinderNN.cxx
#finder_cbm/CbmLitMuchTrackFinderWeight.cxx
finder_cbm/CbmLitStsBasedTrackFinder.cxx
finder_cbm/CbmLitFindGlobalTracks.cxx 
#finder_cbm/CbmLitCudaTest.cxx
fitter/CbmLitTrackFitterImp.cxx 
fitter/CbmLitKalmanFilter.cxx
fitter/CbmLitTrackFitterRobust.cxx 
fitter/CbmLitKalmanSmoother.cxx
fitter/CbmLitTrackFitterIter.cxx 
geo/CbmLitDet.cxx
geo/CbmLitDetContFact.cxx 
geo/CbmLitDetGeo.cxx 
geo/CbmLitDetGeoPar.cxx
geo/CbmLitDetPoint.cxx 
#parallel/LitTrackFinderNNParallel.cxx
propagation/CbmLitTGeoTrackPropagator.cxx 
propagation/CbmLitRK4TrackExtrapolator.cxx
propagation/CbmLitLineTrackExtrapolator.cxx
propagation/CbmLitTrackPropagatorGeane.cxx
propagation/CbmLitTGeoNavigator.cxx
propagation/CbmLitMaterialEffectsImp.cxx
propagation/CbmLitCleverTrackExtrapolator.cxx
propagation/CbmLitMyGeoNavigator.cxx
propagation/CbmLitMyTrackPropagator.cxx 
qa/CbmLitReconstructionQa.cxx
qa/CbmLitFindGlobalTracksIdeal.cxx 
qa/CbmLitCheckMultipleScattering.cxx
qa/CbmLitPropagationAnalysis.cxx 
qa/CbmLitRobustFitterAnalysis.cxx
qa/CbmLitCheckSimpleGeo.cxx 
qa/CbmLitCheckField.cxx
#qa/CbmLitParallelTrackFitterTest.cxx 
qa/CbmLitCheckEnergyLoss.cxx
qa/CbmLitCheckBrem.cxx 
#qa/CbmLitTrackFitSpeedTest.cxx
weight/CbmLitWeightedHitCalculatorImp.cxx
weight/CbmLitWeightCalculatorSimple.cxx
weight/CbmLitWeightCalculatorGauss.cxx
#weight/CbmLitWeightCalculatorOptimal.cxx
weight/CbmLitWeightCalculatorTukey.cxx
selection/CbmLitTrackSelectionEmpty.cxx
selection/CbmLitTrackSelectionA.cxx 
selection/CbmLitTrackSelectionB.cxx
selection/CbmLitTrackSelectionC.cxx 
selection/CbmLitTrackSelectionD.cxx
selection/CbmLitTrackSelectionMomentum.cxx
selection/CbmLitTrackSelectionTrd.cxx
selection/CbmLitTrackSelectionMuch.cxx
selection/CbmLitTrackSelectionMuchRobust.cxx
selection/CbmLitTrackSelectionChiSq.cxx 
utils/CbmLitConverter.cxx
utils/CbmLitMath.cxx 
utils/CbmLitMatrixMath.cxx )

set(LITTRACK_HEADERS
cbm/CbmPropagatorGeane.h
cbm/CbmPropagatorLit.h
finder_cbm/CbmLitTrdTrackFinderBranch.h
finder_cbm/CbmLitTrdTrackFinderNN.h
finder_cbm/CbmLitTrdTrackFinderWeight.h
finder_cbm/CbmLitTrdTrackFinderStandalone.h
#finder_cbm/CbmLitMuchTrackFinderBranch.h
#finder_cbm/CbmLitMuchTrackFinderNN.h
#finder_cbm/CbmLitMuchTrackFinderWeight.h
finder_cbm/CbmLitStsBasedTrackFinder.h
finder_cbm/CbmLitFindGlobalTracks.h
#finder_cbm/CbmLitCudaTest.h 
geo/CbmLitDet.h 
geo/CbmLitDetContFact.h
geo/CbmLitDetGeo.h 
geo/CbmLitDetGeoPar.h 
geo/CbmLitDetPoint.h
qa/CbmLitReconstructionQa.h 
qa/CbmLitFindGlobalTracksIdeal.h
qa/CbmLitPropagationAnalysis.h 
qa/CbmLitRobustFitterAnalysis.h
qa/CbmLitCheckSimpleGeo.h 
qa/CbmLitCheckField.h
qa/CbmLitCheckEnergyLoss.h 
qa/CbmLitCheckMultipleScattering.h
qa/CbmLitCheckBrem.h 
#qa/CbmLitTrackFitSpeedTest.h
)


SET(LITTRACK_SRCS ${LITTRACK_SRCS_RULECHECKER})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LITTRACK_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" LITTRACK_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h LITTRACK_HEADERS "${LITTRACK_SRCS}")



# Check if SSE instructions are available on the machine where 
# the project is compiled.

IF(CMAKE_SYSTEM_NAME MATCHES Linux)
  EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)
  STRING(REGEX REPLACE "^.*(sse).*$" "\\1" SSE_THERE ${CPUINFO})
  STRING(COMPARE EQUAL "sse" "${SSE_THERE}" SSE_TRUE)
ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)
  IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
    EXEC_PROGRAM("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE CPUINFO)
    STRING(REGEX REPLACE "^.*(SSE).*$" "\\1" SSE_THERE ${CPUINFO})
    STRING(COMPARE EQUAL "SSE" "${SSE_THERE}" SSE_TRUE)
  ENDIF(CMAKE_SYSTEM_NAME MATCHES Darwin)
ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)

IF (SSE_TRUE)
  MESSAGE(STATUS "SSE extensions available")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${LITTRACK_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_TRUE)
  MESSAGE (STATUS "*********************************************************************************")
  MESSAGE(STATUS "NO SSE extensions available.")
  MESSAGE (STATUS "*********************************************************************************")
  SET_SOURCE_FILES_PROPERTIES(${LITTRACK_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_TRUE)


set(LITTRACK_LINKDEF  LittrackLinkDef.h)
set(LITTRACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmLittrackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${LITTRACK_HEADERS}" "${LITTRACK_LINKDEF}" "${LITTRACK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(LITTRACK_SRCS ${LITTRACK_SRCS} ${LITTRACK_DICTIONARY})

add_library(Littrack SHARED ${LITTRACK_SRCS})
target_link_libraries(Littrack ${ROOT_LIBRARIES})
set_target_properties(Littrack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Littrack DESTINATION ${CMAKE_BINARY_DIR}/lib)
