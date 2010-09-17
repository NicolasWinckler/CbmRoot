# Check if cmake has the required version
cmake_minimum_required(VERSION 2.4.3 FATAL_ERROR)
       
# Project name                    
project(standalone)

# Where to look first for cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")


#find_package(TBB)

#if(NOT TBB_FOUND)
#    MESSAGE (STATUS "TBB not found. ")
#else(NOT TBB_FOUND)
#	MESSAGE (STATUS "TBB found.")
#endif(NOT TBB_FOUND)

set(TBB_INCLUDE_DIRS "/u/andrey/soft/tbb/tbb22_009oss/include")
set(TBB_LIBRARIES "/u/andrey/soft/tbb/Lenny64/libtbb.so")
set(VC_INCLUDE_DIRS "/u/slebedev/Baykal/soft/Vc")
set(VC_LIBRARIES "/u/slebedev/Baykal/soft/Vc/build/libVc.a")


set(INCLUDE_DIRECTORIES
${PROJECT_SOURCE_DIR}/algorithm
${TBB_INCLUDE_DIRS}
${VC_INCLUDE_DIRS}
)

include_directories(${INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
${TBB_LIBRARY_DIRS}
)

link_directories( ${LINK_DIRECTORIES})


set(STANDALONE_SRCS
main.cxx
algorithm/CbmRichRingFinderHough.cxx
algorithm/CbmRichRingFinderHoughImpl.cxx
algorithm/NNfunction.cxx
algorithm/CbmRichRingFinderHoughSimd.cxx
)

#set(STANDALONE_HEADERS
#)

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
  SET_SOURCE_FILES_PROPERTIES(${STANDALONE_SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3")
ELSE (SSE_TRUE)
  MESSAGE (STATUS "*********************************************************************************")
  MESSAGE(STATUS "NO SSE extensions available.")
  MESSAGE (STATUS "*********************************************************************************")
  SET_SOURCE_FILES_PROPERTIES(${STANDALONE_SRCS} PROPERTIES COMPILE_FLAGS "-O3")
ENDIF (SSE_TRUE)


# Executable name
add_executable(standalone main.cxx)

add_library(parallel SHARED ${STANDALONE_SRCS})
target_link_libraries(standalone parallel)

target_link_libraries(parallel ${TBB_LIBRARIES})
target_link_libraries(parallel ${VC_LIBRARIES})
