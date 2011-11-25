# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${CBMROOT_SOURCE_DIR}/rich
${CBMROOT_SOURCE_DIR}/rich/fitter
${CBMROOT_SOURCE_DIR}/rich/finder
${CBMROOT_SOURCE_DIR}/rich/qa
${CBMROOT_SOURCE_DIR}/rich/tracks
${CBMROOT_SOURCE_DIR}/rich/selection
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/littrack
#/u/slebedev/Baykal/tbb21_017oss/include
#/u/andrey/soft/tbb/tbb22_009oss/include
)

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

set(RICH_SRCS
CbmGeoRich.cxx 
CbmRich.cxx 
CbmRichHitProducer.cxx   
CbmGeoRichPar.cxx 
CbmRichContFact.cxx
CbmRichMatchRings.cxx
CbmRichElectronIdAnn.cxx
finder/CbmRichRingFinderTrack.cxx
finder/CbmRichRingFinderIdeal.cxx 
finder/CbmRichFindRings.cxx
#finder/CbmRichRingFinderHoughSimd.cxx
finder/CbmRichRingFinderHough.cxx
finder/CbmRichRingFinderHoughImpl.cxx
fitter/CbmRichRingFitterCircle.cxx  
fitter/CbmRichFitRings.cxx
fitter/CbmRichRingFitterCOP.cxx  
fitter/CbmRichRingFitterRobustCOP.cxx 
fitter/CbmRichRingFitterTAU.cxx 
fitter/CbmRichRingFitterImpl.cxx
fitter/CbmRichRingFitterEllipse.cxx
fitter/CbmRichRingFitterEllipseTau.cxx
fitter/CbmRichRingFitterEllipseBase.cxx
tracks/CbmRichProjectionProducer.cxx 
tracks/CbmRichTrackExtrapolationIdeal.cxx
tracks/CbmRichTrackExtrapolationMirrorIdeal.cxx
tracks/CbmRichExtrapolateTracks.cxx
tracks/CbmRichRingTrackAssignClosestD.cxx
tracks/CbmRichRingTrackAssignIdeal.cxx  
tracks/CbmRichAssignTrack.cxx
qa/CbmRichTestSim.cxx  
qa/CbmRichTestHits.cxx 
qa/CbmRichRingQa.cxx
qa/CbmRichGeoTest.cxx 
qa/CbmRichRingFitterQa.cxx
qa/CbmRichElectronsQa.cxx 
#qa/CbmRichParallelQa.cxx 
selection/CbmRichRingSelectImpl.cxx
selection/CbmRichRingSelectNeuralNet.cxx  
selection/NNfunction.cxx 
#LhcbRingFinder.cxx  
prototype/CbmRichProt.cxx
prototype/CbmRichProtHitProducer.cxx
prototype/CbmRichProtPrepareExtrapolation.cxx
prototype/CbmRichProtProjectionProducer.cxx
prototype/CbmRichProtRingFinderHough.cxx
prototype/CbmRichProtRingFinderHoughImpl.cxx
)

If(TBB_FOUND)
  Set(RICH_SRCS 
      ${RICH_SRCS}
      finder/CbmRichRingFinderHoughParallel.cxx
     )
EndIf(TBB_FOUND)

set(RICH_HEADERS
CbmGeoRich.h
CbmRich.h
CbmRichHitProducer.h  
CbmGeoRichPar.h
CbmRichContFact.h
CbmRichMatchRings.h
CbmRichElectronIdAnn.h
finder/CbmRichRingFinderTrack.h
finder/CbmRichRingFinderIdeal.h
finder/CbmRichFindRings.h
#finder/CbmRichRingFinderHoughParallel.h
finder/CbmRichRingFinderHough.h 
fitter/CbmRichRingFitterCircle.h
fitter/CbmRichFitRings.h 
fitter/CbmRichRingFitterRobustCOP.h
fitter/CbmRichRingFitterTAU.h 
fitter/CbmRichRingFitterImpl.h
fitter/CbmRichRingFitterEllipse.h 
fitter/CbmRichRingFitterEllipseTau.h
fitter/CbmRichRingFitterEllipseBase.h
tracks/CbmRichProjectionProducer.h
tracks/CbmRichTrackExtrapolationIdeal.h
tracks/CbmRichTrackExtrapolationMirrorIdeal.h
tracks/CbmRichExtrapolateTracks.h
tracks/CbmRichRingTrackAssignClosestD.h
tracks/CbmRichRingTrackAssignIdeal.h 
tracks/CbmRichAssignTrack.h
qa/CbmRichTestSim.h qa/CbmRichTestHits.h 
qa/CbmRichRingQa.h
qa/CbmRichGeoTest.h 
qa/CbmRichRingFitterQa.h 
qa/CbmRichElectronsQa.h
#qa/CbmRichParallelQa.h  
#LhcbRingFinder.h  
prototype/CbmRichProt.h
prototype/CbmRichProtHitProducer.h
prototype/CbmRichProtPrepareExtrapolation.h
prototype/CbmRichProtProjectionProducer.h
prototype/CbmRichProtRingFinderHough.h
prototype/CbmRichProtRingFinderHoughImpl.h
)

If(TBB_FOUND)
  Set(RICH_HEADERS 
      ${RICH_HEADERS}
      finder/CbmRichRingFinderHoughParallel.h
     )
EndIf(TBB_FOUND)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RICH_SRCS}" "${INCLUDE_DIRECTORIES}" RICH_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h RICH_HEADERS "${RICH_SRCS}")

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${RICH_SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3")
  Message(STATUS "Rich will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Rich will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${RICH_SRCS} PROPERTIES COMPILE_FLAGS "-O3")
ENDIF (SSE_FOUND)

# END SSE


If(TBB_FOUND)
  set(RICH_LINKDEF  RichLinkDef_tbb.h)
Else(TBB_FOUND)
  set(RICH_LINKDEF  RichLinkDef.h)
EndIf(TBB_FOUND)

set(RICH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmRichDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RICH_HEADERS}" "${RICH_LINKDEF}" "${RICH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(RICH_SRCS ${RICH_SRCS} ${RICH_DICTIONARY})


add_library(Rich SHARED ${RICH_SRCS})
target_link_libraries(Rich ${ROOT_LIBRARIES})
set_target_properties(Rich PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

If(TBB_FOUND)
  target_link_libraries(Rich
                        ${TBB_LIBRARIES}
                       )

EndIf(TBB_FOUND)

################ install ###################
install(TARGETS Rich DESTINATION ${CMAKE_BINARY_DIR}/lib)
