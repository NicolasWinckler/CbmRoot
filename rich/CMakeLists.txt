# Create a library called "libRich" which includes the source files given in
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
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/rich 
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
#LhcbRingFinder.cxx
CbmRichRingFinderHoughBase.cxx
#CbmRichRingFinderHoughSimd.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RICH_SRCS}" "${INCLUDE_DIRECTORIES}" RICH_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RICH_HEADERS "${RICH_SRCS}")

# BEGIN SSE
# Check if SSE instructions are available on the machine where 
# the project is compiled.

#IF(CMAKE_SYSTEM_NAME MATCHES Linux)
#  EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)
#  STRING(REGEX REPLACE "^.*(sse).*$" "\\1" SSE_THERE ${CPUINFO})
#  STRING(COMPARE EQUAL "sse" "${SSE_THERE}" SSE_TRUE)
#ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)
#  IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
#    EXEC_PROGRAM("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE CPUINFO)
#    STRING(REGEX REPLACE "^.*(SSE).*$" "\\1" SSE_THERE ${CPUINFO})
#    STRING(COMPARE EQUAL "SSE" "${SSE_THERE}" SSE_TRUE)
#  ENDIF(CMAKE_SYSTEM_NAME MATCHES Darwin)
#ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)
#
#IF (SSE_TRUE)
#  MESSAGE(STATUS "SSE extensions available")
#  ADD_DEFINITIONS(-DHAVE_SSE)
#  SET_SOURCE_FILES_PROPERTIES(${RICH_SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3")
#ELSE (SSE_TRUE)
#  MESSAGE (STATUS "*********************************************************************************")
#  MESSAGE(STATUS "NO SSE extensions available")
#  MESSAGE (STATUS "*********************************************************************************")
#  SET_SOURCE_FILES_PROPERTIES(${RICH_SRCS} PROPERTIES COMPILE_FLAGS "-O3")
#ENDIF (SSE_TRUE)

# END SSE


set(RICH_LINKDEF  RichLinkDef.h)
set(RICH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmRichDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RICH_HEADERS}" "${RICH_LINKDEF}" "${RICH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(RICH_SRCS ${RICH_SRCS} ${RICH_DICTIONARY})


add_library(Rich SHARED ${RICH_SRCS})
target_link_libraries(Rich ${ROOT_LIBRARIES})
set_target_properties(Rich PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Rich DESTINATION ${CMAKE_BINARY_DIR}/lib)
