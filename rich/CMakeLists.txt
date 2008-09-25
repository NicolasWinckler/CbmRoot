# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/global 
${CBMROOT_SOURCE_DIR}/rich 
${CBMROOT_SOURCE_DIR}/trd
${CBMROOT_SOURCE_DIR}/tof
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RICH_SRCS
CbmGeoRich.cxx 
CbmRichRing.cxx	
CbmRichRingFinder.cxx 
CbmRich.cxx 
CbmRichHit.cxx 
CbmRichHitProducer.cxx   
CbmRichProjectionProducer.cxx 
CbmGeoRichPar.cxx 
CbmRichContFact.cxx   
CbmRichPoint.cxx
CbmRichRingFinderHough.cxx 
CbmRichRingFinderTrack.cxx
CbmRichRingFinderIdeal.cxx 
CbmRichFindRings.cxx
CbmRichRingFitter.cxx 
CbmRichRingFitterCircle.cxx 
CbmRichFitRings.cxx
CbmRichRingFitterCOP.cxx 
CbmRichRingFitterRobustCOP.cxx 
CbmRichRingFitterTAU.cxx
CbmRichTrackExtrapolation.cxx 
CbmRichTrackExtrapolationIdeal.cxx
CbmRichTrackExtrapolationMirrorIdeal.cxx
CbmRichExtrapolateTracks.cxx
CbmRichRingMatch.cxx 
CbmRichMatchRings.cxx
CbmRichRingTrackAssign.cxx 
CbmRichRingTrackAssignClosestD.cxx
CbmRichRingTrackAssignIdeal.cxx 
CbmRichAssignTrack.cxx
CbmRichTestSim.cxx 
CbmRichTestHits.cxx
CbmRichRingQa.cxx
CbmRichRingSelect.cxx 
CbmRichRingSelectNeuralNet.cxx
CbmRichRingSelect2DCuts.cxx 
CbmRichSelectRings.cxx
CbmRichRingFitterEllipse.cxx
#CbmRichFuzzyKE.cxx
#CbmRichElectrons.cxx
#CbmRichElectronsQa.cxx
#CbmRichElIdAnn.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RICH_SRCS}" "${INCLUDE_DIRECTORIES}" RICH_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RICH_HEADERS "${RICH_SRCS}")

set(RICH_LINKDEF  RichLinkDef.h)
set(RICH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmRichDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RICH_HEADERS}" "${RICH_LINKDEF}" "${RICH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(RICH_SRCS ${RICH_SRCS} ${RICH_DICTIONARY})


add_library(Rich SHARED ${RICH_SRCS})
target_link_libraries(Rich ${ROOT_LIBRARIES})
set_target_properties(Rich PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Rich DESTINATION ${CMAKE_BINARY_DIR}/lib)
