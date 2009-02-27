# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/global 
${CBMROOT_SOURCE_DIR}/rich 

${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/tof
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RICH_SRCS
CbmGeoRich.cxx 
CbmRich.cxx 
CbmRichHitProducer.cxx   
CbmRichProjectionProducer.cxx 
CbmGeoRichPar.cxx 
CbmRichContFact.cxx   
CbmRichRingFinderHough.cxx 
CbmRichRingFinderTrack.cxx
CbmRichRingFinderIdeal.cxx 
CbmRichFindRings.cxx
CbmRichRingFitterCircle.cxx 
CbmRichFitRings.cxx
CbmRichRingFitterCOP.cxx 
CbmRichRingFitterRobustCOP.cxx 
CbmRichRingFitterTAU.cxx
CbmRichTrackExtrapolationIdeal.cxx
CbmRichTrackExtrapolationMirrorIdeal.cxx
CbmRichExtrapolateTracks.cxx
CbmRichRingTrackAssignClosestD.cxx
CbmRichRingTrackAssignIdeal.cxx 
CbmRichAssignTrack.cxx
CbmRichTestSim.cxx 
CbmRichTestHits.cxx
CbmRichRingQa.cxx
CbmRichRingSelectImpl.cxx
CbmRichRingSelectNeuralNet.cxx
CbmRichSelectRings.cxx
CbmRichMatchRings.cxx
CbmRichRingFitterImpl.cxx
CbmRichRingFitterEllipse.cxx
CbmRichGeoTest.cxx
CbmRichRingFitterEllipseTau.cxx
CbmRichRingFitterQa.cxx
CbmRichFuzzyKE.cxx
CbmRichElectronsQa.cxx
CbmRichElectronIdAnn.cxx
CbmRichRingFitterEllipseBase.cxx
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
set_target_properties(Rich PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Rich DESTINATION ${CMAKE_BINARY_DIR}/lib)
