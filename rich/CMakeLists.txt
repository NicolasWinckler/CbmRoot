# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
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
#/u/slebedev/Baykal/tbb21_017oss/include

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
CbmGeoRichPar.cxx 
CbmRichContFact.cxx   
finder/CbmRichRingFinderHough.cxx
finder/CbmRichRingFinderTrack.cxx
finder/CbmRichRingFinderIdeal.cxx 
finder/CbmRichFindRings.cxx
finder/CbmRichFuzzyKE.cxx
#finder/CbmRichRingFinderHoughSimd.cxx
finder/CbmRichRingFinderHoughBase.cxx
finder/CbmRichRingFinderHoughParallel.cxx
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
selection/CbmRichRingSelectImpl.cxx
selection/CbmRichRingSelectNeuralNet.cxx
selection/CbmRichSelectRings.cxx
CbmRichMatchRings.cxx
CbmRichElectronIdAnn.cxx
#LhcbRingFinder.cxx
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
