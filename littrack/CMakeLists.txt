# Create a library called "libLittrack".

add_subdirectory(parallel)
#add_subdirectory(std)
INCLUDE(std/CMakeLists.txt)

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${GEANT3_INCLUDE_DIR}
${Boost_INCLUDE_DIR}
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

cbm/base/CbmLitTask.cxx

cbm/elid/CbmLitGlobalElectronId.cxx

cbm/finder/CbmLitFindMvdTracks.cxx

cbm/generators/CbmLitPolarizedGenerator.cxx

cbm/geo/CbmLitDet.cxx
cbm/geo/CbmLitDetContFact.cxx 
cbm/geo/CbmLitDetGeo.cxx 
cbm/geo/CbmLitDetGeoPar.cxx
cbm/geo/CbmLitDetPoint.cxx 

cbm/propagation/CbmPropagatorGeane.cxx
cbm/propagation/CbmPropagatorLit.cxx

cbm/qa/geo/CbmLitSimpleGeoQa.cxx 
cbm/qa/material/CbmLitCheckMultipleScattering.cxx
cbm/qa/material/CbmLitCheckEnergyLossMuons.cxx
cbm/qa/material/CbmLitCheckBrem.cxx 
cbm/qa/mc/CbmLitMCTrack.cxx
cbm/qa/mc/CbmLitMCTrackCreator.cxx
cbm/qa/CbmTrdHitsQa.cxx
cbm/qa/CbmLitCreateStandaloneData.cxx
cbm/qa/CbmLitTestMatrixMath.cxx

cbm/utils/CbmLitDrawHist.cxx

cbm/parallel/CbmLitTrackFinderNNParallel.cxx
cbm/parallel/CbmLitParallelTrackFitterTestMuon.cxx 
cbm/parallel/CbmLitParallelTrackFitterTestElectron.cxx 
)

If(TBB_FOUND)
  Set(LITTRACK_SRCS_RULECHECKER 
      ${LITTRACK_SRCS_RULECHECKER}
      cbm/parallel/CbmLitTrackFitSpeedTest.cxx
     )
EndIf(TBB_FOUND)

If(Boost_FOUND)
  Set(LITTRACK_SRCS_RULECHECKER 
      ${LITTRACK_SRCS_RULECHECKER}
      cbm/qa/base/CbmLitResultChecker.cxx
      cbm/qa/tracking/CbmLitReconstructionQaImpl.cxx
      cbm/qa/tracking/CbmLitTrackingQaHistCreator.cxx      
      cbm/qa/tracking/CbmLitTrackingQaDraw.cxx    
      cbm/qa/tracking/CbmLitQaPTreeCreator.cxx       
      cbm/qa/tracking/CbmLitTrackingQaHistNames.cxx
      cbm/qa/tracking/CbmLitQaReconstructionReportStudy.cxx            
      cbm/qa/tracking/CbmLitQaReconstructionReport.cxx   
      cbm/qa/field/CbmLitFieldQa.cxx
      cbm/qa/field/CbmLitFieldQaReport.cxx
      cbm/qa/report/CbmLitReport.cxx      
      cbm/qa/report/CbmLitStudyReport.cxx
      cbm/qa/report/CbmLitSimulationReport.cxx
      cbm/qa/report/CbmLitTextReportElement.cxx
      cbm/qa/report/CbmLitHtmlReportElement.cxx       
      cbm/qa/report/CbmLitLatexReportElement.cxx  
  )  
EndIf(Boost_FOUND)

set(LITTRACK_HEADERS
cbm/base/CbmLitTask.h
cbm/elid/CbmLitGlobalElectronId.h
cbm/finder/CbmLitFindGlobalTracks.h
cbm/finder/CbmLitFindMvdTracks.h
cbm/generators/CbmLitPolarizedGenerator.h
cbm/geo/CbmLitDet.h 
cbm/geo/CbmLitDetContFact.h
cbm/geo/CbmLitDetGeo.h 
cbm/geo/CbmLitDetGeoPar.h 
cbm/geo/CbmLitDetPoint.h
cbm/propagation/CbmPropagatorGeane.h
cbm/propagation/CbmPropagatorLit.h
cbm/finder/CbmLitFindGlobalTracksIdeal.h
cbm/qa/propagation/CbmLitPropagationQa.h
cbm/qa/propagation/CbmLitFitQa.h
cbm/qa/geo/CbmLitSimpleGeoQa.h 
cbm/qa/material/CbmLitCheckEnergyLossMuons.h 
cbm/qa/material/CbmLitCheckMultipleScattering.h
cbm/qa/material/CbmLitCheckBrem.h 
cbm/qa/CbmTrdHitsQa.h
cbm/qa/CbmLitCreateStandaloneData.h
cbm/qa/CbmLitTestMatrixMath.h
cbm/utils/CbmLitDrawHist.h
)

If(Boost_FOUND)
  Set(LITTRACK_HEADERS 
      ${LITTRACK_HEADERS}
      cbm/qa/field/CbmLitFieldQa.h
     )
EndIf(Boost_FOUND)

If(TBB_FOUND)
  Set(LITTRACK_HEADERS 
      ${LITTRACK_HEADERS}
      cbm/parallel/CbmLitTrackFitSpeedTest.h
     )
EndIf(TBB_FOUND)

If(Boost_FOUND)
  Set(LITTRACK_HEADERS 
      ${LITTRACK_HEADERS}
      cbm/qa/tracking/CbmLitReconstructionQa.h
     )
EndIf(Boost_FOUND)

SET(LITTRACK_SRCS 
${LITTRACK_SRCS_RULECHECKER}
${LITTRACKSTD_SRCS}
cbm/base/CbmLitFieldGridCreator.cxx
cbm/base/CbmLitEnvironment.cxx
cbm/base/CbmLitFieldFitter.cxx
cbm/base/CbmLitToolFactory.cxx
cbm/base/CbmLitSimpleGeometryConstructor.cxx
cbm/finder/CbmLitFindGlobalTracks.cxx 
cbm/finder/CbmLitFindGlobalTracksIdeal.cxx 
cbm/qa/propagation/CbmLitPropagationQa.cxx 
cbm/qa/propagation/CbmLitFitQa.cxx
cbm/utils/CbmLitConverter.cxx
)

If(Boost_FOUND)
  Set(LITTRACK_SRCS
      ${LITTRACK_SRCS}
      cbm/qa/tracking/CbmLitReconstructionQa.cxx
     )
EndIf(Boost_FOUND)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LITTRACK_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" LITTRACK_RULES)
endif(RULE_CHECKER_FOUND)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${LITTRACK_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3 -w")
  Message(STATUS "LitTrack will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "LitTrack will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${LITTRACK_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3 -w")
ENDIF (SSE_FOUND)


#set(LITTRACK_LINKDEF  LittrackLinkDef.h)
#set(LITTRACK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmLittrackDict.cxx) 

set(LINKDEF  LittrackLinkDef.h)
set(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmLittrackDict.cxx) 
set(HDRS ${LITTRACK_HEADERS})
If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

ROOT_GENERATE_DICTIONARY()

#SET(LITTRACK_SRCS ${LITTRACK_SRCS} ${LITTRACK_DICTIONARY})
SET(LITTRACK_SRCS ${LITTRACK_SRCS} ${DICTIONARY})

add_library(Littrack SHARED ${LITTRACK_SRCS})
target_link_libraries(Littrack ${ROOT_LIBRARIES})
set_target_properties(Littrack PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

If(TBB_FOUND)
  target_link_libraries(Littrack
        ${TBB_LIBRARIES}
  )
EndIf(TBB_FOUND)

install(TARGETS Littrack DESTINATION ${CMAKE_BINARY_DIR}/lib)

#add_subdirectory(parallel)
#add_subdirectory(cuda)
