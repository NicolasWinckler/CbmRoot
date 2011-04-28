# Create a library called "libLittrack".

#add_subdirectory(std)
INCLUDE(std/CMakeLists.txt)

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${GEANT3_INCLUDE_DIR}
#${CLHEP_INCLUDE_DIR}
${SIMPATH}/basics/boost/include
${CBMROOT_SOURCE_DIR}/littrack
${CBMROOT_SOURCE_DIR}/littrack/std
${CBMROOT_SOURCE_DIR}/littrack/cbm
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
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/trackbase
${CBMROOT_SOURCE_DIR}/geane
${CBMROOT_SOURCE_DIR}/field
${CBMROOT_SOURCE_DIR}/much
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/global
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/rich
${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
)

set(TBB_FOUND FALSE)
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

##cbm/base/CbmLitEnvironment.cxx  
##cbm/base/CbmLitToolFactory.cxx
##cbm/base/CbmLitFieldFitter.cxx
##cbm/base/CbmLitSimpleGeometryConstructor.cxx
cbm/base/CbmLitTask.cxx

cbm/elid/CbmLitGlobalElectronId.cxx

##cbm/finder/CbmLitTrdTrackFinderStandalone.cxx
##cbm/finder/CbmLitFindGlobalTracks.cxx 
cbm/finder/CbmLitFindMvdTracks.cxx

cbm/geo/CbmLitDet.cxx
cbm/geo/CbmLitDetContFact.cxx 
cbm/geo/CbmLitDetGeo.cxx 
cbm/geo/CbmLitDetGeoPar.cxx
cbm/geo/CbmLitDetPoint.cxx 

cbm/propagation/CbmPropagatorGeane.cxx
cbm/propagation/CbmPropagatorLit.cxx

##cbm/qa/CbmLitReconstructionQa.cxx
##cbm/qa/CbmLitFindGlobalTracksIdeal.cxx 
cbm/qa/CbmLitCheckMultipleScattering.cxx
##cbm/qa/CbmLitPropagationAnalysis.cxx 
##cbm/qa/CbmLitRobustFitterAnalysis.cxx
cbm/qa/CbmLitCheckSimpleGeo.cxx 
##cbm/qa/CbmLitCheckField.cxx
cbm/qa/CbmLitCheckEnergyLossMuons.cxx
cbm/qa/CbmLitCheckBrem.cxx 
cbm/qa/CbmTrdHitsQa.cxx
cbm/qa/CbmLitCreateStandaloneData.cxx
cbm/qa/CbmLitTestMatrixMath.cxx

##cbm/utils/CbmLitConverter.cxx
cbm/utils/CbmLitDrawHist.cxx

cbm/parallel/CbmLitTrackFinderNNParallel.cxx
cbm/parallel/CbmLitParallelTrackFitterTest.cxx 
cbm/parallel/CbmLitParallelTrackFitterTestElectron.cxx 
)

If(TBB_FOUND)
  Set(LITTRACK_SRCS_RULECHECKER 
      ${LITTRACK_SRCS_RULECHECKER}
      cbm/parallel/CbmLitTrackFitSpeedTest.cxx
     )
EndIf(TBB_FOUND)

set(LITTRACK_HEADERS
cbm/base/CbmLitTask.h
cbm/elid/CbmLitGlobalElectronId.h
#cbm/finder/CbmLitTrdTrackFinderStandalone.h
cbm/finder/CbmLitFindGlobalTracks.h
#cbm/finder/CbmLitCudaTest.h 
cbm/finder/CbmLitFindMvdTracks.h
cbm/geo/CbmLitDet.h 
cbm/geo/CbmLitDetContFact.h
cbm/geo/CbmLitDetGeo.h 
cbm/geo/CbmLitDetGeoPar.h 
cbm/geo/CbmLitDetPoint.h
cbm/propagation/CbmPropagatorGeane.h
cbm/propagation/CbmPropagatorLit.h
cbm/qa/CbmLitReconstructionQa.h 
cbm/qa/CbmLitFindGlobalTracksIdeal.h
cbm/qa/CbmLitPropagationAnalysis.h 
#cbm/qa/CbmLitRobustFitterAnalysis.h
cbm/qa/CbmLitCheckSimpleGeo.h 
cbm/qa/CbmLitCheckField.h
cbm/qa/CbmLitCheckEnergyLossMuons.h 
cbm/qa/CbmLitCheckMultipleScattering.h
cbm/qa/CbmLitCheckBrem.h 
cbm/qa/CbmTrdHitsQa.h
cbm/qa/CbmLitCreateStandaloneData.h
cbm/qa/CbmLitTestMatrixMath.h
#cuda/CudaTest.h
cbm/utils/CbmLitDrawHist.h
)

If(TBB_FOUND)
  Set(LITTRACK_HEADERS 
      ${LITTRACK_HEADERS}
      cbm/parallel/CbmLitTrackFitSpeedTest.h
     )
EndIf(TBB_FOUND)

SET(LITTRACK_SRCS 
${LITTRACK_SRCS_RULECHECKER}
${LITTRACKSTD_SRCS}
cbm/base/CbmLitEnvironment.cxx
cbm/base/CbmLitFieldFitter.cxx
cbm/base/CbmLitToolFactory.cxx
cbm/base/CbmLitSimpleGeometryConstructor.cxx
#cbm/finder/CbmLitTrdTrackFinderStandalone.cxx
cbm/finder/CbmLitFindGlobalTracks.cxx 
cbm/qa/CbmLitReconstructionQa.cxx
cbm/qa/CbmLitFindGlobalTracksIdeal.cxx 
cbm/qa/CbmLitCheckField.cxx
cbm/qa/CbmLitPropagationAnalysis.cxx 
#cbm/qa/CbmLitRobustFitterAnalysis.cxx
cbm/utils/CbmLitConverter.cxx
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

install(TARGETS Littrack DESTINATION ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(parallel)
#add_subdirectory(cuda)
