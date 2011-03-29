# Create a library called "libKF" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 

${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/mvd 
${CBMROOT_SOURCE_DIR}/sts 
${CBMROOT_SOURCE_DIR}/rich 
${CBMROOT_SOURCE_DIR}/much 
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/trd 
${CBMROOT_SOURCE_DIR}/tof 
${CBMROOT_SOURCE_DIR}/ecal
${CBMROOT_SOURCE_DIR}/stt 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(KF_SRCS
CbmKF.cxx 
CbmKFFieldMath.cxx 
CbmKFHit.cxx 
CbmKFMaterial.cxx 
CbmKFMath.cxx 
CbmKFPixelMeasurement.cxx 
CbmKFPrimaryVertexFinder.cxx 
CbmKFSecondaryVertexFinder.cxx 
CbmKFTrackInterface.cxx 
CbmKFUMeasurement.cxx 
CbmKFVertexInterface.cxx 
Interface/CbmEcalTrackExtrapolationKF.cxx
Interface/CbmKFStsHit.cxx 
Interface/CbmKFTrack.cxx 
Interface/CbmKFTrdHit.cxx 
Interface/CbmPVFinderKF.cxx 
Interface/CbmRichTrackExtrapolationKF.cxx 
Interface/CbmStsFitPerformanceTask.cxx 
Interface/CbmStsKFTrackFitter.cxx 
Interface/CbmStsKFSecondaryVertexFinder.cxx 
Interface/CbmTrdTrackFitterKF.cxx 
Interface/CbmGlobalTrackFitterKF.cxx
CbmKFParticle.cxx
CbmKFParticle_simd.cxx
CbmKFParticleInterface.cxx
KFQA/CbmKFTrErrMCPoints.cxx
KFQA/CbmKFTrackFitQA.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${KF_SRCS}" "${INCLUDE_DIRECTORIES}" KF_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h KF_HEADERS "${KF_SRCS}")
set(KF_HEADERS
CbmKF.h 
CbmKFFieldMath.h
CbmKFHit.h
CbmKFMaterial.h
CbmKFMath.h
CbmKFPixelMeasurement.h
CbmKFPrimaryVertexFinder.h
CbmKFSecondaryVertexFinder.h
CbmKFTrackInterface.h
CbmKFUMeasurement.h 
CbmKFVertexInterface.h
Interface/CbmEcalTrackExtrapolationKF.h
Interface/CbmKFStsHit.h
Interface/CbmKFTrack.h 
Interface/CbmKFTrdHit.h 
Interface/CbmPVFinderKF.h
Interface/CbmRichTrackExtrapolationKF.h
Interface/CbmStsFitPerformanceTask.h
Interface/CbmStsKFTrackFitter.h
Interface/CbmStsKFSecondaryVertexFinder.h
Interface/CbmTrdTrackFitterKF.h
Interface/CbmGlobalTrackFitterKF.h
CbmKFParticle.h
KFQA/CbmKFTrErrMCPoints.h
KFQA/CbmKFTrackFitQA.h
)

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
  SET_SOURCE_FILES_PROPERTIES(${KF_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_TRUE)
  MESSAGE (STATUS "*********************************************************************************")
  MESSAGE(STATUS "NO SSE extensions available. The L1 tracking will be compiled in the slow version")
  MESSAGE (STATUS "*********************************************************************************")
  SET_SOURCE_FILES_PROPERTIES(${KF_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_TRUE)

set(KF_LINKDEF KFLinkDef.h)
set(KF_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/KFDict.cxx) 

ROOT_GENERATE_DICTIONARY("${KF_HEADERS}" "${KF_LINKDEF}" "${KF_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(KF_SRCS ${KF_SRCS} ${KF_DICTIONARY})


add_library(KF SHARED ${KF_SRCS})
target_link_libraries(KF ${ROOT_LIBRARIES})
set_target_properties(KF PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS KF DESTINATION ${CMAKE_BINARY_DIR}/lib)
