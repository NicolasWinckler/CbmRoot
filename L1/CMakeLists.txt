# Create a library called "libL1" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase
${CBMROOT_SOURCE_DIR}/base 

${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/rich
#${CBMROOT_SOURCE_DIR}/cbmdata/much
#${CBMROOT_SOURCE_DIR}/cbmdata/stt
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/L1
${CBMROOT_SOURCE_DIR}/L1/OffLineInterface
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/trd 
${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})


set(L1RULECHECKER_SRCS
#L1Algo/L1CATrackFinder.cxx 
#CbmL1Performance.cxx 
#CbmL1ReadEvent.cxx 
CbmL1CATrdTrackFinderSA.cxx 
CbmL1TrdTracklet.cxx 
CbmL1TrdTracklet4.cxx 
CbmL1.cxx 
CbmL1TrdTrackFinderSts.cxx 
CbmL1TrackMerger.cxx 
CbmL1TofMerger.cxx 

)
set(L1_SRCS
${L1RULECHECKER_SRCS}
OffLineInterface/CbmL1RichENNRingFinder.cxx
OffLineInterface/CbmL1RichENNRingFinderParallel.cxx 
OffLineInterface/CbmL1StsTrackFinder.cxx 
#OffLineInterface/CbmL1MuchFinder.cxx 
#OffLineInterface/CbmL1MuchHit.cxx 
#OffLineInterface/CbmL1MuchTrack.cxx 
#OffLineInterface/CbmL1MuchFinderQa.cxx
OffLineInterface/CbmL1RichRingQa.cxx 
#OffLineInterface/CbmL1SttHit.cxx 
#OffLineInterface/CbmL1SttTrackFinder.cxx 
#OffLineInterface/CbmL1SttTrack.cxx 

L1Algo/L1Algo.cxx
L1Algo/L1CATrackFinder.cxx
L1Algo/L1TrackFitter.cxx
CbmL1Performance.cxx 
CbmL1ReadEvent.cxx

CbmL1MCTrack.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${L1RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" L1_RULES)
endif(RULE_CHECKER_FOUND)


set(L1_HEADERS
CbmL1CATrdTrackFinderSA.h
CbmL1.h
CbmL1MCTrack.h
CbmL1MCPoint.h
CbmL1StsHit.h
CbmL1TofMerger.h
CbmL1Track.h
CbmL1TrackMerger.h
CbmL1TrackPar.h
CbmL1TrdHit.h
CbmL1TrdTrackFinderSts.h
CbmL1TrdTracklet4.h
CbmL1TrdTracklet.h
CbmL1Vtx.h
#OffLineInterface/CbmL1MuchFinder.h
#OffLineInterface/CbmL1MuchFinderQa.h
#OffLineInterface/CbmL1MuchHit.h
#OffLineInterface/CbmL1MuchTrack.h
OffLineInterface/CbmL1RichENNRingFinder.h
OffLineInterface/CbmL1RichRingQa.h
OffLineInterface/CbmL1StsTrackFinder.h
#OffLineInterface/CbmL1SttHit.h
#OffLineInterface/CbmL1SttTrackFinder.h
#OffLineInterface/CbmL1SttTrack.h
# L1Algo/L1Algo.h
)

ADD_DEFINITIONS(-Wall -Wabi -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend ) #   -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual: wait for other parts of cbmroot.  TODO: -Wold-style-cast   - warnings in "root/include"

# Check if SSE instructions are available on the machine where 
# the project is compiled.

IF(CMAKE_SYSTEM_NAME MATCHES Linux)
  EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)
  STRING(REGEX REPLACE "^.*(sse).*$" "\\1" SSE_THERE ${CPUINFO})
  STRING(COMPARE EQUAL "sse" "${SSE_THERE}" SSE_TRUE)
ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)
  IF(CMAKE_SYSTEM_NAME MATCHES Darwin)
    EXEC_PROGRAM("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE CPUINFO)
    STRING(REGEX REPLACE "^.*(SSE).*$" "\\1" SSE_THERE ${CPUINFO})
    STRING(COMPARE EQUAL "SSE" "${SSE_THERE}" SSE_TRUE)
  ENDIF(CMAKE_SYSTEM_NAME MATCHES Darwin)
ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)

IF (SSE_TRUE)
  MESSAGE(STATUS "SSE extensions available")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${L1_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_TRUE)
  MESSAGE (STATUS "*********************************************************************************")
  MESSAGE(STATUS "NO SSE extensions available. The L1 tracking will be compiled in the slow version")
  MESSAGE (STATUS "*********************************************************************************")
  SET_SOURCE_FILES_PROPERTIES(${L1_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_TRUE)

set(L1_LINKDEF L1LinkDef.h)
set(L1_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/L1Dict.cxx) 

ROOT_GENERATE_DICTIONARY("${L1_HEADERS}" "${L1_LINKDEF}" "${L1_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(L1_SRCS ${L1_SRCS} ${L1_DICTIONARY})

add_library(L1 SHARED ${L1_SRCS})
target_link_libraries(L1 ${ROOT_LIBRARIES})
set_target_properties(L1 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS L1 DESTINATION ${CMAKE_BINARY_DIR}/lib)
