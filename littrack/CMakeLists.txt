# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
#${CLHEP_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/field
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/trd 
${CBMROOT_SOURCE_DIR}/sts 
${CBMROOT_SOURCE_DIR}/rich 
${CBMROOT_SOURCE_DIR}/littrack
${CBMROOT_SOURCE_DIR}/littrack/ana
${CBMROOT_SOURCE_DIR}/littrack/base
${CBMROOT_SOURCE_DIR}/littrack/data
${CBMROOT_SOURCE_DIR}/littrack/finder
${CBMROOT_SOURCE_DIR}/littrack/fitter
${CBMROOT_SOURCE_DIR}/littrack/interface
${CBMROOT_SOURCE_DIR}/littrack/propagation
${CBMROOT_SOURCE_DIR}/littrack/qa
${CBMROOT_SOURCE_DIR}/littrack/robust
${CBMROOT_SOURCE_DIR}/littrack/selection
${CBMROOT_SOURCE_DIR}/littrack/utils
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/trackbase
${CBMROOT_SOURCE_DIR}/geane
#/u/andrey/soft/boost/boost_1_35_0
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(LITTRACK_SRCS_RULECHECKER
ana/CbmLitElossAna.cxx
ana/CbmLitMuchAna.cxx
ana/CbmLitRobustAna.cxx
base/CbmLitTool.cxx
base/CbmLitDetectorLayout.cxx
base/CbmLitPDG.cxx
data/CbmLitHit.cxx
data/CbmLitTrackParam.cxx
data/CbmLitTrack.cxx
data/CbmLitFitNode.cxx
#finder/CbmLitMuchTrackFinderTrigger.cxx
finder/CbmLitTrackFinderImp.cxx
finder/CbmLitTrackFinderBranch.cxx
finder/CbmLitTrackFinderRobust.cxx
finder/CbmLitTrdTrackFinderSts.cxx
finder/CbmLitTrdTrackFinderS.cxx
finder/CbmLitMuchTrackFinderBranch.cxx
finder/CbmLitMuchTrackFinderRobust.cxx
fitter/CbmLitTrackFitterImp.cxx
fitter/CbmLitKalmanFilter.cxx
fitter/CbmLitTrackFitterRobust.cxx
fitter/CbmLitKalmanSmoother.cxx
interface/CbmLitTrackFitter.cxx   
interface/CbmLitTrackPropagator.cxx 
interface/CbmLitTrackUpdate.cxx  
interface/CbmLitTrackExtrapolator.cxx
interface/CbmLitTrackSelection.cxx
interface/CbmLitTrackFinder.cxx
interface/CbmLitWeightCalculator.cxx
interface/CbmLitEffHitCalculator.cxx
propagation/CbmLitTrackPropagatorImp.cxx 
propagation/CbmLitRK4TrackExtrapolator.cxx
propagation/CbmLitLineTrackExtrapolator.cxx
propagation/CbmTrackPropagatorGeane.cxx
propagation/CbmLitGeoNavigatorImp.cxx
propagation/CbmLitMaterialInfo.cxx
propagation/CbmLitMaterialEffectsImp.cxx
qa/CbmLitMuchRecQa.cxx
qa/CbmLitTrdRecQa.cxx
robust/CbmLitEffHitCalculatorImp.cxx
robust/CbmLitWeightCalculatorSimple.cxx
robust/CbmLitRobustSelection.cxx
selection/CbmLitTrackSelectionEmpty.cxx
selection/CbmLitTrackSelectionA.cxx
selection/CbmLitTrackSelectionB.cxx
selection/CbmLitTrackSelectionC.cxx
selection/CbmLitTrackSelectionD.cxx
selection/CbmLitTrackSelectionMomentum.cxx
selection/CbmLitTrackSelectionTrd.cxx
selection/CbmLitTrackSelectionMuch.cxx
selection/CbmLitTrackSelectionMuchRobust.cxx
utils/CbmLitComparators.cxx
utils/CbmLitMemoryManagment.cxx
utils/CbmLitConverter.cxx
utils/CbmLitTypes.cxx
utils/CbmLitMath.cxx
CbmLitEnvironment.cxx  
CbmLitToolFactory.cxx
#CbmLitTestGm.cxx
)

SET(LITTRACK_SRCS ${LITTRACK_SRCS_RULECHECKER})
#base/CbmProperty.cxx)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LITTRACK_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" LITTRACK_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LITTRACK_HEADERS "${LITTRACK_SRCS}")

set(LITTRACK_LINKDEF  LittrackLinkDef.h)
set(LITTRACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmLittrackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${LITTRACK_HEADERS}" "${LITTRACK_LINKDEF}" "${LITTRACK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(LITTRACK_SRCS ${LITTRACK_SRCS} ${LITTRACK_DICTIONARY})

add_library(Littrack SHARED ${LITTRACK_SRCS})
target_link_libraries(Littrack ${ROOT_LIBRARIES})
set_target_properties(Littrack PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Littrack DESTINATION ${CMAKE_BINARY_DIR}/lib)
