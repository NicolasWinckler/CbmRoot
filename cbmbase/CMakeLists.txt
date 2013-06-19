# Create a library called "libCbmBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${Boost_INCLUDE_DIR}
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmAddress.cxx
CbmModuleList.cxx

#CbmDigiManager.cxx
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

CbmHistManager.cxx
draw/CbmDrawHist.cxx
report/CbmReport.cxx      
report/CbmStudyReport.cxx
report/CbmSimulationReport.cxx
report/CbmTextReportElement.cxx
report/CbmHtmlReportElement.cxx       
report/CbmLatexReportElement.cxx  
)


EXEC_PROGRAM( ${ROOT_CONFIG_EXECUTABLE} 
    ARGS "--has-gdml"
    OUTPUT_VARIABLE ROOT_HAS_GDML )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )



Set(LINKDEF CbmBaseLinkDef.h)
Set(LIBRARY_NAME CbmBase)
Set(DEPENDENCIES
    CbmData Base
)

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

GENERATE_LIBRARY()


