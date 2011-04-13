# Create a library called "libLittrack" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${GEANT3_INCLUDE_DIR}
#${CLHEP_INCLUDE_DIR}
${SIMPATH}/basics/boost/include
${CBMROOT_SOURCE_DIR}/littrack
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
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/trackbase
${CBMROOT_SOURCE_DIR}/geane
${CBMROOT_SOURCE_DIR}/field
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/sts
)

#set(TBB_FOUND TRUE)
#set(TBB_INCLUDE_DIRS "/u/andrey/soft/tbb/tbb22_009oss/include")
#set(TBB_LIBRARIES "/u/andrey/soft/tbb/Lenny64/libtbb.so")

If(TBB_FOUND)
  Set(INCLUDE_DIRECTORIES 
      ${INCLUDE_DIRECTORIES}
      ${TBB_INCLUDE_DIRS}
     )
EndIf(TBB_FOUND)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
If(TBB_FOUND)
  Set(LINK_DIRECTORIES
      ${LINK_DIRECTORIES} 
      ${TBB_LIBRARY_DIRS}
     )
EndIf(TBB_FOUND)

link_directories( ${LINK_DIRECTORIES})

set(LITTRACK_SRCS_RULECHECKER
base/CbmLitDetectorLayout.cxx
base/CbmLitPDG.cxx
##base/CbmLitEnvironment.cxx  
##base/CbmLitToolFactory.cxx
base/CbmLitHitData.cxx
##base/CbmLitFieldFitter.cxx
##base/CbmLitSimpleGeometryConstructor.cxx
base/CbmLitTask.cxx
cbm/CbmPropagatorGeane.cxx
cbm/CbmPropagatorLit.cxx
data/CbmLitTrackParam.cxx
data/CbmLitTrack.cxx
#data/CbmLitTrackBranch.cxx
finder/CbmLitTrackFinderWeight.cxx
finder/CbmLitTrackFinderBase.cxx
finder/CbmLitTrackFinderBranch.cxx
finder/CbmLitTrackFinderNN.cxx
finder/CbmLitNearestHitToTrackMerger.cxx
finder/CbmLitGating.cxx
#finder/CbmLitTrackFinderNNCuda.cxx
##finder_cbm/CbmLitTrdTrackFinderStandalone.cxx
##finder_cbm/CbmLitFindGlobalTracks.cxx 
finder_cbm/CbmLitFindMvdTracks.cxx
fitter/CbmLitTrackFitterImp.cxx 
fitter/CbmLitKalmanFilter.cxx
fitter/CbmLitTrackFitterWeight.cxx 
fitter/CbmLitKalmanSmoother.cxx
fitter/CbmLitTrackFitterIter.cxx 
geo/CbmLitDet.cxx
geo/CbmLitDetContFact.cxx 
geo/CbmLitDetGeo.cxx 
geo/CbmLitDetGeoPar.cxx
geo/CbmLitDetPoint.cxx 
propagation/CbmLitTGeoTrackPropagator.cxx 
propagation/CbmLitRK4TrackExtrapolator.cxx
propagation/CbmLitLineTrackExtrapolator.cxx
propagation/CbmLitTrackPropagatorGeane.cxx
propagation/CbmLitTGeoNavigator.cxx
##propagation/CbmLitMaterialEffectsImp.cxx
propagation/CbmLitCleverTrackExtrapolator.cxx
propagation/CbmLitMyGeoNavigator.cxx
propagation/CbmLitMyTrackPropagator.cxx 
##qa/CbmLitReconstructionQa.cxx
##qa/CbmLitFindGlobalTracksIdeal.cxx 
qa/CbmLitCheckMultipleScattering.cxx
##qa/CbmLitPropagationAnalysis.cxx 
##qa/CbmLitRobustFitterAnalysis.cxx
qa/CbmLitCheckSimpleGeo.cxx 
##qa/CbmLitCheckField.cxx
qa/CbmLitCheckEnergyLossMuons.cxx
qa/CbmLitCheckBrem.cxx 
qa/CbmTrdHitsQa.cxx
qa/CbmLitCreateStandaloneData.cxx
qa/CbmLitTestMatrixMath.cxx
weight/CbmLitWeightedHitCalculatorImp.cxx
weight/CbmLitWeightCalculatorSimple.cxx
weight/CbmLitWeightCalculatorGauss.cxx
#weight/CbmLitWeightCalculatorOptimal.cxx
weight/CbmLitWeightCalculatorTukey.cxx
selection/CbmLitTrackSelectionEmpty.cxx
selection/CbmLitTrackSelectionShortTracks.cxx 
selection/CbmLitTrackSelectionSameSeed.cxx
selection/CbmLitTrackSelectionSharedHits.cxx 
#selection/CbmLitTrackSelectionD.cxx
#selection/CbmLitTrackSelectionMomentum.cxx
selection/CbmLitTrackSelectionTrd.cxx
selection/CbmLitTrackSelectionMuch.cxx
selection/CbmLitTrackSelectionMuchRobust.cxx
#selection/CbmLitTrackSelectionChiSq.cxx 
selection/CbmLitQualitySort.cxx
selection/CbmLitTrackSelectionCuts.cxx
##utils/CbmLitConverter.cxx
utils/CbmLitMath.cxx 
utils/CbmLitMatrixMath.cxx 
utils/CbmLitDrawHist.cxx
utils/CbmLitUtils.cxx)

If(TBB_FOUND)
  Set(LITTRACK_SRCS_RULECHECKER 
      ${LITTRACK_SRCS_RULECHECKER}
      finder/CbmLitTrackFinderNNParallel.cxx
      qa/CbmLitParallelTrackFitterTest.cxx 
      qa/CbmLitParallelTrackFitterTestElectron.cxx 
      qa/CbmLitTrackFitSpeedTest.cxx
     )
EndIf(TBB_FOUND)

set(LITTRACK_HEADERS
base/CbmLitTask.h
cbm/CbmPropagatorGeane.h
cbm/CbmPropagatorLit.h
#finder_cbm/CbmLitTrdTrackFinderStandalone.h
finder_cbm/CbmLitFindGlobalTracks.h
#finder_cbm/CbmLitCudaTest.h 
finder_cbm/CbmLitFindMvdTracks.h
geo/CbmLitDet.h 
geo/CbmLitDetContFact.h
geo/CbmLitDetGeo.h 
geo/CbmLitDetGeoPar.h 
geo/CbmLitDetPoint.h
qa/CbmLitReconstructionQa.h 
qa/CbmLitFindGlobalTracksIdeal.h
qa/CbmLitPropagationAnalysis.h 
#qa/CbmLitRobustFitterAnalysis.h
qa/CbmLitCheckSimpleGeo.h 
qa/CbmLitCheckField.h
qa/CbmLitCheckEnergyLossMuons.h 
qa/CbmLitCheckMultipleScattering.h
qa/CbmLitCheckBrem.h 
qa/CbmTrdHitsQa.h
qa/CbmLitCreateStandaloneData.h
qa/CbmLitTestMatrixMath.h
#cuda/CudaTest.h
)

If(TBB_FOUND)
  Set(LITTRACK_HEADERS 
      ${LITTRACK_HEADERS}
      qa/CbmLitTrackFitSpeedTest.h
     )
EndIf(TBB_FOUND)

SET(LITTRACK_SRCS 
${LITTRACK_SRCS_RULECHECKER}
base/CbmLitEnvironment.cxx
base/CbmLitFieldFitter.cxx
base/CbmLitToolFactory.cxx
base/CbmLitSimpleGeometryConstructor.cxx
#finder_cbm/CbmLitTrdTrackFinderStandalone.cxx
finder_cbm/CbmLitFindGlobalTracks.cxx 
propagation/CbmLitMaterialEffectsImp.cxx
qa/CbmLitReconstructionQa.cxx
qa/CbmLitFindGlobalTracksIdeal.cxx 
qa/CbmLitCheckField.cxx
qa/CbmLitPropagationAnalysis.cxx 
#qa/CbmLitRobustFitterAnalysis.cxx
utils/CbmLitConverter.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LITTRACK_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" LITTRACK_RULES)
endif(RULE_CHECKER_FOUND)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${LITTRACK_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3 ")
  Message(STATUS "LitTrack will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "LitTrack will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${LITTRACK_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3 ")
ENDIF (SSE_FOUND)


set(LITTRACK_LINKDEF  LittrackLinkDef.h)
set(LITTRACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmLittrackDict.cxx) 

ROOT_GENERATE_DICTIONARY("${LITTRACK_HEADERS}" "${LITTRACK_LINKDEF}" "${LITTRACK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(LITTRACK_SRCS ${LITTRACK_SRCS} ${LITTRACK_DICTIONARY})

add_library(Littrack SHARED ${LITTRACK_SRCS})
target_link_libraries(Littrack ${ROOT_LIBRARIES})
set_target_properties(Littrack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

If(TBB_FOUND)
  target_link_libraries(Littrack
        ${TBB_LIBRARIES}
  )
EndIf(TBB_FOUND)

################ install ###################
install(TARGETS Littrack DESTINATION ${CMAKE_BINARY_DIR}/lib)


#add_subdirectory(parallel)
#add_subdirectory(cuda)
