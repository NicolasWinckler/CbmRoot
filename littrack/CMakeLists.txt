# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
#${CLHEP_INCLUDE_DIR}
${SIMPATH}/basics/boost/include
${CBMROOT_SOURCE_DIR}/littrack
${CBMROOT_SOURCE_DIR}/littrack/ana
${CBMROOT_SOURCE_DIR}/littrack/base
${CBMROOT_SOURCE_DIR}/littrack/cbm
${CBMROOT_SOURCE_DIR}/littrack/data
${CBMROOT_SOURCE_DIR}/littrack/finder
${CBMROOT_SOURCE_DIR}/littrack/fitter
${CBMROOT_SOURCE_DIR}/littrack/interface
${CBMROOT_SOURCE_DIR}/littrack/propagation
${CBMROOT_SOURCE_DIR}/littrack/qa
${CBMROOT_SOURCE_DIR}/littrack/robust
${CBMROOT_SOURCE_DIR}/littrack/selection
${CBMROOT_SOURCE_DIR}/littrack/utils
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
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/sts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(LITTRACK_SRCS_RULECHECKER
ana/CbmLitElossAna.cxx
ana/CbmLitPropagationAnalysis.cxx
ana/CbmLitRobustFitterAnalysis.cxx
ana/CbmLitCheckSimpleGeo.cxx
base/CbmLitTool.cxx
base/CbmLitSubstation.cxx
base/CbmLitStation.cxx
base/CbmLitStationGroup.cxx
base/CbmLitDetectorLayout.cxx
base/CbmLitPDG.cxx
base/CbmLitEnvironment.cxx  
base/CbmLitToolFactory.cxx
base/CbmLitHitData.cxx
cbm/CbmPropagatorGeane.cxx
cbm/CbmPropagatorLit.cxx
data/CbmLitHit.cxx
data/CbmLitPixelHit.cxx
data/CbmLitStripHit.cxx
data/CbmLitTrackParam.cxx
data/CbmLitTrack.cxx
data/CbmLitTrackBranch.cxx
data/CbmLitFitNode.cxx
finder/CbmLitTrackFinderWeight.cxx
finder/CbmLitTrackFinderBase.cxx
finder/CbmLitTrackFinderBranch.cxx
finder/CbmLitTrackFinderNN.cxx
finder/CbmLitNearestHitToTrackMerger.cxx
finder/CbmLitGating.cxx
finder_cbm/CbmLitTrdTrackFinderBranch.cxx
finder_cbm/CbmLitTrdTrackFinderNN.cxx
finder_cbm/CbmLitTrdTrackFinderWeight.cxx
finder_cbm/CbmLitTrdTrackFinderStandalone.cxx
finder_cbm/CbmLitMuchTrackFinderBranch.cxx
finder_cbm/CbmLitMuchTrackFinderNN.cxx
finder_cbm/CbmLitMuchTrackFinderWeight.cxx
finder_cbm/CbmLitStsBasedTrackFinder.cxx
finder_cbm/CbmLitTofMerger.cxx
finder_cbm/CbmLitFindGlobalTracks.cxx
fitter/CbmLitTrackFitterImp.cxx
fitter/CbmLitKalmanFilter.cxx
fitter/CbmLitTrackFitterRobust.cxx
fitter/CbmLitKalmanSmoother.cxx
fitter/CbmLitTrackFitterIter.cxx
interface/CbmLitGeoNavigator.cxx
interface/CbmLitMaterialEffects.cxx
interface/CbmLitTrackFitter.cxx   
interface/CbmLitTrackPropagator.cxx 
interface/CbmLitTrackUpdate.cxx  
interface/CbmLitTrackExtrapolator.cxx
interface/CbmLitTrackSelection.cxx
interface/CbmLitTrackFinder.cxx
interface/CbmLitWeightCalculator.cxx
interface/CbmLitWeightedHitCalculator.cxx
interface/CbmLitHitToTrackMerger.cxx
propagation/CbmLitTGeoTrackPropagator.cxx 
propagation/CbmLitRK4TrackExtrapolator.cxx
propagation/CbmLitLineTrackExtrapolator.cxx
propagation/CbmLitTrackPropagatorGeane.cxx
propagation/CbmLitTGeoNavigator.cxx
propagation/CbmLitMaterialInfo.cxx
propagation/CbmLitMaterialEffectsImp.cxx
propagation/CbmLitCleverTrackExtrapolator.cxx
propagation/CbmLitSimpleGeometryConstructor.cxx
propagation/CbmLitMyGeoNavigator.cxx
propagation/CbmLitMyTrackPropagator.cxx
qa/CbmLitReconstructionQa.cxx
qa/CbmLitFindGlobalTracksIdeal.cxx
qa/CbmLitRecQa.cxx
robust/CbmLitWeightedHitCalculatorImp.cxx
robust/CbmLitWeightCalculatorSimple.cxx
robust/CbmLitWeightCalculatorGauss.cxx
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
)

SET(LITTRACK_SRCS ${LITTRACK_SRCS_RULECHECKER})

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
set_target_properties(Littrack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Littrack DESTINATION ${CMAKE_BINARY_DIR}/lib)
