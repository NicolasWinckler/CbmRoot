# Create a library called "libLittrackparallel".

set(INCLUDE_DIRECTORIES

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

)
 
If(TBB_FOUND)
  Set(LINK_DIRECTORIES
    ${LINK_DIRECTORIES} 
    ${TBB_LIBRARY_DIRS}
  )
EndIf(TBB_FOUND)

link_directories(${LINK_DIRECTORIES})

set(LITTRACKPARALLEL_SRCS

)

#If(TBB_FOUND)
  Set(LITTRACKPARALLEL_SRCS
    ${LITTRACKPARALLEL_SRCS}
    LitTrackFinderNNBase.cxx
    electron/LitTrackFinderNNBaseElectron.cxx
    electron/LitTrackFinderNNScalarElectron.cxx
    electron/LitTrackFinderNNVecElectron.cxx
    muon/LitTrackFinderNNVecMuon.cxx
  )
#EndIf(TBB_FOUND)

set(LITTRACKPARALLEL_HEADERS

)

#If(TBB_FOUND)
  Set(LITTRACKPARALLEL_HEADERS 
    ${LITTRACKPARALLEL_HEADERS}
  )
#EndIf(TBB_FOUND)


if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LITTRACKPARALLEL_SRCS}" "${INCLUDE_DIRECTORIES}" LITTRACKPARALLEL_RULES)
endif(RULE_CHECKER_FOUND)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${LITTRACKPARALLEL_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3 ")
  Message(STATUS "Littrackparallel will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Littrackparallel will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${LITTRACKPARALLEL_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3 ")
ENDIF (SSE_FOUND)

add_library(Littrackparallel SHARED ${LITTRACKPARALLEL_SRCS})
set_target_properties(Littrackparallel PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

If(TBB_FOUND)
  target_link_libraries(Littrackparallel
    ${TBB_LIBRARIES}
  )
EndIf(TBB_FOUND)

install(TARGETS Littrackparallel DESTINATION ${CMAKE_BINARY_DIR}/lib)
