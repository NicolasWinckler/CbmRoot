# Create a library called "libCbmBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
${Boost_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/geobase 
${CBMROOT_SOURCE_DIR}/parbase 
${CBMROOT_SOURCE_DIR}/base 
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane

#${CBMROOT_SOURCE_DIR}/cbmdata/rich
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(CBMBASE_SRCS
CbmDetectorList.cxx

CbmDigiManager.cxx
CbmPropagator.cxx

CbmTrdTrackFinder.cxx  
CbmTrdTrackFitter.cxx  

CbmStsTrackFinder.cxx 
CbmStsTrackFitter.cxx 

CbmMuchTrackFinder.cxx

CbmRichRingFinder.cxx 
CbmRichRingFitter.cxx 
CbmRichTrackExtrapolation.cxx 
CbmRichRingTrackAssign.cxx 
CbmRichRingSelect.cxx 

CbmGlobalTrackFitter.cxx
CbmPrimaryVertexFinder.cxx
CbmRichMerger.cxx
CbmTofMerger.cxx
CbmTrackMerger.cxx

CbmMCMatch.cxx
CbmMCMatchCreatorTask.cxx
CbmMCMatchLoaderTask.cxx
CbmMCMatchSelectorTask.cxx
CbmMCObject.cxx
CbmMCStage.cxx
CbmMCResult.cxx
#CbmMCTestMomentumCompare.cxx
CbmMCTestHitCompare.cxx
CbmMCEntry.cxx
CbmMCList.cxx

CbmModule.cxx
)

If(Boost_FOUND)
  Set(CBMBASE_SRCS 
    ${CBMBASE_SRCS}
    report/CbmReport.cxx      
    report/CbmStudyReport.cxx
    report/CbmSimulationReport.cxx
    report/CbmTextReportElement.cxx
    report/CbmHtmlReportElement.cxx       
    report/CbmLatexReportElement.cxx  
  )
EndIf(Boost_FOUND)

set(CBMBASE_HEADERS
CbmDetectorList.h

CbmDigiManager.h
CbmPropagator.h

CbmTrdTrackFinder.h
CbmTrdTrackFitter.h  

CbmStsTrackFinder.h 
CbmStsTrackFitter.h 

CbmMuchTrackFinder.h

CbmRichRingFinder.h
CbmRichRingFitter.h 
CbmRichTrackExtrapolation.h
CbmRichRingTrackAssign.h
CbmRichRingSelect.h

CbmGlobalTrackFitter.h
CbmPrimaryVertexFinder.h
CbmRichMerger.h
CbmTofMerger.h
CbmTrackMerger.h

CbmMCMatch.h
CbmMCMatchCreatorTask.h
CbmMCMatchLoaderTask.h
CbmMCMatchSelectorTask.h
CbmMCObject.h
CbmMCStage.h
CbmMCResult.h
#CbmMCTestMomentumCompare.h
CbmMCTestHitCompare.h
CbmMCEntry.h
CbmMCList.h

CbmModule.h
)

EXEC_PROGRAM( ${ROOT_CONFIG_EXECUTABLE} 
    ARGS "--has-gdml"
    OUTPUT_VARIABLE ROOT_HAS_GDML )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CBMBASE_SRCS}" "${INCLUDE_DIRECTORIES}" CBMBASE_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h CBMBASE_HEADERS "${CBMBASE_SRCS}")

set(CBMBASE_LINKDEF  CbmBaseLinkDef.h)
set(CBMBASE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmBaseDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CBMBASE_HEADERS}" "${CBMBASE_LINKDEF}" "${CBMBASE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(CBMBASE_SRCS ${CBMBASE_SRCS} ${CBMBASE_DICTIONARY})

add_library(CbmBase SHARED ${CBMBASE_SRCS})

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  target_link_libraries(CbmBase ${ROOT_LIBRARIES} -lGdml)
Else ( ${ROOT_HAS_GDML} STREQUAL yes )
  target_link_libraries(CbmBase ${ROOT_LIBRARIES})
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

set_target_properties(CbmBase PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS CbmBase DESTINATION ${CMAKE_BINARY_DIR}/lib)
